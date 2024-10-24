/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "mbtk_comm_api.h"
#include "ol_ws_client.h"


#define NO_DATA_TIMEOUT_SEC 10

#define CONFIG_WEBSOCKET_URI	"wss://118.114.239.159:30073/v1"

#define CONFIG_WEBSOCKET_URI_O	"ws://118.114.239.159:30073/v1"

#define portTICK_RATE_MS   5
#define portTICK_PERIOD_MS 1000

#define SOCKET_LOCAL_CID mbtk_cid_index_2
#define LOCAL_CID_PDN_TYPE mbtk_data_call_v4v6
int ws_demo_wait_network(void)
{
    if(ol_wait_network_regist(120) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_wait_network_regist time out\n");
        return -1;
    }
    op_uart_printf("socket_demo_wait_network execute ol_data_call_start \n");
    if(ol_data_call_start(SOCKET_LOCAL_CID, LOCAL_CID_PDN_TYPE, "cmnet", "", "", 1) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_data_call_start fail\n");
        return -1;
    }

    return 0;
}


static void websocket_event_handler(int32_t event_id, void *event_data)
{
	op_uart_printf("%s, client handler is run and event_id is %d", __FUNCTION__, event_id);
    mbtk_websocket_event_data_t *data = (mbtk_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        op_uart_printf("WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        op_uart_printf("WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        op_uart_printf("WEBSOCKET_EVENT_DATA");
        op_uart_printf("Received opcode=%x", data->op_code);
        if (data->op_code == 0x08 && data->data_len == 2) {
            op_uart_printf("Received closed message with code=%d", 256*data->data_ptr[0] + data->data_ptr[1]);
        } else {
            op_uart_printf("Received=%.*s", data->data_len, (char *)data->data_ptr);
        }
        op_uart_printf("Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);
		//OSATimerStart(shutdown_signal_timer, portMAX_DELAY, 0, shutdown_signaler, 0);
		break;
    case WEBSOCKET_EVENT_ERROR:
        op_uart_printf("WEBSOCKET_EVENT_ERROR");
        break;
	case WEBSOCKET_EVENT_CLOSED:
		op_uart_printf("WEBSOCKET_EVENT_CLOSED");
		break;
	default:
        op_uart_printf("websocket event_id is NULL");
		break;
    }

	if (NULL != data) {
		if (NULL != data->data_ptr) {
			ol_free(data->data_ptr);
			data->data_ptr = NULL;
			op_uart_printf("%s: free data_ptr", __FUNCTION__);
		}
		op_uart_printf("%s: free data", __FUNCTION__);
		ol_free(data);
		data = NULL;
	}
	return;
}

static void websocket_app_start(void)
{
	mbtk_os_status	status;
	int 		ret = 0;
    mbtk_websocket_client_config_t websocket_cfg = {0};

    websocket_cfg.uri = CONFIG_WEBSOCKET_URI_O;

    op_uart_printf("Connecting to %s...", websocket_cfg.uri);

    mbtk_websocket_client_handle_t client = ol_ws_client_init(&websocket_cfg);
    ol_ws_register_events(client, websocket_event_handler);

    ol_ws_client_start(client);
	
	//status = OSATimerStart(shutdown_signal_timer, portMAX_DELAY, 0, shutdown_signaler, 0);
    //ASSERT( status == mbtk_os_success );
#if 1
    char data[32];
    int i = 0;
    while (i < 10) {
        if (ol_ws_client_is_connected(client)) {
            int len = sprintf(data, "hello %d", i++);
            op_uart_printf("Sending %s", data);
            ret = ol_ws_client_send_text(client, data, len, portMAX_DELAY);
			if (0 > ret) {
				op_uart_printf("Sending failed and ret=%d", ret);
				continue;
			}
        }
        ol_os_task_sleep(1000 / portTICK_RATE_MS);
    }

	//status = OSASemaphoreAcquire(shutdown_sema, OS_SUSPEND);
	ol_os_task_sleep(5000 / portTICK_RATE_MS);
	op_uart_printf("Websocket Stopped");
	ol_ws_client_close(client,portMAX_DELAY);
	ol_ws_client_destroy(client);	
#endif
	return;
}

int wb_app_main(void)
{
    op_uart_printf("[APP] Startup..");

    if(ws_demo_wait_network() == 0)
		websocket_app_start();
	return 0;
}
