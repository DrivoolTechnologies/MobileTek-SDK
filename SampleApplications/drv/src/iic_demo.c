#include "mbtk_comm_api.h"
#include "mbtk_i2c.h"
#include "mbtk_gpio.h"

//***********************************************************
//*PIN configuration needs to be adapted to different hardware versions. 
//*For details, see the gpio manual. 
//*This example is for L510CN
//***********************************************************
#define IIC_SCL	mbtk_pin_43
#define IIC_SDA	mbtk_pin_44

int demo_iic_pin_config(void)
{
	mbtk_pin_config_struct config;
	
  config.gpio_af_num = mbtk_gpio_config_maf1;
  config.gpio_pull = mbtk_gpio_config_pull_high;
  config.gpio_sleep = mbtk_gpio_config_sleep_none;
  config.gpio_edge = mbtk_gpio_config_edge_none;
    
  if (ol_pin_config(mbtk_pin_43, &config) != 0)
  {
    op_uart_printf("gpio_demo_output : mbtk_pin_config fail \n");
    return -1;
  }

	if (ol_pin_config(mbtk_pin_44, &config) != 0)
  {
    op_uart_printf("gpio_demo_output : mbtk_pin_config fail \n");
    return -1;
  }
	return 0;
}



int iic_demo(void)
{	
	int read_len = 0;
	char read_buf[20] = {0};
	uint8_t slave_addr = 0x01;
	uint8_t cmdbuf[] = {0x78, 0x89};

	demo_iic_pin_config();

	if(ol_i2c_init(mbtk_i2c_devno_1, mbtk_i2c_standrd_mode) != 0)
	{
		op_uart_printf("mbtk_i2c_demo : mbtk_i2c_init fail \n");
		return -1;
	}

	if(ol_i2c_write(mbtk_i2c_devno_1, slave_addr, cmdbuf,2, "mbtk_i2c_demo", strlen("mbtk_i2c_demo")) < 0)
	{
		op_uart_printf("mbtk_i2c_demo: mbtk_i2c_write fail \n");
		return -1;
	}

	if(ol_i2c_read(mbtk_i2c_devno_1, slave_addr,cmdbuf,2, read_buf, 20) < 0)
	{
		op_uart_printf("mbtk_i2c_demo: mbtk_i2c_read fail \n");
		return -1;
	}
	op_uart_printf("mbtk_i2c_demo: mbtk_i2c_read : %s \n", read_buf);

	op_uart_printf("mbtk_i2c_demo : test success \n");
	return 0;
}


