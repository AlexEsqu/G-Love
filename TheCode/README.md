# GLove

## DEPENDENCIES
make\
avr-gcc\
avr-objcopy\
avrdude

## USAGE
Firmware is separate in two code with atm328p.c and atm2561.c depending on which mcu is chosen when compiling with avr-gcc (avr-gcc add a flag when compiling with -mmcu with a definition of your mcu, __AVR_ATmega328P__ or __AVR_ATmega2561__ for this project)

To install the firmware, go to the /TheCode folder first
```bash
cd TheCode
```

To flash atm2561, you can use `make mega`.
```bash
make mega
```

To flash atm328p, you need to `make bridge` before to redirect uart from atm2561 to atm328p, then you can use `make uno`, after, reinstall your code for atm2561 with `make mega`.
```bash
make bridge
make uno
make mega
```

To quicly test uart is responding, use `make uarttest` and you can interact your atm2561 with `screen`for example

You can also use `make bridge` first then `make uarttest` to interact with your atm328p 