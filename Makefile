F_CPU    = 16000000UL
DEVICE   = atmega2560
BAUDRATE = 115200
PORT     = /dev/ttyACM1
NAME     = main

BIN      = ${NAME}.bin
HEX      = ${NAME}.hex

CC       = avr-gcc
OBJCOPY  = avr-objcopy
CFLAGS   = -Os -mmcu=${DEVICE} -DF_CPU=${F_CPU}

LIBDIR = lib

SRCS = Code_atmega2560/main.c $(LIBDIR)/uart.c $(LIBDIR)/i2c.c $(LIBDIR)/spi.c

OBJS = $(SRCS:.c=.o)

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

all: flash

hex: ${HEX}

${BIN}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${BIN}

${HEX}: ${BIN}
	${OBJCOPY} -O ihex ${BIN} ${HEX}

flash: hex
	avrdude -p ${DEVICE} -c wiring -P ${PORT} -b ${BAUDRATE} -D -v -U flash:w:${HEX}:i

screen: flash
	screen ${PORT} ${BAUDRATE}
	
clean:
	rm -f ${BIN} ${HEX} ${OBJS}

fclean: clean

re: fclean all

.PHONY: all clean fclean re hex flash

# avr-gcc
	# -Os : -O optimise -s size// necessaire pour <utils/delay.h> plustard
	# -mmcu : compile avec le .h compatible avec notre board
	# -DF_CPU : define F_CPU (8000000UL si pas define)

#objcopy
	# -O : output intel hex voulu (format de fichier destines aux microcontrolleurs, aux eeprom)

#avrdude
	# -p : specifie le board avec lequel on travail
	# -c : utilise le programmeur donne en argument
	# -b : 115200 baud rate //"la bande passante" pour transmettre des donnees entre des appareils
	# -P : port usb du pc // ls /dev/ttyUSB*
	# -U : "flash en write le fichier hexa vers l'input de la board"