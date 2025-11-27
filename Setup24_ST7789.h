// Display: 1.3" 240x240 ST7789 IPS RGB565

#define USER_SETUP_ID 24

#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_MOSI 23  // also called SDA
#define TFT_SCLK 18  // also called SCL
#define TFT_DC   15
#define TFT_RST  4   // also called RES

#define LOAD_GLCD  // Original Adafruit 8 pixel font

#define SPI_FREQUENCY  40000000  // 40 MHz
