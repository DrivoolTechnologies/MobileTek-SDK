#include "mbtk_open_at.h"
#include "ol_os_comm_event.h"
#include "mbtk_comm_api.h"

int ol_factory_get(char* product_id, char* device_name, char* device_secret);


void visual_at_demo(void)
{
    op_uart_printf("-----visual_at_demo start!!!------\n");

    //create a msgref
    mbtk_msgqref msgref;
    int result = 0;
    int event = 0;

    result = ol_os_msgq_creat(&msgref, "visual_at_demo", sizeof(int), 20, MBTK_OS_FIFO);
    if (result != mbtk_os_success)
    {
        op_uart_printf("-----create msgref fail!!!------\n");
        return;
    }
    op_uart_printf("-----create msgref success!!!------\n");
    
    //init at
    if (ol_at_init(&msgref) != OL_E_NONE)
    {
        op_uart_printf("-----at init fail!!!------\n");
        ol_os_msgq_delete(msgref);
        return;
    }
    op_uart_printf("-----at init success!!!------\n");
    
    //send at
    ol_send_at_command("AT+CSQ\r\n");

    //wait at event
    result = ol_os_msgq_recv(msgref, &event, sizeof(int), 5*200);
    op_uart_printf("-----msgq recv result = %d, event = %d!!!------\n",result,event);
    if (result == mbtk_os_success && event == OL_EVENT_AT_RESP)
    {
        //at event recved,read resp
        char rsp_str[128] = {0};

        ol_read_at_response(rsp_str,128);
        op_uart_printf("-----at rsp str = %s------\n",rsp_str);
    }
    else
    {
        op_uart_printf("-----at rsp err or timeout ------\n");
    }
    ol_at_deinit();
    ol_os_msgq_delete(msgref);

    op_uart_printf("-----visual_at_demo end!!!------\n");
}

int ol_factory_get(char* product_id, char* device_name, char* device_secret)
{
    op_uart_printf("-----visual_at_demo start!!!------\n");

    //create a msgref
    mbtk_msgqref msgref;
    int result = 0;
    int event = 0;

    result = ol_os_msgq_creat(&msgref, "visual_at_demo", sizeof(int), 20, MBTK_OS_FIFO);
    if (result != mbtk_os_success)
    {
        op_uart_printf("-----create msgref fail!!!------\n");
        return -1;
    }
    op_uart_printf("-----create msgref success!!!------\n");
    
    //init at
    if (ol_at_init(&msgref) != OL_E_NONE)
    {
        op_uart_printf("-----at init fail!!!------\n");
        ol_os_msgq_delete(msgref);
        return -1;
    }
    op_uart_printf("-----at init success!!!------\n");
    
    //send at
    ol_send_at_command("AT+FACTORY?\r\n");

    //wait at event
    result = ol_os_msgq_recv(msgref, &event, sizeof(int), 5*200);
    op_uart_printf("-----msgq recv result = %d, event = %d!!!------\n",result,event);
    if (result == mbtk_os_success && event == OL_EVENT_AT_RESP)
    {
        //at event recved,read resp
        char rsp_str[128] = {0};
        char *start, *end;
        ol_read_at_response(rsp_str,128);
        op_uart_printf("-----at rsp str = %s------\n",rsp_str);
        start = strchr(rsp_str, '<');
        end = strchr(start, '>');
        memcpy(product_id, start + 1, end - start - 1);
        start = strchr(end, '<');
        end = strchr(start, '>');
        memcpy(device_name, start + 1, end - start - 1);
        start = strchr(end, '<');
        end = strchr(start, '>');
        memcpy(device_secret, start + 1, end - start - 1);
        
    }
    else
    {
        op_uart_printf("-----at rsp err or timeout ------\n");
    }
    ol_at_deinit();
    ol_os_msgq_delete(msgref);

    op_uart_printf("-----visual_at_demo end!!!------\n");
    return 0;
}

