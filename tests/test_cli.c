#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ---- test infrastructure ------------------------------------------------- */

static char   s_output[1024];
static size_t s_output_len;

static void mock_write(const char *data, uint16_t len)
{
    if (s_output_len + len < sizeof(s_output))
    {
        memcpy(s_output + s_output_len, data, len);
        s_output_len += len;
        s_output[s_output_len] = '\0';
    }
}

static void output_reset(void)
{
    memset(s_output, 0, sizeof(s_output));
    s_output_len = 0;
}

static void feed(const char *str)
{
    while (*str)
    {
        cli_process_byte((uint8_t)*str++);
    }
}

#define TEST(name)      static void name(void)
#define RUN(name)       do { output_reset(); name(); printf("PASS  %s\n", #name); } while(0)
#define ASSERT_CONTAINS(substr) \
    assert(strstr(s_output, substr) != NULL && "expected: " substr)
#define ASSERT_NOT_CONTAINS(substr) \
    assert(strstr(s_output, substr) == NULL && "not expected: " substr)

/* ---- dummy commands for tests -------------------------------------------- */

static void cmd_ping(int argc, char *argv[])
{
    (void)argc; (void)argv;
    cli_println("pong");
}

static void cmd_echo(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        cli_print(argv[i]);
        if (i < argc - 1) cli_print(" ");
    }
    cli_println("");
}

static const cli_cmd_t s_test_cmds[] = {
    { "ping", "Reply with pong", cmd_ping },
    { "echo", "Echo arguments",  cmd_echo },
};

static void setup(void)
{
    output_reset();
    cli_init(mock_write);
    cli_register_table(s_test_cmds, 2);
    output_reset(); /* clear prompt output from cli_init */
}

/* ---- tests --------------------------------------------------------------- */

TEST(test_ping_command)
{
    setup();
    feed("ping\r");
    ASSERT_CONTAINS("pong");
}

TEST(test_echo_command)
{
    setup();
    feed("echo hello world\r");
    ASSERT_CONTAINS("hello world");
}

TEST(test_help_lists_commands)
{
    setup();
    feed("help\r");
    ASSERT_CONTAINS("ping");
    ASSERT_CONTAINS("echo");
    ASSERT_CONTAINS("help");
}

TEST(test_unknown_command)
{
    setup();
    feed("foobar\r");
    ASSERT_CONTAINS("Unknown command");
    ASSERT_CONTAINS("foobar");
}

TEST(test_empty_input_no_crash)
{
    setup();
    feed("\r");
    /* only the prompt should appear, must not crash */
    ASSERT_CONTAINS(">");
}

TEST(test_backspace)
{
    setup();
    feed("pinx\b");   /* type pinx, erase last char */
    feed("g\r");      /* append g → ping */
    ASSERT_CONTAINS("pong");
}

/* -------------------------------------------------------------------------- */

int main(void)
{
    RUN(test_ping_command);
    RUN(test_echo_command);
    RUN(test_help_lists_commands);
    RUN(test_unknown_command);
    RUN(test_empty_input_no_crash);
    RUN(test_backspace);

    printf("\nAll tests passed.\n");
    return 0;
}
