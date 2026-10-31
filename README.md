# c_firmware_demo

Bare-metal C firmware for an ARM Cortex-M3, run on a Mac via QEMU (no hardware needed).
A C counterpart of [rust_firmware_demo](https://github.com/ttsukahara967/rust_firmware_demo): same output, same QEMU machine, but with no runtime crates or C library — the startup code, linker script and semihosting are all written by hand.

## Setup

```bash
brew install qemu arm-none-eabi-gcc
```

## Run

```bash
make run
```

Expected output:

```
Hello from bare-metal C!
SysTick #1
...
SysTick #5
CPUID = 0x410fc231
```

## Files

| File | Role | Rust version equivalent |
| --- | --- | --- |
| `src/startup.c` | Vector table, reset handler (copy `.data`, zero `.bss`, call `main`) | `cortex-m-rt` (`#[entry]`) |
| `link.ld` | Flash / RAM layout and section placement | `memory.x` + `link.x` from `cortex-m-rt` |
| `src/semihosting.h` | `BKPT 0xAB` calls: open `:tt`, write, exit | `cortex-m-semihosting` (`hprintln!`, `debug::exit`) |
| `src/main.c` | SysTick polling and CPUID read via raw register addresses | `cortex-m` (`Peripherals::take()`) |
| `Makefile` | Cross-compile with `arm-none-eabi-gcc`, run with QEMU | `.cargo/config.toml` |

## Things to compare with the Rust version

- **Hardware access**: registers are plain `volatile` pointers to fixed addresses. Nothing stops two parts of the code from configuring SysTick at the same time; in Rust, `Peripherals::take()` makes that a compile error.
- **No C library**: built with `-nostdlib -ffreestanding`, so there is no `printf`, `memcpy` or `memset`. Numbers are formatted by hand, and `-fno-tree-loop-distribute-patterns` stops GCC from turning the reset handler's loops into `memcpy`/`memset` calls.
- **Size**: `make size` reports about 470 bytes, versus about 4.7 KB for the Rust version, which links in `core::fmt` for `hprintln!`.
- **Vector table**: `xxd -l 8 build/fw-demo.bin` shows the initial SP (`0x20010000`) and the reset handler address with the Thumb bit set.
