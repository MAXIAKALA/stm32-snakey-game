# stm32-snakey-game

A small **Snake** game running on an STM32F103 board, built with **Keil MDK** and the **STM32F10x Standard Peripheral Library** (bare metal, no RTOS).

## Hardware

- STM32F103 series MCU
- OLED display (driver in `User/dev`)
- 2-axis slider / joystick read through ADC (`x_voltage`, `y_voltage`)
- `START` button
- Status LED
- High score stored in on-chip Flash (`data_store`)

## Controls

| Action | Input |
|---|---|
| Move | Slide / joystick x / y |
| Start / restart | `START` button |
| Speed up | Hold `START` while playing (tick 500 ms → 150 ms) |

## Features

- Boot animation and three UI pages: `HOME` / `GAME` / `RESULT`
- Random food placement, growth, wall/self collision, score
- High score persisted in Flash
- Layered firmware: `app` / `bsp` / `dev` / `sys` / `ui`

## Build

1. Open `Project/Project.uvprojx` in Keil MDK (ARMCC).
2. Build the `Target 1` target.
3. Flash the generated `.hex` / `.axf` to the board.

`keilkill.bat` removes Keil build artifacts on Windows.

## Layout

```
User/app       main loop and game logic
User/bsp       button / LED / slider drivers
User/dev       OLED driver
User/sys       clock, timer, delay, log, Flash data store
User/ui        pages, font, BMP
Project        Keil MDK project (Project.uvprojx)
Libraries      CMSIS + STM32F10x StdPeriph
Doc            original notes
```

## Notes

- Source comments are **GBK-encoded**; GitHub may show garbled Chinese. Keil on a Chinese Windows install displays them correctly.
- `bsp_battery.c` is **not included** in this snapshot: the original archive only had old object files for it, and the current Keil project does not reference it.
- The original archive also contained build output and an unrelated `Project/fs` header (electricity-meter code); those were removed when preparing this repo.
- No license is declared. Add one if you plan to reuse the code.
