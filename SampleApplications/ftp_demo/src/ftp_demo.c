
#include "mbtk_pub_type.h"
#include "mbtk_pub_def.h"
#include "mbtk_api.h"
#include "mbtk_comm_api.h"

static char * response_data = NULL;
static int response_len = 0;



int ftp_demo_wait_network(void)
{
    if (ol_wait_network_regist(120) != mbtk_data_call_ok)
    {
        op_uart_printf("mqtt_demo_wait_network ol_wait_network_regist time out\n");
        return -1;
    }
    op_uart_printf("mqtt_demo_wait_network execute ol_data_call_start \n");
    if (ol_data_call_start(mbtk_cid_index_2, mbtk_data_call_v4, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
    {
        op_uart_printf("mqtt_demo_wait_network ol_data_call_start fail\n");
        return -1;
    }
    return 0;
}

mbtk_open_ftp_config_params_t config_params;

static char * server_addr = "118.114.239.159";
static char * server_name = "mbtk";
static char * server_passwd = "mbtk";
static int server_port  = 30110;

bool ftp_demo(void)
{
    char private_data[]="hello world!";
    int response_code,trynum=0;
    http_client * client = NULL;
    struct http_client_list * header = NULL;
    int ret = 1;
    char * p_list = NULL;
    unsigned long p_len = 0;

    op_uart_printf("ftp_demo hello world\n");

    if (ftp_demo_wait_network() != 0)
    {
        op_uart_printf("http_demo_wait_network fail\n");
        return -1;
    }
    op_uart_printf("ftp_demo ftp_demo_wait_network succ\n");

    memset(&config_params,0,sizeof(mbtk_open_ftp_config_params_t));
    memcpy(config_params.host, server_addr,strlen(server_addr));
    memcpy(config_params.username, server_name,strlen(server_name));
    memcpy(config_params.password, server_passwd,strlen(server_passwd));

    config_params.port = server_port;
    config_params.mode = 0;
    config_params.timeout = 30;
    config_params.ftpType = 1;
    config_params.ssl_mode = 0;
    config_params.cert = 0;
    ol_ftp_setparams(&config_params);

    //ret = ol_ftp_list("./hello",&p_list,&p_len);
    //op_uart_printf("ftp_demo ret = %d p_list = %s p_len = %d\n",ret,p_list,p_len);
    //ret = ol_ftp_size("./hello/123");

    ret = ol_ftp_getfile("/hello/123", "/123", 0);
    op_uart_printf("ol_ftp_getfile 123 ret = %d",ret);

    ret = ol_ftp_putfile("/hello/234", "/123", 0);
    op_uart_printf("ol_ftp_getfile 123 ret = %d",ret);

    if (p_list!= NULL)
    {
        ol_free(p_list);
        p_list = NULL;
    }
    return 0;
}

