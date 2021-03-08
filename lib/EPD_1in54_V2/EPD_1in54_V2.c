#include "EPD_1in54_V2.h"

// Init GPIO and SPI
void Init_Device()
{
    /* Enable SPI at 1MHz*/
    spi_init(EPD_SPI_PORT, 1000 * 1000);

    gpio_set_function(EPD_PIN_DC, GPIO_FUNC_SPI);
    gpio_set_function(EPD_PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(EPD_PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(EPD_PIN_DIN, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(EPD_PIN_DC);
    gpio_init(EPD_PIN_CS);
    gpio_init(EPD_PIN_RST);

    // set also clock and data pin to high level
    gpio_set_dir(EPD_PIN_DC, GPIO_OUT);
    gpio_set_dir(EPD_PIN_CS, GPIO_OUT);
    gpio_set_dir(EPD_PIN_DIN, GPIO_OUT);
    gpio_set_dir(EPD_PIN_RST, GPIO_OUT);
    gpio_set_dir(EPD_PIN_BUSY, GPIO_IN);

    gpio_put(EPD_PIN_CS, 1);
}

// Software reset
void EPD_1IN54_V2_Reset()
{
    gpio_put(EPD_PIN_RST, 1);
    sleep_ms(200);
    gpio_put(EPD_PIN_RST, 0);
    sleep_ms(2);
    gpio_put(EPD_PIN_RST, 1);
    sleep_ms(200);
}

// send command to register
void EPD_1IN54_V2_SendCommand(UBYTE Reg)
{
    gpio_put(EPD_PIN_DC, 0);
    gpio_put(EPD_PIN_CS, 0);
    spi_write_blocking(spi0, &Reg, 1);
    gpio_put(EPD_PIN_CS, 1);
}

// send data
void EPD_1IN54_V2_SendData(UBYTE Data)
{
    gpio_put(EPD_PIN_DC, 1);
    gpio_put(EPD_PIN_CS, 0);
    spi_write_blocking(spi0, &Data, 1);
    gpio_put(EPD_PIN_CS, 1);
}

// Wait until the busy_pin goes LOW
void EPD_1IN54_V2_ReadBusy()
{
    while (gpio_get(EPD_PIN_BUSY) == 1)
    { //LOW: idle, HIGH: busy
        sleep_ms(100);
    }
}

// Turn On Display full
void EPD_1IN54_V2_TurnOnDisplay()
{
    EPD_1IN54_V2_SendCommand(0x22);
    EPD_1IN54_V2_SendData(0xF7);
    EPD_1IN54_V2_SendCommand(0x20);
    EPD_1IN54_V2_ReadBusy();
}

// Turn On Display part
void EPD_1IN54_V2_TurnOnDisplayPart()
{
    EPD_1IN54_V2_SendCommand(0x22);
    EPD_1IN54_V2_SendData(0xFF);
    EPD_1IN54_V2_SendCommand(0x20);
    EPD_1IN54_V2_ReadBusy();
}

// Initialize the e-Paper register
void EPD_1IN54_V2_Init()
{
    EPD_1IN54_V2_Reset();

    EPD_1IN54_V2_ReadBusy();
    EPD_1IN54_V2_SendCommand(0x12); // SWRESET
    EPD_1IN54_V2_ReadBusy();

    EPD_1IN54_V2_SendCommand(0x01); // Driver output control
    EPD_1IN54_V2_SendData(0xC7);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SendCommand(0x11); // data entry mode
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SendCommand(0x44); // set Ram-X address start/end position
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x18); // 0x0C-->(18+1)*8=200

    EPD_1IN54_V2_SendCommand(0x45); // set Ram-Y address start/end position
    EPD_1IN54_V2_SendData(0xC7);    // 0xC7-->(199+1)=200
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendData(0x00);

    EPD_1IN54_V2_SendCommand(0x3C); // BorderWavefrom
    EPD_1IN54_V2_SendData(0x01);

    EPD_1IN54_V2_SendCommand(0x18);
    EPD_1IN54_V2_SendData(0x80);

    EPD_1IN54_V2_SendCommand(0x22); // Load Temperature and waveform setting.
    EPD_1IN54_V2_SendData(0XB1);
    EPD_1IN54_V2_SendCommand(0x20);

    EPD_1IN54_V2_SendCommand(0x4E); // set RAM x address count to 0;
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_SendCommand(0x4F); // set RAM y address count to 0X199;
    EPD_1IN54_V2_SendData(0xC7);
    EPD_1IN54_V2_SendData(0x00);
    EPD_1IN54_V2_ReadBusy();
}

// Clear screen
void EPD_1IN54_V2_Clear()
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_1IN54_V2_SendData(0XFF);
        }
    }
    EPD_1IN54_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            EPD_1IN54_V2_SendData(0XFF);
        }
    }
    EPD_1IN54_V2_TurnOnDisplay();
}

// Sends the image buffer in RAM to e-Paper and displays
void EPD_1IN54_V2_Display(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_TurnOnDisplay();
}

// The image of the previous frame must be uploaded, otherwise the first few seconds will display an exception
void EPD_1IN54_V2_DisplayPartBaseImage(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_TurnOnDisplayPart();
}

// Sends the image buffer in RAM to e-Paper and displays
void EPD_1IN54_V2_DisplayPart(UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1);
    Height = EPD_1IN54_V2_HEIGHT;

    gpio_put(EPD_PIN_RST, 0);
    sleep_ms(10);
    gpio_put(EPD_PIN_RST, 1);
    sleep_ms(10);
    EPD_1IN54_V2_SendCommand(0x3C); //BorderWavefrom
    EPD_1IN54_V2_SendData(0x80);

    UDOUBLE Addr = 0;
    EPD_1IN54_V2_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
    {
        for (UWORD i = 0; i < Width; i++)
        {
            Addr = i + j * Width;
            EPD_1IN54_V2_SendData(Image[Addr]);
        }
    }
    EPD_1IN54_V2_TurnOnDisplayPart();
}

// Enter sleep mode
void EPD_1IN54_V2_Sleep()
{
    EPD_1IN54_V2_SendCommand(0x10); //enter deep sleep
    EPD_1IN54_V2_SendData(0x01);
    sleep_ms(100);
}