#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "Mbtk_pmu.h"
#include "mbtk_api.h"
#include "ntp_api.h"


int ntp_demo_wait_network(void)
{
  if (ol_wait_network_regist(120) != mbtk_data_call_ok)
  {
      op_uart_printf("fota_demo_wait_network ol_wait_network_regist time out\n");
      return -1;
  }
  op_uart_printf("fota_demo_wait_network execute ol_data_call_start \n");

  //ol_os_task_sleep(200 *5);
  /*
  if(ol_data_call_start(FOTA_LOCAL_CID, FOTA_LOCAL_CID_PDN_TYPE, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
  {
      op_uart_printf("fota_demo_wait_network ol_data_call_start fail\n");
      return -1;
  }
  */
  return 0;
}

void ntp_demo(void)
{
  int ret = 0;
  char * host_name = "ntp1.aliyun.com";
  char * host_name_temp  = NULL;
  unsigned int utc_time = 0;
  int result = 0;
  
  op_uart_printf("ntp demo start!!\r\n");
  
  if (ntp_demo_wait_network() != 0)
  {
    op_uart_printf("ntp_demo_wait_network fail\n");
    return;
  }
 
  host_name_temp = ol_ntp_get_host_name();
  if(host_name_temp == NULL)
  {
   	op_uart_printf("get ntp host NULL,set host as %s\r\n",host_name);
		ol_ntp_set_host_name(host_name);
  }
  
  op_uart_printf("NTP host_name  %s\n",ol_ntp_get_host_name());
  op_uart_printf("NTP sync before status = %d\n",ol_ntp_get_status());
  ol_ntp_sync_time(); /*sync and auotomatic*/
 	do{
		ol_os_task_sleep(200);
		result = ol_ntp_get_status();
	}while(result == 0);
  op_uart_printf("NTP sync after status = %d\n",result);
  utc_time = ol_ntp_get_utc_time();
  op_uart_printf("NTP sync after utc_time = %d\n",utc_time);      
}


