#include <stdio.h>
#include <stdlib.h>
#include "lib/GUI/GUI_Paint.h"
#include "lib/EPD_1in54_V2/EPD_1in54_V2.h"

// convert bmp to byte array: https://www.waveshare.com/wiki/File:Image2Lcd.7z

#define PIN_LED 25

void blink(UBYTE amount, UDOUBLE time)
{
    UBYTE i = 0;
    while (i < amount)
    {
        sleep_ms(time);
        gpio_put(PIN_LED, 1);
        sleep_ms(time);
        gpio_put(PIN_LED, 0);
        i++;
    }
}

int main()
{
    stdio_init_all();
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    blink(2, 150);

    // Init EPD //
    Init_Device();
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();

    // Create new image cache
    UBYTE *ImageBuffer;
    UWORD Imagesize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? (EPD_1IN54_V2_WIDTH / 8) : (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
    if ((ImageBuffer = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for memory...\n");
    }
    Paint_NewImage(ImageBuffer, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 270, WHITE);
    Paint_SelectImage(ImageBuffer);

    blink(1, 250);

    // Edit image and display it

    Paint_Clear(WHITE);

    Paint_DrawString_EN(10, 5, "Hello World", &Font24, WHITE, BLACK);

    EPD_1IN54_V2_Display(ImageBuffer);

    // Exit //
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();
    EPD_1IN54_V2_Sleep();

    free(ImageBuffer);

    sleep_ms(2000);
    blink(2, 150);
}