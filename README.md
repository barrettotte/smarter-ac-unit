# smarter-ac-unit

Making my AC unit a little smarter with MQTT and Home Assistant

## Summary

TODO:

TODO: home assistant summary / setup

## IR Code Decoding

```txt
Example IR Code:

Protocol  : WHIRLPOOL_AC
Code      : 0x830601A20000C000000000000063001100803800A9 (168 Bits)
Mesg Desc.: Model: 2 (DG11J191), Power Toggle: Off, Mode: 2 (Cool), 
            Temp: 26C, Fan: 1 (High), Swing: Off, Light: On, 
            Clock: 00:00, On Timer: Off, Off Timer: Off, 
            Sleep: Off, Super: Off, Command: 17 (Fan)
```

Used [ir-sniffer](https://github.com/barrettotte/ir-sniffer) to manually gather all relevant IR codes
of button presses and states for AC unit. IR codes can be found in [ir-codes.txt](ir-codes.txt), these codes were
used to create the table below.

Note: I'm only looking at Cool mode...other modes may effect the values below.
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
| 17:24   | 00000000                        | ?                                                                         |
| 25      | C,D,E,F,8,9,A,B,4,5,6,7,0,1,2,3 | Temperature: 60F-75F                                                      |
|         | C,D,E,F,8,9,A,B,4,5,6           | Temperature: 76F-86F                                                      |
| 26      | 3,1                             | Fan: 3=High, 1=Low                                                        |
| 27:28   | 00                              | ?                                                                         |
| 29:30   | 02,25                           | command (temperature change, power toggle)                                |
| 31:32   | 00                              | ?                                                                         |
| 33:35   | 003                             | cool mode, high/low fan, Temperature: 60F-75F, [temp change or power on]  |
|         | 803                             | cool mode, high/low fan, Temperature: 76F-86F, [temp change or power on]  |
|         | 002                             | cool mode, high/low fan, Temperature: 60F-75F, power off                  |
|         | 802                             | cool mode, high/low fan, Temperature: 76F-86F, power off                  |
| 36:38   | 800                             | ?                                                                         |
| 39:40   | 3A                              | cool mode, high/low fan, Temperature: 60F-75F, temp change                |
|         | BA                              | cool mode, high/low fan, Temperature: 76F-86F, temp change                |
|         | 0D                              | cool mode, high/low fan, Temperature: 60F-75F, power off                  |
|         | 8D                              | cool mode, high/low fan, Temperature: 76F-86F, power off                  |
|         | 1D                              | cool mode, high/low fan, Temperature: 60F-75F, power on                   |
|         | 9D                              | cool mode, high/low fan, Temperature: 76F-86F, power on                   |
|         | 29                              | cool mode, high/low fan, Temperature: 60F-75F, fan toggle                 |
|         | A9                              | cool mode, high/low fan, Temperature: 76F-86F, fan toggle                 |

## References

- https://github.com/Arduino-IRremote/Arduino-IRremote
- https://www.home-assistant.io/integrations/climate.mqtt/
