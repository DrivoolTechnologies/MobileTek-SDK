#include "mbtk_uart.h"
#include "mbtk_gpio.h"
#include "mbtk_comm_api.h"
#include "mbtk_os.h"

static bool uart_quit =0;


//***********************************************************
//*PIN configuration needs to be adapted to different hardware versions. 
//*For details, see the gpio manual. 
//*This example is for L510CN
//***********************************************************
#define UART_TX	mbtk_pin_54
#define UART_RX mbtk_pin_53

int demo_uart_pin_config(void)
{
	mbtk_pin_config_struct config;
	
  config.gpio_af_num = mbtk_gpio_config_maf1;
  config.gpio_pull = mbtk_gpio_config_pull_high;
  config.gpio_sleep = mbtk_gpio_config_sleep_none;
  config.gpio_edge = mbtk_gpio_config_edge_none;
    
  if (ol_pin_config(UART_TX, &config) != 0)
  {
    op_uart_printf("gpio_demo_output : mbtk_pin_config fail \n");
    return -1;
  }

	if (ol_pin_config(UART_RX, &config) != 0)
  {
    op_uart_printf("gpio_demo_output : mbtk_pin_config fail \n");
    return -1;
  }
	return 0;
}

void demo_uart_cb(MBTK_UART_Port p)
{
	u8 data[100] = {0};
	u16 read_len = 0;
	
	ol_Uart_Read(p, data, 100, &read_len);
	if(read_len>0)
	{
		if(read_len == 4 && memcmp(data,"quit",4)==0)
		{
			uart_quit = 1;
			return;
		}
		ol_Uart_Write(p, data , read_len);
	}
	else
		op_uart_printf("\r\nol_Uart_Read NULL");

}

void uart_demo(void)
{
	OL_UART_DCB uart_dcb = {0};
	int err = 0;

	op_uart_printf("\r\nuart_demo start");
	err = demo_uart_pin_config();
	if(err != 0)
		return;
	err = ol_Uart_GetDcb(OL_UART_PORT_STUART, &uart_dcb);
	//默认会是中断模式
	if(OL_UART_RC_OK != err)
	{
		op_uart_printf("\r\nuart_demo ol_Uart_GetDcb error %d", err);
		return ;
	}

	op_uart_printf("uart_demo opMode:%d, baudRate:%d, numDataBits:%d, parityBitType:%d,flowControl%d\r\n",
	                           uart_dcb.opMode, uart_dcb.baudRate, uart_dcb.numDataBits, uart_dcb.parityBitType, uart_dcb.flowControl);

	uart_dcb.rd_cb = demo_uart_cb;
	
	ol_Uart_SetDcb(OL_UART_PORT_STUART, &uart_dcb);

	err = ol_Uart_Open(OL_UART_PORT_STUART);
	
	if(OL_UART_RC_OK == err)
	{
		while(1)
		{
			ol_os_task_sleep(100);
			//sleep 500ms

			if(uart_quit)
				break;
		}
	}

	op_uart_printf("\r\nuart_demo finish");
	
}


