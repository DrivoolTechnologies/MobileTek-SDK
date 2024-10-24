#include "mbtk_os.h"
#include "string.h"
#include "ol_wifi_api.h"
#include "mbtk_comm_api.h"
#include "mbtk_os.h"

#define TASK_STACK_SIZE 1024*10
#define TASK_PRIORITY 100

#define DEMO_TASK_NAME "wifi_demo_task"
#define WIFI_TIMEOUT 60

void mbtk_wifi_scan_callback(app_adp_wifi_result_t result, app_adp_wifi_ap_list * ap_list)
{
    int i;
    op_uart_printf("wifi_result:%d \n", result);
    op_uart_printf("wifi_ap_list count: %d \n", ap_list->count);
    op_uart_printf("Ap_list MAC rssi \n");
    for(i = 0; i < ap_list->count; i++)
    {
        uint8_t *mac = ap_list->item[i].mac;
        op_uart_printf(" [%d] [%02x %02x %02x %02x %02x %02x] [%d]\n", i+1,
        mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], ap_list->item[i].rssi);
    }
    return;
}
void wifi_demo_task(void *data)
{
    int ret = 0;

    op_uart_printf("===wifi demo task start!!===\r\n");

    ret = ol_wifi_mac_scan(WIFI_TIMEOUT,mbtk_wifi_scan_callback);
    if(ret != 0)
    {
        op_uart_printf("===wifi mac sacn error!!===\r\n");
        goto exit;
    }

    ol_os_task_sleep(20*200);

    ret = ol_wifi_mac_scan(WIFI_TIMEOUT,mbtk_wifi_scan_callback);
    if(ret != 0)
    {
        op_uart_printf("===wifi mac sacn error!!===\r\n");
        goto exit;
    }

    op_uart_printf("===wifi demo task end!!===\r\n");
    return;

exit:
    op_uart_printf("===wifi demo error %d!!===\r\n",ret);
    return;
}

void wifi_demo(void * argv)
{
    mbtk_taskref demo_task_ref = NULL;
    void *mbtk_wifi_task_stack = NULL;

    mbtk_wifi_task_stack = ol_malloc(TASK_STACK_SIZE);
    ol_os_task_creat(&demo_task_ref, mbtk_wifi_task_stack, TASK_STACK_SIZE, TASK_PRIORITY, DEMO_TASK_NAME, wifi_demo_task, NULL);
}