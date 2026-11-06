#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>
#include <driver/spi_master.h>

// ============================================================
// Board identity
// ============================================================

#define TDL_S3_DIY_BOARD_NAME "TDL-S3-DIY"
#define TDL_S3_DIY_BOARD_TYPE "tdl-esp32s3-n16r8"

// ============================================================
// LCD: ST7789, 240 x 320, SPI
// ============================================================

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

// Initial orientation settings; verify on the physical display.
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY false

// ============================================================
// Touch: XPT2046, sharing LCD SPI bus
// ============================================================

#define TOUCH_SPI_HOST DISPLAY_SPI_HOST
#define TOUCH_SCLK_PIN DISPLAY_SCLK_PIN
#define TOUCH_MOSI_PIN DISPLAY_MOSI_PIN
#define TOUCH_MISO_PIN DISPLAY_MISO_PIN
#define TOUCH_CS_PIN GPIO_NUM_9
#define TOUCH_INT_PIN GPIO_NUM_8

// ============================================================
// Audio: INMP441 microphone + MAX98357A amplifier
// ============================================================

// Shared I2S clock signals
#define AUDIO_I2S_BCLK_PIN GPIO_NUM_11
#define AUDIO_I2S_WS_PIN GPIO_NUM_12

// Separate data signals
#define AUDIO_I2S_MIC_DIN_PIN GPIO_NUM_14
#define AUDIO_I2S_SPK_DOUT_PIN GPIO_NUM_13

// Provisional software sample rates.
// Confirm against the selected audio implementation before use.
#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// ============================================================
// Buttons and indicators
// ============================================================

#define BOOT_BUTTON_GPIO GPIO_NUM_0

// WS2812 RGB LED
#define BUILTIN_LED_GPIO GPIO_NUM_48

// No separate volume buttons on this board.
#define VOLUME_UP_BUTTON_GPIO GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// ============================================================
// Optional OLED: SH1106, I2C
// Not enabled in V1 LCD build.
// ============================================================

#define OLED_I2C_SDA_PIN GPIO_NUM_1
#define OLED_I2C_SCL_PIN GPIO_NUM_2

#endif  // _BOARD_CONFIG_H_