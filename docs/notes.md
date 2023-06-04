# Notes

AC Unit Model no: [LG LP1017WSR](https://www.lg.com/us/air-conditioners/lg-LP1017WSR-portable-air-conditioner)

## Remote

Remote ID: DG11J1-61

- Power on/off
- Temperature adjust: 60F-86F (16C-30C)
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

IR 30-40 kHz, most common is 38 kHz

Used https://github.com/barrettotte/ir-sniffer to sniff all actions:

```txt
### Toggle through all operation modes

Protocol  : WHIRLPOOL_AC
Code      : 0x830603030000C0000000000000C0000600803800BE (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 3 (Dry), Temp: 16C, Fan: 3 (Low), Swing: Off, Light: On, Clock: 00:00, On Timer: Off, Off Timer: Off, Sleep: Off, Super: Off, Command: 6 (Mode)
-----------------------------------------------------------
Protocol  : WHIRLPOOL_AC
Code      : 0x830601040000C0000000000000C5000600803800BE (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 4 (Fan), Temp: 16C, Fan: 1 (High), Swing: Off, Light: On, Clock: 00:00, On Timer: Off, Off Timer: Off, Sleep: Off, Super: Off, Command: 6 (Mode)
-----------------------------------------------------------
Protocol  : WHIRLPOOL_AC
Code      : 0x830601A20000C000000000000063000600803800BE (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 2 (Cool), Temp: 26C, Fan: 1 (High), Swing: Off, Light: On, Clock: 00:00, On Timer: Off, Off Timer: Off, Sleep: Off, Super: Off, Command: 6 (Mode)
-----------------------------------------------------------

0x830603030000C0000000000000C0000600803800BE  3 (dry)
0x830601040000C0000000000000C5000600803800BE  4 (fan)
0x830601A20000C000000000000063000600803800BE  2 (cool)
```

```txt
### Flip through fan modes

Protocol  : WHIRLPOOL_AC
Code      : 0x830601A20000C000000000000063001100803800A9 (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 2 (Cool), Temp: 26C, Fan: 1 (High), Swing: Off, Light: On, Clock: 00:00, On Timer: Off, Off Timer: Off, Sleep: Off, Super: Off, Command: 17 (Fan)
-----------------------------------------------------------
Protocol  : WHIRLPOOL_AC
Code      : 0x830603A20000C000000000000061001100803800A9 (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 2 (Cool), Temp: 26C, Fan: 3 (Low), Swing: Off, Light: On, Clock: 00:00, On Timer: Off, Off Timer: Off, Sleep: Off, Super: Off, Command: 17 (Fan)
-----------------------------------------------------------

0x830601A20000C000000000000063001100803800A9  1 (High)
0x830603A20000C000000000000061001100803800A9  3 (Low)
```

| Command      | Button              |
| ------------ | ------------------- |
| 0            | Timer toggle        |
| 2            | Temperature up/down |
| 3            | Sleep               |
| 5            | Set timer           |
| 6            | Mode switch         |
| 7            | Swing               |
| 17           | Fan toggle          |
| 37           | Power on/off        |

### IR Code Decoding

```txt
Example IR Code (via ir-sniffer):

Protocol  : WHIRLPOOL_AC
Code      : 0x830601A20000C000000000000063001100803800A9 (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 2 (Cool), 
            Temp: 26C, Fan: 1 (High), Swing: Off, Light: On, 
            Clock: 00:00, On Timer: Off, Off Timer: Off, 
            Sleep: Off, Super: Off, Command: 17 (Fan)
```

Used [ir-sniffer](https://github.com/barrettotte/ir-sniffer) to manually gather all relevant IR codes
of button presses and states for my AC unit.
IR codes can be found in [ir-codes.txt](ir-codes.txt), these codes were used to create the table below.

Note: I'm only looking at cool mode...other modes may effect the values below.
Also, not figuring out sleep and timer...I don't use these at all.
Some of the mystery constants are probably part of this.

| Indices | Values                          | Notes                                                                     |
| ------- | ------------------------------- | ------------------------------------------------------------------------- |
| 0:3     | 8306                            | model...I guess?                                                          |
| 4:5     | 01,03                           | Fan: 1=High, 3=Low                                                        |
| 6       | 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F | Temperature: 60F-75F                                                      |
|         | 0,1,2,3,4,5,6,7,8,9,A           | Temperature: 76F-86F                                                      |
| 7       | 2                               | Mode: 2=Cool                                                              |
| 8:15    | 0000C000                        | ?                                                                         |
| 16      | 0,4                             | Swing: 0=off, 4=high                                                      |
| 17:25   | 000000000                       | ?                                                                         |
| 26      | C,D,E,F,8,9,A,B,4,5,6,7,0,1,2,3 | Temperature: 60F-75F                                                      |
|         | C,D,E,F,8,9,A,B,4,5,6           | Temperature: 76F-86F                                                      |
| 27      | 3,1                             | Fan: 3=High, 1=Low                                                        |
| 28:29   | 00                              | ?                                                                         |
| 30:31   | 02,25                           | command (temperature change, power toggle)                                |
| 32:33   | 00                              | ?                                                                         |
| 34:36   | 003                             | cool mode, high/low fan, Temperature: 60F-75F, [temp change or power on]  |
|         | 803                             | cool mode, high/low fan, Temperature: 76F-86F, [temp change or power on]  |
|         | 002                             | cool mode, high/low fan, Temperature: 60F-75F, power off                  |
|         | 802                             | cool mode, high/low fan, Temperature: 76F-86F, power off                  |
| 37:39   | 800                             | ?                                                                         |
| 40:41   | 3A                              | cool mode, high/low fan, Temperature: 60F-75F, temp change                |
|         | BA                              | cool mode, high/low fan, Temperature: 76F-86F, temp change                |
|         | 0D                              | cool mode, high/low fan, Temperature: 60F-75F, power off                  |
|         | 8D                              | cool mode, high/low fan, Temperature: 76F-86F, power off                  |
|         | 1D                              | cool mode, high/low fan, Temperature: 60F-75F, power on                   |
|         | 9D                              | cool mode, high/low fan, Temperature: 76F-86F, power on                   |
|         | 29                              | cool mode, high/low fan, Temperature: 60F-75F, fan toggle                 |
|         | A9                              | cool mode, high/low fan, Temperature: 76F-86F, fan toggle                 |

## References

- https://mylonasc.github.io/2019-02-09-reverseeng/
- http://www.righto.com/2009/09/arduino-universal-remote-record-and.html (Ken Shirriff)
- [Andreas Spiess - Arduino Guide to Infrared (IR) Communication](https://www.youtube.com/watch?v=gADIb1Xw8PE)
- NEC protocol - https://www.sbprojects.net/knowledge/ir/nec.php
