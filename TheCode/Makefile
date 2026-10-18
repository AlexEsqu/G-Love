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
CODEDIR2  = srcs/main
ALT_F_CPU    = 16000000UL

NAME     = main
NAME_BRIDGE   = bridge

BIN1     = ${NAME}_uno.bin
HEX1     = ${NAME}_uno.hex
BIN2     = ${NAME}_mega.bin
HEX2     = ${NAME}_mega.hex
BIN3     = ${NAME}_arduinomega.bin
HEX3     = ${NAME}_arduinomega.hex

BIN_BOOT = ${NAME_BRIDGE}_mega.bin
HEX_BOOT = ${NAME_BRIDGE}_mega.hex

CC       = avr-gcc -Wextra -Werror -Wall
OBJCOPY  = avr-objcopy

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

OBJS1    = $(SRCS1:.c=_uno.o)
OBJS2    = $(SRCS2:.c=_mega.o)
OBJS3    = $(SRCS2:.c=_arduinomega.o)
OBJS_BOOT1	 = $(SRC_BOOT1:.c=_mega.o)

# for UNO / atmega328P
%_uno.o: %.c
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -I${INCDIR} -c $< -o $@

# for atmega2561
%_mega.o: %.c
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -I${INCDIR} -c $< -o $@

# for MEGA / atmega2560
%_arduinomega.o: %.c
	${CC} -Os -mmcu=${DEVICE3} -DF_CPU=${ALT_F_CPU} -I${INCDIR} -c $< -o $@


all: uno mega

unoarduino: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -DPROTO=1 ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}
	avrdude -p ${DEVICE1} -c arduino -P ${PORT1} -b ${BAUDRATE1} -D -v -U flash:w:${HEX1}:i

uno: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}
	avrdude -p ${DEVICE1} -c urclock -P ${PORT1} -b ${BAUDRATE1} -x delay=1500 -v -U flash:w:${HEX1}:i

mega: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}
	avrdude -p ${DEVICE2} -c wiring -P ${PORT2} -b ${BAUDRATE2} -D -v -U flash:w:${HEX2}:i

megaarduino: ${OBJS3}
	${CC} -Os -mmcu=${DEVICE3} -DF_CPU=${ALT_F_CPU} ${OBJS3} -o ${BIN3}
	${OBJCOPY} -O ihex ${BIN3} ${HEX3}
	avrdude -p ${DEVICE3} -c wiring -P ${PORT3} -b ${BAUDRATE3} -D -v -U flash:w:${HEX3}:i

mega_screen:	megaarduino
				screen ${PORT3} ${BAUDRATE3}

bridge: ${OBJS_BOOT1}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS_BOOT1} -o ${BIN_BOOT}
	${OBJCOPY} -O ihex ${BIN_BOOT} ${HEX_BOOT}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -D -F -v -U flash:w:${HEX_BOOT}:i

clean:
	rm -f ${BIN1} ${HEX1} ${BIN2} ${HEX2} ${HEX_BOOT} ${BIN_BOOT} ${OBJS1} ${OBJS2} ${OBJS3} ${OBJS_BOOT1}

fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega
