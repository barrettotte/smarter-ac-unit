# Notes

AC Unit Model no: [LG LP1017WSR](https://www.lg.com/us/air-conditioners/lg-LP1017WSR-portable-air-conditioner)

## Remote

Remote ID: DG11J1-61

- Power on/off
- Temperature adjust: 60F-86F
- Modes: Cool,Dehumidify,Fan
- Going to ignore Swing, Sleep, Timer buttons

LCD displays temperature, current mode, and signal icon when pressing buttons

So its definitely holding state in some way.

Pressing power button turns off LCD display.

Taking battery out resets state

When AC unit running and remote batteries are taken out, AC unit retains state.
After replacing batteries and turning remote on, AC unit's state is overridden by reset remote's state.

When in range, turning off remote, turns off AC unit.
If remote is turned off out of range, then turned back on, AC unit remains unchanged (it doesn't toggle power).
This also holds true for the reverse states.

## IR

- receiver = opaque/black, transmitter = clear
- receiver is black to block visible light, but does not block entirely
- IR 30-40 kHz, most common is 38 kHz

## IR Sniffer

- Infrared Receiver VS1838B
- Arduino Nano
- SSD1306
- 10KΩ resistor
- 100Ω resistor

## References

- https://mylonasc.github.io/2019-02-09-reverseeng/
- http://www.righto.com/2009/09/arduino-universal-remote-record-and.html (Ken Shirriff)
- [Andreas Spiess - Arduino Guide to Infrared (IR) Communication](https://www.youtube.com/watch?v=gADIb1Xw8PE)
- NEC protocol - https://www.sbprojects.net/knowledge/ir/nec.php
