/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Padmavathi Venna <padma.v@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include <asm/arch-exynos/spi.h>
#include <asm/io.h>

/* Information about each SPI controller */
struct spi_bus {
	enum periph_id periph_id;
	s32 frequency;		/* Default clock frequency, -1 for none */
	struct exynos_spi *regs;
	int inited;		/* 1 if this bus is ready for use */
};

/* A list of spi buses that we know about */
static struct spi_bus spi_bus[EXYNOS5_SPI_NUM_CONTROLLERS];

struct exynos_spi_slave {
	struct spi_slave slave;
	struct exynos_spi *regs;
	unsigned int freq;		/* Default frequency */
	unsigned int mode;
	enum periph_id periph_id;	/* Peripheral ID for this device */
	unsigned int fifo_size;
};

static struct spi_bus *spi_get_bus(unsigned dev_index)
{
	if (dev_index < EXYNOS5_SPI_NUM_CONTROLLERS)
		return &spi_bus[dev_index];
	debug("%s: invalid bus %d", __func__, dev_index);

	return NULL;
}

static inline struct exynos_spi_slave *to_exynos_spi(struct spi_slave *slave)
{
	return container_of(slave, struct exynos_spi_slave, slave);
}

/**
 * Setup the driver private data
 *
 * @param bus		ID of the bus that the slave is attached to
 * @param cs		ID of the chip select connected to the slave
 * @param max_hz	Required spi frequency
 * @param mode		Required spi mode (clk polarity, clk phase and
 *			master or slave)
 * @return new device or NULL
 */
struct spi_slave *spi_setup_slave(unsigned int busnum, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct exynos_spi_slave *spi_slave;
	struct spi_bus *bus;

	if (!spi_cs_is_valid(busnum, cs)) {
		debug("%s: Invalid bus/chip select %d, %d\n", __func__,
		      busnum, cs);
		return NULL;
	}

	spi_slave = malloc(sizeof(*spi_slave));
	if (!spi_slave) {
		debug("%s: Could not allocate spi_slave\n", __func__);
		return NULL;
	}

	bus = &spi_bus[busnum];
	spi_slave->slave.bus = busnum;
	spi_slave->slave.cs = cs;
	spi_slave->regs = bus->regs;
	spi_slave->mode = mode;
	spi_slave->periph_id = bus->periph_id;
	if (bus->periph_id == PERIPH_ID_SPI1 ||
	    bus->periph_id == PERIPH_ID_SPI2)
		spi_slave->fifo_size = 64;
	else
		spi_slave->fifo_size = 256;

	spi_slave->freq = bus->frequency;
	if (max_hz)
		spi_slave->freq = min(max_hz, spi_slave->freq);

	return &spi_slave->slave;
}

/**
 * Free spi controller
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 */
void spi_free_slave(struct spi_slave *slave)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);

	free(spi_slave);
}

/**
 * Flush spi tx, rx fifos and reset the SPI controller
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 */
static void spi_flush_fifo(struct spi_slave *slave)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);
	struct exynos_spi *regs = spi_slave->regs;

	clrsetbits_le32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	setbits_le32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
}

/**
 * Initialize the spi base registers, set the required clock frequency and
 * initialize the gpios
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 * @return zero on success else a negative value
 */
int spi_claim_bus(struct spi_slave *slave)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);
	struct exynos_spi *regs = spi_slave->regs;
	u32 reg = 0;
	int ret;

	ret = set_spi_clk(spi_slave->periph_id,
					spi_slave->freq);
	if (ret < 0) {
		debug("%s: Failed to setup spi clock\n", __func__);
		return ret;
	}

	exynos_pinmux_config(spi_slave->periph_id, PINMUX_FLAG_NONE);

	spi_flush_fifo(slave);

	reg = readl(&regs->ch_cfg);
	reg &= ~(SPI_CH_CPHA_B | SPI_CH_CPOL_L);

	if (spi_slave->mode & SPI_CPHA)
		reg |= SPI_CH_CPHA_B;

	if (spi_slave->mode & SPI_CPOL)
		reg |= SPI_CH_CPOL_L;

	writel(reg, &regs->ch_cfg);
	writel(SPI_FB_DELAY_180, &regs->fb_clk);

	return 0;
}

/**
 * Reset the spi H/W and flush the tx and rx fifos
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 */
void spi_release_bus(struct spi_slave *slave)
{
	spi_flush_fifo(slave);
}

static void spi_get_fifo_levels(struct exynos_spi *regs,
	int *rx_lvl, int *tx_lvl)
{
	uint32_t spi_sts = readl(&regs->spi_sts);

	*rx_lvl = (spi_sts >> SPI_RX_LVL_OFFSET) & SPI_FIFO_LVL_MASK;
	*tx_lvl = (spi_sts >> SPI_TX_LVL_OFFSET) & SPI_FIFO_LVL_MASK;
}

/**
 * If there's something to transfer, do a software reset and set a
 * transaction size.
 *
 * @param regs	SPI peripheral registers
 * @param count	Number of bytes to transfer
 */
static void spi_request_bytes(struct exynos_spi *regs, int count)
{
	assert(count && count < (1 << 16));
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	writel(count | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
}

static void spi_rx_tx(struct exynos_spi_slave *spi_slave, int todo,
			void **dinp, void const **doutp)
{
	struct exynos_spi *regs = spi_slave->regs;
	uchar *rxp = *dinp;
	const uchar *txp = *doutp;
	int rx_lvl, tx_lvl;
	uint out_bytes, in_bytes;

	out_bytes = in_bytes = todo;

	/*
	 * If there's something to send, do a software reset and set a
	 * transaction size.
	 */
	spi_request_bytes(regs, todo);

	/*
	 * Bytes are transmitted/received in pairs. Wait to receive all the
	 * data because then transmission will be done as well.
	 */
	while (in_bytes) {
		int temp;

		/* Keep the fifos full/empty. */
		spi_get_fifo_levels(regs, &rx_lvl, &tx_lvl);
		if (tx_lvl < spi_slave->fifo_size && out_bytes) {
			temp = txp ? *txp++ : 0xff;
			writel(temp, &regs->tx_data);
			out_bytes--;
		}
		if (rx_lvl > 0 && in_bytes) {
			temp = readl(&regs->rx_data);
			if (rxp)
				*rxp++ = temp;
			in_bytes--;
		}
	}
	*dinp = rxp;
	*doutp = txp;
}

/**
 * Transfer and receive data
 *
 * @param slave		Pointer to spi_slave to which controller has to
 *			communicate with
 * @param bitlen	No of bits to tranfer or receive
 * @param dout		Pointer to transfer buffer
 * @param din		Pointer to receive buffer
 * @param flags		Flags for transfer begin and end
 * @return zero on success else a negative value
 */
int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
	     void *din, unsigned long flags)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);
	int upto, todo;
	int bytelen;

	/* spi core configured to do 8 bit transfers */
	if (bitlen % 8) {
		debug("Non byte aligned SPI transfer.\n");
		return -1;
	}

	/* Start the transaction, if necessary. */
	if ((flags & SPI_XFER_BEGIN))
		spi_cs_activate(slave);

	/* Exynos SPI limits each transfer to 65535 bytes */
	bytelen =  bitlen / 8;
	for (upto = 0; upto < bytelen; upto += todo) {
		todo = min(bytelen - upto, (1 << 16) - 1);
		spi_rx_tx(spi_slave, todo, &din, &dout);
	}

	/* Stop the transaction, if necessary. */
	if ((flags & SPI_XFER_END))
		spi_cs_deactivate(slave);

	return 0;
}

/**
 * Validates the bus and chip select numbers
 *
 * @param bus	ID of the bus that the slave is attached to
 * @param cs	ID of the chip select connected to the slave
 * @return one on success else zero
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return spi_get_bus(bus) && cs == 0;
}

extern int artik5_ax88796c_cs_activate(void);
extern int artik5_ax88796c_cs_deactivate(void);

/**
 * Activate the CS by driving it LOW
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 */
void spi_cs_activate(struct spi_slave *slave)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);

#ifdef CONFIG_MACH_ARTIK5
	artik5_ax88796c_cs_activate();
#endif

	clrbits_le32(&spi_slave->regs->cs_reg, SPI_SLAVE_SIG_INACT);
	debug("Activate CS, bus %d\n", spi_slave->slave.bus);
}

/**
 * Deactivate the CS by driving it HIGH
 *
 * @param slave	Pointer to spi_slave to which controller has to
 *		communicate with
 */
void spi_cs_deactivate(struct spi_slave *slave)
{
	struct exynos_spi_slave *spi_slave = to_exynos_spi(slave);

#ifdef CONFIG_MACH_ARTIK5
	artik5_ax88796c_cs_deactivate();
#endif
	setbits_le32(&spi_slave->regs->cs_reg, SPI_SLAVE_SIG_INACT);
	debug("Deactivate CS, bus %d\n", spi_slave->slave.bus);
}

static inline struct exynos_spi *get_spi_base(int dev_index)
{
	if (dev_index < 3)
		return (struct exynos_spi *)samsung_get_base_spi() + dev_index;
	else
		return (struct exynos_spi *)samsung_get_base_spi_isp() +
					(dev_index - 3);
}

/* Sadly there is no error return from this function */
void spi_init(void)
{
	int i;
	struct spi_bus *bus;

	for (i = 0; i < EXYNOS5_SPI_NUM_CONTROLLERS; i++) {
		bus = &spi_bus[i];
		bus->regs = get_spi_base(i);
		bus->periph_id = PERIPH_ID_SPI0 + i;

		/* Although Exynos5 supports upto 50Mhz speed,
		 * we are setting it to 10Mhz for safe side
		 */
		bus->frequency = 10000000;
		bus->inited = 1;
	}
}