SRCS = main.c uart.c twi.c
SRCS_DIR = srcs/
INC_DIR = includes/

TARGET = prog
MCU = atmega328p

F_CPU = 16000000UL
BAUD = 115200

CFLAGS = -Wextra -Wall -Werror -I$(INC_DIR) -mmcu=$(MCU) -Os
CFLAGS += -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)

SRC_PATH = $(addprefix $(SRCS_DIR), $(SRCS))

all: hex flash

hex: $(SRC_PATH)
	@avr-gcc $(CFLAGS) $(SRC_PATH) -o $(TARGET).bin
	@avr-objcopy -O ihex -R .eeprom $(TARGET).bin $(TARGET).hex
	@echo "$(TARGET).hex generated!"

flash: hex
	@avrdude -p $(MCU) -c arduino -b 115200 -P /dev/ttyACM0 -U flash:w:$(TARGET).hex:i	
	@make --no-print-directory clean

clean:
	@rm -f $(TARGET).bin $(TARGET).hex
	@echo "Clean done!"