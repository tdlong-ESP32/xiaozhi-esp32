#include "application.h"
#include "button.h"
#include "codecs/no_audio_codec.h"
#include "config.h"
#include "display/lcd_display.h"
#include "led/single_led.h"
#include "wifi_board.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>

#include <algorithm>
#include <cstdint>

namespace {
constexpr char kTag[] = "TdlS3Diy";
constexpr uint8_t kReadX = 0xD0;
constexpr uint8_t kReadY = 0x90;
constexpr uint32_t kTouchClockHz = 2 * 1000 * 1000;

class Xpt2046Touch {
public:
    esp_err_t Init() {
        gpio_config_t irq = {};
        irq.pin_bit_mask = 1ULL << TOUCH_INT_PIN;
        irq.mode = GPIO_MODE_INPUT;
        irq.pull_up_en = GPIO_PULLUP_ENABLE;
        irq.pull_down_en = GPIO_PULLDOWN_DISABLE;
        irq.intr_type = GPIO_INTR_DISABLE;
        ESP_RETURN_ON_ERROR(gpio_config(&irq), kTag, "Configure touch IRQ");

        spi_device_interface_config_t cfg = {};
        cfg.clock_speed_hz = kTouchClockHz;
        cfg.mode = 0;
        cfg.spics_io_num = TOUCH_CS_PIN;
        cfg.queue_size = 1;
        return spi_bus_add_device(TOUCH_SPI_HOST, &cfg, &device_);
    }

    bool ReadPoint(int16_t& sx, int16_t& sy) {
        if (device_ == nullptr || gpio_get_level(TOUCH_INT_PIN) != 0) return false;

        uint32_t xs = 0, ys = 0;
        constexpr int samples = 3;
        for (int i = 0; i < samples; ++i) {
            uint16_t x = 0, y = 0;
            if (ReadChannel(kReadX, x) != ESP_OK || ReadChannel(kReadY, y) != ESP_OK)
                return false;
            xs += x;
            ys += y;
        }
        int32_t x = xs / samples;
        int32_t y = ys / samples;
#if TOUCH_SWAP_XY
        std::swap(x, y);
#endif
#if TOUCH_MIRROR_X
        x = TOUCH_RAW_X_MIN + TOUCH_RAW_X_MAX - x;
#endif
#if TOUCH_MIRROR_Y
        y = TOUCH_RAW_Y_MIN + TOUCH_RAW_Y_MAX - y;
#endif
        sx = Map(x, TOUCH_RAW_X_MIN, TOUCH_RAW_X_MAX, DISPLAY_WIDTH);
        sy = Map(y, TOUCH_RAW_Y_MIN, TOUCH_RAW_Y_MAX, DISPLAY_HEIGHT);
        return true;
    }

private:
    esp_err_t ReadChannel(uint8_t command, uint16_t& value) {
        uint8_t tx[3] = {command, 0, 0};
        uint8_t rx[3] = {};
        spi_transaction_t t = {};
        t.length = 24;
        t.rxlength = 24;
        t.tx_buffer = tx;
        t.rx_buffer = rx;
        esp_err_t err = spi_device_polling_transmit(device_, &t);
        if (err != ESP_OK) return err;
        value = (((uint16_t)rx[1] << 8) | rx[2]) >> 3;
        value &= 0x0FFF;
        return ESP_OK;
    }

    static int16_t Map(int32_t raw, int32_t lo, int32_t hi, int32_t extent) {
        if (hi <= lo) return 0;
        raw = std::clamp(raw, lo, hi);
        return static_cast<int16_t>((raw - lo) * (extent - 1) / (hi - lo));
    }

    spi_device_handle_t device_ = nullptr;
};

Xpt2046Touch* g_touch = nullptr;

void TouchRead(lv_indev_t*, lv_indev_data_t* data) {
    int16_t x = 0, y = 0;
    if (g_touch && g_touch->ReadPoint(x, y)) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
}  // namespace

class TdlS3DiyBoard : public WifiBoard {
private:
    Button boot_button_{BOOT_BUTTON_GPIO};
    LcdDisplay* display_ = nullptr;
    Xpt2046Touch touch_;

    void InitializeSpi() {
        spi_bus_config_t bus = {};
        bus.mosi_io_num = DISPLAY_MOSI_PIN;
        bus.miso_io_num = DISPLAY_MISO_PIN;
        bus.sclk_io_num = DISPLAY_SCLK_PIN;
        bus.quadwp_io_num = GPIO_NUM_NC;
        bus.quadhd_io_num = GPIO_NUM_NC;
        bus.max_transfer_sz = DISPLAY_WIDTH * 40 * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(DISPLAY_SPI_HOST, &bus, SPI_DMA_CH_AUTO));
    }

    void InitializeDisplay() {
        esp_lcd_panel_io_handle_t io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;

        esp_lcd_panel_io_spi_config_t io_cfg = {};
        io_cfg.cs_gpio_num = DISPLAY_CS_PIN;
        io_cfg.dc_gpio_num = DISPLAY_DC_PIN;
        io_cfg.spi_mode = 0;
        io_cfg.pclk_hz = DISPLAY_SPI_CLOCK_HZ;
        io_cfg.trans_queue_depth = 10;
        io_cfg.lcd_cmd_bits = 8;
        io_cfg.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(DISPLAY_SPI_HOST, &io_cfg, &io));

        esp_lcd_panel_dev_config_t panel_cfg = {};
        panel_cfg.reset_gpio_num = DISPLAY_RST_PIN;
        panel_cfg.rgb_ele_order = DISPLAY_RGB_ORDER;
        panel_cfg.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io, &panel_cfg, &panel));
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, DISPLAY_INVERT_COLOR));
        ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY));
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y));

        display_ = new SpiLcdDisplay(io, panel, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y,
            DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
        ESP_LOGI(kTag, "ST7789 ready: %d x %d", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    void InitializeTouch() {
        ESP_ERROR_CHECK(touch_.Init());
        g_touch = &touch_;

        ESP_ERROR_CHECK(lvgl_port_lock(0) ? ESP_OK : ESP_ERR_TIMEOUT);
        lv_indev_t* indev = lv_indev_create();
        if (indev) {
            lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
            lv_indev_set_display(indev, lv_display_get_default());
            lv_indev_set_read_cb(indev, TouchRead);
        }
        lvgl_port_unlock();
        ESP_ERROR_CHECK(indev ? ESP_OK : ESP_ERR_NO_MEM);
        ESP_LOGI(kTag, "XPT2046 registered as LVGL pointer input");
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

public:
    TdlS3DiyBoard() {
        InitializeSpi();
        InitializeDisplay();
        InitializeTouch();
        InitializeButtons();
        GetBacklight()->RestoreBrightness();
    }

    AudioCodec* GetAudioCodec() override {
        // One I2S controller, shared BCLK/WS, separate RX/TX data pins.
        static NoAudioCodecDuplex codec(
            AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_BCLK_PIN, AUDIO_I2S_WS_PIN,
            AUDIO_I2S_SPK_DOUT_PIN, AUDIO_I2S_MIC_DIN_PIN);
        return &codec;
    }

    Display* GetDisplay() override { return display_; }

    Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }

    Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }
};

DECLARE_BOARD(TdlS3DiyBoard);
