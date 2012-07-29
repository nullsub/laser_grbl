#  Grbl is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Grbl is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

PATH := /usr/share/gnurx_v10.03_elf-1/bin:/usr/share/gnurx_v10.03_elf-1/rx-elf/bin:$(PATH)

#100Mhz
#CLOCK      = 100000000
#what is the frequency??
CLOCK      =   96000000 
## Application

RTOSSRCDIR = freertos
RTOSINCDIR = $(RTOSSRCDIR)/include

DEV_SRC += eeprom.c
DEV_SRC += dev_misc.c 
DEV_SRC += serial.c 
DEV_SRC += stepper.c

#stuff
DEV_SRC += printf.c

## RTOS
DEV_SRC += $(RTOSSRCDIR)/list.c
DEV_SRC += $(RTOSSRCDIR)/queue.c
DEV_SRC += $(RTOSSRCDIR)/tasks.c

## RTOS Portable
DEV_SRC += $(RTOSSRCDIR)/portable/GCC/RX600/port.c
DEV_SRC += $(RTOSSRCDIR)/portable/MemMang/heap_2.c


TCHAIN_PREFIX = rx-elf-

DEV_INC  += hardware

DEV_INC  += hardware/ethernet/telnetd
DEV_INC  += freertos
DEV_INC  += $(RTOSINCDIR)
DEV_INC += freertos/portable/GCC/RX600 
#DEV_INC += hardware/network-apps
#DEV_INC  += hardware/wifi_driver
DEV_INC  += hardware/ethernet/FreeTCPIP 
DEV_INC  += hardware/ethernet
DEV_INC  += hardware/fatfs
DEV_INC  += hardware/ethernet/webserver
DEV_INC  += test


DEV_SRC += hardware/temp_board.c \
	hardware/adc.c \
	hardware/i2c.c \
	hardware/accelerometer.c \
	hardware/start.s \
	hardware/p5q.c \
	inthandler.c \
	hardware/hwsetup.c \
	hardware/vects.c \
	hardware/lcd.c \
	hardware/font_x5x7.c \
	hardware/spi.c \
	hardware/audio.c \
	hardware/ethernet/FreeTCPIP/psock.c \
	hardware/ethernet/FreeTCPIP/timer.c \
	hardware/ethernet/FreeTCPIP/uip.c \
	hardware/ethernet/FreeTCPIP/uip_arp.c \
	hardware/ethernet/EMAC.c \
	hardware/ethernet/phy.c \
	hardware/ethernet/uIP_Task.c \
	hardware/ethernet/memb.c \
	hardware/ethernet/telnetd/telnetd.c \
	hardware/ethernet/telnetd/shell.c \
	hardware/fatfs/diskio.c \
	hardware/fatfs/ff.c \
	test/sci2.c
#	hardware/network-apps/ftpd.c \
#	hardware/network-apps/memb.c \
#	hardware/network-apps/shell.c \
#	hardware/network-apps/telnetd.c \
#	hardware/network-apps/uIP_Task.c \

CFLAGS += \
	-I telnetd \
	-I hardware \
	-I FreeRTOS/include \
	-I FreeRTOS/portable/GCC/RX600 \
	-std=c99
#	-I ethernet/FreeTCPIP/sys \
#	-I ethernet \	
#	-I ethernet/FreeTCPIP \
#	-I ethernet/FreeTCPIP/net \
#	-I hardware/wifi_driver \

CFLAGS += -DF_CPU=$(CLOCK)
CFLAGS += -O1 

LDFLAGS = -nostartfiles -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref,--gc-sections
LDFLAGS = -lm -lc -lgcc -lc

LDSCRIPT = arch/rx62n/main.gsi
LINK_COMMAND  =	$(TCHAIN_PREFIX)gcc -nostartfiles $(ALLOBJ) -o $(OUTDIR)/$(TARGET).elf -T$(LDSCRIPT) $(LDFLAGS) $(CFLAGS) && \
		rx-elf-size $(OUTDIR)/$(TARGET).elf

FLASH_COMMAND  = arch/rx62n/flash-tools/rxusb -v $(OUTDIR)/$(TARGET).elf
