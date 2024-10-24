#include "mbtk_comm_api.h"
#include "mbtk_spi.h"
#include "mbtk_gpio.h"

#define SSP0_RXD	mbtk_pin_73
#define SSP0_TXD	mbtk_pin_74
#define SSP0_FRM	mbtk_pin_75
#define SSP0_CLK	mbtk_pin_76

int spi_demo(void)
{
	int read_buf[50] = {0};
  mbtk_pin_config_struct config;
  config.gpio_af_num = mbtk_gpio_config_maf2;
  config.gpio_pull = mbtk_gpio_config_pull_none;
  config.gpio_sleep = mbtk_gpio_config_sleep_none;
  config.gpio_edge = mbtk_gpio_config_edge_none;
    
  op_uart_printf("mbtk_spi_demo : spi gpio config first\n");
  if (ol_pin_config(SSP0_RXD, &config) != 0)
  {
    op_uart_printf("mbtk_spi_demo : SSP0_RXD fail \n");
    return -1;
  }

	if (ol_pin_config(SSP0_TXD, &config) != 0)
  {
    op_uart_printf("mbtk_spi_demo : SSP0_TXD fail \n");
    return -1;
  }

	 if (ol_pin_config(SSP0_FRM, &config) != 0)
  {
    op_uart_printf("mbtk_spi_demo : SSP0_FRM fail \n");
    return -1;
  }

	if (ol_pin_config(SSP0_CLK, &config) != 0)
  {
    op_uart_printf("mbtk_spi_demo : SSP0_CLK fail \n");
    return -1;
  }
	op_uart_printf("mbtk_spi_demo: pin config finish\n");

		
	if(ol_spi_init(mbtk_spi_index_0, mbtk_spi_mode0, mbtk_spi_clk_3_25M) != 0)
	{
		op_uart_printf("mbtk_spi_demo : mbtk_spi_init fail \n");
		return -1;
	}
	
	if(ol_spi_write(mbtk_spi_index_0, "mbtk_spi_demo", strlen("mbtk_spi_demo")) != 0)
	{
		op_uart_printf("mbtk_spi_demo : mbtk_spi_write fail \n");
		return -1;
	}

	if(ol_spi_read(mbtk_spi_index_0, read_buf, 20) != 0)
	{
		op_uart_printf("mbtk_spi_demo : mbtk_spi_read fail \n");
		return -1;
	}
	op_uart_printf("mbtk_spi_demo : mbtk_spi_read : %s", read_buf);
	memset(read_buf, 0, sizeof(read_buf));	

	if(ol_spi_write_read(mbtk_spi_index_0, read_buf, "mbtk_spi_demo_write_read", strlen("mbtk_spi_demo_write_read")) != 0)
	{
		op_uart_printf("mbtk_spi_demo : mbtk_spi_write_read fail \n");
		return -1;
	}
	op_uart_printf("mbtk_spi_demo : mbtk_spi_write_read : %s \n", read_buf);
	op_uart_printf("mbtk_spi_demo : test successsss \n");

	return 0;
}


