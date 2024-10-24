
#include "mbtk_pub_type.h"
#include "mbtk_pub_def.h"

#include "mbtk_api.h"
#include "mbtk_comm_api.h"

static char * response_data = NULL;
static int response_len = 0;

/*
data: the data is received http context data
len: the length of http context data 
num: the http response header "Content-Length" value
cbdata: user private data
*/
static void response_cb(char * data, int len, int num, void *cbdata)
{
    char * temp;
    char *private_data=(char *)cbdata;

    op_uart_printf("[http_client_test]Get private_data %s", data);

    op_uart_printf("[http_client_test]Get data %d", len);
/*
    temp = (void*)ol_malloc(response_len + len);
    if (!temp)
    {
        op_uart_printf("[http_client_test]Malloc failed...");
        return;
    }
    memset(temp, 0, response_len + len);
    if (response_data)
    {
        memcpy(temp, response_data, response_len);
        ol_free(response_data);
    }
    memcpy(&temp[response_len], data, len);
    response_data = temp;
    response_len += len;
*/
}

int http_demo_wait_network(void)
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

bool http_demo(void)
{
    char private_data[]="hello world!";
    int response_code,trynum=0;
    http_client * client = NULL;
    struct http_client_list * header = NULL;

    if (http_demo_wait_network() != 0)
    {
        op_uart_printf("http_demo_wait_network fail\n");
        return -1;
    }

    client = ol_http_client_init();

    while (trynum<10) 
    {
        trynum++;
        ol_os_task_sleep(2000); // 10 SEC
        //http_client_setopt(client, HTTPCLIENT_OPT_PDP_CID, 0);              /*set PDP cid,if not set,using default PDP*/
        
        //http_client_setopt(client, HTTPCLIENT_OPT_BIND_PORT, 56521);              /*set tcp src port,if not set,using random port*/
        
        ol_http_client_setopt(client, HTTPCLIENT_OPT_URL, "http://www.baidu.com");    /*set URL:support http/https and ip4/ip6*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_HTTP1_0, 0);                /*"0" is HTTP 1.1, "1" is HTTP 1.0*/
        
        ol_http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, response_cb);     /*response callback*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, private_data);            /*set user private data,*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);    /*set method,support GET/POST/PUT*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, NULL);            /*post data is http context*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, 0);            /*http context length*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_TYPE, HTTP_AUTH_TYPE_BASE);    /*auth type support base and digest*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_USERNAME, "admin");             /*username*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_PASSWORD, "admin");             /*password*/
#ifdef USER_PRIVATE_HEADER
        // Add private HTTP header
        header = ol_http_client_list_append(header, "Content-Type: text/xml;charset=UTF-8\r\n");
        header = ol_http_client_list_append(header, "SOAPAction:\r\n");
#endif
        /*set http private header,our http stack already support Referer/Host/Authorization/User-Agent/Connection/cookie/Content-Length,
        this header is other http header,ex: Content-Type..., we call it private header,*/
        ol_http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

        ol_http_client_getinfo(client, HTTPCLIENT_GETINFO_TCP_STATE, &response_code);
        op_uart_printf("[http_client_test]Get tcp state %d", response_code);
        
        if(response_code != HTTP_TCP_ESTABLISHED){
            op_uart_printf("[http_client_test]http tcp not connecting");
            ol_http_client_shutdown(client); /*release http resources*/
            client = ol_http_client_init();
            continue;
        }

        response_code=ol_http_client_perform(client);      /*execute http send and recv*/
        if(response_code != HTTP_CLIENT_OK){
            op_uart_printf("[http_client_test]http perform have some wrong[%d]",response_code);
            ol_http_client_shutdown(client); /*release http resources*/
            client = ol_http_client_init();
            continue;

        }
        
        ol_http_client_getinfo(client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200 && response_code != 204) {
            op_uart_printf("[http_client_test]response is not 'HTTP OK'(%d)", response_code);
        }

        op_uart_printf("[http_client_test]Get total data %d", response_len);
        op_uart_printf("[http_client_test]Get total data %s", response_data);
        if (response_data) ol_free(response_data);
        response_data = NULL;
        response_len = 0;

#ifdef USER_PRIVATE_HEADER
        // Free private HTTP header
        if (header) {
            ol_http_client_list_destroy(header);
            header = NULL;
        }
#endif
    }
    ol_http_client_shutdown(client); /*release http resources*/
    return 1;
}

struct http_client * g_http_client = NULL;
struct http_client_list * g_http_header = NULL;

void  test_http_asyn_finish_cb(int error)
{
    int response_code=0;

    op_uart_printf("mbtk test_http_asyn_finish_cb error %d", error);
    if(error == HTTP_CLIENT_OK)
    {
        ol_http_client_getinfo(g_http_client, HTTPCLIENT_GETINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200 && response_code != 204) {
            op_uart_printf("[http_client_test]response is not 'HTTP OK'(%d)", response_code);
        }
        op_uart_printf("[http_client_test]Get total data %d", response_len);
        op_uart_printf(response_data);
        if (response_data) ol_free(response_data);
        response_data = NULL;
        response_len = 0;
#ifdef USER_PRIVATE_HEADER
        // Free private HTTP header
        if (g_http_header) {
            ol_http_client_list_destroy(g_http_header);
            g_http_header = NULL;
        }
#endif
    }
}

void http_client_demo_noblock(void )
{
    static char private_data_test[]="hello world!";
    int response_code;
    struct http_client *client;
    struct http_client_list * header = NULL;
    op_uart_printf("mbtk http_client_test_asyn");
    op_uart_printf("[http_client_test] mbtk http_client_test_asyn");
    if(http_demo_wait_network() != 0)
    {
        op_uart_printf("http_demo_wait_network fail\n");
        return;
    }
    client = ol_http_client_init();
    g_http_client = client;

    //ol_http_client_setopt(client, HTTPCLIENT_OPT_PDP_CID, 0);              /*set PDP cid,if not set,using default PDP*/

    //ol_http_client_setopt(client, HTTPCLIENT_OPT_BIND_PORT, 56521);              /*set tcp src port,if not set,using random port*/
    
    ol_http_client_setopt(client, HTTPCLIENT_OPT_URL, "https://www.baidu.com");    /*set URL:support http/https and ip4/ip6*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_HTTP1_0, 0);                /*"0" is HTTP 1.1, "1" is HTTP 1.0*/

    ol_http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB, response_cb);     /*response callback*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_RESPONSECB_DATA, private_data_test);            /*set user private data,*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_METHOD, HTTPCLIENT_REQUEST_GET);    /*set method,support GET/POST/PUT*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_POSTDATA, NULL);            /*post data is http context*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_POSTLENGTH, 0);            /*http context length*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_TYPE, HTTP_AUTH_TYPE_BASE);    /*auth type support base and digest*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_USERNAME, "admin");             /*username*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_AUTH_PASSWORD, "admin");             /*password*/
#ifdef USER_PRIVATE_HEADER
    // Add private HTTP header
    header = ol_http_client_list_append(header, "Content-Type: text/xml;charset=UTF-8\r\n");
    header = ol_http_client_list_append(header, "SOAPAction:\r\n");
#endif
    /*set http private header,our http stack already support Referer/Host/Authorization/User-Agent/Connection/cookie/Content-Length,
    this header is other http header,ex: Content-Type..., we call it private header,*/
    ol_http_client_setopt(client, HTTPCLIENT_OPT_HTTPHEADER, header);

    g_http_header = header;
    ol_http_client_getinfo(client, HTTPCLIENT_GETINFO_TCP_STATE, &response_code);
    op_uart_printf("[http_client_test]Get tcp state %d", response_code);
    
    if(response_code != HTTP_TCP_ESTABLISHED){
        op_uart_printf("[http_client_test]http tcp not connecting");
        ol_http_client_shutdown(client); /*release http resources*/
        client = ol_http_client_init();
    }
    op_uart_printf("mbtk befor http_client_perform_asyn");
    op_uart_printf("[http_client_test] mbtk befor http_client_perform_asyn");
    op_uart_printf("[http_client_test]client =%p",client);
    response_code=ol_http_client_perform_asyn(client, test_http_asyn_finish_cb);    /*execute http send and recv*/
    if(response_code == 0)
    {
        op_uart_printf("mbtk http_client_perform_asyn ok");
    }
    else
    {
        op_uart_printf("mbtk http_client_perform_asyn fail");
    }
}

