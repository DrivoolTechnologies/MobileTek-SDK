#include "ol_fota.h"
#include "ol_flash_fs.h"
#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "mbtk_pmu.h"
#include "menu_demo_api.h"

#define FOTA_TEST_HTTP_URL    "http://118.114.239.159:30167/mbtk_fota.bin"
#define FOTA_TEST_FTP_URL		"118.114.239.159:30071/mbtk_fota.bin"
#define FOTA_LOCAL_CID mbtk_cid_index_2
#define FOTA_LOCAL_CID_PDN_TYPE mbtk_data_call_v4

extern void app_update_demo(void);
void fota_local_upgreade_demo(void);
void fota_network_upgreade_demo(void);
void fota_stop_reboot_test(void);

static demo_menu_info menu_info[] =
{
	{"local fota","[file_name]",fota_local_upgreade_demo,NULL},
	{"network fota","[type],[url],[name],[pass]",fota_network_upgreade_demo,NULL},
	{"app upgrade","[file_name]",app_update_demo,NULL},
    {"stop reboot","",fota_stop_reboot_test,NULL},
};

demo_menu_info *upgrade_demo_menu_info(unsigned int *num)
{
	*num = sizeof(menu_info)/sizeof(menu_info[0]);
	return menu_info;
}

int fota_demo_wait_network(void)
{
    if (ol_wait_network_regist(120) != mbtk_data_call_ok)
    {
        op_uart_printf("fota_demo_wait_network ol_wait_network_regist time out\n");
        return -1;
    }
    op_uart_printf("fota_demo_wait_network execute ol_data_call_start \n");

    ol_os_task_sleep(200 *5);
    
    if(ol_data_call_start(FOTA_LOCAL_CID, FOTA_LOCAL_CID_PDN_TYPE, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
    {
        op_uart_printf("fota_demo_wait_network ol_data_call_start fail\n");
        return -1;
    }
 
    return 0;
}

void fota_demo_callback(void *param)
{
    unsigned int process = ol_fota_get_proccess();
    mbtk_fota_status_result_t *status_res = param;
    
    op_uart_printf("fota_demo_callback\r\n");

    op_uart_printf("fota_demo_callback process = %d\r\n", process);
    op_uart_printf("fota_demo_callback status = %d\r\n", status_res->status);
    op_uart_printf("fota_demo_callback res = %d\r\n", status_res->res);
}


void fota_local_upgreade_demo(void)
{
		unsigned char local_file_name[64] = {0};
		unsigned char file_cache[256] = {0};
		int file_size = 0;
		int status = 0;
		int fp = 0;
		int ret = 0;

		ret = DEMO_MEUN_GET_STR_PARAM(0,local_file_name,64,"mbtk_fota.bin");
		op_uart_printf("get local fota package file ret %d,value %s",ret,local_file_name);

		file_size = ol_ffs_getsize(local_file_name);
		if (file_size < 0){
			ret = -1; 
			goto exit;
		}

		op_uart_printf("fota_demo get file size = %d\n", file_size);
		fp = ol_ffs_open(local_file_name, "rb");
		if (fp < 0) {
			ret = -1;
			goto exit;
		}
		ol_fota_context_init(NULL, file_size, false, NULL);

		while ((ret = ol_ffs_read(fp, file_cache, 256)) > 0){
			status = MBTK_FOTA_INPROGRESS;
			fota_demo_callback(&status);
			ret = ol_fota_pkg_write((char*)file_cache, ret, file_size);
			if (ret != 0){
				ret = -1;
				goto exit;
			}
		}
		
		if (ret < 0){
			ret = -1;
			goto exit;
		}
		status = MBTK_FOTA_SUCCEED;
		fota_demo_callback(&status);

		ret = ol_fota_pkg_flush_flash();
		if (ret != 0){
			ret = -1;
			goto exit;
		}

		ret = ol_fota_image_verify();
		if (ret != 0){
			ret = -1;
			goto exit;
		}

		status = MBTK_FOTA_SETFLAG;
		fota_demo_callback(&status);
		ret = 0;

exit:
		if (ret < 0){
			status = MBTK_FOTA_FAIL;
			fota_demo_callback(&status);
		}
		ol_fota_context_deinit();
		if (fp){
			ol_ffs_close(fp);
			ol_ffs_delete(local_file_name);
		}

		if(!ret){
			ol_power_reset();
		}

}

void fota_network_upgreade_demo(void)
{
    int ret = 0;
		mbtk_fota_server_info server_info = {0};
		
    op_uart_printf("fota_network_upgreade demo start!!\r\n");
		
		DEMO_MEUN_GET_INT_PARAM(0,&server_info.mode, 0,1,0);
		if(server_info.mode == 0){//ftp
			DEMO_MEUN_GET_STR_PARAM(1,server_info.host,MBTK_FOTA_SERVER_CONTEXT_STRLEN,FOTA_TEST_FTP_URL);
			DEMO_MEUN_GET_STR_PARAM(2,server_info.username,MBTK_FOTA_SERVER_CONTEXT_STRLEN,"ffx_test");
			DEMO_MEUN_GET_STR_PARAM(3,server_info.password,MBTK_FOTA_SERVER_CONTEXT_STRLEN,"ffx_test");
		}else{
			DEMO_MEUN_GET_STR_PARAM(1,server_info.host,MBTK_FOTA_SERVER_CONTEXT_STRLEN,FOTA_TEST_HTTP_URL);
		}

		op_uart_printf("fota_network_upgreade mode %d,url %s",server_info.mode,server_info.host);
		
		if (fota_demo_wait_network() != 0)
    {
        op_uart_printf("fota_demo_wait_network fail\n");
        return;
    }

    ret = ol_fota_firmware_download(&server_info, true, fota_demo_callback);
    op_uart_printf("fota_demo ol_fota_firmware_download ret = %d\n", ret);

}

void fota_stop_reboot_test(void)
{
    ol_fota_stop_reboot();
}
