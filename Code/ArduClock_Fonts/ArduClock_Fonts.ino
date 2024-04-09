/*
Original sketch name  :	ArduinoMatrixClock.ino
Author                : mylms.cz
Customize by          : Christian Suryanto
Created               :	08 April 2024 16:02
*/

#include <U8g2lib.h>    //https://github.com/olikraus/u8g2
#include <EEPROM.h>     //Built-in arduino IDE library
#include <Wire.h>       //built-in arduino IDE library


//MATRIX DISPLAY
byte devices = 4;
U8G2_MAX7219_32X8_F_4W_SW_SPI display(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);

typedef struct
{
  uint8_t *fontName;
  int8_t hBigOffset;
  int8_t hSmallOffset;
  int8_t mBigOffset;
  int8_t hSignOffset;
  int8_t dotOffset;
  int8_t startOffset;
} classFont;

const classFont curFont[] =
{
  {u8g2_font_squeezed_b7_tr, 2, 0, 2, 0, 0, 0}, 
  {u8g2_font_nokiafc22_tr, 2, 0, 2, 0, 0, -3}, 
  {u8g2_font_haxrcorp4089_tr, 2, 0, 2, 0, 0, -2}, 
  {u8g2_font_resoledbold_tr, 1, 0, 1, 0, 0, -2}, 
  {u8g2_font_BitTypeWriter_tr, 2, 0, 2, 0, 1, -4}, 
  {u8g2_font_NokiaSmallBold_tr, 2, 0, 2, 0, 0, -3}, 
  //{u8g2_font_victoriabold8_8r, 0, -1, 2, -1, 0, 0},
  {u8g2_font_timB08_tr, 2, 0, 2, -3, 0, -4},
};

uint8_t fontIndex = 0;

//RTC DS3231
#define DS3231_I2C_ADDRESS 0x68 //address of DS3231 module
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; //global variables

//TIMMING
unsigned long presentTime;
unsigned long displayTime;	//drawing

//IO
#define BTN1 2
#define BTN2 3
bool lastInput1; //last state of #1 input 
bool lastInput2; //last state of #2 input 
bool presentInput1; //actual state of input #1
bool presentInput2; //actual state of input #2

//SYSTEM STATE
byte systemState;

//chars
uint8_t offsetChar  = 2;

byte bright = 0;
bool showDots;

void(* resetFunc) (void) = 0;

void setup()
{
	//COMMUNICATION
	Wire.begin(); //start I2C communication
	Serial.begin(9600);

	//IO
	pinMode(BTN1, INPUT_PULLUP);
	pinMode(BTN2, INPUT_PULLUP);

	bright = EEPROM.read(0);	//load light intensity from EEPROM
  fontIndex = EEPROM.read(1); //load font index from EEPROM
	delay(10);

	//SET DISPLAY
  display.begin();
  display.setContrast(bright*17);
  display.setFont(curFont[fontIndex].fontName);

	//INIT TIME SETTING
	//SetRtc(0, 38, 14, 2, 8, 4, 24);	//sec, min, hour, dayOfWeek, dayOfMonth, month, year

	Intro();	//show intro
}


void loop()
{
	//store input to var
	presentInput1 = digitalRead(BTN1); 
	presentInput2 = digitalRead(BTN2);

	switch (systemState)
	{
	case 0:
		//SHOW ACTUAL TIME
		presentTime = millis();

		//"multi tasking"
		if (presentTime - displayTime >= 500)
		{
			displayTime = presentTime;

			GetRtc();		//get actual time
			WriteTime();	//write actual time to matrix display
		}

		//if (!presentInput1 && !presentInput2) //Both Button A & Button B pressed, enter setup menu
    if (!presentInput1 && presentInput2) //Button A pressed, enter setup menu
		{
		  delay(60);
			systemState = 1;
			//go to "pre"menu
		}

    if (presentInput1 && !presentInput2)  //Button B pressed to RESET
    {
      delay(60);
      //RESET Arduino
      resetFunc();
    }
		break;

	case 1:
		if (presentInput1 && presentInput2)
		{
    delay(60);
			systemState = 2; //Go to menu
		}
		break;

	case 2:
		//menu 1
		//set hour
		WriteTime();
		if (presentInput1 != lastInput1)
		{
			//change detected BTN1
      delay(60);
			if (presentInput1)
			{
        delay(60);
				//rising edge detected
				systemState = 3;
			}
		}

		if (presentInput2 != lastInput2)
		{
			//change detected BTN2
      delay(60);
			if (presentInput2)
			{
				//rising edge detected
				//add hour
				hour++;
				if (hour > 23)
				{
					hour = 0;
				}
      //Serial.println(hour);
			}
		}
		break;

	case 3:
		//menu 2
		//set minute
		WriteTime();
		if (presentInput1 != lastInput1)
		{
      delay(60);
			//change detected BTN1
			if (presentInput1)
			{
				//rising edge detected
				systemState = 4;

        display.clearBuffer();
        display.setCursor((0*8)+offsetChar, 7);
        display.print("B");
        drawDots(true);

        display.setCursor((2*8)+curFont[fontIndex].mBigOffset, 7);
        display.print((bright / 10));
        display.setCursor((3*8), 7);             display.print((bright % 10));
        display.sendBuffer();
			}
		}

		if (presentInput2 != lastInput2)
		{
      delay(60);
			//change detected BTN2
			if (presentInput2)
			{
				//rising edge detected
				//add hour
				minute++;
				if (minute > 59)
				{
					minute = 0;
				}
			}
		}
		break;

	case 4:
		//menu 3
		//set brightnes
		if (presentInput1 != lastInput1)
		{
			//change detected BTN1
      delay(60);
			if (presentInput1)
			{
				//rising edge detected
				systemState = 5;

        display.clearBuffer();
        display.setCursor((0*8)+offsetChar, 7);
        display.print("F");
        drawDots(true);

        display.setCursor((2*8)+curFont[fontIndex].mBigOffset, 7);
        display.print(((fontIndex+1) / 10));
        display.setCursor((3*8), 7);             display.print(((fontIndex+1) % 10));
        display.sendBuffer();
			}
		}

		if (presentInput2 != lastInput2)
		{
			//change detected BTN2
      delay(60);
			if (presentInput2)
			{
				//rising edge detected
				//add brightness
				bright++;
				if (bright > 15)
				{
					bright = 0;			
				}

        display.clearBuffer();
        display.setCursor((0*8)+offsetChar, 7);
        display.print("B");
        drawDots(true);

        display.setCursor((2*8)+curFont[fontIndex].mBigOffset, 7);
        display.print((bright / 10));
        display.setCursor((3*8), 7);             display.print((bright % 10));
        display.sendBuffer();

        display.setContrast(bright*17);
			}
		}
		break;

  case 5:
    //menu 4
    //set Font
    if (presentInput1 != lastInput1)
    {
      //change detected BTN1
      delay(60);
      if (presentInput1)
      {
        //rising edge detected
        systemState = 6;

       display.clearBuffer();
       display.setCursor(12+curFont[fontIndex].startOffset, 7);
       display.print("OK");
       display.sendBuffer();
      }
    }

    if (presentInput2 != lastInput2)
    {
      //change detected BTN2
      delay(60);
      if (presentInput2)
      {
        //rising edge detected
        //add fontIndex
        fontIndex++;
        if (fontIndex > (sizeof(curFont)/8)-1)
        {
          fontIndex = 0;
        }

        display.clearBuffer();
        display.setFont(curFont[fontIndex].fontName);
        display.setCursor((0*8)+offsetChar, 7);
        display.print("F");
        drawDots(true);

        display.setCursor((2*8)+curFont[fontIndex].mBigOffset, 7);
        display.print(((fontIndex+1) / 10));
        display.setCursor((3*8), 7); 
        display.print(((fontIndex+1) % 10));
        display.sendBuffer();
      }
    }
    break;

  case 6:
    //menu 5
    //if (presentInput1 && !presentInput2)  //Button B pressed to RESET
    if (presentInput2 != lastInput2)  //Button B pressed to RESET
    {
      display.clearBuffer();
      display.setCursor(3+curFont[fontIndex].startOffset, 7);
      display.print("ABORT");
      display.sendBuffer();
      delay(1000);
      //RESET Arduino
      resetFunc();
    }
    
    if (presentInput1 != lastInput1)
    {
      //change detected BTN1
      delay(60);
      if (presentInput1)
      {
        //rising edge detected
        SetRtc(0, minute, hour, dayOfWeek, dayOfMonth, month, year);  //set time and zero second
        EEPROM.update(0, bright);  //store actual light intensity to addr 0
        EEPROM.update(1, fontIndex);  //store actual font index to addr 1
        systemState = 0;
      }
    }
    break;

	
	}

	lastInput1 = presentInput1; //save current state to last state
	lastInput2 = presentInput2; //save current state to last state

	SerialComm();	//read data from PC
}

void drawDots(bool dotState)
{
  if (dotState)
  {
    //display.drawPixel(14,1);
    display.drawPixel(15+curFont[fontIndex].dotOffset,1);
    //display.drawPixel(14,2);
    display.drawPixel(15+curFont[fontIndex].dotOffset,2);
    //display.drawPixel(14,4);
    display.drawPixel(15+curFont[fontIndex].dotOffset,4);
    //display.drawPixel(14,5);
    display.drawPixel(15+curFont[fontIndex].dotOffset,5);
  }
  else
  {
    display.drawStr(14+curFont[fontIndex].dotOffset, 7, " ");
  }
}


void WriteTime()
{
  display.clearBuffer();
	
	//write time to matrix display
	if (systemState == 0 || systemState == 2)
	{
    uint8_t thisHour = hour%24 == 0? 0 : hour%24;
    display.setCursor((0*8)+curFont[fontIndex].hBigOffset, 7);
    display.print((thisHour / 10));
    display.setCursor((1*8)+curFont[fontIndex].hSmallOffset, 7); display.print((thisHour % 10));
	}
     
  if (systemState == 0 || systemState == 3)
  {
    display.setCursor((2*8)+curFont[fontIndex].mBigOffset, 7);
    display.print((minute / 10));
    display.setCursor((3*8), 7); display.print((minute % 10));
	}
	
	//blinking dots on display
  drawDots(showDots);

  if (systemState == 2)
  {
    display.drawStr((3*8)+curFont[fontIndex].hSignOffset, 7, "H");
    display.drawStr((2*8)+offsetChar, 7, " "); 
  }

  if (systemState == 3)
  {
    display.drawStr((0*8)+offsetChar, 7, "M");
    display.drawStr((1*8), 7, " ");
  }

  display.sendBuffer();
  if (systemState == 0) showDots = !showDots; else showDots = true;
}

void Intro()
{
  display.clearBuffer();          // clear the internal memory
  display.drawStr(24, 7, "-");
  display.drawStr(16+curFont[fontIndex].mBigOffset, 7, "-");
  
  display.drawStr(8+curFont[fontIndex].hSmallOffset, 7, "-");
  display.drawStr(0+curFont[fontIndex].hBigOffset, 7, "-");
  drawDots(true);
  display.sendBuffer();
	delay(1000);
}


//Set RTC
void SetRtc(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); //set 0 to first register

	Wire.write(decToBcd(second)); //set second
	Wire.write(decToBcd(minute)); //set minute 
	Wire.write(decToBcd(hour)); //set hour
	Wire.write(decToBcd(dayOfWeek)); //set day of week (1=su, 2=mo, 3=tu) 
	Wire.write(decToBcd(dayOfMonth)); //set day of month
	Wire.write(decToBcd(month)); //set month
	Wire.write(decToBcd(year)); //set year
	Wire.endTransmission();
}

//read RTC
void GetRtc()
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); //write "0"
	Wire.endTransmission();

	Wire.requestFrom(DS3231_I2C_ADDRESS, 7);	//request - 7 bytes from RTC
	second = bcdToDec(Wire.read() & 0x7f);
	minute = bcdToDec(Wire.read());
	hour = bcdToDec(Wire.read() & 0x3f);
	dayOfWeek = bcdToDec(Wire.read());
	dayOfMonth = bcdToDec(Wire.read());
	month = bcdToDec(Wire.read());
	year = bcdToDec(Wire.read());
}

//conversion Dec to BCD 
byte decToBcd(byte val)
{
	return((val / 10 * 16) + (val % 10));
}

//conversion BCD to Dec 
byte bcdToDec(byte val)
{
	return((val / 16 * 10) + (val % 16));
}

//serial communication with PC
//set time via PC
void SerialComm()
{
	//first letter - type of data
	//second letter - data (space = 0, ! = 1, etc. see ASCII table)

	if (Serial.available() > 0)
	{
		byte receivedCommand;
		receivedCommand = Serial.read();	//read first letter

		if (receivedCommand < 90)
		{
			//received data is less than 90 (letter Z)
			delay(10);	//wait for second letter

			byte receivedData;
      receivedData = Serial.read();
      Serial.println(receivedData);
			receivedData -= 32;	//shift - 32 -> 32 = space
      Serial.println(receivedData);

			switch (receivedCommand)
			{
			case 65:
				//year 65 = A
				year = receivedData;
				//lc.setLed(3, 7, 0, true);	//show setting dot
				break;
			case 66:
				//month 66 = B
				month = receivedData;
				//lc.setLed(3, 7, 1, true);	//show setting dot
				break;
			case 67:
				//dayOfMonth 67 = C
				dayOfMonth = receivedData;
				//lc.setLed(3, 7, 2, true);	//show setting dot
				break;
			case 68:
				//hour 68 = D
				hour = receivedData;
				//lc.setLed(3, 7, 3, true);	//show setting dot
				break;
			case 69:
				//minute 69 = E
				minute = receivedData;
				//lc.setLed(3, 7, 4, true);	//show setting dot
				break;
			case 70:
				//second 70 = F
				second = receivedData;
				//lc.setLed(3, 7, 5, true);	//show setting dot
				break;
			case 71:
				//dayofWeek 71 = G
				dayOfWeek = receivedData;
				//lc.setLed(3, 7, 6, true);	//show setting dot
				break;
			}
			SetRtc(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
		}

		//clearing serial buffer
		Serial.flush();
	}
}
