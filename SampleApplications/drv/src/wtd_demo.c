
#include "mbtk_gpio.h"
#include "mbtk_api.h"
#include "mbtk_wtd.h"
#include "mbtk_comm_api.h"
#include "menu_demo_api.h"

typedef MBTK_WTD_ERRCODE (*wtd_demo_func)(void);

static void feedTheDogTest(void);
static MBTK_WTD_ERRCODE watchdogTest1(void);
static MBTK_WTD_ERRCODE watchdogTest2(void);
static MBTK_WTD_ERRCODE watchdogTest3(void);
static MBTK_WTD_ERRCODE watchdogTest4(void);
static MBTK_WTD_ERRCODE watchdogTest5(void);
static MBTK_WTD_ERRCODE watchdogTest6(void);


static demo_menu_info menu_info[] =
{
	{"feedTheDogTest","",feedTheDogTest,NULL},
	{"watchdogTest1","",watchdogTest1,NULL},
	{"watchdogTest2","",watchdogTest2,NULL},
	{"watchdogTest3","",watchdogTest3,NULL},
	{"watchdogTest4","",watchdogTest4,NULL},
	{"watchdogTest5","",watchdogTest5,NULL},
	{"watchdogTest6","",watchdogTest6,NULL},
};

demo_menu_info *wtd_demo_menu_info(unsigned int *num)
{
	*num = sizeof(menu_info)/sizeof(menu_info[0]);
	return menu_info;
}





mbtk_hisrref wtd_demo_hisr = NULL;




//this functin can not use uart print
static void watchdogTestHandler(void)
{
	static UINT32 counter = 0;

    if(counter++ == 2)
	{
		ol_wtd_deactive();
        counter = 0;
	}

	if(wtd_demo_hisr)
		ol_os_active_hisr(&wtd_demo_hisr);

}

void wtd_demo_hisr_func(void)
{
	op_uart_printf("wtd_demo_hisr_func enter \n");
}



/******************************************************************************
* Function: watchdogTest6
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest6(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;
    MBTK_WTD_STATUS status;

	op_uart_printf("%s	START\n", __FUNCTION__);

	if(!wtd_demo_hisr)
		ol_os_creat_hisr(&wtd_demo_hisr, "wtd_demo_hisr", wtd_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);

	// Register:
	rc = ol_wtd_int_register(watchdogTestHandler);


    // Configure: match value = 500ms, interrupt mode:
    config.matchValue = 500;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);

	// Activate:
	rc = ol_wtd_active();
	op_uart_printf("%s %dms interrupt mode start\n", __FUNCTION__, config.matchValue);

    // Configure: match value = 5000ms, interrupt mode:
    config.matchValue = 5000;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);
	op_uart_printf("%s ol_wtd_set_config  %dms \n", __FUNCTION__, config.matchValue);
	rc = ol_wtd_active();
	op_uart_printf("%s %dms interrupt mode start\n", __FUNCTION__, config.matchValue);


    if(status.timeTillMatch != 5000)
    {
      //  return MBTK_WTD_BAD_MATCH_VALUE;
    }

    rc = ol_wtd_get_config(&config);
	op_uart_printf("%s ol_wtd_get_config matchValue[%d], mode[%d]\n", __FUNCTION__, config.matchValue, config.mode);

    if((config.matchValue != 5000) || (config.mode != MBTK_WTD_INTERRUPT_MODE))
    {
        return MBTK_WTD_BAD_MATCH_VALUE;
    }

	
	ol_os_task_sleep(6*200);
	//sleep 6秒等wtd超时触发


	config.matchValue = 1000;
	config.mode = MBTK_WTD_RESET_MODE;
	rc = ol_wtd_set_config(&config);
	
    rc = ol_wtd_get_status(&status);
	op_uart_printf("%s ol_wtd_get_status[rc=%d] timeTillMatch%ums\n", __FUNCTION__, rc, status.timeTillMatch);
	rc = ol_wtd_active();
	ol_os_task_sleep(100);
    rc = ol_wtd_get_status(&status);
	op_uart_printf("%s ol_wtd_get_status[rc=%d] timeTillMatch%ums\n", __FUNCTION__, rc, status.timeTillMatch);
	op_uart_printf("%s %dms interrupt mode start\n", __FUNCTION__, config.matchValue);
	op_uart_printf("%s ol_wtd_set_config %d， mode[1]\n", __FUNCTION__, status.timeTillMatch);
	op_uart_printf("%s ol_wtd_get_config will reset 1 second\n", __FUNCTION__);

	return rc;
}   /*      End of watchdogTest6    */



/******************************************************************************
* Function: watchdogTest5
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest5(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;

	op_uart_printf("%s	START\n", __FUNCTION__);

	if(!wtd_demo_hisr)
		ol_os_creat_hisr(&wtd_demo_hisr, "wtd_demo_hisr", wtd_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);

	// Register:
	rc = ol_wtd_int_register(watchdogTestHandler);


    // Configure: match value = 65535ms, interrupt mode:
    config.matchValue = 0xFFFF;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);

	// Activate:
	rc = ol_wtd_active();

	return rc;
}   /*      End of watchdogTest5    */




/******************************************************************************
* Function: watchdogTest4
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest4(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;

	op_uart_printf("%s	START\n", __FUNCTION__);

	if(!wtd_demo_hisr)
		ol_os_creat_hisr(&wtd_demo_hisr, "wtd_demo_hisr", wtd_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);

	// Register:
	rc = ol_wtd_int_register(watchdogTestHandler);


    // Configure: match value = 4ms, interrupt mode:
    config.matchValue = 4;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);

	// Activate:
	rc = ol_wtd_active();

	return rc;
}   /*      End of watchdogTest5a    */




/******************************************************************************
* Function: watchdogTest3
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest3(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;

	
	op_uart_printf("%s	START\n", __FUNCTION__);
	if(!wtd_demo_hisr)
		ol_os_creat_hisr(&wtd_demo_hisr, "wtd_demo_hisr", wtd_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);

	rc = ol_wtd_int_register(NULL);
	
	op_uart_printf("%s1	START\n", __FUNCTION__);
	rc = ol_wtd_set_config(NULL);
	
	op_uart_printf("%s2	START\n", __FUNCTION__);
	rc = ol_wtd_get_config(NULL);
	
	op_uart_printf("%s3	START\n", __FUNCTION__);
	rc = ol_wtd_get_status(NULL);
	
	op_uart_printf("%s4	START\n", __FUNCTION__);

	if(rc != MBTK_WTD_NULL_POINTER)
	{
		return rc;
	}

    config.matchValue = 2;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);

	if(rc != MBTK_WTD_BAD_MATCH_VALUE)
	{
		return rc;
	}

	config.matchValue = 2000;

	rc = ol_wtd_set_config(&config);

	if(rc != MBTK_WTD_NO_HANDLER_REGISTERED)
	{
		return rc;
	}

	return MBTK_WTD_OK;
}   /*      End of watchdogTest4        */




/******************************************************************************
* Function: watchdogTest2
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest2(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;
    UINT32 count, i;
  	//RTC_Handle handle;
	op_uart_printf("%s	START\n", __FUNCTION__);
	
	if(!wtd_demo_hisr)
		ol_os_creat_hisr(&wtd_demo_hisr, "wtd_demo_hisr", wtd_demo_hisr_func, MBTK_GPIO_HISR_PRIORITY_HIGH);

	// Register:
	rc = ol_wtd_int_register(watchdogTestHandler);

  	// Configure: match value = 2sec, interrupt mode:
    config.matchValue = 12000;
	config.mode = MBTK_WTD_INTERRUPT_MODE;

	rc = ol_wtd_set_config(&config);


	// Activate:
	rc = ol_wtd_active();

	return rc;
}	/*		End of watchdogTest2	*/




/******************************************************************************
* Function: watchdogTest1
*******************************************************************************
*
* Parameters: None
*
* Return value: MBTK_WTD_OK
*
* Notes:
******************************************************************************/
static MBTK_WTD_ERRCODE watchdogTest1(void)
{
    MBTK_WTD_ERRCODE rc = MBTK_WTD_OK;
    MBTK_WTD_CONFIG config;
	op_uart_printf("%s	START\n", __FUNCTION__);

	// Configure: match value = 12000ms, reset mode:
	config.matchValue = 12000;
	config.mode = MBTK_WTD_RESET_MODE;

	rc = ol_wtd_set_config(&config);

	// Activate:
	rc = ol_wtd_active();
	return rc;
}	/*		End of watchdogTest1	*/

static void feedTheDogTest(void)
{
	op_uart_printf("%s	START\n", __FUNCTION__);
    ol_wtd_kick();
}

