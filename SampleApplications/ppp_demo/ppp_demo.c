#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "ol_nw_api.h"
#include "ol_ppp_api.h"

#define TASK_STACK_SIZE 1024*4
#define TASK_PRIORITY 100

#define PPP_DEMO_LOCAL_CID mbtk_cid_index_1
#define LOCAL_CID_PDN_TYPE mbtk_data_call_v4v6

#define DEMO_TASK_NAME "ppp_demo_task"

int ppp_terminate = 0;

int ppp_demo_wait_network(void)
{
	if(ol_wait_network_regist(120) != mbtk_data_call_ok)
	{
		op_uart_printf("socket_demo_wait_network ol_wait_network_regist time out\n");
		return -1;
	}
	op_uart_printf("socket_demo_wait_network execute ol_data_call_start \n");
	if(ol_data_call_start(PPP_DEMO_LOCAL_CID, LOCAL_CID_PDN_TYPE, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
	{
		op_uart_printf("socket_demo_wait_network ol_data_call_start fail\n");
		return -1;
	}

	return 0;
}

void ppp_demo_status_callback(int status)
{
	op_uart_printf("***ppp demo status callback status %d***\r\n",status);

	switch(status){
		case ppp_status_connected:
			op_uart_printf("ppp connected!!!\r\n");
			break;
		case ppp_status_disconnect:
			op_uart_printf("ppp disconnected!!!\r\n");
			ppp_terminate = 1;
			break;
	}
}

void ppp_demo_task(void *data)
{
	int ret = 0;
	unsigned int loop_count = 30;//wait 5min

	ppp_terminate = 0;
	op_uart_printf("***ppp demo start***\r\n");

	ret = ppp_demo_wait_network();
	if(ret != 0){
		op_uart_printf("network active fail\r\n");
		return;
	}

	ol_os_task_sleep(200);

	ol_ppp_device_config(ppp_io_by_app,ppp_io_by_uart);
	ret = ol_ppp_start_call("*99#",PPP_DEMO_LOCAL_CID,ppp_demo_status_callback);
	op_uart_printf("ppp start call ret %d",ret);

	if(ret == 0){
		while(loop_count > 0){
			ol_os_task_sleep(10*200);
			loop_count--;
			if(ppp_terminate)
				break;
		}
	}

	ret = ol_ppp_stop_call();
	op_uart_printf("ppp stop call ret %d",ret);
	op_uart_printf("***ppp demo end***\r\n");
}


void ppp_demo(void)
{
	mbtk_taskref demo_task_ref = NULL;
	ol_os_task_creat(&demo_task_ref, NULL,TASK_STACK_SIZE, TASK_PRIORITY, DEMO_TASK_NAME, ppp_demo_task, NULL);
}

