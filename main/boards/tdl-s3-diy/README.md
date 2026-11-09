# TDL-S3-DIY OLED build variant

This package adds an SH1106 128x64 I2C OLED build variant to the existing
`main/boards/tdl-s3-diy` board. It does **not** add a new physical board type.

## Files

- `main/boards/tdl-s3-diy/config.json` — two firmware names; OLED variant sets `CONFIG_TDL_S3_DIY_DISPLAY_OLED=y`.
- `main/boards/tdl-s3-diy/config.h` — shared board pins plus OLED dimensions.
- `main/boards/tdl-s3-diy/tdl_s3_diy.cc` — one board class with compile-time LCD/OLED display selection.
- `patches/Kconfig.projbuild.patch` — adds the build-variant Kconfig symbol.
- `build-tdl-s3-diy-oled.sh` — convenience build script for the OLED variant.

## Apply

From the repository root, copy the three files under `main/boards/tdl-s3-diy/`
over the existing files, then apply the Kconfig patch:

```bash
git apply /path/to/tdl-s3-diy-oled-variant/patches/Kconfig.projbuild.patch
```

Or apply the patch from the package root. Do not create another board directory,
add a new `BOARD_TYPE`, or add another `DECLARE_BOARD`.

## Build

```bash
python scripts/build.py tdl-s3-diy --name tdl-s3-diy-lcd
./build-tdl-s3-diy-oled.sh
```

The existing `main/idf_component.yml` already declares
`tny-robotics/sh1106-esp-idf: ^1.0.1`; the OLED code uses its documented
`esp_lcd_panel_sh1106.h` API and `ESP_SH1106_DEFAULT_IO_CONFIG`.

## Hardware assumptions to verify

- OLED: SH1106, 128x64, I2C address 0x3C, SDA GPIO1, SCL GPIO2.
- OLED module has no separate reset/backlight pin.
- Internal I2C pull-ups are enabled in the bus configuration; if the module
  already has pull-ups this is normally harmless, but verify the electrical
  setup if bus errors occur.

## Important validation note

This source package has been prepared against repository `dev` HEAD
`8f5320e7d5ceb6c366605c8ec26f983287ebb24c`. It has **not** been compiled or
flashed on the physical OLED module in this environment. The SH1106 component
API and project `OledDisplay` constructor were checked against source, but the
first hardware build and display orientation/contrast still need validation.
