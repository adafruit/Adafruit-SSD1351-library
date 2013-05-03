/*************************************************** 
  This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/684

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.c"
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

/********************************** low level pin interface */

inline void Adafruit_SSD1351::spiwrite(uint8_t c) {
    
    //Serial.println(c, HEX);
    
    if (!_sid) {
        SPI.transfer(c);
        return;
    }
    
    volatile uint8_t *sclkportreg = portOutputRegister(sclkport);
    volatile uint8_t *sidportreg = portOutputRegister(sidport);
    
    int8_t i;
    
    *sclkportreg |= sclkpin;
    
    for (i=7; i>=0; i--) {
        *sclkportreg &= ~sclkpin;
        //SCLK_PORT &= ~_BV(SCLK);
        
        if (c & _BV(i)) {
            *sidportreg |= sidpin;
            //digitalWrite(_sid, HIGH);
            //SID_PORT |= _BV(SID);
        } else {
            *sidportreg &= ~sidpin;
            //digitalWrite(_sid, LOW);
            //SID_PORT &= ~_BV(SID);
        }
        
        *sclkportreg |= sclkpin;
        //SCLK_PORT |= _BV(SCLK);
    }
}


void Adafruit_SSD1351::writeCommand(uint8_t c) {
    *portOutputRegister(rsport) &= ~ rspin;
    //digitalWrite(_rs, LOW);
    
    *portOutputRegister(csport) &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("C ");
    spiwrite(c);
    
    *portOutputRegister(csport) |= cspin;
    //digitalWrite(_cs, HIGH);
}


void Adafruit_SSD1351::writeData(uint8_t c) {
    *portOutputRegister(rsport) |= rspin;
    //digitalWrite(_rs, HIGH);
    
    *portOutputRegister(csport) &= ~ cspin;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("D ");
    spiwrite(c);
    
    *portOutputRegister(csport) |= cspin;
    //digitalWrite(_cs, HIGH);
} 

/***********************************/

void Adafruit_SSD1351::goTo(int x, int y) {
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(SSD1351WIDTH-1);

  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(SSD1351HEIGHT-1);

  writeCommand(SSD1351_CMD_WRITERAM);  
}

uint16_t Adafruit_SSD1351::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}

void Adafruit_SSD1351::fillScreen(uint16_t fillcolor) {
  fillRect(0, 0, 128, 128, fillcolor);
}

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void Adafruit_SSD1351::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) 
{	

  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
	return;

  // Y bounds check
  if (y+h > SSD1351HEIGHT)
  {
    h = SSD1351HEIGHT - y - 1;
  }

  // X bounds check
  if (x+w > SSD1351WIDTH)
  {
    w = SSD1351WIDTH - x - 1;
  }
  
  Serial.print(x); Serial.print(", ");
  Serial.print(y); Serial.print(", ");
  Serial.print(w); Serial.print(", ");
  Serial.print(h); Serial.println(", ");


  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x+w-1);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y+h-1);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < w*h; i++) {
    writeData(fillcolor >> 8);
    writeData(fillcolor);
  }
}

void Adafruit_SSD1351::drawFastVLine(int16_t x, int16_t y, 
				 int16_t h, uint16_t color) {
  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
	return;

  // X bounds check
  if (y+h > SSD1351WIDTH)
  {
    h = SSD1351WIDTH - y - 1;
  }

  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y+h-1);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < h; i++) {
    writeData(color >> 8);
    writeData(color);
  }
}



void Adafruit_SSD1351::drawFastHLine(int16_t x, int16_t y, 
				 int16_t w, uint16_t color) {
  // Bounds check
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
	return;

  // X bounds check
  if (x+w > SSD1351WIDTH)
  {
    w = SSD1351WIDTH - x - 1;
  }

  // set location
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(x+w-1);
  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(y);
  // fill!
  writeCommand(SSD1351_CMD_WRITERAM);  

  for (uint16_t i=0; i < w; i++) {
    writeData(color >> 8);
    writeData(color);
  }
}



void Adafruit_SSD1351::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT)) return;
  if ((x < 0) || (y < 0)) return;

  goTo(x, y);
  
  // setup for data
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;
  
  spiwrite(color >> 8);    
  spiwrite(color);
  
  *portOutputRegister(csport) |= cspin;  
}

void Adafruit_SSD1351::begin(void) {
    // set pin directions
    pinMode(_rs, OUTPUT);
    
    if (_sclk) {
        pinMode(_sclk, OUTPUT);
        sclkport = digitalPinToPort(_sclk);
        sclkpin = digitalPinToBitMask(_sclk);
        
        pinMode(_sid, OUTPUT);
        sidport = digitalPinToPort(_sid);
        sidpin = digitalPinToBitMask(_sid);
    } else {
        // using the hardware SPI
        SPI.begin();
        SPI.setDataMode(SPI_MODE3);
    }
	
    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    cspin = digitalPinToBitMask(_cs);
    csport = digitalPinToPort(_cs);
    
    rspin = digitalPinToBitMask(_rs);
    rsport = digitalPinToPort(_rs);
    
    if (_rst) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(500);
        digitalWrite(_rst, LOW);
        delay(500);
        digitalWrite(_rst, HIGH);
        delay(500);
    }

    // Initialization Sequence
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0x12);  
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0xB1);

    writeCommand(SSD1351_CMD_DISPLAYOFF);  		// 0xAE

    writeCommand(SSD1351_CMD_CLOCKDIV);  		// 0xB3
    writeCommand(0xF1);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(127);
    
    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(0x74);
  
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(0x00);
    writeData(0x7F);
    writeCommand(SSD1351_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);

    writeCommand(SSD1351_CMD_STARTLINE); 		// 0xA1
    writeData(0x0);

    writeCommand(SSD1351_CMD_DISPLAYOFFSET); 	// 0xA2
    writeData(0x0);

    writeCommand(SSD1351_CMD_SETGPIO);
    writeData(0x00);
    
    writeCommand(SSD1351_CMD_FUNCTIONSELECT);
    writeData(0x01); // internal (diode drop)
    //writeData(0x01); // external bias

//    writeCommand(SSSD1351_CMD_SETPHASELENGTH);
//    writeData(0x32);

    writeCommand(SSD1351_CMD_PRECHARGE);  		// 0xB1
    writeCommand(0x32);
 
    writeCommand(SSD1351_CMD_VCOMH);  			// 0xBE
    writeCommand(0x05);

    writeCommand(SSD1351_CMD_NORMALDISPLAY);  	// 0xA6

    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);

    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(0x0F);

    writeCommand(SSD1351_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(0x01);
    
    writeCommand(SSD1351_CMD_DISPLAYON);		//--turn on oled panel    
}

void  Adafruit_SSD1351::invert(boolean v) {
   if (v) {
     writeCommand(SSD1351_CMD_INVERTDISPLAY);
   } else {
     	writeCommand(SSD1351_CMD_NORMALDISPLAY);
   }
 }

/********************************* low level pin initialization */

Adafruit_SSD1351::Adafruit_SSD1351(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk, uint8_t rst) {
    _cs = cs;
    _rs = rs;
    _sid = sid;
    _sclk = sclk;
    _rst = rst;

    constructor(128, 128);
}

Adafruit_SSD1351::Adafruit_SSD1351(uint8_t cs, uint8_t rs,  uint8_t rst) {
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;
    constructor(128, 128);
}



































