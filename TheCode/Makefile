F_CPU    = 8000000UL

DEVICE1   = atmega328p
BAUDRATE1 = 57600
PORT1     = /dev/cu.wchusbserial110
#PORT1     = /dev/cu.usbmodem101
CODEDIR1  = srcs/main

DEVICE2   = atmega2561
BAUDRATE2 = 57600
PORT2     = /dev/cu.wchusbserial110
CODEDIR2  = srcs/main

NAME     = main
NAME_BRIDGE = bridge
NAME_TEST 	= test

BIN1     = ${NAME}_uno.bin
HEX1     = ${NAME}_uno.hex
BIN2     = ${NAME}_mega.bin
HEX2     = ${NAME}_mega.hex

BIN_BOOT = ${NAME_BRIDGE}_mega.bin
HEX_BOOT = ${NAME_BRIDGE}_mega.hex

BIN_TEST = ${NAME_TEST}_mega.bin
HEX_TEST = ${NAME_TEST}_mega.hex

CC       = avr-gcc
OBJCOPY  = avr-objcopy

OBJDIR   = objets
OBJDIR1  = $(OBJDIR)/uno
OBJDIR2  = $(OBJDIR)/mega

LIBDIR   = srcs/lib

SRCS1    = $(CODEDIR1)/main.c \
			$(CODEDIR1)/atm328p.c \
        	$(LIBDIR)/adc.c \
        	$(LIBDIR)/i2c.c \
        	$(LIBDIR)/spi.c \
        	$(LIBDIR)/uart.c \
			$(LIBDIR)/crc.c \
			srcs/muxSelect/mux.c \
			srcs/ledStrip/led.c \
			srcs/ledStrip/led_utils.c \
			srcs/imu/icm42688p.c

SRCS2    = $(CODEDIR2)/main.c \
			$(CODEDIR2)/atm2560.c \
        	$(LIBDIR)/adc.c \
        	$(LIBDIR)/i2c.c \
        	$(LIBDIR)/spi.c \
        	$(LIBDIR)/uart.c  \
			$(LIBDIR)/crc.c 

SRC_BOOT1 = srcs/boot/bridge_atm2561.c

SRC_TEST = srcs/testing/uart_test.c \
			srcs/lib/uart.c \
			srcs/lib/i2c.c

OBJS1      = $(SRCS1:%.c=$(OBJDIR1)/%_uno.o)
OBJS2      = $(SRCS2:%.c=$(OBJDIR2)/%_mega.o)
OBJS_BOOT1 = $(SRC_BOOT1:%.c=$(OBJDIR2)/%_mega.o)
OBJS_TEST  = $(SRC_TEST:%.c=$(OBJDIR2)/%_mega.o)

# for UNO / atmega328P
$(OBJDIR1)/%_uno.o: %.c
	@mkdir -p $(dir $@)
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -c $< -o $@

# for MEGA / atmega2560
$(OBJDIR2)/%_mega.o: %.c
	@mkdir -p $(dir $@)
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -c $< -o $@

all: ${BIN1} ${BIN2}

${BIN1}: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}

${BIN2}: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}

uno: all ${HEX1}
	avrdude -p ${DEVICE1} -c urclock -P ${PORT1} -b ${BAUDRATE1} -x delay=1500 -v -U flash:w:${HEX1}:i

mega: all ${HEX2}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX2}:i

both: uno mega

screen:
	screen ${PORT1} ${BAUDRATE1}

screen-mega:
	screen ${PORT2} ${BAUDRATE2}

screen-uno: screen

bridge: ${OBJS_BOOT1}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS_BOOT1} -o ${BIN_BOOT}
	${OBJCOPY} -O ihex ${BIN_BOOT} ${HEX_BOOT}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX_BOOT}:i

uarttest: ${OBJS_TEST}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS_TEST} -o ${BIN_TEST}
	${OBJCOPY} -O ihex ${BIN_TEST} ${HEX_TEST}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX_TEST}:i

clean:
	rm -f ${BIN1} ${HEX1} ${OBJS1} \
			${BIN2} ${HEX2} ${OBJS2} \
			${HEX_BOOT} ${BIN_BOOT} ${OBJS_BOOT1} \
			${HEX_TEST} ${BIN_TEST} ${OBJS_TEST}
	rm -rf $(OBJDIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega both screen screen-uno screen-mega bridge uarttest
