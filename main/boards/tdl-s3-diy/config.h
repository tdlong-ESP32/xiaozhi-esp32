#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>
#include <driver/spi_master.h>

// TDL DIY ESP32-S3 N16R8
#define TDL_S3_DIY_BOARD_NAME "TDL-S3-DIY"
#define TDL_S3_DIY_BOARD_TYPE "tdl-esp32s3-n16r8"

// ST7789 240 x 320 SPI LCD
#define DISPLAY_SPI_HOST SPI2_HOST
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320
#define DISPLAY_SCLK_PIN GPIO_NUM_17
#define DISPLAY_MOSI_PIN GPIO_NUM_18
#define DISPLAY_MISO_PIN GPIO_NUM_15
#define DISPLAY_CS_PIN GPIO_NUM_10
#define DISPLAY_DC_PIN GPIO_NUM_3
#define DISPLAY_RST_PIN GPIO_NUM_46
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_16
#define DISPLAY_SPI_CLOCK_HZ (40 * 1000 * 1000)
#define DISPLAY_RGB_ORDER LCD_RGB_ELEMENT_ORDER_RGB
#define DISPLAY_INVERT_COLOR false
#define DISPLAY_OFFSET_X 0
#define DISPLAY_OFFSET_Y 0
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY false
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

// XPT2046 shares LCD SPI signals, separate CS and active-low IRQ.
#define TOUCH_SPI_HOST DISPLAY_SPI_HOST
#define TOUCH_CS_PIN GPIO_NUM_9
#define TOUCH_INT_PIN GPIO_NUM_8
// Placeholder calibration values; adjust after logging raw coordinates.
#define TOUCH_RAW_X_MIN 200
#define TOUCH_RAW_X_MAX 3900
#define TOUCH_RAW_Y_MIN 200
#define TOUCH_RAW_Y_MAX 3900
#define TOUCH_SWAP_XY false
#define TOUCH_MIRROR_X false
#define TOUCH_MIRROR_Y false

// INMP441 + MAX98357A, shared I2S clocks, separate data signals.
#define AUDIO_I2S_BCLK_PIN GPIO_NUM_11
#define AUDIO_I2S_WS_PIN GPIO_NUM_12
#define AUDIO_I2S_MIC_DIN_PIN GPIO_NUM_14
#define AUDIO_I2S_SPK_DOUT_PIN GPIO_NUM_13

// NoAudioCodecDuplex uses a common I2S clock configuration for RX and TX.
#define AUDIO_INPUT_SAMPLE_RATE 24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define BUILTIN_LED_GPIO GPIO_NUM_48
#define VOLUME_UP_BUTTON_GPIO GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// Optional SH1106 OLED; not used by this LCD build.
#define OLED_I2C_SDA_PIN GPIO_NUM_1
#define OLED_I2C_SCL_PIN GPIO_NUM_2

#endif  // _BOARD_CONFIG_H_
