# tinycli

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

A lightweight, transport-agnostic CLI engine for embedded systems.

No UART dependency. No HAL dependency. Works over any byte stream: UART, USB CDC, socket, ITM — your choice.

## Features

- Command table registration
- Built-in `help` command
- Local echo
- Backspace support
- No dynamic memory allocation

## Integration

### As a git submodule

```bash
git submodule add https://github.com/eaytur/tinycli libs/tinycli
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory(libs/tinycli)
target_link_libraries(your_target PRIVATE tinycli)
```

### Usage

```c
#include "cli.h"

/* 1. provide a write function for your transport */
static void uart_write(const char *data, uint16_t len)
{
    HAL_UART_Transmit(&huart, (uint8_t *)data, len, HAL_MAX_DELAY);
}

/* 2. define your commands */
static void cmd_version(int argc, char *argv[])
{
    cli_println("v1.0.0");
}

static const cli_cmd_t cmds[] = {
    { "version", "Show firmware version", cmd_version },
};

/* 3. init */
cli_init(uart_write);
cli_register_table(cmds, 1);

/* 4. feed bytes from your transport (e.g. in UART RX callback) */
cli_process_byte(byte);
```

## API

```c
void cli_init(cli_write_fn_t write_fn);
void cli_register_table(const cli_cmd_t *table, uint8_t count);
void cli_process_byte(uint8_t byte);
void cli_print(const char *str);
void cli_println(const char *str);
```

## Running Tests

Requires a native GCC toolchain. On Windows, use MSYS2 MinGW64.

```bash
cmake -S tests -B build/tests -G "Ninja"
cmake --build build/tests
./build/tests/test_cli.exe
```

See [tests/README.md](tests/README.md) for details.

## Limits

| Parameter | Default | Macro |
|-----------|---------|-------|
| Line buffer | 128 bytes | `CLI_LINE_BUF_SIZE` |
| Max commands | 16 | `CLI_MAX_CMDS` |
| Max arguments | 8 | `CLI_MAX_ARGS` |
