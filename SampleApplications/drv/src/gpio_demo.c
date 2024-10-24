
#include "mbtk_gpio.h"
#include "mbtk_api.h"
#include "mbtk_comm_api.h"

#define demo_pin mbtk_pin_58
mbtk_hisrref gpio_demo_hisr;

void gpio_demo_isr_callback(void)
{
    ol_diable_pin_wakeup_edge_detect(demo_pin, mbtk_gpio_config_edge_both);
    ol_disable_pin_edge_detect(demo_pin, mbtk_gpio_config_edge_both);
    ol_os_active_hisr(&gpio_demo_hisr);
}

void gpio_demo_hisr_func(void)
{
    op_uart_printf("gpio_demo_hisr_func enter \n");
    ol_enable_pin_wakeup_edge_detect(demo_pin, mbtk_gpio_config_edge_both);
    ol_enable_pin_edge_detect(demo_pin, mbtk_gpio_config_edge_both);
}

int gpio_demo_output(void)
{
    uint8_t dir = 0;
    mbtk_pin_config_struct config;
    config.gpio_af_num = mbtk_gpio_config_maf0;
    config.gpio_pull = mbtk_gpio_config_pull_low;
    config.gpio_sleep = mbtk_gpio_config_sleep_none;
    config.gpio_edge = mbtk_gpio_config_edge_none;
    
    op_uart_printf("gpio_demo_output run\n");
    if (ol_pin_config(demo_pin, &config) != 0)
    {
        op_uart_printf("gpio_demo_output : mbtk_pin_config fail \n");
        return -1;
    }

    if (ol_set_pin_dir(demo_pin, mbtk_gpio_dir_output) != 0)
    {
        op_uart_printf("gpio_demo_output : mbtk_set_pin_dir_input fail \n");
        return -1;
    }

    if (ol_get_pin_dir(demo_pin, &dir) != 0)
    {
        op_uart_printf("mbkt_gpio_demo : mbtk_get_pin_dir fail \n");
        return -1;
    }

    op_uart_printf("gpio_demo_output : mbtk_get_pin_dir = %d \n", dir);
    if (ol_set_pin_level(demo_pin, mbtk_gpio_level_low) != 0)
    {
        op_uart_printf("gpio_demo_output : mbtk_set_pin_level fail \n");
        return -1;
    }
    op_uart_printf("check demo_pin level is high\n");
    return 0;
}

int gpio_demo_input(void)
{
    uint8_t dir = 0;
    mbtk_pin_config_struct config;
    config.gpio_af_num = mbtk_gpio_config_maf0;
    config.gpio_pull = mbtk_gpio_config_pull_low;
    config.gpio_sleep = mbtk_gpio_config_sleep_none;
    config.gpio_edge = mbtk_gpio_config_edge_none;

    op_uart_printf("gpio_demo_input run\n");
    if (ol_pin_config(demo_pin, &config) != 0)
    {
        op_uart_printf("gpio_demo_input : mbtk_pin_config fail \n");
        return -1;
    }

    if (ol_set_pin_dir(demo_pin, mbtk_gpio_dir_input) != 0)
    {
        op_uart_printf("gpio_demo_input : mbtk_gpio_dir_input fail \n");
        return -1;
    }

    if (ol_get_pin_dir(demo_pin, &dir) != 0)
    {
        op_uart_printf("gpio_demo_input : mbtk_get_pin_dir fail \n");
        return -1;
    }

    op_uart_printf("gpio_demo_input : mbtk_get_pin_dir = %d \n", dir);
    return 0;
}

int gpio_demo_isr_mode(void)
{
    uint8_t dir = 0;
    mbtk_pin_config_struct config;
    config.gpio_af_num = mbtk_gpio_config_maf0;
    config.gpio_pull = mbtk_gpio_config_pull_low;
    config.gpio_sleep = mbtk_gpio_config_sleep_none;
    config.gpio_edge = mbtk_gpio_config_edge_none;

    op_uart_printf("gpio_demo_isr_mode run\n");
    if (ol_pin_config(demo_pin, &config) != 0)
    {
        op_uart_printf("gpio_demo_isr_mode : mbtk_pin_config fail \n");
        return -1;
    }

    if (ol_set_pin_dir(demo_pin, mbtk_gpio_dir_input) != 0)
    {
        op_uart_printf("gpio_demo_isr_mode : mbtk_gpio_dir_input fail \n");
        return -1;
    }

    if (ol_get_pin_dir(demo_pin, &dir) != 0)
    {
        op_uart_printf("gpio_demo_isr_mode : mbtk_get_pin_dir fail \n");
        return -1;
    }

    ol_os_creat_hisr(&gpio_demo_hisr, "gpio_demo_hisr", gpio_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);
    if (ol_enable_pin_edge_detect(demo_pin, mbtk_gpio_rising_edge) != 0)
    {
        op_uart_printf("gpio_demo_isr_mode : mbtk_enable_pin_edge_detect fail \n");
        return -1;
    }

    if (ol_bind_pin_irq_callback(demo_pin, gpio_demo_isr_callback) != 0)
    {
        op_uart_printf("gpio_demo_isr_mode : mbtk_pin_bind_irq_callback fail \n");
        return -1;
    }

    op_uart_printf("gpio_demo_isr_mode : test gpio hisr func \n");
    return 0;
}

int config_demo_pin_wakeup(void)
{
    mbtk_pin_config_struct config;
    config.gpio_af_num = mbtk_gpio_config_maf0;
    config.gpio_pull = mbtk_gpio_config_pull_low;
    config.gpio_sleep = mbtk_gpio_config_sleep_none;
    config.gpio_edge = mbtk_gpio_config_edge_both;

    if(ol_pin_config(demo_pin, &config) != 0)
    {
        op_uart_printf("config_demo_pin_wakeup : mbtk_pin_config fail \n");
        return -1;
    }

    if(ol_set_pin_dir(demo_pin, mbtk_gpio_dir_input) != 0)
    {
        op_uart_printf("config_demo_pin_wakeup : mbtk_gpio_dir_input fail \n");
        return -1;
    }

    ol_os_creat_hisr(&gpio_demo_hisr, "gpio_demo_hisr", gpio_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);
    if(ol_pin_bind_wakeup_callback(demo_pin, gpio_demo_isr_callback) != 0)
    {
        op_uart_printf("config_demo_pin_wakeup : ol_pin_bind_wakeup_callback fail \n");
        return -1;
    }

    if(ol_enable_pin_wakeup_edge_detect(demo_pin, mbtk_gpio_config_edge_both) != 0)
    {
        op_uart_printf("config_demo_pin_wakeup : ol_enable_pin_wakeup_edge_detect fail \n");
        return -1;
    }

    op_uart_printf("config_demo_pin_wakeup : config_demo_pin_wakeup end \n");
    return 0;
}

