#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbtk_comm_api.h"
#include "mbtk_uart.h"
#include "menu_demo_api.h"

extern void flash_fs_demo(void);
extern void flash_dir_demo(void);
extern int mbedtls_aes_self_test( int enable_log );
extern int gpio_demo_output(void);
extern void sim_demo(void);
extern void visual_at_demo(void);
extern int gpio_demo_output(void);
extern demo_menu_info *upgrade_demo_menu_info(unsigned int *num);
extern void nw_demo(void);
extern int socket_demo(void);
extern int socket_server_demo(void);
extern int iic_demo(void);
extern void app_update_demo(void);
extern void gps_demo(void);
extern void ids_test(void);
extern void mqtt_demo(void);
extern demo_menu_info *datacall_demo_menu_info(unsigned int *num);
extern int ping_demo(void);
extern void sms_demo(void);
extern void mbtk_acc_timer_test_case(void);
extern bool http_demo(void);
extern void audio_demo(void);
extern void uart_demo(void);
extern void device_demo(void);
extern bool ftp_demo(void);
extern int ssl_socket_demo(void);
extern demo_menu_info *os_demo_menu_info(unsigned int *num);
extern int time_demo(void);
extern void ntp_demo(void);
extern void aliyun_demo(void);
extern void tts_demo(void);
extern void lbs_demo(void);
extern int wb_app_main(void);
extern demo_menu_info *wtd_demo_menu_info(unsigned int *num);
#ifdef MBTK_AZURE_SUPPORT
extern void azure_demo(void);
#endif

#define DEMO_MENU_MAX_INPUT_LEN				(128)
#define DEMO_MENU_UART_PORT 					OL_UART_PORT_CDCUART
#define DEMO_MENU_PARAM_DELIM   			(0x2C) //','
#define DEMO_MENU_PARAM_END						(0x0D) //'\n'
#define DEMO_MENU_LAYOUT_STR					"============================================="
#define DEMO_MENU_BUSY_INFO_STR			"\r\nAnother Demo Is Runing,Please Wait..."

typedef struct 
{
    void *param_msg;
		unsigned char quit_flag;
    unsigned char index;
}demo_menu_msg_struct; 

typedef struct menu_node {
  struct menu_node *prev;
	unsigned int prev_index;
	unsigned int data_num;
	demo_menu_info *menu_data;
} menu_node_t;

typedef struct{ 
	menu_node_t *node;
	demo_menu_param_list param;
}demo_menu_ctx;


menu_node_t *demo_menu_node_new(demo_menu_info *node_info,unsigned int num);
void demo_menu_node_free(menu_node_t *node);
void demo_menu_path_push(demo_menu_ctx *ctx,menu_node_t *node,unsigned int prev_index);
menu_node_t *demo_menu_path_pop(demo_menu_ctx *ctx);

#define DEMO_MENU_NEWNODE(info,num)							demo_menu_node_new(info,num)
#define DEMO_MENU_FREENODE(node)								demo_menu_node_free(node)
#define DEMO_MENU_PATHPUSH(ctx,node,index)			demo_menu_path_push(ctx,node,index)
#define DEMO_MENU_PATHPOP(ctx)									demo_menu_path_pop(ctx)

static demo_menu_ctx context = {0};
static unsigned char menu_ready_flag = 0;
static mbtk_taskref menu_flag_ref = NULL;
static mbtk_msgqref demo_menu_msgref =  NULL;


static demo_menu_info menu_info[] =
{
	{"emmc file operation test", "",flash_fs_demo, NULL},
	{"nw test", "",nw_demo, NULL},
	{"device test", "", device_demo, NULL},
	{"sim test", "", sim_demo, NULL},
	{"visual at test", "", visual_at_demo, NULL},
	{"gpio test", "", gpio_demo_output, NULL},
	{"upgrade test", "", NULL, upgrade_demo_menu_info},
	{"fs test", "", flash_fs_demo, NULL},
	{"ftp test", "", ftp_demo, NULL},
	{"gps test", "", gps_demo, NULL},
	{"http test", "", http_demo, NULL},
	{"mqtt test", "", mqtt_demo, NULL},
	{"data call test", "", NULL, datacall_demo_menu_info},
	{"ping test", "", ping_demo, NULL},
	{"socket test", "", socket_demo,NULL},
	{"ssl test", "", ssl_socket_demo, NULL},
	{"os test", "", NULL, os_demo_menu_info},
	{"sms test", "", sms_demo, NULL},
	{"audio test", "", audio_demo , NULL},
	{"uart_test", "", uart_demo, NULL},
	{"fs dir test", "", flash_dir_demo, NULL},
	{"socket server test", "",socket_server_demo, NULL},
	{"iic test", "",iic_demo, NULL},
	{"time test", "",time_demo, NULL},
	{"ntp test", "",ntp_demo, NULL},
	{"aliyun test", "",aliyun_demo, NULL},
	{"tts test", "", tts_demo, NULL},
	{"lbs test", "", lbs_demo, NULL},
	{"ws test", "", wb_app_main, NULL},
#ifdef MBTK_AZURE_SUPPORT
	{"azure test", "", azure_demo, NULL},
#endif
	{"wtd test", "", NULL, wtd_demo_menu_info},
};

menu_node_t *demo_menu_node_new(demo_menu_info *node_info,unsigned int num)
{
	menu_node_t *new_node = NULL;

	if(node_info && num > 0){
		new_node = ol_malloc(sizeof(menu_node_t));
		if(!new_node)
			return NULL;
		new_node->prev = NULL;
		new_node->prev_index = 0;
		new_node->data_num = num;
		new_node->menu_data = node_info;
	}

	return new_node;
}

void demo_menu_node_free(menu_node_t *node)
{
	if(node)
		ol_free(node);
}


void demo_menu_path_push(demo_menu_ctx *ctx,menu_node_t *node,unsigned int prev_index)
{
	menu_node_t *current_node_pos = ctx->node;

	if(node){
		node->prev = current_node_pos;
		node->prev_index = prev_index;
		ctx->node = node;
	}
}

menu_node_t *demo_menu_path_pop(demo_menu_ctx *ctx)
{	
	menu_node_t *current_node_pos = ctx->node;
	menu_node_t *prev_node = current_node_pos->prev;
		
	ctx->node = prev_node;
	return current_node_pos;
}

demo_menu_param_list *demo_menu_param_list_get(void)
{
	return &context.param;
}

char demo_menu_param_get_int(demo_menu_param_list *param_list,unsigned int index,
		int *value,unsigned int min,unsigned int max,int default_value)
{
	char ret_value = -1;
	unsigned char *value_str = NULL;
	int value_int = 0;

	*value = default_value;

	if(index >= param_list->param_num || param_list->param_num == 0)
		return ret_value;
	
	value_str = param_list->param[index];
	if(strlen(value_str) > 0){
		value_int = atoi(value_str);
		if(value_int >= min && value_int <= max){
			*value = value_int;
			ret_value = 0;
		}
	}
	return ret_value;
}

char demo_menu_param_get_str(demo_menu_param_list *param_list,unsigned int index,
		char *value,unsigned int max_len,char *default_value)
{
	char ret_value = -1;
	unsigned char *value_str = NULL;
	unsigned int copy_len = 0;

	//set to default
	memcpy(value,default_value,strlen(default_value));

	if(index >= param_list->param_num || param_list->param_num == 0)
		return ret_value;

	value_str = param_list->param[index];
	copy_len = strlen(value_str);
	copy_len = copy_len > max_len ? max_len : copy_len;
	if(copy_len > 0){
		memset(value,0x0,max_len);
		memcpy(value,value_str,copy_len);
		ret_value = 0;
	}
	return ret_value;
}

void demo_menu_param_parser(demo_menu_ctx *ctx,char *param_str)
{
	demo_menu_param_list *param_list = &ctx->param;
  char *next_param = param_str;

  if(!param_str){
    op_uart_printf("demo menu no param,use default\r\n");		
    return;
  }

	op_uart_printf("demo menu get param str %s\r\n",param_str);
  memset(param_list,0x0,sizeof(demo_menu_param_list));
  do{
  	char *last_param = next_param;

    next_param = strchr(next_param,DEMO_MENU_PARAM_DELIM);
   	//op_uart_printf("next param %s\r\n",next_param);
   	if(next_param){
   		*next_param = '\0';//do string block
     	next_param++;
   	}
   	//op_uart_printf("get param section %s\r\n",last_param);
   	param_list->param[param_list->param_num] = last_param;
    param_list->param_num++;
		if(param_list->param_num >= DEMO_MENU_MAX_PARMA_NUM){
			op_uart_printf("param num out of rang,max param num %d\r\n",DEMO_MENU_MAX_PARMA_NUM);
			memset(param_list,0x0,sizeof(demo_menu_param_list));
			return;
		}	
  }while(next_param);

  return;
}


void demo_menu_uart_handle(MBTK_UART_Port p)
{
	static unsigned char uart_data[DEMO_MENU_MAX_INPUT_LEN+1] = {0};
	static unsigned char data_len = 0;
	unsigned int read_len = 0;
	demo_menu_msg_struct msg = {0};
	int index = 0;

	if(data_len >= DEMO_MENU_MAX_INPUT_LEN){
		op_uart_printf("the uart data can't find end flag,reset it\r\n");
		memset(uart_data,0,sizeof(uart_data));
		data_len = 0;
	}
	
	ol_Uart_Read(p, uart_data+data_len, DEMO_MENU_MAX_INPUT_LEN - data_len, &read_len);
	//op_uart_printf("\r\ndemo_menu_uart_handle, %d, %s", read_len, uart_data);

	if(!menu_ready_flag){
		ol_Uart_Write(p,DEMO_MENU_BUSY_INFO_STR,strlen(DEMO_MENU_BUSY_INFO_STR));
		return;
	}
	
	if(read_len>0)
	{
		char *param = NULL;

		ol_Uart_Write(p, uart_data + data_len, strlen(uart_data + data_len));
		data_len += read_len;
		
		if((param = strstr(uart_data,"\r\n")) ||(param = strchr(uart_data,DEMO_MENU_PARAM_END)))
			*param = '\0';
		else
			return;
		
		if(strlen(uart_data) == 4 && memcmp(uart_data,"quit",4)==0){
			msg.quit_flag = 1;
		}else{	
			param = strchr(uart_data,DEMO_MENU_PARAM_DELIM);
			if(param){
				*param = '\0';
				param++;
				msg.param_msg = ol_malloc(strlen(param)+1);
				if(msg.param_msg){
					memset(msg.param_msg,0x0,strlen(param)+1);
					memcpy(msg.param_msg,param,strlen(param)+1);
				}
			}
			index = atoi(uart_data);
			msg.quit_flag = 0;
			msg.index = index;			
		}
		memset(uart_data,0,sizeof(uart_data));
		data_len = 0;
		op_uart_printf("\r\ndemo_menu_uart_handle parser quit %d,index %d",msg.quit_flag,msg.index);
		ol_os_msgq_flush(demo_menu_msgref);
		ol_os_msgq_send(demo_menu_msgref, sizeof(demo_menu_msg_struct), &msg, MBTK_OS_SUSPEND);
	}
	else
		op_uart_printf("ol_Uart_Read NULL");

}


void demo_menu_display(char port,menu_node_t *dir)
{
	unsigned char i = 0;
	unsigned char buffer[100+1] = {0};
	unsigned char *title = NULL;
	menu_node_t *prev_node = dir->prev;
	demo_menu_info *info = dir->menu_data;

	if(!prev_node){//no prev,main title
		title = "Demo Main Menu";
	}
	else{
		title = prev_node->menu_data[dir->prev_index].menu_str;
	}

	ol_Uart_Write(port, "\r\n"DEMO_MENU_LAYOUT_STR"\r\n",strlen("\r\n"DEMO_MENU_LAYOUT_STR"\r\n"));
	sprintf(buffer,"* %s :",title);
	ol_Uart_Write(port, buffer,strlen(buffer));
	ol_Uart_Write(port, "\r\n"DEMO_MENU_LAYOUT_STR"\r\n",strlen("\r\n"DEMO_MENU_LAYOUT_STR"\r\n"));

	for(i=0; i< dir->data_num; i++)
	{
		memset(buffer, 0, sizeof(buffer));
		if(strlen(info[i].menu_str)!=0)
		{
			sprintf(buffer, "*%d. %s %s", i, info[i].menu_str, info[i].param_syntax);
			strcat(buffer,"\r\n");
			ol_Uart_Write(port, buffer , strlen(buffer));
		}
	}
	ol_Uart_Write(port, DEMO_MENU_LAYOUT_STR"\r\n",strlen(DEMO_MENU_LAYOUT_STR"\r\n"));
	ol_Uart_Write(port, "quit or choice:" , strlen("quit or choice:"));
}

int demo_menu_uart_control(char port,char on)
{
	if(on)
	{
		OL_UART_DCB uart_dcb = {0};
		int err = 0;
	
		err = ol_Uart_Close(port);
		ol_os_task_sleep(200);	
		err = ol_Uart_GetDcb(port, &uart_dcb);
		//op_uart_printf("\r\nol_Uart_GetDcb %d", err);
		if(OL_UART_RC_OK != err){
			op_uart_printf("\r\nuart_demo ol_Uart_GetDcb error %d", err);
			return -1 ;
		}
		uart_dcb.rd_cb = demo_menu_uart_handle;	
		ol_Uart_SetDcb(port, &uart_dcb);
		op_uart_printf("\r\ndemo MENU uart opMode:%d, baudRate:%d, numDataBits:%d, parityBitType:%d,flowControl:%d",
			uart_dcb.opMode, uart_dcb.baudRate, uart_dcb.numDataBits, uart_dcb.parityBitType, uart_dcb.flowControl);
		
		err = ol_Uart_Open(port);
		op_uart_printf("\r\nol_Uart_Open %d", err);
		if(OL_UART_RC_OK != err)
			return -1;
	}
	else
	{
		ol_Uart_Close(port);	
	}

	return 0;
}



void demo_menu_test(void)
{
	char ret = 0;
	unsigned char menu_num = sizeof(menu_info)/sizeof(menu_info[0]);
	mbtk_os_status os_status;
	
	//init context
	memset(&context,0x0,sizeof(demo_menu_ctx));
	//push main menu,no prev
	DEMO_MENU_PATHPUSH(&context,DEMO_MENU_NEWNODE(menu_info,menu_num),0);

	os_status = ol_os_msgq_creat(&demo_menu_msgref,"demo_menu_msg", 
		sizeof(demo_menu_msg_struct), 1, MBTK_OS_FIFO);
	if(os_status != mbtk_os_success){
		op_uart_printf("demo menu create msgq fail\r\n");
		return;
	}
	
	ret = demo_menu_uart_control(DEMO_MENU_UART_PORT,1);
	if(0 == ret)
	{
		while(1)
		{
			char demo_menu_str[100] = {0};
			unsigned char demo_index = 0;
			menu_node_t *current_dir = context.node;
			demo_menu_msg_struct msg = {0};
			
			if(current_dir == NULL)
				break;
			demo_menu_display(DEMO_MENU_UART_PORT,current_dir);
			menu_ready_flag = 1;
			memset(&msg,0x0,sizeof(demo_menu_msg_struct));
			ol_os_msgq_recv(demo_menu_msgref, &msg, sizeof(demo_menu_msg_struct), MBTK_OS_SUSPEND);
			menu_ready_flag = 0;
			if(msg.quit_flag){
				DEMO_MENU_FREENODE(DEMO_MENU_PATHPOP(&context));
				continue;
			}

			if(msg.index > current_dir->data_num - 1){
				ol_Uart_Write(DEMO_MENU_UART_PORT, "\r\nInput is error" , strlen("\r\nInput is error"));
				continue;
			}
			demo_menu_param_parser(&context,msg.param_msg);
			
			demo_index = msg.index;
			if(current_dir->menu_data[demo_index].info_handler)
			{
				unsigned int sub_menu_num = 0;
				demo_menu_info *sub_menu = NULL;

				sub_menu = (demo_menu_info *)current_dir->menu_data[demo_index].info_handler(&sub_menu_num);
				DEMO_MENU_PATHPUSH(&context,DEMO_MENU_NEWNODE(sub_menu,sub_menu_num),demo_index);
			}
			else if(current_dir->menu_data[demo_index].exec_handler)
			{
				sprintf(demo_menu_str, "\r\n >>>>%s Start:", current_dir->menu_data[demo_index].menu_str);
				ol_Uart_Write(DEMO_MENU_UART_PORT,demo_menu_str, strlen(demo_menu_str));
				
				current_dir->menu_data[demo_index].exec_handler();
				
				memset(demo_menu_str,0,sizeof(demo_menu_str));
				sprintf(demo_menu_str, "\r\n <<<<%s Finish:", current_dir->menu_data[demo_index].menu_str);
				ol_Uart_Write(DEMO_MENU_UART_PORT,demo_menu_str, strlen(demo_menu_str));
			}
			
			if(msg.param_msg){
				ol_free(msg.param_msg);
			}			
		}
	}
	
	ol_os_task_sleep(200);	
	demo_menu_uart_control(DEMO_MENU_UART_PORT,0);	
	ol_os_msgq_delete(demo_menu_msgref);
	demo_menu_msgref = NULL;
}

