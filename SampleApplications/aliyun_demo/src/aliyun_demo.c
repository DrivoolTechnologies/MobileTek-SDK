#include "mbtk_comm_api.h"
#include "ol_aliyunclient.h"
#include "mbtk_datacall_api.h"
#include "stdio.h"
#include "string.h"

#define MQTT_LOCAL_CID mbtk_cid_index_2
#define MQTT_LOCAL_CID_PDN_TYPE mbtk_data_call_v4

int aliyun_demo_wait_network(void)
{
	if(ol_wait_network_regist(120) != mbtk_data_call_ok)
	{
		op_uart_printf("aliyun_demo_wait_network ol_wait_network_regist time out");
		return -1;
	}
	op_uart_printf("aliyun_demo_wait_network execute ol_data_call_start ");
	if(ol_data_call_start(MQTT_LOCAL_CID, MQTT_LOCAL_CID_PDN_TYPE, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
	{
		op_uart_printf("aliyun_demo_wait_network ol_data_call_start fail");
		return -1;
	}

	return 0;
}


static void aliyun_sub_topic_handle(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    op_uart_printf("pub, qos: %d, topic: %.*s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
    op_uart_printf("pub, payload: %.*s", packet->data.pub.payload_len, packet->data.pub.payload);
}
static int32_t aliyun_pub_handle1(aliyun_client_t *aliyun_c)
{
    char *pub_topic = "/a1mnkmhVJBL/device2/user/TEST";
    char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";

    return ol_aliyun_pub(aliyun_c, pub_topic, pub_payload, (uint32_t)strlen(pub_payload), 1);
}

static int32_t aliyun_pub_handle2(aliyun_client_t *aliyun_c)
{
    char *pub_topic = "/a1mnkmhVJBL/device2/user/get";
    char *pub_payload = "{\"id\":\"1\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":0}}";

    return ol_aliyun_pub(aliyun_c, pub_topic, pub_payload, (uint32_t)strlen(pub_payload), 1);
}


/* MQTT事件回调函数, 当网络连接/重连/断开时被触发, 事件定义见core/aiot_mqtt_api.h */
void aliyun_demo_event_handler(void *handle, const aiot_mqtt_event_t *event, void *userdata)
{
    switch (event->type) {
        /* SDK因为用户调用了aiot_mqtt_connect()接口, 与mqtt服务器建立连接已成功 */
        case AIOT_MQTTEVT_CONNECT: {
            op_uart_printf("AIOT_MQTTEVT_CONNECT");
            /* TODO: 处理SDK建连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络状况被动断连后, 自动发起重连已成功 */
        case AIOT_MQTTEVT_RECONNECT: {
            op_uart_printf("AIOT_MQTTEVT_RECONNECT");
            /* TODO: 处理SDK重连成功, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        /* SDK因为网络的状况而被动断开了连接, network是底层读写失败, heartbeat是没有按预期得到服务端心跳应答 */
        case AIOT_MQTTEVT_DISCONNECT: {
            char *cause = (event->data.disconnect == AIOT_MQTTDISCONNEVT_NETWORK_DISCONNECT) ? ("network disconnect") :
                          ("heartbeat disconnect");
            op_uart_printf("AIOT_MQTTEVT_DISCONNECT: %s", cause);
            /* TODO: 处理SDK被动断连, 不可以在这里调用耗时较长的阻塞函数 */
        }
        break;

        default: {

        }
    }
}

void aliyun_demo_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    switch (packet->type) {
        case AIOT_MQTTRECV_HEARTBEAT_RESPONSE: {
            op_uart_printf("heartbeat response");
            /* TODO: 处理服务器对心跳的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_UNSUB_ACK: {
            op_uart_printf("unsuback, packet id: %d", packet->data.unsub_ack.packet_id);
        }
        break;

        case AIOT_MQTTRECV_SUB_ACK: {
            op_uart_printf("suback, res: -0x%04X, packet id: %d, max qos: %d",
                   -packet->data.sub_ack.res, packet->data.sub_ack.packet_id, packet->data.sub_ack.max_qos);
            /* TODO: 处理服务器对订阅请求的回应, 一般不处理 */
        }
        break;

        case AIOT_MQTTRECV_PUB: {
            op_uart_printf("pub, qos: %d, topic_len: %d, topic: %s", packet->data.pub.qos, packet->data.pub.topic_len, packet->data.pub.topic);
            op_uart_printf("pub, payload_len: %d, payload: %s", packet->data.pub.payload_len, packet->data.pub.payload);
            /* TODO: 处理服务器下发的业务报文 */
        }
        break;

        case AIOT_MQTTRECV_PUB_ACK: {
            op_uart_printf("puback, packet id: %d", packet->data.pub_ack.packet_id);
            /* TODO: 处理服务器对QoS1上报消息的回应, 一般不处理 */
        }
        break;

        default: {

        }
    }
}


void aliyun_demo(void)
{
    int ret;
    void *aliyun_c = NULL;

    
    if(aliyun_demo_wait_network() != 0)
    {
        op_uart_printf("aliyun_demo socket_demo_wait_network fail");
        return ;
    }

    aliyun_c = ol_aliyun_lease();

    ret = ol_aliyun_set_con_mode(aliyun_c, ONE_MACHINE_ONE_SECRET, 0);
    op_uart_printf("set_con_mode ret = %d", ret);
/*
    ret = ol_aliyun_set_host(aliyun_c, "iot-06z00fqohg83t4i.mqtt.iothub.aliyuncs.com");
    op_uart_printf("set_host ret = %d", ret);    
*/    
    ret = ol_aliyun_set_product_key(aliyun_c, "a1mnkmhVJBL");
    op_uart_printf("set_product_key ret = %d", ret);
    

    ret = ol_aliyun_set_product_secret(aliyun_c, "ES7PcVbDY5qWfISP");
    op_uart_printf("set_product_secret ret = %d", ret);
    
    ret = ol_aliyun_set_device_name(aliyun_c, "device2");
    op_uart_printf("set_device_name ret = %d", ret);

    ret = ol_aliyun_set_device_secret(aliyun_c, "c052106f19887e3f1671935da5d14224");
    op_uart_printf("set_device_secret ret = %d", ret);
    
    ret = ol_aliyun_set_keep_alive_sec(aliyun_c, 30);
    op_uart_printf("keep_alive_sec ret = %d", ret);

    ret = ol_aliyun_set_heartbeat_interval_ms(aliyun_c, 100 * 1000);
    op_uart_printf("keep_alive_sec ret = %d", ret);
        
    ret = ol_aliyun_set_event_handler(aliyun_c, aliyun_demo_event_handler);
    op_uart_printf("set_heartbeat_interval_ms ret = %d", ret);

    ret = ol_aliyun_set_recv_handler(aliyun_c, aliyun_demo_recv_handler);
    op_uart_printf("set_recv_handle ret = %d", ret);
    
    ret = ol_aliyun_device_auth(aliyun_c);
    op_uart_printf("device_auth ret = %d", ret);
    
    ret = ol_aliyun_connect(aliyun_c);
    op_uart_printf("connect ret = %d", ret);
    if(ret < 0)
    {
        return;
    }

    ret = ol_aliyun_sub(aliyun_c, "/a1mnkmhVJBL/device2/user/TEST", aliyun_sub_topic_handle, 0);
    op_uart_printf("sub TEST ret = %d", ret);
    
    ret = ol_aliyun_sub(aliyun_c, "/a1mnkmhVJBL/device2/user/update", aliyun_sub_topic_handle, 0);
    op_uart_printf("sub TEST update = %d", ret);

    ol_os_task_sleep(2 * 200);
        
    ret = aliyun_pub_handle1(aliyun_c);
    op_uart_printf("pub TEST ret = %d", ret);
    
    ret = aliyun_pub_handle2(aliyun_c);
    op_uart_printf("pub get ret = %d", ret);
    
    ol_os_task_sleep(30*200);

//    ret = ol_aliyun_unsub(aliyun_c, "/a1mnkmhVJBL/device2/user/TEST");
//    op_uart_printf("unsub TEST ret = %d", ret);
    
    ret = ol_aliyun_unsub(aliyun_c, "/a1mnkmhVJBL/device2/user/AAAAAA");
    op_uart_printf("unsub AAAAAA ret = %d", ret);
        
    ol_os_task_sleep(500*200);

    ret = ol_aliyun_disconnect(aliyun_c);
    op_uart_printf("disconnect ret = %d", ret);

    ol_os_task_sleep(20*200);
    ret = ol_aliyun_release(aliyun_c);
    op_uart_printf("release ret = %d", ret);

	op_uart_printf("aliyun demo end!!");
}
