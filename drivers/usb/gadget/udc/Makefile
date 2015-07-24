#
# USB peripheral controller drivers
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libudc.o

COBJS-$(CONFIG_USB_DWC3_GADGET)	+= udc-core.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
