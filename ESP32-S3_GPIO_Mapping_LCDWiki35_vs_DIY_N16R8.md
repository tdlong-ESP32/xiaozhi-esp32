# ESP32-S3 GPIO Mapping
## LCD Wiki 3.5-inch vs DIY ESP32-S3 N16R8

**Purpose:** Compare ESP32-S3 GPIO assignments for the LCD Wiki 3.5-inch board and the DIY N16R8 board.

**Conventions**
- GPIO numbers refer to ESP32-S3 GPIOs.
- `—` means not applicable or not connected in the current V1 design.
- DIY microSD is omitted from active pin assignments because its module pads are not connected to the ESP32 yet.
- The optional DIY SH1106 OLED is listed separately.

---

## 1. LCD and Touch

| Function | LCD Wiki 3.5-inch | DIY N16R8 |
|---|---|---|
| LCD controller | ST77922 | ST7789 |
| LCD interface | QSPI | SPI |
| LCD CS | GPIO10 | GPIO10 |
| LCD CLK / SCLK | GPIO12 | GPIO17 |
| LCD DATA0 / MOSI | GPIO11 | GPIO18 |
| LCD DATA1 | GPIO13 | — |
| LCD DATA2 | GPIO14 | — |
| LCD DATA3 | GPIO9 | — |
| LCD MISO | — | GPIO15 |
| LCD DC | — | GPIO3 |
| LCD RESET | EN (shared) | GPIO46 |
| LCD backlight | GPIO41 | GPIO16 |
| Touch controller | ST77922 integrated capacitive touch | XPT2046 |
| Touch interface | I2C | SPI |
| Touch CS | — | GPIO9 |
| Touch SCLK | — | GPIO17 (shared LCD) |
| Touch MOSI / DIN | — | GPIO18 (shared LCD) |
| Touch MISO / DO | — | GPIO15 (shared LCD) |
| Touch SDA | GPIO38 | — |
| Touch SCL | GPIO39 | — |
| Touch INT | GPIO47 (declared; current code polls) | GPIO8 |
| Touch RESET | GPIO48 | — |

### Notes
- LCD Wiki uses QSPI with four data lines; DIY uses conventional SPI.
- DIY LCD and XPT2046 share SCLK, MOSI and MISO, with separate chip-select pins.
- LCD Wiki touch shares the I2C bus with the ES8311 codec.
- LCD Wiki GPIO47 is declared as touch interrupt, but the supplied board implementation reads touch through LVGL polling.
- LCD Wiki uses EN as LCD reset according to its README/configuration.

## 2. Audio

| Function | LCD Wiki 3.5-inch | DIY N16R8 |
|---|---|---|
| Microphone | LMA2718B381 MEMS | INMP441 |
| Microphone BCLK | GPIO18 | GPIO11 |
| Microphone WS / LRCK | GPIO21 | GPIO12 |
| Microphone data to ESP32 DIN | GPIO16 | GPIO14 |
| Speaker amplifier | FM8002E | MAX98357A |
| Amplifier BCLK | GPIO18 | GPIO11 |
| Amplifier WS / LRCK | GPIO21 | GPIO12 |
| ESP32 data to amplifier DIN | GPIO15 | GPIO13 |
| I2S MCLK | GPIO17 | Not used |
| Audio codec | ES8311 | No separate codec |
| Codec I2C SDA | GPIO38 | — |
| Codec I2C SCL | GPIO39 | — |
| PA enable | GPIO1 | — |
| Sample rate | 24 kHz input/output | To be confirmed |

### Audio architecture
- **LCD Wiki:** ES8311 audio codec controlled over I2C, with I2S audio data; FM8002E is the speaker amplifier.
- **DIY:** INMP441 microphone and MAX98357A amplifier use I2S directly. BCLK and WS are shared; microphone data and speaker data use separate ESP32 pins.
- The exact ESP-IDF I2S full-duplex configuration for DIY must be verified during integration.

## 3. Other Peripherals

| Function | LCD Wiki 3.5-inch | DIY N16R8 |
|---|---|---|
| RGB LED | GPIO40 (WS2812B) | GPIO48 (RGB LED) |
| LCD backlight | GPIO41 | GPIO16 |
| BOOT button | GPIO0 | GPIO0 |
| Reset | EN | EN |
| Battery ADC | GPIO8 | — |
| Power amplifier enable | GPIO1 | — |
| microSD interface | SDIO 4-bit | Not connected to ESP32 |
| SD CLK | GPIO5 | — |
| SD CMD | GPIO4 | — |
| SD D0 | GPIO6 | — |
| SD D1 | GPIO7 | — |
| SD D2 | GPIO2 | — |
| SD D3 | GPIO3 | — |
| UART RX | GPIO43 | GPIO44 |
| UART TX | GPIO44 | GPIO43 |
| USB D- | GPIO19 | GPIO19 |
| USB D+ | GPIO20 | GPIO20 |
| Expansion IO | GPIO45, GPIO46 | GPIO4, GPIO5, GPIO6, GPIO7 |

## 4. GPIO-by-GPIO Comparison

| GPIO | LCD Wiki 3.5-inch | DIY N16R8 |
|---:|---|---|
| 0 | BOOT | BOOT |
| 1 | PA enable | Not assigned in current mapping |
| 2 | SD D2 | Not assigned |
| 3 | SD D3 | LCD DC |
| 4 | SD CMD | Expansion IO |
| 5 | SD CLK | Expansion IO |
| 6 | SD D0 | Expansion IO |
| 7 | SD D1 | Expansion IO |
| 8 | Battery ADC | Touch INT |
| 9 | LCD QSPI D3 | Touch CS |
| 10 | LCD CS | LCD CS |
| 11 | LCD QSPI D0 | Audio BCLK |
| 12 | LCD QSPI CLK | Audio WS |
| 13 | LCD QSPI D1 | Audio speaker DOUT |
| 14 | LCD QSPI D2 | Audio microphone DIN |
| 15 | Audio speaker DOUT | LCD MISO |
| 16 | Audio microphone DIN | LCD backlight |
| 17 | Audio MCLK | LCD SCLK |
| 18 | Audio BCLK | LCD MOSI |
| 19 | USB D- | USB D- |
| 20 | USB D+ | USB D+ |
| 21 | Audio WS | Not assigned |
| 38 | I2C SDA (touch + ES8311) | Not assigned |
| 39 | I2C SCL (touch + ES8311) | Not assigned |
| 40 | WS2812B RGB LED | Not assigned |
| 41 | LCD backlight | Not assigned |
| 43 | UART RX | UART TX |
| 44 | UART TX | UART RX |
| 45 | Expansion IO | Not assigned |
| 46 | Expansion IO | LCD RESET |
| 47 | Touch INT (declared; polling in supplied code) | Not assigned |
| 48 | Touch RESET | RGB LED |

## 5. Optional DIY OLED

| Function | DIY N16R8 |
|---|---|
| OLED controller | SH1106 |
| Interface | I2C |
| SDA | GPIO1 |
| SCL | GPIO2 |

The OLED is not part of the current V1 LCD + touch integration target.

## 6. Important Design Differences

1. **GPIO11–18:** LCD Wiki assigns many of these pins to QSPI display and I2S audio. DIY reallocates them across SPI display, touch and direct-I2S audio.
2. **GPIO8–10 and GPIO46–48:** Peripheral roles differ substantially between the two boards.
3. **Audio:** LCD Wiki uses an ES8311 codec; DIY uses an INMP441 digital microphone and MAX98357A I2S amplifier without a separate audio codec.
4. **Display/touch:** LCD Wiki combines ST77922 QSPI display and capacitive I2C touch; DIY uses ST7789 SPI display and XPT2046 SPI touch.
5. **microSD:** LCD Wiki's SDIO pins are assigned. The DIY module's SD socket pads are currently not connected to the ESP32, so no DIY SD GPIO assignment is included.

## 7. Source and Verification Notes

### LCD Wiki 3.5-inch
Based on the supplied project files:
- `config.h`
- `config.json`
- `lcdwiki-es3c35p.cc`
- `README.md`

### DIY ESP32-S3 N16R8
Based on the pin mapping and schematic details established in the project conversation.

### Items to verify during hardware integration
- Confirm DIY UART TX/RX naming against the schematic connector labels.
- Confirm the DIY RGB LED implementation and polarity.
- Confirm exact I2S full-duplex configuration and sample rate for INMP441 + MAX98357A.
- Confirm LCD reset behavior on the DIY board.
- Do not treat an unassigned GPIO as guaranteed free; check the full schematic and ESP32-S3 strapping/USB/debug requirements before reusing it.
