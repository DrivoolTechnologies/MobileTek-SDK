#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "mbtk_api.h"
#include "ol_nw_api.h"
#include "menu_demo_api.h"


#define DNS_PARAM_PRI_IPV4_ADDR	"8.8.8.8"
#define DNS_PARAM_SEC_IPV4_ADDR	"114.114.114.114"
#define DNS_PARAM_PRI_IPV6_ADDR	"2400:3200::1"
#define DNS_PARAM_SEC_IPV6_ADDR	"2400:da00::6666"

int data_call_demo(void);
void datacall_default_param_demo(void);
void datacall_dns_param_demo(void);

static demo_menu_info menu_info[] =
{
	{"datacall test","[cid],[iptype],[nostop]",data_call_demo,NULL},
	{"defalut param test","[iptype],[apn],[user],[pass],[auth_type],[force_set]",datacall_default_param_demo,NULL},
	{"dns param test","[iptype],[onlyread]",datacall_dns_param_demo,NULL},
};

demo_menu_info *datacall_demo_menu_info(unsigned int *num)
{
	*num = sizeof(menu_info)/sizeof(menu_info[0]);
	return menu_info;
}


void data_call_demo_callback(uint8_t index, uint8_t status)
{
 	op_uart_printf("data_call_demo_callback enter index[%d] , status[%d]\n", index, status);
}

void mbtk_nw_status_callback_demo(mbtk_nw_status_struct *status)
{
  op_uart_printf("mbtk_nw_status_callback_demo enter event[%d]\n", status->event);
  if (status->event == MBTK_NW_REG_EVENT)
  {
  	op_uart_printf("mbtk_nw_status_callback_demo enter st[%d] , act[%d]\n", status->nw_status.reg_status.state, 
          status->nw_status.reg_status.act );
  }
  else if (status->event == MBTK_NW_PDP_EVENT)
  {
  	op_uart_printf("mbtk_nw_status_callback_demo enter st[%d] , pdpstat[%d]\n", status->nw_status.pdp_status.cid, 
          status->nw_status.pdp_status.state );
  }
  else
  {
  	op_uart_printf("mbtk_nw_status_callback_demo enter unknown");
  }
}

void data_call_get_remote_addr(char iptype,char cid_index)
{
	char ipaddr[50] = {0};
	char get_iptype = iptype;
	char try_count = 0;

	if(iptype == mbtk_data_call_v4v6)
		get_iptype = mbtk_data_call_v4;
	
	if(get_iptype == mbtk_data_call_v4)
	{
		try_count = 0;
		do{
			ol_os_task_sleep(200);
			memset(ipaddr,0x0,50);
			ol_get_pdp_ipv4addr((unsigned char *)ipaddr, cid_index);
			try_count++;
		}while(strlen(ipaddr) == 0 && try_count < 5);
  	op_uart_printf("data call get ipv4 %s\n", ipaddr);
	}

	if(iptype == mbtk_data_call_v4v6)
		get_iptype = mbtk_data_call_v6;
	
	if(get_iptype == mbtk_data_call_v6)
	{
		try_count = 0;
		do{
			ol_os_task_sleep(200);
			memset(ipaddr,0x0,50);
			ol_get_pdp_ipv6addr((unsigned char *)ipaddr, cid_index);
			try_count++;
		}while(strlen(ipaddr) == 0 && try_count < 5);
  	op_uart_printf("data call get ipv6 %s\n", ipaddr);
	}			
}



int data_call_demo(void)
{
  int iret = 0;
  char ipaddr[50] = {0};
  char pridns[50] = {0};
  char secdns[50] = {0};
  mbtk_data_call_info_strcut data_call_info = {0};
	int cid_index = 0;
	int ip_type = 0;
	int not_stop = 0;

  op_uart_printf("data_call_demo enter\n");
	DEMO_MEUN_GET_INT_PARAM(0,&cid_index,mbtk_cid_index_1,mbtk_cid_index_15,mbtk_cid_index_2);
	DEMO_MEUN_GET_INT_PARAM(1,&ip_type,mbtk_data_call_v4,mbtk_data_call_v4v6,mbtk_data_call_v4v6);
	DEMO_MEUN_GET_INT_PARAM(2,&not_stop,0,1,0);
	op_uart_printf("get demo param cid_index = %d,iptype = %d,not_stop = %d",cid_index,ip_type,not_stop);

  ol_regist_nw_cb(mbtk_nw_status_callback_demo);
  if ((iret = ol_wait_network_regist(120)) != mbtk_data_call_ok)
  {
    op_uart_printf("ol_wait_network_regist wait network regist timeout\n");        
    return -1;
  }
  else 
  {
		ol_data_call_stop(cid_index, ip_type);
    ol_regist_data_call_callback(data_call_demo_callback);
    ol_set_data_call_auto_reconnect(cid_index, mbtk_auto_reconnect_enable);
    iret = ol_data_call_start(cid_index, ip_type, "ctnet", NULL, NULL, OL_NW_AUTH_NONE);
    if (iret != mbtk_data_call_ok)
    {
      op_uart_printf(" demo_data_call mbtk_data_call_start fail iret = %d \n", iret);        
      return -1;
    }

    iret = ol_get_data_call_info(cid_index, ip_type, &data_call_info);
    if (iret != mbtk_data_call_ok)
    {
      op_uart_printf(" demo_data_call mbtk_get_data_call_info fail iret = %d \n", iret);        
      return -1;
    }

    op_uart_printf("data_call_demo ol_get_data_call_info index[%d]\r\n  info list start ===>\n", data_call_info.index);
    op_uart_printf("iptype[%d]   \n", data_call_info.iptype);
		if(data_call_info.v4info.state){
    	op_uart_printf("ipv4 reconn_sta[%d]   \n", data_call_info.v4info.reconnect);

			memset(ipaddr,0x0,50);
			memset(pridns,0x0,50);
			memset(secdns,0x0,50);
			ol_inet_ntop(OL_AF_INET, &data_call_info.v4info.ipv4addr.ip, ipaddr, sizeof(ipaddr));
    	ol_inet_ntop(OL_AF_INET, &data_call_info.v4info.ipv4addr.pri_dns, pridns, sizeof(pridns));
    	ol_inet_ntop(OL_AF_INET, &data_call_info.v4info.ipv4addr.sec_dns, secdns, sizeof(secdns));
    
    	op_uart_printf("ipv4 ipaddr[%s]    \n", ipaddr);
    	op_uart_printf("ipv4 pri_dns[%s]   \n", pridns);
    	op_uart_printf("ipv4 sec_dns[%s]   \n", secdns);
		}
		if(data_call_info.v6info.state){
			op_uart_printf("ipv6 reconn_sta[%d]   \n", data_call_info.v6info.reconnect);

			memset(ipaddr,0x0,50);
			memset(pridns,0x0,50);
			memset(secdns,0x0,50);
			ol_inet_ntop(OL_AF_INET6, &data_call_info.v6info.ipv6addr.ip, ipaddr, sizeof(ipaddr));
    	ol_inet_ntop(OL_AF_INET6, &data_call_info.v6info.ipv6addr.pri_dns, pridns, sizeof(pridns));
    	ol_inet_ntop(OL_AF_INET6, &data_call_info.v6info.ipv6addr.sec_dns, secdns, sizeof(secdns));
    
    	op_uart_printf("ipv6 ipaddr[%s]    \n", ipaddr);
    	op_uart_printf("ipv6 pri_dns[%s]   \n", pridns);
    	op_uart_printf("ipv6 sec_dns[%s]   \n", secdns);
		}
		
    op_uart_printf("data_call_demo ol_get_data_call_info info list end <===\n");
		
		data_call_get_remote_addr(ip_type,cid_index);
		if(!not_stop){
    	iret = ol_data_call_stop(cid_index, ip_type);
    	if (iret != mbtk_data_call_ok)
    	{
      	op_uart_printf("data_call_demo ol_data_call_stop fail iret = %d \n", iret);        
      	return -1;
    	}
		}
  }
  op_uart_printf("data_call_demo end\n");    

  return 0;
}


void datacall_default_param_demo(void)
{
	char temp_iptype = 0;
	char temp_apn[32] = {0};
	char temp_authtype = 0;
	char temp_user[32] = {0};
	char temp_pass[32] = {0};
	char temp_ems_flag = 0;
	char force_set = 0;
	char iptype = 0;
	char apn[32] = {0};
	char user[32] = {0};
	char pass[32] = {0};
	char auth_type = 0;

	op_uart_printf("default datacall param demo start,check flag[%d]==\r\n",force_set);
	DEMO_MEUN_GET_INT_PARAM(0,&iptype,mbtk_data_call_v4,mbtk_data_call_v4v6,mbtk_data_call_v4);
	DEMO_MEUN_GET_STR_PARAM(1,apn,32,"ctnet");
	DEMO_MEUN_GET_STR_PARAM(2,user,32,"");
	DEMO_MEUN_GET_STR_PARAM(3,pass,32,"");
	DEMO_MEUN_GET_INT_PARAM(4,&auth_type,0,1,0);
	DEMO_MEUN_GET_INT_PARAM(5,&force_set,0,1,0);
	op_uart_printf("default datacall param get param tptype = %d,apn = %s",iptype,apn);
	op_uart_printf("user = %s,pass = %s,auth_type = %d,force_set = %d",user,pass,auth_type,force_set);

	ol_get_default_lte_apn(&temp_iptype,temp_apn);
	op_uart_printf("get default apn:iptype %d,apn %s",temp_iptype,temp_apn);
	if(strlen(temp_apn) == 0 || force_set)//default apn not set
	{
		ol_set_default_lte_apn(iptype,apn);
	}
	ol_get_default_auth(&temp_authtype,temp_user,temp_pass);
	op_uart_printf("get default auth:authtype %d,user %s,pass %s",temp_authtype,temp_user,temp_pass);
	if((strlen(temp_user) == 0 ||strlen(temp_pass) == 0) ||force_set)
	{
		ol_set_default_auth(auth_type,user,pass);
	}
	ol_get_default_lte_apn_valid_in_ems(&temp_ems_flag);
	op_uart_printf("get default eti flag %d",temp_ems_flag);
	if(temp_ems_flag == 0 || force_set)
	{
		ol_set_default_lte_apn_valid_in_ems(1);
	}

	op_uart_printf("default datacall param demo end\r\n");
}


void datacall_dns_param_demo(void)
{
	char* ip_type_str = NULL;
	char *pri_dns = NULL;
	char *sec_dns = NULL;
	char ip_type = 0;
	char is_onlyread = 0;
	char temp_dns1[64] = {0};
	char temp_dns2[64] = {0};

	op_uart_printf("datacall_dns_param_demo start");
	DEMO_MEUN_GET_INT_PARAM(0,&ip_type,mbtk_data_call_v4,mbtk_data_call_v6,mbtk_data_call_v4);
	DEMO_MEUN_GET_INT_PARAM(1,&is_onlyread,0,1,1);
	op_uart_printf("datacall_dns_param_demo get param iptype = %d,is_onlyread = %d",ip_type,is_onlyread);

	if(ip_type == mbtk_data_call_v4v6){
		op_uart_printf("unsupported iptype");
		return;
	}

	if(ip_type == mbtk_data_call_v4){
		pri_dns = DNS_PARAM_PRI_IPV4_ADDR;
		sec_dns = DNS_PARAM_SEC_IPV4_ADDR;
		ip_type_str = "IPV4";
	}else{
		pri_dns = DNS_PARAM_PRI_IPV6_ADDR;
		sec_dns = DNS_PARAM_SEC_IPV6_ADDR;;
		ip_type_str = "IPV6";
	}
	
	ol_get_dns(ip_type,temp_dns1,temp_dns2);
	op_uart_printf("get %s :dns1 = %s,dns2 = %s",ip_type_str,temp_dns1,temp_dns2);

	if(is_onlyread != 1){
		memset(temp_dns1,0x0,64);
		memset(temp_dns2,0x0,64);	
		ol_set_dns(ip_type,pri_dns,sec_dns);
	
		memset(temp_dns1,0x0,64);
		memset(temp_dns2,0x0,64);
		ol_get_dns(ip_type,temp_dns1,temp_dns2);
		op_uart_printf("get %s after set,dns1 = %s,dns2 = %s",ip_type_str,temp_dns1,temp_dns2);
	}
}

