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

void Adafruit_SSD1351::goHome(void) {
  goTo(0,0);
}

uint16_t Adafruit_SSD1351::width(void) {
  return _width;
}

uint16_t Adafruit_SSD1351::height(void) {
  return _height;
}

void Adafruit_SSD1351::goTo(int x, int y) {
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(x);
  writeData(TFTWIDTH-1);

  writeCommand(SSD1351_CMD_SETROW);
  writeData(y);
  writeData(TFTHEIGHT-1);

  writeCommand(SSD1351_CMD_WRITERAM);  
}

void Adafruit_SSD1351::setCursor(uint16_t x, uint16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Adafruit_SSD1351::setTextSize(uint8_t s) {
  textsize = s;
}

void Adafruit_SSD1351::setTextColor(uint16_t c) {
  textcolor = c;
}

#if ARDUINO >= 100
size_t Adafruit_SSD1351::write(uint8_t c) {
#else
void Adafruit_SSD1351::write(uint8_t c) {
#endif
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textsize);
    cursor_x += textsize*6;
  }
#if ARDUINO >= 100
  return 1;
#endif
}

void Adafruit_SSD1351::drawString(uint16_t x, uint16_t y, char *c, 
			uint16_t color, uint8_t size) {
  while (c[0] != 0) {
    drawChar(x, y, c[0], color, size);
    x += size*6;
    if ((x+5) > TFTWIDTH) {
      x = 0;
      y += size*8;
    }
    c++;
  }
}
// draw a character
void Adafruit_SSD1351::drawChar(uint16_t x, uint16_t y, char c, 
		      uint16_t color, uint8_t size) {
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  drawPixel(x+i, y+j, color);
	else {  // big size
	  fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}

// draw a triangle!
void Adafruit_SSD1351::drawTriangle(uint16_t x0, uint16_t y0,
			  uint16_t x1, uint16_t y1,
			  uint16_t x2, uint16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color); 
}

void Adafruit_SSD1351::fillTriangle ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  int32_t dx1, dx2, dx3; // Interpolation deltas
  int32_t sx1, sx2, sy; // Scanline co-ordinates
  
  sx2=(int32_t)x0 * (int32_t)1000; // Use fixed point math for x axis values
  sx1 = sx2;
  sy=y0;

  // Calculate interpolation deltas
  if (y1-y0 > 0) dx1=((x1-x0)*1000)/(y1-y0);
    else dx1=0;
  if (y2-y0 > 0) dx2=((x2-x0)*1000)/(y2-y0);
    else dx2=0;
  if (y2-y1 > 0) dx3=((x2-x1)*1000)/(y2-y1);
    else dx3=0;

  // Render scanlines (horizontal lines are the fastest rendering method)
  if (dx1 > dx2)
  {
    for(; sy<=y1; sy++, sx1+=dx2, sx2+=dx1)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
    sx2 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx2, sx2+=dx3)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
  }
  else
  {
    for(; sy<=y1; sy++, sx1+=dx1, sx2+=dx2)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
    sx1 = x1*1000;
    sy = y1;
    for(; sy<=y2; sy++, sx1+=dx3, sx2+=dx2)
    {
      drawHorizontalLine(sx1/1000, sy, (sx2-sx1)/1000, color);
    }
  }
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

// draw a rectangle
void Adafruit_SSD1351::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
		      uint16_t color) {
			  
  if ((w == 1) && (h == 1))
  {
	drawPixel(x, y, color);
	return;
  }
  
  // smarter version
  drawHorizontalLine(x, y, w, color);
  drawHorizontalLine(x, y+h-1, w, color);
  drawVerticalLine(x, y, h, color);
  drawVerticalLine(x+w-1, y, h, color);
}

// draw a rounded rectangle
void Adafruit_SSD1351::drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
			   uint16_t color) {
  // smarter version
  drawHorizontalLine(x+r, y, w-2*r, color);
  drawHorizontalLine(x+r, y+h-1, w-2*r, color);
  drawVerticalLine(x, y+r, h-2*r, color);
  drawVerticalLine(x+w-1, y+r, h-2*r, color);
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}


// fill a rounded rectangle
void Adafruit_SSD1351::fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
			   uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

// fill a circle
void Adafruit_SSD1351::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
  drawVerticalLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects!
void Adafruit_SSD1351::fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t delta,
			uint16_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    if (cornername & 0x1) {
      drawVerticalLine(x0+x, y0-y, 2*y+1+delta, color);
      drawVerticalLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawVerticalLine(x0-x, y0-y, 2*y+1+delta, color);
      drawVerticalLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}


// draw a circle outline

void Adafruit_SSD1351::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, 
			uint16_t color) {
  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  drawCircleHelper(x0, y0, r, 0xF, color);
}

void Adafruit_SSD1351::drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername,
			uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;


  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/
void Adafruit_SSD1351::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolor) 
{	
  // Bounds check
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT))
	return;

  // Y bounds check
  if (y+h > TFTHEIGHT)
  {
    h = TFTHEIGHT - y - 1;
  }

  // X bounds check
  if (x+w > TFTWIDTH)
  {
    w = TFTWIDTH - x - 1;
  }
  

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

void Adafruit_SSD1351::drawVerticalLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
  if (x >= TFTWIDTH) return;
  if (y >= TFTHEIGHT) return;
    
  // Y bounds check
  if (y+length >= TFTHEIGHT)
  {
    length = TFTHEIGHT - y - 1;
  }
	
  drawLine(x, y, x, y+length-1, color);
}

void Adafruit_SSD1351::drawHorizontalLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
  if (y >= TFTHEIGHT) return;
  if (x >= TFTWIDTH) return;

  // X bounds check
  if (x+length >= TFTWIDTH)
  {
    length = TFTWIDTH - x - 1;
  }
  
  drawLine(x, y, x+length-1, y, color);
}

void Adafruit_SSD1351::drawFastLine(uint16_t x, uint16_t y, uint16_t length, 
			  uint16_t color, uint8_t rotflag) {
			  
	// Normal drawLine is already HW accelerated ... keep this method for backward
	// compatibility, but use drawLine instead
	if ((x >= TFTWIDTH) || (x+length >= TFTWIDTH) || (y >= TFTHEIGHT)) 
	{
	  return;
	}
	
	if (rotflag)
	{
		// Vertical line
		drawLine(x, y, x+length-1, y, color);
	}
	else
	{
		// Horizontal line
		drawLine(x, y, x, y+length-1, color);
	}
}

void Adafruit_SSD1351::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {	
  // Boundary check
  if ((y0 >= TFTHEIGHT) && (y1 >= TFTHEIGHT))
	return;
  if ((x0 >= TFTWIDTH) && (x1 >= TFTWIDTH))
	return;	
  if (x0 >= TFTWIDTH)
    x0 = TFTWIDTH - 1;
  if (y0 >= TFTHEIGHT)
    y0 = TFTHEIGHT - 1;
  if (x1 >= TFTWIDTH)
    x1 = TFTWIDTH - 1;
  if (y1 >= TFTHEIGHT)
    y1 = TFTHEIGHT - 1;
  
  //writeCommand(SSD1351_CMD_DRAWLINE);
  writeCommand(x0);
  writeCommand(y0);
  writeCommand(x1);
  writeCommand(y1);
  delay(SSD1351_DELAYS_HWLINE);  
  writeCommand((uint8_t)((color >> 11) << 1));
  writeCommand((uint8_t)((color >> 5) & 0x3F));
  writeCommand((uint8_t)((color << 1) & 0x3F));
  delay(SSD1351_DELAYS_HWLINE);  
}

void Adafruit_SSD1351::fillScreen(uint16_t color) {
	fillRect(0, 0, TFTWIDTH, TFTHEIGHT, color);
}

void Adafruit_SSD1351::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return;
  
  goTo(x, y);
  
  // setup for data
  *portOutputRegister(rsport) |= rspin;
  *portOutputRegister(csport) &= ~ cspin;
  
  spiwrite(color >> 8);    
  spiwrite(color);
  
  *portOutputRegister(csport) |= cspin;  
}

void Adafruit_SSD1351::initDisplay(void) {
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

	_width = TFTWIDTH;
	_height = TFTHEIGHT;

	cursor_y = cursor_x = 0;
	textsize = 1;
	textcolor = 0xFFFF;
}

Adafruit_SSD1351::Adafruit_SSD1351(uint8_t cs, uint8_t rs,  uint8_t rst) {
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;

	_width = TFTWIDTH;
	_height = TFTHEIGHT;

	cursor_y = cursor_x = 0;
	textsize = 1;
	textcolor = 0xFFFF;
}



































