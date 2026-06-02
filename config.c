#include "config.h"
#include "uart_drv.h"
#include "string.h"

#define TEMP_MAX_DEFAULT 25U
#define TEMP_MAX_LOWER_LIMIT 0U
#define TEMP_MAX_UPPER_LIMIT 60U

static uint8_t s_temp_max;

typedef struct
{
    const char *key;
    void (*handler)(const char *value_str);
} config_cmd_t;

static void cmd_set_temp_max(const char *value_str);

static const config_cmd_t s_commands[] =
    {
        {"TEMP_MAX", cmd_set_temp_max}};

#define CONFIG_NUM_COMMANDS (sizeof(s_commands) / sizeof(s_commands[0]))

// Decimal string -> unsigned. Returns 0 if empty, non-digit, or out of uint16.
static int parse_uint(const char *s, uint16_t *out)
{
    uint16_t value = 0;
    int digits = 0;

    while (*s != '\0')
    {
        if (*s < '0' || *s > '9' || value > 6553U)
        {
            return 0;
        }
        value = (uint16_t)(value * 10U + (uint16_t)(*s - '0'));
        digits++;
        s++;
    }

    if (digits == 0)
    {
        return 0;
    }

    *out = value;
    return 1;
}

static void cmd_set_temp_max(const char *value_str)
{
    uint16_t value;

    if (!parse_uint(value_str, &value))
    {
        uart_send_string("ERROR: \"");
        uart_send_string(value_str);
        uart_send_string("\" is invalid\r\n");
        return;
    }

    if (value < TEMP_MAX_LOWER_LIMIT || value > TEMP_MAX_UPPER_LIMIT)
    {
        uart_send_string("ERROR: \"");
        uart_send_string(value_str);
        uart_send_string("\" is out of range (");
        uart_send_uint(TEMP_MAX_LOWER_LIMIT);
        uart_send_string(" - ");
        uart_send_uint(TEMP_MAX_UPPER_LIMIT);
        uart_send_string(")\r\n");
        return;
    }

    s_temp_max = (uint8_t)value;

    uart_send_string("OK: TEMP_MAX=");
    uart_send_uint(s_temp_max);
    uart_send_string("\r\n");
}

void config_init(void)
{
    s_temp_max = TEMP_MAX_DEFAULT;
}

uint8_t config_get_temp_max(void)
{
    return s_temp_max;
}

void config_process_line(const char *line)
{
    const char *eq;
    uint16_t key_len;
    uint16_t i;

    eq = strchr(line, '=');
    if (eq == 0)
    {
        uart_send_string("ERROR: format (use KEY=value)\r\n");
        return;
    }

    key_len = (uint16_t)(eq - line);

    for (i = 0; i < CONFIG_NUM_COMMANDS; i++)
    {
        if (strlen(s_commands[i].key) == key_len &&
            strncmp(line, s_commands[i].key, key_len) == 0)
        {
            s_commands[i].handler(eq + 1); // skips '='
            return;
        }
    }

    uart_send_string("ERROR: unknown command\r\n");
}
