/*
 * ASIX AX88796C Ethernet
 * (C) Copyright 2011
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

#include <common.h>
#include <miiphy.h>
#include <malloc.h>
#include <net.h>
#include <spi.h>
#include <asm/errno.h>
#include "ax88796c_spi.h"

#ifdef CONFIG_DRIVER_AX88796C_SPI

static unsigned char tx_packet[2048];
static unsigned char rx_packet[2048];

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_claim_bus
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_claim_bus(struct eth_device *dev)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;

	int rc = spi_claim_bus(ax_local->slave);

	if (rc)
		printf("Failed to claim SPI bus!\n");

	return rc;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_release_bus
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_release_bus(struct eth_device *dev)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;

	spi_release_bus(ax_local->slave);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: read_reg
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static unsigned short read_reg(struct eth_device *dev, unsigned char reg_addr)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;
	unsigned char opbuf[4];
	unsigned char rxbuf[2];

	opbuf[0] = OP_READ_GREG;	/* OP: read register */
	opbuf[1] = reg_addr;		/* Reg Addr */
	opbuf[2] = 0xFF;		/* Dummy */
	opbuf[3] = 0xFF;		/* Dummy */
	spi_xfer(ax_local->slave, 4 * 8, opbuf, NULL, SPI_XFER_BEGIN);

	rxbuf[0] = rxbuf[1] = 0;
	spi_xfer(ax_local->slave, 2 * 8, NULL, rxbuf, SPI_XFER_END);

	return le16_to_cpu(*((unsigned short *)rxbuf));
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: write_reg
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void write_reg(struct eth_device *dev, unsigned char reg_addr,
	unsigned short reg_value)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;
	unsigned char opbuf[2];
	unsigned char txbuf[2];

	opbuf[0] = OP_WRITE_GREG;	/* OP: write register */
	opbuf[1] = reg_addr;		/* Reg Addr */
	spi_xfer(ax_local->slave, 2 * 8, opbuf, NULL, SPI_XFER_BEGIN);

	txbuf[0] = (unsigned char)reg_value;
	txbuf[1] = (unsigned char)(reg_value >> 8);
	spi_xfer(ax_local->slave, 2 * 8, txbuf, NULL, SPI_XFER_END);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_read_fifo_pio
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_read_fifo_pio(struct eth_device *dev, unsigned char *rxbuf,
	int count)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;
	unsigned char opbuf[5];

	opbuf[0] = OP_READ_RXQ;		/* OP: read RXQ */
	opbuf[1] = 0xFF;		/* Dummy */
	opbuf[2] = 0xFF;		/* Dummy */
	opbuf[3] = 0xFF;		/* Dummy */
	opbuf[4] = 0xFF;		/* Dummy */
	spi_xfer(ax_local->slave, 5 * 8, opbuf, NULL, SPI_XFER_BEGIN);

	spi_xfer(ax_local->slave, count * 8, NULL, rxbuf, SPI_XFER_END);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_write_fifo_pio
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_write_fifo_pio(struct eth_device *dev, unsigned char *txbuf,
	int count)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;
	unsigned char opbuf[4];

	opbuf[0] = OP_WRITE_TXQ;	/* OP: write RXQ */
	opbuf[1] = 0xFF;		/* Dummy */
	opbuf[2] = 0xFF;		/* Dummy */
	opbuf[3] = 0xFF;		/* Dummy */
	spi_xfer(ax_local->slave, 4 * 8, opbuf, NULL, SPI_XFER_BEGIN);

	spi_xfer(ax_local->slave, count * 8, txbuf, NULL, SPI_XFER_END);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_set_enetaddr
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_set_enetaddr(struct eth_device *dev)
{
	write_reg(dev, P3_MACASR0, (dev->enetaddr[4] << 8) | dev->enetaddr[5]);
	write_reg(dev, P3_MACASR1, (dev->enetaddr[2] << 8) | dev->enetaddr[3]);
	write_reg(dev, P3_MACASR2, (dev->enetaddr[0] << 8) | dev->enetaddr[1]);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_get_enetaddr
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_get_enetaddr(struct eth_device *dev)
{
	unsigned short mac_addr;

	mac_addr = read_reg(dev, P3_MACASR0);
	dev->enetaddr[5] = mac_addr & 0x00FF;
	dev->enetaddr[4] = mac_addr >> 8;
	mac_addr = read_reg(dev, P3_MACASR1);
	dev->enetaddr[3] = mac_addr & 0x00FF;
	dev->enetaddr[2] = mac_addr >> 8;
	mac_addr = read_reg(dev, P3_MACASR2);
	dev->enetaddr[1] = mac_addr & 0x00FF;
	dev->enetaddr[0] = mac_addr >> 8;

	if ((dev->enetaddr[0] & 0x01) ||
	    ((dev->enetaddr[0] == 0) && (dev->enetaddr[1] == 0) &&
	    (dev->enetaddr[2] == 0) && (dev->enetaddr[3] == 0) &&
	    (dev->enetaddr[4] == 0) && (dev->enetaddr[5] == 0))) {
		dev->enetaddr[0] = 0x08;
		dev->enetaddr[1] = 0x00;
		dev->enetaddr[2] = 0x3e;
		dev->enetaddr[3] = 0x26;
		dev->enetaddr[4] = 0x0a;
		dev->enetaddr[5] = 0x5b;
	}
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_mdio_read
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_mdio_read(struct eth_device *dev, int phy_id, int loc)
{
	unsigned long time_out;
	unsigned short val;

	write_reg(dev, P2_MDIOCR, MDIOCR_RADDR(loc) |
		MDIOCR_FADDR(phy_id) | MDIOCR_READ);

	time_out = get_timer(0) + (CFG_HZ / 100);
	while ((read_reg(dev, P2_MDIOCR) & MDIOCR_VALID) == 0){
		if (get_timer(0) > time_out) {
			return -EIO;
		}
	}

	val = read_reg(dev, P2_MDIODR);

	return val;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_mdio_write
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_mdio_write(struct eth_device *dev, int phy_id, int loc,
	int value)
{
	unsigned long time_out;

	write_reg(dev, P2_MDIODR, value);
	write_reg(dev, P2_MDIOCR, MDIOCR_RADDR(loc) |
		MDIOCR_FADDR(phy_id) | MDIOCR_WRITE);

	time_out = get_timer(0) + (CFG_HZ / 100);
	while ((read_reg(dev, P2_MDIOCR) & MDIOCR_VALID) == 0){
		if (get_timer(0) > time_out) {
			return -EIO;
		}
	}

	return 0;
}

#ifdef CONFIG_AX88796C_DBG
/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_dump_regs
 * Purpose: Dump all MAC registers
 * -----------------------------------------------------------------------------
 */
static void ax88796c_dump_regs(struct eth_device *dev)
{
	unsigned char i, j;

	printf("\n");
	printf("       Page0   Page1   Page2   Page3   "
				"Page4   Page5   Page6   Page7\n");
	for (i = 0; i < 0x20; i += 2) {

		printf("0x%02x   ", i);
		for (j = 0; j < 8; j++) {
			printf("0x%04x  ",
				read_reg(dev, j * 0x20 + i));
		}
		printf("\n");
	}
	printf("\n");
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_dump_phy_regs
 * Purpose: Dump PHY register from MR0 to MR5
 * -----------------------------------------------------------------------------
 */
static void ax88796c_dump_phy_regs(struct eth_device *dev)
{
	int i;

	printf("Dump PHY registers:\n");
	for (i = 0; i < 6; i++) {
		printf("  MR%d = 0x%04x\n", i,
			ax88796c_mdio_read(dev, PHY_ID, i));
	}
	printf("\n");
}
#endif

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_reset
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_reset(struct eth_device *dev)
{
	unsigned long time_out;

	write_reg(dev, P0_PSR, PSR_RESET);
	write_reg(dev, P0_PSR, PSR_RESET_CLR);

	time_out = get_timer(0) + (CFG_HZ / 100);
	while (!(read_reg(dev, P0_PSR) & PSR_DEV_READY)){
		if (get_timer(0) > time_out) {
			return -ENXIO;
		}
	}

	return 0;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_bind
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_bind(struct eth_device *dev)
{
	ax88796c_get_enetaddr(dev);

	/* Setup LED mode */
	write_reg(dev, P2_LCR0, LCR_LED0_EN | LCR_LED0_DUPLEX |
		LCR_LED1_EN | LCR_LED1_100MODE);
	write_reg(dev, P2_LCR1, (read_reg(dev, P2_LCR1) & LCR_LED2_MASK) |
		LCR_LED2_EN | LCR_LED2_LINK);

	ax88796c_mdio_write(dev, PHY_ID, PHY_ANAR, MII_ADVERTISE);
	ax88796c_mdio_write(dev, PHY_ID, PHY_BMCR,
		PHY_BMCR_100MB | PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_halt
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_halt(struct eth_device *dev)
{
	if (!ax88796c_claim_bus(dev)){
		ax88796c_reset(dev);
		ax88796c_release_bus(dev);
	}
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_init
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_init(struct eth_device *dev, bd_t * bis)
{
	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;

	ax_local->seq_num = 0;

	if (ax88796c_claim_bus(dev))
		return -EBUSY;

	/* Enable RX Packet process */
	write_reg(dev, P1_RPPER, 0x01);

	/* Disable RX Stuffing Padding */
	write_reg(dev, P1_RXBSPCR, 0);

	/* Byte Swap, Enable TX RX bridge */
	write_reg(dev, P0_FER, read_reg(dev, P0_FER) |
		FER_IPALM | FER_BSWAP | FER_RXEN | FER_TXEN);

	/* Set MAC address */
	ax88796c_set_enetaddr(dev);

	/* Set Unicast + Broadcast */
	write_reg(dev, P2_RXCR, RXCR_AB);

	/* Set LED mode */
	write_reg(dev, P2_LCR0, LCR_LED0_EN | LCR_LED0_DUPLEX |
		LCR_LED1_EN | LCR_LED1_100MODE);
	write_reg(dev, P2_LCR1, (read_reg(dev, P2_LCR1) & LCR_LED2_MASK) |
		LCR_LED2_EN | LCR_LED2_LINK);

	/* Init PHY auto-polling */
	write_reg(dev, P2_POOLCR, (PHY_ID << 8) | POOLCR_POLL_EN |
		POOLCR_POLL_BMCR);

#ifdef CONFIG_AX88796C_DBG
	printf("Dump all MAC registers after initialization:\n");
	ax88796c_dump_regs(dev);
	ax88796c_dump_phy_regs(dev);
#endif

	ax88796c_release_bus(dev);

	return 0;
}


/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_nic_to_pc
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static unsigned char ax88796c_nic_to_pc(struct eth_device *dev)
{
	unsigned short rcphr, rxlen, burst_len, pkt_cnt;
	unsigned long time_out;

	/* check rx packet and total word count */
	write_reg(dev, P0_RTWCR, read_reg(dev, P0_RTWCR) | RTWCR_RX_LATCH);

	pkt_cnt = read_reg (dev, P0_RXBCR2) & RXBCR2_PKT_MASK;
	if (!pkt_cnt)
		return 0;

	rcphr = read_reg(dev, P0_RCPHR);

	/* Check the correctness of packet */
	if (rcphr & RX_HDR_ERROR) {
		write_reg(dev, P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return -EIO;
	}

	rxlen = (rcphr & RX_HDR_LEN);

	if ((rxlen < 60) || (rxlen > 1518)) {
		write_reg(dev, P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return -EIO;
	}

	/*
	 * Burst Word Count (header length + packet length has
	 * to be double word alignment)
	 */
	burst_len = ((rxlen + sizeof(struct rx_header) + 3) & 0xFFFC) >> 1;
	write_reg(dev, P0_RXBCR1, (burst_len | RXBCR1_RXB_START));

	time_out = get_timer(0) + (CFG_HZ / 100);
	while ((read_reg(dev, P0_RXBCR2) & RXBCR2_RXB_READY) == 0) {
		if (get_timer(0) > time_out) {
			write_reg(dev, P0_RXBCR1, RXBCR1_RXB_DISCARD);
			goto error_out;
		}
	}

	/* Receive RX Header, data and padding */
	ax88796c_read_fifo_pio(dev, (unsigned char*)&rx_packet[0], burst_len * 2);

	time_out = get_timer(0) + (CFG_HZ / 100);
	while ((read_reg(dev, P0_RXBCR2) & RXBCR2_RXB_IDLE) == 0) {
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}

	/* Pass the packet up to the protocol layers, skip the header */
	ax88796c_release_bus(dev);

	NetReceive(rx_packet + RX_HDR_SIZE, rxlen);

	if (ax88796c_claim_bus(dev))
		return -EBUSY;

	return 0;

error_out:
	write_reg(dev, P0_RXBCR2, RXBCR2_RXB_REINIT);

	return -EIO;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_recv
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_recv(struct eth_device *dev)
{
	unsigned short interrupts;

	if (ax88796c_claim_bus(dev))
		return -EBUSY;

	interrupts = read_reg(dev, P0_ISR);

	/* Acknowledge all interrupts */
	write_reg(dev, P0_ISR, interrupts);

	if (interrupts & ISR_RXPCT)
		ax88796c_nic_to_pc(dev);

	ax88796c_release_bus(dev);

	return 0;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_handle_tx_hdr
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static void ax88796c_handle_tx_hdr(struct tx_header * txhdr,
	unsigned short len, unsigned short seq_num)
{
	unsigned short len_bar = (~len & TX_HDR_SOP_PKTLENBAR);

	/* SOP Header */
	txhdr->sop.flags_pktlen = cpu_to_be16(len);
	txhdr->sop.seqnum_pktlenbar =
		cpu_to_be16(TX_HDR_SEQNUM (seq_num) | len_bar);

	/* Segment Header */
	txhdr->seg.flags_seqnum_seglen =
		cpu_to_be16(TX_HDR_SEG_FS | TX_HDR_SEG_LS | len);
	txhdr->seg.eo_so_seglenbar = cpu_to_be16(len_bar);

	/* EOP Header */
	txhdr->eop.seqnum_pktlen = cpu_to_be16(TX_HDR_SEQNUM (seq_num) | len);
	txhdr->eop.seqnumbar_pktlenbar =
		cpu_to_be16(TX_HDR_SEQNUM(~seq_num) | len_bar);
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_send
 * Purpose:
 * -----------------------------------------------------------------------------
 */
static int ax88796c_send(struct eth_device *dev, volatile void *packet,
	int length)
{
	unsigned long time_out;
	unsigned char align_count;
	unsigned char align_byte = 0xFF;
	int i;

	struct tx_header hdr;

	struct ax88796c_private *ax_local =
		(struct ax88796c_private *)dev->priv;

	ax88796c_handle_tx_hdr(&hdr, length, ax_local->seq_num);

	memcpy(tx_packet, &hdr, TX_HDR_SIZE);
	memcpy(&tx_packet[TX_HDR_SIZE], (unsigned char *)packet, length);

	/* Double-word alignment */
	align_count = ((length + 3 ) & 0x7FC) - length;
	if (align_count != 0){
		for (i = 0; i < align_count; i++){
			memcpy(&tx_packet[TX_HDR_SIZE + length + i], &align_byte, 1);
		}
	}

	memcpy(&tx_packet[TX_HDR_SIZE + length + align_count], &hdr.eop, EOP_SIZE);

	if (ax88796c_claim_bus(dev))
		return -EBUSY;

	write_reg(dev, P0_TSNR, TSNR_TXB_START | TSNR_PKT_CNT(1));

	/* Send packet */
	ax88796c_write_fifo_pio(dev, (unsigned char*)&tx_packet,
		TX_HDR_SIZE + length + EOP_SIZE + align_count);

	time_out = get_timer(0) + (CFG_HZ / 100);
	while ((read_reg(dev, P0_TSNR) & TSNR_TXB_IDLE) == 0){
		if (get_timer(0) > time_out) {
			goto error_out;
		}
	}

	if (read_reg(dev, P0_ISR) & ISR_TXERR)
		goto error_out;

	ax_local->seq_num++;
	ax_local->seq_num &= 0x1F;

	ax88796c_release_bus(dev);

	return 0;

error_out:
	write_reg(dev, P0_TSNR, TSNR_TXB_REINIT);

	ax_local->seq_num = 0;

	ax88796c_release_bus(dev);

	return -EBUSY;
}

/*
 * -----------------------------------------------------------------------------
 * Function Name: ax88796c_spi_initialize
 * Purpose:
 * -----------------------------------------------------------------------------
 */
int ax88796c_spi_initialize(int SPI_BUS, int CS, int MAX_HZ, int MODE)
{
	struct eth_device *dev;
	struct ax88796c_private *ax_local;

	dev = (struct eth_device *)malloc(sizeof *dev);
	if (NULL == dev)
		return -EFAULT;

	ax_local = (struct ax88796c_private *)malloc(sizeof *ax_local);
	if (NULL == ax_local){
		free(dev);
		return -EFAULT;
	}

	memset(dev, 0, sizeof *dev);
	memset(ax_local, 0, sizeof *ax_local);

	sprintf(dev->name, "AX88796C_SPI");
	dev->priv = ax_local;
	dev->init = ax88796c_init;
	dev->halt = ax88796c_halt;
	dev->send = ax88796c_send;
	dev->recv = ax88796c_recv;

	ax_local->slave = spi_setup_slave(SPI_BUS, CS, MAX_HZ, MODE);
	if (!ax_local->slave){
		printf("invalid SPI device!\n");
		free(dev);
		free(ax_local);
		return -ENXIO;
	}

	if (ax88796c_claim_bus(dev))
		return -EBUSY;

#ifdef CONFIG_AX88796C_DBG
	printf("Dump all MAC registers before initialization:\n");
	ax88796c_dump_regs(dev);
	ax88796c_dump_phy_regs(dev);
#endif

	if (!ax88796c_reset(dev)){
		ax88796c_bind(dev);
		eth_register(dev);
	}else {
		ax88796c_release_bus(dev);
		return -EIO;
	}

	ax88796c_release_bus(dev);

	return 0;
}

#endif /* CONFIG_DRIVER_AX88796C_SPI */
