#ifndef __EPD_1in54_V2__
#define __EPD_1in54_V2__

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "stdint.h"

/**
 * data
**/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

/**
 * GPIO config
**/
#define EPD_PIN_DIN 3  // SPI MOSI
#define EPD_PIN_CLK 2  // SPI SCK
#define EPD_PIN_CS 1   // SPI chip select (Low active)
#define EPD_PIN_DC 6   // Data/Command control pin (High for data, and low for command)
#define EPD_PIN_RST 4  // External reset pin (Low for reset)
#define EPD_PIN_BUSY 7 // Busy state output pin (Low for busy)

#define EPD_SPI_PORT spi0

/**
 * Display resolution
**/
#define EPD_1IN54_V2_WIDTH 200
#define EPD_1IN54_V2_HEIGHT 200

// Init GPIO and SPI
void Init_Device();

// Initialize the e-Paper register
void EPD_1IN54_V2_Init();
// Clear screen
void EPD_1IN54_V2_Clear();
// Software reset
void EPD_1IN54_V2_Reset();
// Wait until the busy_pin goes LOW
void EPD_1IN54_V2_ReadBusy();
// Sends the image buffer in RAM to e-Paper and displays
void EPD_1IN54_V2_Display(UBYTE *Image);
// The image of the previous frame must be uploaded, otherwise the first few seconds will display an exception
void EPD_1IN54_V2_DisplayPartBaseImage(UBYTE *Image);
// Sends the image buffer in RAM to e-Paper and displays
void EPD_1IN54_V2_DisplayPart(UBYTE *Image);
// Enter sleep mode
void EPD_1IN54_V2_Sleep();

#endif