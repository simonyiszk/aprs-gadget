avrdude -c avrisp -p m328p -P /dev/ttyUSB0 -b 19200 -U lfuse:w:0xFF:m -U hfuse:w:0xD8:m -U efuse:w:0xFF:m
avrdude -c avrisp -p m328p -P /dev/ttyUSB0 -b 19200 -U flash:w:atmega328p_4k_16000000.hex
avrdude -c avrisp -p m328p -P /dev/ttyUSB0 -b 19200 -U lock:w:0xCF:m
