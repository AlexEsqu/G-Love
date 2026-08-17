SRCS = main.c uart.c i2c.c imu.c utils.c touchscreen.c
SRCS_DIR = srcs/
INC_DIR = includes/

TARGET = prog
MCU = atmega328p
BIN = $(TARGET).bin
HEX = $(TARGET).hex

F_CPU = 16000000UL
BAUD = 115200

CFLAGS = -Wextra -Wall -Werror -I$(INC_DIR) -mmcu=$(MCU) -Os
CFLAGS += -DF_CPU=$(F_CPU) -DBAUDRATE=$(BAUD)

SRC_PATH = $(addprefix $(SRCS_DIR), $(SRCS))

all: $(HEX)

$(BIN): $(SRC_PATH)
	@avr-gcc $(CFLAGS) $(SRC_PATH) -o $(BIN)
	@echo "$(BIN) generated!"

$(HEX): $(BIN)
	@avr-objcopy -O ihex -R .eeprom $(BIN) $(HEX)
	@echo "$(HEX) generated!"

hex: $(HEX)

flash: $(HEX)
	@avrdude -p $(MCU) -c arduino -b 115200 -P /dev/ttyACM0 -U flash:w:$(HEX):i

display: all
	@printf "\n---------------------------------------------------\n"
	@echo "Displaying output from /dev/ttyACM0 at 115200 baud:"
	@stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb
	@cat /dev/ttyACM0

clean:
	@rm -f $(BIN) $(HEX)
	@echo "Clean done!"

re: clean all

.PHONY: all hex flash display clean re