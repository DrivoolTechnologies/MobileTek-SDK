#include "mbtk_api.h"
#include "mbtk_comm_api.h"
#include "menu_demo_api.h"

#define TASK_STARK_MALLOC_BY_CALLER	(1)
#define SUB_TASK_STACK_SIZE  		(2*1024)   
#define SUB_TASK_PRIORITY   		(224)

#define DEMO_FLAG_MASK        8 
#define OS_TIMER_DEMO_QUIT_FALG	(0x10)

void os_task_demo(void);
void os_timer_demo(void);
void os_pmu_demo(void);

static demo_menu_info menu_info[] =
{
	{"task test","",os_task_demo,NULL},
	{"timer test","[isloop],[timeout]",os_timer_demo,NULL},
	{"pmu test","mode",os_pmu_demo,NULL},
};

demo_menu_info *os_demo_menu_info(unsigned int *num)
{
	*num = sizeof(menu_info)/sizeof(menu_info[0]);
	return menu_info;
}


typedef struct 
{
    void *msg;
    uint8_t value;
}demo_msg_struct;     

typedef struct
{
	mbtk_taskref sub_task_ref;
	mbtk_msgqref demo_msgq_ref;
	mbtk_mailboxqref demo_mailboxq_ref;
	mbtk_flagref demo_flag_ref; 
	mbtk_mutexref demo_mutex_ref;
}os_task_demo_argc;

typedef struct
{
	unsigned char isloop;
	unsigned char quit_flag;
}os_timer_demo_argc;


void os_timer_demo_callback(unsigned int argc)
{
	os_timer_demo_argc *timer_argc = (os_timer_demo_argc*)argc;

	op_uart_printf("os_timer_demo_callback enter tick %d\n",ol_os_get_ticks());
	if(timer_argc->isloop){		
		op_uart_printf("timer loop \n");
		timer_argc->quit_flag++;
	}else{
		op_uart_printf("timer once \n");
		timer_argc->quit_flag = OS_TIMER_DEMO_QUIT_FALG;
	}
}

void os_sub_task(void *argc)
{
  mbtk_os_status os_status;
	os_task_demo_argc task_argc = {0};
  demo_msg_struct msg = {0};

	memcpy(&task_argc,(os_task_demo_argc *)argc,sizeof(os_task_demo_argc));

	os_status = ol_os_mutex_lock(task_argc.demo_mutex_ref, MBTK_OS_SUSPEND);
	op_uart_printf("main task release lock,os_sub_task runing  \n");
	ol_os_mutex_unlock(task_argc.demo_mutex_ref);
  while (1)
  {    
    int flag = 0;

    os_status = ol_os_flag_wait(task_argc.demo_flag_ref, DEMO_FLAG_MASK, MBTK_OS_FLAG_OR_CLEAR, &flag, MBTK_OS_SUSPEND); // task2 run to here wait flag set, task switching
    if (os_status == mbtk_os_success && (DEMO_FLAG_MASK & flag)) // task2 is aready get falg set by task1
    {
      msg.value = 5;
      msg.msg = ol_malloc(50);// creat msg param;
      if (msg.msg != NULL){
      	strcpy(msg.msg, "hello remote task");
      }
      op_uart_printf("os_sub_task malloc msg.msg[%#x] \n", msg.msg);
      os_status = ol_os_msgq_send(task_argc.demo_msgq_ref, sizeof(msg), &msg, MBTK_OS_SUSPEND); // send msg to msg queue, task1 is waiting for it
      if (os_status != mbtk_os_success){
      	op_uart_printf("os_sub_task ol_os_msgq_send fail, os_status = %d  \n", os_status);
      }
      else{
        op_uart_printf("os_sub_task ol_os_msgq_send msg success \n");
      }

			os_status = ol_os_mailboxq_send(task_argc.demo_mailboxq_ref, "hello main task", MBTK_OS_SUSPEND);
			if(os_status != mbtk_os_success){
				op_uart_printf("os_sub_task ol_os_mailboxq_send fail, os_status = %d \n",os_status);
			}
			else{
				op_uart_printf("os_sub_task ol_os_mailboxq_send msg success \n");
			}
    }    
    else // task2 get falg fail or unwanted flag;
    {
        op_uart_printf("demo2_task wait flag fail \n");
    }
  }
}

void os_demo_main_task_process(os_task_demo_argc *argc)
{
  mbtk_os_status os_status;
  demo_msg_struct msg = {0};
	char *mailbox_rive = NULL;

 	os_status = ol_os_flag_set(argc->demo_flag_ref, DEMO_FLAG_MASK, MBTK_OS_FLAG_OR);
 	op_uart_printf("demo1_task ol_os_flag_set os_status = %d  \n", os_status);
  if (os_status == mbtk_os_success)
  {
    os_status = ol_os_msgq_recv(argc->demo_msgq_ref, &msg, sizeof(demo_msg_struct), MBTK_OS_SUSPEND); // flag is aready set success, task1 is waiting task2 send msg to msg queue
    if(os_status != mbtk_os_success){
   		op_uart_printf("demo1_task ol_os_msgq_recv fail, os_status = %d  \n", os_status);
    }
    else // task1 aready recv the msg send from task2, parse it
    {
      if (msg.msg != NULL) // task2 malloc ptr is not null
      {
        op_uart_printf("demo1_task ol_os_msgq_recv msg[%s],msg[%#x], value[%d]", msg.msg, msg.msg, msg.value);
        ol_free(msg.msg);
      }
      else{
      	op_uart_printf("demo1_task ol_os_msgq_recv value[%d]", msg.value);
      }
    }

		os_status = ol_os_mailboxq_recv(argc->demo_mailboxq_ref, &mailbox_rive, 200);
		if(os_status != mbtk_os_success){
			op_uart_printf("demo1_task ol_os_mailboxq_recv fail\n");
		}
		else{
			if(strlen(mailbox_rive) !=0){
				op_uart_printf("demo1_task ol_os_mailboxq_recv success,data: %s\n", mailbox_rive);
			}
		}
		
  }
  else// task 1 set flag fail;
  {
    op_uart_printf("demo1_task ol_os_flag_set fail \n");
  }
}


void os_task_demo(void)
{
  mbtk_os_status os_status;
	os_task_demo_argc task_argc = {0};
  char *sub_task_stack_ptr = NULL;

	op_uart_printf("os_task_demo satrt");
	
	os_status = ol_os_msgq_creat(&(task_argc.demo_msgq_ref), "demo_msgq", sizeof(demo_msg_struct), 10, MBTK_OS_FIFO);
	if (os_status != mbtk_os_success){
		op_uart_printf("os_demo ol_os_msgq_creat fail , os_status = %d\n", os_status);
		goto done;
	}

	os_status = ol_os_mailboxq_creat(&(task_argc.demo_mailboxq_ref), "demo_mailbox", 10, MBTK_OS_FIFO);
	if(os_status != mbtk_os_success)
	{
		op_uart_printf("os_demo ol_os_mailboxq_creat fail, os_status = %d\n", os_status);
		goto done;
	}
	os_status = ol_os_flag_creat(&(task_argc.demo_flag_ref));
	if (os_status != mbtk_os_success){
		op_uart_printf("os_demo ol_os_flag_creat fail , os_status = %d\n", os_status);
		goto done;
	}

	os_status = ol_os_mutex_creat(&(task_argc.demo_mutex_ref), MBTK_OS_FIFO);
	if (os_status != mbtk_os_success)
	{
		op_uart_printf("os_demo ol_os_mutex_creat fail , os_status = %d\n", os_status);
		goto done;
	}
	os_status = ol_os_mutex_lock(task_argc.demo_mutex_ref, MBTK_OS_SUSPEND);
  op_uart_printf("os_demo ol_os_mutex_lock os_status = %d  \n", os_status);
	op_uart_printf("os demo main task hold the lock first");
		
#if TASK_STARK_MALLOC_BY_CALLER
  sub_task_stack_ptr = ol_malloc(SUB_TASK_STACK_SIZE);
  if (sub_task_stack_ptr == NULL){
    op_uart_printf("os_demo malloc sub_task_stack_ptr fail \n");
    goto done;
  }
#endif

  os_status = ol_os_task_creat(&(task_argc.sub_task_ref), sub_task_stack_ptr, SUB_TASK_STACK_SIZE,
  	SUB_TASK_PRIORITY, "demo_sub_task", os_sub_task,(void *)&task_argc);
  if (os_status != mbtk_os_success)
  {
    op_uart_printf("os_demo ol_os_task_creat sub task fail, os_status = %d \n", os_status);
    goto done;
  }

	op_uart_printf("os_demo_main_task_process start\n");
	ol_os_mutex_unlock(task_argc.demo_mutex_ref);
	os_demo_main_task_process(&task_argc);

done:
	if(task_argc.demo_msgq_ref){
		ol_os_msgq_delete(task_argc.demo_msgq_ref);
		task_argc.demo_msgq_ref = NULL;
	}

	if(task_argc.demo_mailboxq_ref){
		ol_os_mailboxq_delete(task_argc.demo_mailboxq_ref);
		task_argc.demo_mailboxq_ref = NULL;
	}

	if(task_argc.demo_flag_ref){
		ol_os_flag_delete(task_argc.demo_flag_ref);
		task_argc.demo_flag_ref = NULL;
	}

	if(task_argc.demo_mutex_ref){
		ol_os_mutex_delete(task_argc.demo_mutex_ref);
		task_argc.demo_mutex_ref = NULL;
	}

	if(task_argc.sub_task_ref){
		ol_os_task_delete(task_argc.sub_task_ref);
		task_argc.sub_task_ref = NULL;
	}

	if(sub_task_stack_ptr){
		ol_free(sub_task_stack_ptr);
		sub_task_stack_ptr = NULL;
	}

	op_uart_printf("os_task_demo end");
}

void os_timer_demo(void)
{
	unsigned int isloop;
	unsigned int timeout;
	mbtk_ostimerref timerref = NULL;
	mbtk_os_status status = mbtk_os_success;
	os_timer_demo_argc timer_argc = {0};
		
	op_uart_printf("os_timer_demo start  \n");
	DEMO_MEUN_GET_INT_PARAM(0,&isloop,0,1,0);
	DEMO_MEUN_GET_INT_PARAM(1,&timeout,1,60,5);

	op_uart_printf("os_timer_demo param set isloop = %d,timeout = %d(s)",isloop,timeout);
	status = ol_os_timer_creat(&timerref);
	if(status != mbtk_os_success){
		op_uart_printf("os_timer_demo create timer fail \n");
		return;
	}

	timer_argc.isloop = isloop;
	timer_argc.quit_flag = 0;
	op_uart_printf("os_timer_demo start timer,tick %d",ol_os_get_ticks());
	if(isloop){
		unsigned int rescheduletime = 10*200;
		unsigned int initialtime = timeout * 200;

		status = ol_os_timer_start(timerref,initialtime,rescheduletime, os_timer_demo_callback,(unsigned int)(&timer_argc));
	}else{
		unsigned int initialtime = timeout * 200;

		status = ol_os_timer_start(timerref,initialtime,0,os_timer_demo_callback,(unsigned int)(&timer_argc));
	}

	if(status != mbtk_os_success){
		op_uart_printf("os_timer_demo create timer fail \n");
		ol_os_timer_delete(timerref);
		return;
	}

	while(1){
		if(timer_argc.quit_flag == OS_TIMER_DEMO_QUIT_FALG)//quit
			break;
		else
			ol_os_task_sleep(2*200);
	}

	ol_os_timer_stop(timerref);
	ol_os_timer_delete(timerref);
	op_uart_printf("os_timer_demo end  \n");
}


void os_pmu_demo(void)
{
	unsigned int mode;
	unsigned int ret;
		
	op_uart_printf("os_pmu_demo start  \n");
	DEMO_MEUN_GET_INT_PARAM(0,&mode,0,2,0);

	op_uart_printf("os_pmu_demo param set mode = %d",mode);

	if(mode == 0){
		ret = ol_powerup_get_reason();
		op_uart_printf("os_pmu_demo get powerup reason %d",ret);
	}
	else if(mode == 1){
		op_uart_printf("os_pmu_demo set reset");
		ol_power_reset();
	}else if(mode == 2){
		op_uart_printf("os_pmu_demo set power off");
		ol_power_down();
	}
}

