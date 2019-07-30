/*!
 * @file Adafruit_SSD1351.cpp
 *
 * @mainpage Arduino library for color OLEDs based on SSD1351 drivers.
 *
 * @section intro_sec Introduction
 *
 * This is documentation for Adafruit's SSD1351 library for color OLED
 * displays: http://www.adafruit.com/category/98
 *
 * These displays use SPI to communicate. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit-GFX-Library">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution.
 */

#include "Adafruit_SSD1351.h"

// TODO: this will be moved to SPITFT, but for now:
#if defined (ARDUINO_ARCH_ARC32)
  #define SPI_DEFAULT_FREQ 16000000 ///< ARC32 SPI default frequency
#elif defined (__AVR__) || defined(TEENSYDUINO)
  #define SPI_DEFAULT_FREQ 8000000 ///< AVR SPI default frequency
#elif defined(__SAMD51__)
  #define SPI_DEFAULT_FREQ 12000000 ///< M4 SPI default frequency
#elif defined(ESP8266) || defined (ARDUINO_MAXIM)
  #define SPI_DEFAULT_FREQ 16000000 ///< ESP8266 SPI default frequency
#elif defined(ESP32)
  #define SPI_DEFAULT_FREQ 24000000 ///< ESP32 SPI default frequency
#elif defined(RASPI)
  #define SPI_DEFAULT_FREQ 24000000 ///< RASPI SPI default frequency
#else
  #define SPI_DEFAULT_FREQ 24000000 ///< SPI default frequency
#endif

/*!
    @brief   Constructor for SSD1351 displays, using software (bitbang) SPI.
    @param   width
             Display width in pixels (usu. 128)
    @param   height
             Display height in pixels (usu. 128 or 96)
    @param   cs_pin
             Chip-select pin (using Arduino pin numbering) for sharing the
             bus with other devices. Active low.
    @param   dc_pin
             Data/command pin (using Arduino pin numbering), selects whether
             display is receiving commands (low) or data (high).
    @param   mosi_pin
             MOSI (master out, slave in) pin (using Arduino pin numbering).
             This transfers serial data from microcontroller to display.
    @param   sclk_pin
             SCLK (serial clock) pin (using Arduino pin numbering).
             This clocks each bit from MOSI.
    @param   rst_pin
             Reset pin (using Arduino pin numbering), or -1 if not used
             (some displays might be wired to share the microcontroller's
             reset pin).
    @return  Adafruit_SSD1351 object.
    @note    Call the object's begin() function before use.
*/
Adafruit_SSD1351::Adafruit_SSD1351(uint16_t width, uint16_t height,
  int8_t cs_pin, int8_t dc_pin, int8_t mosi_pin, int8_t sclk_pin,
  int8_t rst_pin) : Adafruit_SPITFT(width, height, cs_pin, dc_pin,
  mosi_pin, sclk_pin, rst_pin, -1) {
}

/*!
    @brief   Constructor for SSD1351 displays, using native hardware SPI.
    @param   width
             Display width in pixels
    @param   height
             Display height in pixels
    @param   spi
             Pointer to an existing SPIClass instance (e.g. &SPI, the
             microcontroller's primary SPI bus).
    @param   cs_pin
             Chip-select pin (using Arduino pin numbering) for sharing the
             bus with other devices. Active low.
    @param   dc_pin
             Data/command pin (using Arduino pin numbering), selects whether
             display is receiving commands (low) or data (high).
    @param   rst_pin
             Reset pin (using Arduino pin numbering), or -1 if not used
             (some displays might be wired to share the microcontroller's
             reset pin).
    @return  Adafruit_SSD1351 object.
    @note    Call the object's begin() function before use.
*/

Adafruit_SSD1351::Adafruit_SSD1351(uint16_t width, uint16_t height,
  SPIClass *spi, int8_t cs_pin, int8_t dc_pin, int8_t rst_pin) :
#if defined(ESP8266)
  Adafruit_SPITFT(width, height, cs_pin, dc_pin, rst_pin) {
#else
  Adafruit_SPITFT(width, height, spi, cs_pin, dc_pin, rst_pin) {
#endif
}

/*!
    @brief   DEPRECATED constructor for SSD1351 displays, using software
             (bitbang) SPI. Provided for older code to maintain
             compatibility with the current library. Screen size is
             determined by editing the SSD1351WIDTH and SSD1351HEIGHT
             defines in Adafruit_SSD1351.h.  New code should NOT use this.
    @param   cs_pin
             Chip-select pin (using Arduino pin numbering) for sharing the
             bus with other devices. Active low.
    @param   dc_pin
             Data/command pin (using Arduino pin numbering), selects whether
             display is receiving commands (low) or data (high).
    @param   mosi_pin
             MOSI (master out, slave in) pin (using Arduino pin numbering).
             This transfers serial data from microcontroller to display.
    @param   sclk_pin
             SCLK (serial clock) pin (using Arduino pin numbering).
             This clocks each bit from MOSI.
    @param   rst_pin
             Reset pin (using Arduino pin numbering), or -1 if not used
             (some displays might be wired to share the microcontroller's
             reset pin).
    @return  Adafruit_SSD1351 object.
    @note    Call the object's begin() function before use.
*/
Adafruit_SSD1351::Adafruit_SSD1351(int8_t cs_pin, int8_t dc_pin,
  int8_t mosi_pin, int8_t sclk_pin, int8_t rst_pin) :
  Adafruit_SPITFT(SSD1351WIDTH, SSD1351HEIGHT, cs_pin, dc_pin, mosi_pin,
  sclk_pin, rst_pin, -1) {
}

/*!
    @brief   DEPRECATED constructor for SSD1351 displays, using native
             hardware SPI. Provided for older code to maintain
             compatibility with the current library. Screen size is
             determined by editing the SSD1351WIDTH and SSD1351HEIGHT
             defines in Adafruit_SSD1351.h. Only the primary SPI bus is
             supported, and bitrate is fixed at a default. New code should
             NOT use this.
    @param   cs_pin
             Chip-select pin (using Arduino pin numbering) for sharing the
             bus with other devices. Active low.
    @param   dc_pin
             Data/command pin (using Arduino pin numbering), selects whether
             display is receiving commands (low) or data (high).
    @param   rst_pin
             Reset pin (using Arduino pin numbering), or -1 if not used
             (some displays might be wired to share the microcontroller's
             reset pin).
    @return  Adafruit_SSD1351 object.
    @note    Call the object's begin() function before use.
*/
Adafruit_SSD1351::Adafruit_SSD1351(int8_t cs_pin, int8_t dc_pin, int8_t rst_pin) :
#if defined(ESP8266)
  Adafruit_SPITFT(SSD1351WIDTH, SSD1351HEIGHT, cs_pin, dc_pin, rst_pin)
#else
  Adafruit_SPITFT(SSD1351WIDTH, SSD1351HEIGHT, &SPI, cs_pin, dc_pin, rst_pin) 
#endif
{ }

/*!
    @brief  Destructor for Adafruit_SSD1351 object.
*/
Adafruit_SSD1351::~Adafruit_SSD1351(void) {
}

// INIT DISPLAY ------------------------------------------------------------

static const uint8_t PROGMEM initList[] = {
  SSD1351_CMD_COMMANDLOCK, 1, // Set command lock, 1 arg
    0x12,
  SSD1351_CMD_COMMANDLOCK, 1, // Set command lock, 1 arg
    0xB1,
  SSD1351_CMD_DISPLAYOFF, 0,  // Display off, no args
  SSD1351_CMD_CLOCKDIV, 1,
    0xF1, // 7:4 = Oscillator Freq, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  SSD1351_CMD_MUXRATIO, 1,
    127,
  SSD1351_CMD_DISPLAYOFFSET, 1,
    0x0,
  SSD1351_CMD_SETGPIO, 1,
    0x00,
  SSD1351_CMD_FUNCTIONSELECT, 1,
    0x01,  // internal (diode drop)
  SSD1351_CMD_PRECHARGE, 1,
    0x32,
  SSD1351_CMD_VCOMH, 1,
    0x05,
  SSD1351_CMD_NORMALDISPLAY, 0,
  SSD1351_CMD_CONTRASTABC, 3,
    0xC8, 0x80, 0xC8,
  SSD1351_CMD_CONTRASTMASTER, 1,
    0x0F,
  SSD1351_CMD_SETVSL, 3,
    0xA0, 0xB5, 0x55,
  SSD1351_CMD_PRECHARGE2, 1,
    0x01,
  SSD1351_CMD_DISPLAYON, 0, // Main screen turn on
  0 }; // END OF COMMAND LIST

/*!
    @brief   Initialize SSD1351 chip. Configures pins, connects to the
             SSD1351 and sends initialization commands.
    @param   freq
             SPI bitrate -- default of 0 will use a (usually) platform-
             optimized value, e.g. 8 MHz on AVR, 12 MHz on M0.
    @return  None (void).
*/
void Adafruit_SSD1351::begin(uint32_t freq) {

  if(!freq) freq = SPI_DEFAULT_FREQ; // Will move to SPITFT initSPI
  initSPI(freq);

  const uint8_t *addr = (const uint8_t *)initList;
  uint8_t        cmd, x, numArgs;

  while((cmd = pgm_read_byte(addr++)) > 0) { // '0' command ends list
    x = pgm_read_byte(addr++);
    numArgs = x & 0x7F;
    if (cmd != 0xFF) { // '255' is ignored
      sendCommand(cmd, addr, numArgs);
    }
    addr += numArgs;
  }
  setRotation(0);
}

// GFX FUNCTIONS -----------------------------------------------------------

/*!
    @brief   Set origin of (0,0) and orientation of OLED display
    @param   r
             The index for rotation, from 0-3 inclusive
    @return  None (void).
    @note    SSD1351 works differently than most (all?) other SPITFT
             displays. With certain rotation changes the screen contents
             may change immediately into a peculiar format (mirrored, not
             necessarily rotated) (other displays, this only affects new
             drawing -- rotation combinations can apply to different
             areas). Therefore, it's recommend to clear the screen
             (fillScreen(0)) before changing rotation.
*/
void Adafruit_SSD1351::setRotation(uint8_t r) {
  // madctl bits:
  // 6,7 Color depth (01 = 64K)
  // 5   Odd/even split COM (0: disable, 1: enable)
  // 4   Scan direction (0: top-down, 1: bottom-up)
  // 3   Reserved
  // 2   Color remap (0: A->B->C, 1: C->B->A)
  // 1   Column remap (0: 0-127, 1: 127-0)
  // 0   Address increment (0: horizontal, 1: vertical)
  uint8_t madctl = 0b01100100; // 64K, enable split, CBA

  rotation = r & 3; // Clip input to valid range

  switch(rotation) {
    case 0:
      madctl |= 0b00010000; // Scan bottom-up
      _width  = WIDTH;
      _height = HEIGHT;
      break;
    case 1:
      madctl |= 0b00010011; // Scan bottom-up, column remap 127-0, vertical
      _width  = HEIGHT;
      _height = WIDTH;
      break;
    case 2:
      madctl |= 0b00000010; // Column remap 127-0
      _width  = WIDTH;
      _height = HEIGHT;
      break;
    case 3:
      madctl |= 0b00000001; // Vertical
      _width  = HEIGHT;
      _height = WIDTH;
      break;
  }

  sendCommand(SSD1351_CMD_SETREMAP, &madctl, 1);
  uint8_t startline = (rotation < 2) ? HEIGHT : 0;
  sendCommand(SSD1351_CMD_STARTLINE, &startline, 1);
}

/*!
    @brief   Enable/Disable display color inversion
    @param   i
             True to invert display, False for normal color.
    @return  None (void).
    @note    This syntax is used by other SPITFT compatible libraries.
             New code should use this.
*/
void Adafruit_SSD1351::invertDisplay(boolean i) {
  sendCommand(i ? SSD1351_CMD_INVERTDISPLAY : SSD1351_CMD_NORMALDISPLAY);
}

/*!
    @brief   Enable/Disable display color inversion
    @param   i
             True to invert display, False for normal color.
    @return  None (void).
    @note    This is an older syntax used by this library prior to the
             SPITFT library. New code should avoid it.
*/
void Adafruit_SSD1351::invert(boolean i) {
  invertDisplay(i);
}

#define ssd1351_swap(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

/*!
    @brief   Set the "address window" - the rectangle we will write to
             graphics RAM with the next chunk of SPI data writes. The
             SSD1351 will automatically wrap the data as each row is filled.
    @param   x1
             Leftmost column of rectangle (screen pixel coordinates).
    @param   y1
             Topmost row of rectangle (screen pixel coordinates).
    @param   w
             Width of rectangle.
    @param   h
             Height of rectangle.
    @return  None (void).
*/
void Adafruit_SSD1351::setAddrWindow(
  uint16_t x1, uint16_t y1, uint16_t w, uint16_t h) {

  uint16_t x2 = x1 + w - 1,
           y2 = y1 + h - 1;
  if(rotation & 1) { // Vertical address increment mode
    ssd1351_swap(x1, y1);
    ssd1351_swap(x2, y2);
  }
  writeCommand(SSD1351_CMD_SETCOLUMN); // X range
  spiWrite(x1);
  spiWrite(x2);
  writeCommand(SSD1351_CMD_SETROW);    // Y range
  spiWrite(y1);
  spiWrite(y2);
  writeCommand(SSD1351_CMD_WRITERAM);  // Begin write
}

/**************************************************************************/
/*!
 @brief  Change whether display is on or off
 @param   enable True if you want the display ON, false OFF
 */
/**************************************************************************/
void Adafruit_SSD1351::enableDisplay(boolean enable) {
  sendCommand(enable ? SSD1351_CMD_DISPLAYON : SSD1351_CMD_DISPLAYOFF);
}
