#ifndef __MENU_DEMO_API_H__
#define __MENU_DEMO_API_H__

#define DEMO_MENU_MAX_PARMA_NUM (8)


typedef void (*menu_exec_handler)(void);
typedef void *(*menu_info_handler)(unsigned int *num);

typedef struct demo_menu_info
{
	char menu_str[40];
	char param_syntax[64];
	menu_exec_handler exec_handler;
	menu_info_handler info_handler;
}demo_menu_info;

typedef struct{
    unsigned char param_num;
    unsigned char *param[DEMO_MENU_MAX_PARMA_NUM];
}demo_menu_param_list;


demo_menu_param_list *demo_menu_param_list_get(void);
char demo_menu_param_get_int(demo_menu_param_list *param_list,unsigned int index,
		int *value,unsigned int min,unsigned int max,int default_value);
char demo_menu_param_get_str(demo_menu_param_list *param_list,unsigned int index,
		char *value,unsigned int max_len,char *default_value);

#define DEMO_MEUN_GET_INT_PARAM(index,value,min,max,defalut)	\
	demo_menu_param_get_int(demo_menu_param_list_get(),index,value,min,max,defalut)

#define DEMO_MEUN_GET_STR_PARAM(index,value,maxlen,defalut) \
	demo_menu_param_get_str(demo_menu_param_list_get(),index,value,maxlen,defalut)

#endif
