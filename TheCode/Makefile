F_CPU    = 16000000UL

DEVICE1   = atmega328p
BAUDRATE1 = 115200
PORT1     = /dev/ttyACM0
CODEDIR1  = srcs/main

DEVICE2   = atmega2560
BAUDRATE2 = 115200
PORT2     = /dev/ttyACM1
CODEDIR2  = srcs/main

NAME     = main

BIN1     = ${NAME}_uno.bin
HEX1     = ${NAME}_uno.hex
BIN2     = ${NAME}_mega.bin
HEX2     = ${NAME}_mega.hex

CC       = avr-gcc
OBJCOPY  = avr-objcopy

LIBDIR   = srcs/lib

SRCS1    = $(CODEDIR1)/main.c \
			$(CODEDIR1)/atm328p.c \
        	$(LIBDIR)/adc.c \
        	$(LIBDIR)/i2c.c \
        	$(LIBDIR)/spi.c \
        	$(LIBDIR)/uart.c \
			srcs/muxSelect/mux.c \
			srcs/ledStrip/led.c

SRCS2    = $(CODEDIR2)/main.c \
			$(CODEDIR2)/atm2560.c \
        	$(LIBDIR)/adc.c \
        	$(LIBDIR)/i2c.c \
        	$(LIBDIR)/spi.c \
        	$(LIBDIR)/uart.c

OBJS1    = $(SRCS1:.c=_uno.o)
OBJS2    = $(SRCS2:.c=_mega.o)

# for UNO / atmega328P
%_uno.o: %.c
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -c $< -o $@

# for MEGA / atmega2560
%_mega.o: %.c
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -c $< -o $@

all: uno mega

unoarduino: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} -DPROTO=1 ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}
	avrdude -p ${DEVICE1} -c arduino -P ${PORT1} -b ${BAUDRATE1} -D -v -U flash:w:${HEX1}:i

uno: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}
	avrdude -p ${DEVICE1} -c arduino -P ${PORT1} -b ${BAUDRATE1} -D -v -U flash:w:${HEX1}:i

mega: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}
	avrdude -p ${DEVICE2} -c wiring -P ${PORT2} -b ${BAUDRATE2} -D -v -U flash:w:${HEX2}:i

megaarduino: ${OBJS2}
	${CC} -Os -mmcu=${DEVICE2} -DF_CPU=${F_CPU} -DPROTO=1 ${OBJS2} -o ${BIN2}
	${OBJCOPY} -O ihex ${BIN2} ${HEX2}
	avrdude -p ${DEVICE2} -c wiring -P ${PORT2} -b ${BAUDRATE2} -D -v -U flash:w:${HEX2}:i

clean:
	rm -f ${BIN1} ${HEX1} ${BIN2} ${HEX2} ${OBJS1} ${OBJS2}

fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega