F_CPU    = 8000000UL

DEVICE1   = atmega328p
BAUDRATE1 = 57600
PORT1     = /dev/cu.wchusbserial110
#PORT1     = /dev/cu.usbmodem101 /dev/cu.wchusbserial110
CODEDIR1  = srcs/main

DEVICE2   = atmega2561
BAUDRATE2 = 57600
PORT2     = /dev/cu.wchusbserial110
CODEDIR2  = srcs/main

DEVICE3   = atmega2560
BAUDRATE3 = 115200
PORT3     = /dev/ttyACM0
ALT_F_CPU    = 16000000UL

NAME     = main
NAME_BRIDGE = bridge
NAME_TEST 	= test

BIN1     = ${NAME}_uno.bin
HEX1     = ${NAME}_uno.hex
BIN2     = ${NAME}_mega.bin
HEX2     = ${NAME}_mega.hex
BIN3     = ${NAME}_arduinomega.bin
HEX3     = ${NAME}_arduinomega.hex

BIN_BOOT = ${NAME_BRIDGE}_mega.bin
HEX_BOOT = ${NAME_BRIDGE}_mega.hex

BIN_TEST = ${NAME_TEST}_mega.bin
HEX_TEST = ${NAME_TEST}_mega.hex

CC       = avr-gcc -Wextra -Werror -Wall -o2
OBJCOPY  = avr-objcopy

OBJDIR   = objets
OBJDIR1  = $(OBJDIR)/uno
OBJDIR2  = $(OBJDIR)/mega
OBJDIR3  = $(OBJDIR)/megaarduino

LIBDIR   = srcs/lib
INCDIR   = includes

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
			$(LIBDIR)/timer.c \
			$(LIBDIR)/readability.c \
			$(LIBDIR)/crc.c \
			srcs/display/gc9a01.c \
			srcs/display/display.c \
			srcs/display/tactile.c \
			srcs/display/screens/imuscreen.c \
			srcs/display/screens/colorscreen.c \
			srcs/display/screens/flexscreen.c \
			srcs/display/screens/comscreen.c \
			srcs/display/screens/homescreen.c

SRC_BOOT1 = srcs/boot/bridge_atm2561.c

SRC_TEST = srcs/testing/uart_test.c \
			srcs/lib/uart.c \
			srcs/lib/i2c.c

OBJS1      = $(SRCS1:%.c=$(OBJDIR1)/%_uno.o)
OBJS2      = $(SRCS2:%.c=$(OBJDIR2)/%_mega.o)
OBJS3	   = $(SRCS2:%.c=$(OBJDIR3)/%_arduinomega.o)
OBJS_BOOT1 = $(SRC_BOOT1:%.c=$(OBJDIR2)/%_mega.o)
OBJS_TEST  = $(SRC_TEST:%.c=$(OBJDIR2)/%_mega.o)

# for UNO / atmega328P
$(OBJDIR1)/%_uno.o: %.c
	@mkdir -p $(dir $@)
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -I${INCDIR} -c $< -o $@

# for atmega2561
$(OBJDIR2)/%_mega.o: %.c
	@mkdir -p $(dir $@)
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -I${INCDIR} -c $< -o $@

# for MEGA / atmega2560
$(OBJDIR3)/%_arduinomega.o: %.c
	@mkdir -p $(dir $@)
	${CC} -Os -mmcu=${DEVICE3} -DF_CPU=${ALT_F_CPU} -I${INCDIR} -c $< -o $@


all: ${BIN1} ${BIN2}

${BIN1}: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}

${BIN2}: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX2}:i

mega: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -DPROTO=1 ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -D -v -U flash:w:${HEX2}:i

megaarduino: ${OBJS3}
	${CC} -Os -mmcu=${DEVICE3} -DF_CPU=${ALT_F_CPU} ${OBJS3} -o ${BIN3}
	${OBJCOPY} -O ihex ${BIN3} ${HEX3}
	avrdude -p ${DEVICE3} -c wiring -P ${PORT3} -b ${BAUDRATE3} -D -v -U flash:w:${HEX3}:i

mega_screen:	megaarduino
				screen ${PORT3} ${BAUDRATE3}

bridge: ${OBJS_BOOT1}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS_BOOT1} -o ${BIN_BOOT}
	${OBJCOPY} -O ihex ${BIN_BOOT} ${HEX_BOOT}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX_BOOT}:i

uarttest: ${OBJS_TEST}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS_TEST} -o ${BIN_TEST}
	${OBJCOPY} -O ihex ${BIN_TEST} ${HEX_TEST}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX_TEST}:i

clean:
	rm -f ${BIN1} ${HEX1} ${BIN2} ${BIN3} ${HEX2} ${HEX3} ${HEX_BOOT} ${BIN_BOOT} ${OBJS1} ${OBJS2} ${OBJ3} ${OBJS_BOOT1}

fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega
