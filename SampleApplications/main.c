#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbtk_api.h"
#include "mbtk_comm_api.h"

extern void mbtk_api_init(open_api_table *api_table);
mbtk_taskref mbtk_test_task_ref;
extern void demo_menu_test(void);

void nitz_ind_callback(void)
{
    op_uart_printf("updateTimeFromNitz finish");
}

void test_reject_callback(void *param)
{
	ol_NW_REJECT_CAUSE *cause = (ol_NW_REJECT_CAUSE *)param;
	
	op_uart_printf("a reject event report\r\n");
	op_uart_printf("mcc = %d,mnc = %d,cause = %d,time = %s",cause->mcc,
		cause->mnc,cause->rejectcause,cause->timestamp);
}

void app_open_task(void *argv)
{
	char *btime;
	ol_set_nitz_ind_cb(nitz_ind_callback);
	ol_nw_set_reject_callback(test_reject_callback);
	ol_os_task_sleep(5*200);
	btime = ol_get_buildtime();
	op_uart_printf("ol_dev_get_build time build time:%s \n",btime);

	demo_menu_test();

}

void user_app_init(open_api_table *api_table)
{
  mbtk_api_init(api_table);
	
	op_uart_printf("app hello, world\r\n");
		
	ol_os_task_creat(&mbtk_test_task_ref,NULL, 10*1024, 220, "app_at_task", app_open_task, NULL);
		
	while(1)
	{
		ol_os_task_sleep(200);	
	}
	
}


