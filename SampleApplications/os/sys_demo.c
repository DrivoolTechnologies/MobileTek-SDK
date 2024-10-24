
#include "mbtk_comm_api.h"


#include "ol_sys.h"
#include "ol_audio.h"

int mbt_demo_timer_id = 0;
static uint32 g_start_ticks = 0;

void  mbtk_timer_test_case_cb(uint32 data)
{
	uint32 current_ticks = 0;
	current_ticks = ol_os_get_ticks();
	op_uart_printf("mbtk_timer_test_case_cb !!!!!! success %d ms", (current_ticks - g_start_ticks)*5);		
}


#define MBTK_TEST_TIMER_DEMO_TIME  1

void mbtk_demo_acc_timer_start(void)
{

	MBTK_ACC_TIMER_STATUS time_status = OL_ACC_INACTIVE;
	int need_create = 1;
	int ret = 0;


	time_status = ol_acc_get_timer_status(mbt_demo_timer_id);	
	
	switch(time_status)
	{
		case OL_ACC_TIMER_ID_NOT_EXIST:
			op_uart_printf("the acc timer  OL_ACC_TIMER_ID_NOT_EXIST!");
			break;
		case OL_ACC_ACTIVE:
			op_uart_printf("the acc timer %d has not been excued!", mbt_demo_timer_id);			
			ol_acc_timer_stop(mbt_demo_timer_id);
			need_create = 0;
			break;
		case OL_ACC_INACTIVE:
			op_uart_printf("the acc timer OL_ACC_INACTIVE!");
			ol_acc_timer_delete(mbt_demo_timer_id);
			break;			
	}
	op_uart_printf("mbtk_acc_timer_test_case333");
	if(need_create)
	{
		//这里会创建并start timer，也可以用ol_acc_timer_create和ol_acc_timer_start
		g_start_ticks = ol_os_get_ticks();
		mbt_demo_timer_id = ol_acc_timer_start_ex(OL_ACC_TIMER_AUTO_DELETE, MBTK_TEST_TIMER_DEMO_TIME * 1000, mbtk_timer_test_case_cb, 0);
		if (mbt_demo_timer_id <= 0) 
		{
			op_uart_printf("start acc timer failed %d!", mbt_demo_timer_id);
		}
	}
	else
	{
		mbtk_acc_timer_config cfg;
		cfg.flag = OL_ACC_TIMER_AUTO_DELETE;
		cfg.period = MBTK_TEST_TIMER_DEMO_TIME * 1000;
		cfg.timerParams = 0;
		cfg.timer_cb = mbtk_timer_test_case_cb;
		g_start_ticks = ol_os_get_ticks();
		ret = ol_acc_timer_start(mbt_demo_timer_id,&cfg);
	
		op_uart_printf("start acc ol_acc_timer_start  ret %d!", ret);
	}
}

void mbtk_acc_timer_test_case(void)
{
	int i= 0;
	
	op_uart_printf("mbtk_acc_timer_test_case");
	while(i<8)
	{
		mbtk_demo_acc_timer_start();
		ol_os_task_sleep(1*200);
		i++;
		op_uart_printf("timer cancel restart %d", i);
		//这里延迟8秒，前几次都是会被delete掉，所以不会触发
	}	

	ol_os_task_sleep(5*200);
	//等待timer触发

	op_uart_printf("mbtk_demo_acc_timer_start");
	//mbtk_demo_acc_timer_start();
	//再来一次

	ol_os_task_sleep(6*200);
	//等待timer触发
	
}


