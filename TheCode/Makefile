F_CPU    = 8000000UL

DEVICE1   = atmega328p
BAUDRATE1 = 57600
PORT1     = /dev/cu.wchusbserial110
CODEDIR1  = srcs/main

DEVICE2   = atmega2561
BAUDRATE2 = 57600
PORT2     = /dev/cu.wchusbserial110
CODEDIR2  = srcs/main

DEVICE3   = atmega2560
BAUDRATE3 = 115200
PORT3     = /dev/ttyACM0
ALT_F_CPU    = 16000000UL

# speed of the firmware UART (BAUDRATE in includes/atm2561.h and atm328p.h),
# BAUDRATE1/2 above are only the bootloader speed used by avrdude
SERIAL_BAUD = 57600

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

CC       = avr-gcc -Wextra -Werror -Wall -MMD -MP
OBJCOPY  = avr-objcopy

OBJDIR   = objects
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
			srcs/imu/icm42688p.c \
			$(LIBDIR)/utils.c \
			$(LIBDIR)/watchdog.c

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
			srcs/display/screens/homescreen.c \
			srcs/comms/comms.c \
			$(LIBDIR)/utils.c \
			$(LIBDIR)/watchdog.c

SRC_BOOT1 = srcs/boot/bridge_atm2561.c \
			srcs/lib/watchdog.c

SRC_TEST = srcs/testing/uart_test.c \
			srcs/lib/uart.c \
			srcs/lib/i2c.c \
			srcs/lib/watchdog.c

OBJS1      = $(SRCS1:%.c=$(OBJDIR1)/%_uno.o)
OBJS2      = $(SRCS2:%.c=$(OBJDIR2)/%_mega.o)
OBJS3	   = $(SRCS2:%.c=$(OBJDIR3)/%_arduinomega.o)
OBJS_BOOT1 = $(SRC_BOOT1:%.c=$(OBJDIR2)/%_mega.o)
OBJS_TEST  = $(SRC_TEST:%.c=$(OBJDIR2)/%_mega.o)

DEPS       = $(OBJS1:.o=.d) $(OBJS2:.o=.d) $(OBJS3:.o=.d) $(OBJS_BOOT1:.o=.d) $(OBJS_TEST:.o=.d)
-include $(DEPS)

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

# flash the atm328P (through the bridge: make bridge first)
uno: ${BIN1}
	avrdude -p ${DEVICE1} -c urclock -P ${PORT1} -b ${BAUDRATE1} -x delay=1500 -v -U flash:w:${HEX1}:i

# flash the atm2561
mega: ${BIN2}
	avrdude -p ${DEVICE2} -c urclock -P ${PORT2} -b ${BAUDRATE2} -v -U flash:w:${HEX2}:i

megaarduino: ${OBJS3}
	${CC} -Os -mmcu=${DEVICE3} -DF_CPU=${ALT_F_CPU} ${OBJS3} -o ${BIN3}
	${OBJCOPY} -O ihex ${BIN3} ${HEX3}
	avrdude -p ${DEVICE3} -c wiring -P ${PORT3} -b ${BAUDRATE3} -D -v -U flash:w:${HEX3}:i

mega_screen:	megaarduino
				screen ${PORT3} ${BAUDRATE3}

# read the PC port
screen:
	screen ${PORT2} ${SERIAL_BAUD}

# read the PC port in hexadecimal (binary packets)
hexdump:
	(stty ${SERIAL_BAUD} raw -echo; hexdump -C) < ${PORT2}

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


fclean: clean

re: fclean all

.PHONY: all clean fclean re uno mega megaarduino mega_screen bridge uarttest screen hexdump
