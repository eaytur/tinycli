#include "cli.h"
#include <string.h>

#define CLI_LINE_BUF_SIZE   128U
#define CLI_MAX_CMDS        16U
#define CLI_MAX_ARGS        8U

static cli_cmd_t    s_cmds[CLI_MAX_CMDS];
static uint8_t      s_cmd_count = 0;

static char         s_line_buf[CLI_LINE_BUF_SIZE];
static uint8_t      s_line_len = 0;

static cli_write_fn_t s_write = NULL;

static const char *PROMPT = "\r\n> ";
static const char *CRLF   = "\r\n";

/* -------------------------------------------------------------------------- */

void cli_print(const char *str)
{
    if (s_write != NULL)
    {
        s_write(str, (uint16_t)strlen(str));
    }
}

void cli_println(const char *str)
{
    cli_print(str);
    cli_print(CRLF);
}

/* -------------------------------------------------------------------------- */

static void cmd_help(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    cli_println("Available commands:");
    for (uint8_t i = 0; i < s_cmd_count; i++)
    {
        cli_print("  ");
        cli_print(s_cmds[i].name);
        cli_print("\t- ");
        cli_println(s_cmds[i].help);
    }
}

/* -------------------------------------------------------------------------- */

void cli_init(cli_write_fn_t write_fn)
{
    static const cli_cmd_t builtin[] = {
        { "help", "List all available commands", cmd_help },
    };

    s_write     = write_fn;
    s_cmd_count = 0;
    s_line_len  = 0;
    cli_register_table(builtin, 1);
    cli_print(PROMPT);
}

void cli_register_table(const cli_cmd_t *table, uint8_t count)
{
    for (uint8_t i = 0; i < count && s_cmd_count < CLI_MAX_CMDS; i++)
    {
        s_cmds[s_cmd_count++] = table[i];
    }
}

/* -------------------------------------------------------------------------- */

static void dispatch(void)
{
    if (s_line_len == 0)
    {
        return;
    }

    char  *argv[CLI_MAX_ARGS];
    int    argc  = 0;
    char  *token = strtok(s_line_buf, " ");

    while (token != NULL && argc < CLI_MAX_ARGS)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0)
    {
        return;
    }

    for (uint8_t i = 0; i < s_cmd_count; i++)
    {
        if (strcmp(argv[0], s_cmds[i].name) == 0)
        {
            cli_print(CRLF);
            if (s_cmds[i].handler != NULL)
            {
                s_cmds[i].handler(argc, argv);
            }
            return;
        }
    }

    cli_print(CRLF);
    cli_print("Unknown command: ");
    cli_println(argv[0]);
    cli_println("Type 'help' to see available commands.");
}

void cli_process_byte(uint8_t byte)
{
    switch (byte)
    {
        case '\r':
        case '\n':
            s_line_buf[s_line_len] = '\0';
            dispatch();
            s_line_len = 0;
            cli_print(PROMPT);
            break;

        case 0x7F: /* backspace (DEL) */
        case '\b':
            if (s_line_len > 0)
            {
                s_line_len--;
                cli_print("\b \b");
            }
            break;

        default:
            if (s_line_len < CLI_LINE_BUF_SIZE - 1U)
            {
                s_line_buf[s_line_len++] = (char)byte;
                if (s_write != NULL)
                {
                    s_write((const char *)&byte, 1); /* local echo */
                }
            }
            break;
    }
}
