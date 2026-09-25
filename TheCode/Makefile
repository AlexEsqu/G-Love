F_CPU    = 8000000UL

DEVICE1   = m328p
BAUDRATE1 = 19200
PORT1     ?= /dev/cu.usbmodem101
CODEDIR1  = srcs/main

DEVICE2   = atmega2560
BAUDRATE2 = 115200
PORT2     ?= /dev/ttyUSB1
CODEDIR2  = srcs/main

# Configuration Arduino as ISP
ISP_PORT     ?= /dev/ttyACM0
ISP_BAUD     ?= 19200


NAME     = main

BIN1     = ${NAME}_uno.bin
HEX1     = ${NAME}_uno.hex
BIN2     = ${NAME}_mega.bin
HEX2     = ${NAME}_mega.hex

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
			srcs/ledStrip/led_utils.c

SRCS2    = $(CODEDIR2)/main.c \
			$(CODEDIR2)/atm2560.c \
        	$(LIBDIR)/adc.c \
        	$(LIBDIR)/i2c.c \
        	$(LIBDIR)/spi.c \
        	$(LIBDIR)/uart.c

OBJS1    = $(SRCS1:%.c=$(OBJDIR1)/%_uno.o)
OBJS2    = $(SRCS2:%.c=$(OBJDIR2)/%_mega.o)

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
	avrdude -c stk500v1 -p ${DEVICE1} -P ${PORT1} -b ${BAUDRATE1} -D -v -U flash:w:${HEX1}:i

mega: all ${HEX2}
	avrdude -p ${DEVICE2} -c wiring -P ${PORT2} -b ${BAUDRATE2} -D -v -U flash:w:${HEX2}:i

unoisp: ${OBJS1}
	${CC} -Os -mmcu=${DEVICE1} -DF_CPU=${F_CPU} ${OBJS1} -o ${BIN1}
	${OBJCOPY} -O ihex ${BIN1} ${HEX1}
	avrdude -p ${DEVICE1} -c stk500v1 -P ${ISP_PORT} -b ${ISP_BAUD} -v -U flash:w:${HEX1}:i


both: uno mega

screen:
	screen ${PORT1} ${BAUDRATE1}

screen-mega:
	screen ${PORT2} ${BAUDRATE2}

screen-uno: screen

clean:
	rm -f ${BIN1} ${HEX1} ${BIN2} ${HEX2} ${OBJS1} ${OBJS2}
	rm -rf $(OBJDIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega both screen screen-uno screen-mega