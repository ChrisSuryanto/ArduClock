# ArduClock
![Title](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/b192444e-e198-4f26-bc23-af38c74e7862)

Arduino 24 Hours Clock using MAX7219 LED matrix display with selectable Fonts.
Original code is from Youtube channel Mount Dynamics (https://www.youtube.com/watch?v=1rt031sYztw), with 12 hours clock. I modified the code to incorporate 24 hours, and added font selection feature.
I build this using spare Arduino Nano and MAX7219 matrix display which I have been using in my previous DIY. So I just need to buy RTC DS3231 and other small parts for this.
This clock is purely displaying clock (hour, minute), and no day / month / year will be displayed. Added font selection because I kind of curious how to properly arrange it to display, and what fonts are good or not good to use in this MAX7219.
Schematics is just standard like many other arduino clock as follows.

![24Hrs_Arduino_Max7219_Clock-02_Schematics](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/71fc351d-daf5-493b-aa3e-6c066500bdcc)

Diode on VCC line is 1N4007, or any other standard diode, just for protection, so 5V pin on Arduino will function as supply input only. LED matrix will be powered from external supply only, not from arduino's 5v pin, thanks to diode's 1 way current direction characteristics.

Cabling and connection is merely breadboard style, so it's quite a mess, but the Arduino PCB with socket and headers can be used for any other project. So, that's an option I choose.


![IMG_20240409_110618](https://github.com/ChrisSuryanto/ArduClock/assets/146957789/be1a3247-2dc7-4031-a17b-1dd51f810931)
