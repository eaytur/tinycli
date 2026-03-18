# tinycli — Tests

Unit tests that run on the host PC. No hardware required.

## Requirements

| Tool | Version |
|------|---------|
| GCC (native) | 13+ |
| CMake | 3.22+ |
| Ninja | any |

> **Windows users:** MSYS2 MinGW64 environment is recommended.
> Mixing toolchains (e.g. STM32CubeCLT CMake + MSYS2 GCC) causes incompatibilities.

## Setup (Windows — MSYS2)

1. Install MSYS2 from [msys2.org](https://www.msys2.org).
2. Open **MSYS2 MinGW x64** terminal:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```

## Build and Run

From the `tinycli` root directory (in **MSYS2 MinGW64** terminal):

```bash
cmake -S tests -B build/tests -G "Ninja"
cmake --build build/tests
./build/tests/test_cli.exe
```

Expected output:

```
PASS  test_ping_command
PASS  test_echo_command
PASS  test_help_lists_commands
PASS  test_unknown_command
PASS  test_empty_input_no_crash
PASS  test_backspace

All tests passed.
```

## Adding a New Test

Use the `TEST` macro in `test_cli.c` and register it with `RUN` in `main`:

```c
TEST(test_my_command)
{
    setup();
    feed("my_command arg1\r");
    ASSERT_CONTAINS("expected output");
}

int main(void)
{
    /* ... existing tests ... */
    RUN(test_my_command);
}
```

### Available Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_CONTAINS(str)` | Output must contain `str` |
| `ASSERT_NOT_CONTAINS(str)` | Output must not contain `str` |

### Helper Functions

| Function | Description |
|----------|-------------|
| `feed("cmd\r")` | Feeds a string into the CLI byte by byte |
| `setup()` | Resets CLI state before each test |
| `output_reset()` | Clears the output buffer |

## Architecture

Tests compile the real `cli.c` directly — no BSP layer involved.
`mock_write()` replaces UART and captures CLI output into a string buffer.

```
test_cli.c
    │
    ├── mock_write()   ← captures output into a string buffer instead of UART
    ├── feed()         ← sends bytes via cli_process_byte()
    │
    └── cli.c          ← real implementation, compiled unchanged
```
