# ArduClock
[<img src="https://i.ytimg.com/vi/1q083EUeKJs/maxresdefault.jpg" width="100%">](https://www.youtube.com/watch?v=1q083EUeKJs "Arduino 24 Hours Digital clock using MAX7219 LED matrix with Fonts selection")

Arduino 24 Hours Clock using MAX7219 LED matrix display with selectable Fonts.

Original code is from Youtube channel Mount Dynamics (https://www.youtube.com/watch?v=1rt031sYztw), with 12 hours clock. I modified the code to incorporate 24 hours, and added font selection feature.

I build this using spare Arduino Nano and MAX7219 matrix display which I have been using in my previous DIY. So I just need to buy RTC DS3231 and other small parts for this.

This clock is purely displaying clock (hour, minute), and no day / month / year will be displayed. Added font selection because I kind of curious how to properly arrange it to display, and what fonts are good or not good to use in this MAX7219.


# Schematics
![Arduino_24hrs_Clock_Fonts_Schematics](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/9d66605b-3abd-45f6-896e-ce897734b4d0)

Diode on VCC line is 1N4007, or any other standard diode, just for protection, so 5V pin on Arduino will function as supply input only. LED matrix and RTC DS3231 will be powered from external supply only, not from arduino's 5v pin, thanks to diode's 1 way current direction characteristics.


# Cabling and Connection
It's merely breadboard style, so it's quite a mess, but the Arduino PCB with socket and headers can be used for any other project. So, that's an option I choose.
![IMG_20240409_110618](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/be1a3247-2dc7-4031-a17b-1dd51f810931)


# Setup Manual
![ARDUINO_DIGITAL_CLOCK](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/3e80e28d-64dc-4a38-af6c-17c9dfaf4be5)
