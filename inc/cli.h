#ifndef CLI_H
#define CLI_H

#include <stdint.h>

typedef void (*cli_handler_t)(int argc, char *argv[]);
typedef void (*cli_write_fn_t)(const char *data, uint16_t len);

typedef struct
{
    const char     *name;
    const char     *help;
    cli_handler_t   handler;
} cli_cmd_t;

void cli_init(cli_write_fn_t write_fn);
void cli_register_table(const cli_cmd_t *table, uint8_t count);
void cli_process_byte(uint8_t byte);
void cli_print(const char *str);
void cli_println(const char *str);

#endif /* CLI_H */
