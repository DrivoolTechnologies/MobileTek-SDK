#include "mbtk_comm_api.h"
#include "ol_mqttclient.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include <stdio.h>

#define MQTT_LOCAL_CID mbtk_cid_index_2
#define MQTT_LOCAL_CID_PDN_TYPE mbtk_data_call_v4

int mqtt_demo_wait_network(void)
{
	if(ol_wait_network_regist(120) != mbtk_data_call_ok)
	{
		op_uart_printf("mqtt_demo_wait_network ol_wait_network_regist time out\n");
		return -1;
	}
	op_uart_printf("mqtt_demo_wait_network execute ol_data_call_start \n");
	if(ol_data_call_start(MQTT_LOCAL_CID, MQTT_LOCAL_CID_PDN_TYPE, "ctnet", NULL, NULL, 0) != mbtk_data_call_ok)
	{
		op_uart_printf("mqtt_demo_wait_network ol_data_call_start fail\n");
		return -1;
	}

	return 0;
}

void mqtt_error_callback(void* client, ol_mqtt_error_t error)
{
    op_uart_printf("op_uart_printf error_num = %d", error);
    if(error == MQTT_CONNECT_FAILED_ERROR)
    {
        op_uart_printf("MQTT CONNECT ERROR");
    }
}

static void sub_topic_handle1(void* client, message_data_t* msg)
{
    (void) client;
	op_uart_printf("sub_topic_handle1\r\n");
	op_uart_printf("topic: %s\r\n",msg->topic_name);
	op_uart_printf("message:%s\r\n",(char*)msg->message->payload);
}

static int mqtt_publish_handle1(mqtt_client_t *client)
{
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));

	static int value = 0;
	char payload[128] = {0};

	if(value == 10)
		value = 0;
	value ++;
	sprintf(payload,"{\"id\":\"%d\",\"version\":\"1.0\",\"params\":{\"aa\":{\"value\":%d}}}" ,100 + value,value);
	
    msg.qos = QOS0;
    msg.payload = (void *)payload;

    return ol_mqtt_publish(client, "test", &msg);
}

static const char *ca_crt = {
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDKjCCAhICCQCOewfZiRCiNjANBgkqhkiG9w0BAQUFADBXMQswCQYDVQQGEwJD\r\n"
"TjEQMA4GA1UECBMHU2lDaHVhbjEVMBMGA1UEChMMTU9CSUxFVEVLLkNBMQswCQYD\r\n"
"VQQLEwJJVDESMBAGA1UEAxMJTU9CSUxFVEVLMB4XDTE4MDkxODA4MDUzMloXDTMz\r\n"
"MDkxOTA4MDUzMlowVzELMAkGA1UEBhMCQ04xEDAOBgNVBAgTB1NpQ2h1YW4xFTAT\r\n"
"BgNVBAoTDE1PQklMRVRFSy5DQTELMAkGA1UECxMCSVQxEjAQBgNVBAMTCU1PQklM\r\n"
"RVRFSzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOkdYJF1h1xjKbY0\r\n"
"ipbl88G653PiCh8ZMjmIUYeiDKC8+0wtXZtHvQIl6AncOzBy9XHVOctbKn34exC8\r\n"
"SEotMuo2T49vs9VtE8GYu2pOrf3m42NpLRnYAxfm9qw53CMHx+Jn7Oa9fnxa8haA\r\n"
"pRc2BTVadWGoS8EEwoZFk0eNb7Z2Gc7U0c+GhISI4oVTTocGvGgMzkvduu5JJbbc\r\n"
"BOcNFrii9sRO9vtOYQtqOEg01Uum2Dwp/o2bDLXNJEqAIh4WACiM4iPmmlRHWT2y\r\n"
"NjQ3vcbEdrFwbHRtO46+Vw54HnSyCoFb3uCHMNMvXObZ/8AU9E3Cgat4j0sgEeB0\r\n"
"hqA4MiMCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEAtEAjf0CjsLgG9ROdmp1qXYft\r\n"
"+ndIT5l82KRK57ZQsfdFbnJOvALeF/ICKU0M2TXgJNiGOA5RxDi00YYdMbOIPwVZ\r\n"
"JH4b87J/LYdLAGf+Q+kVI6gWH3hPm4Jzfzq/40KVrf3mpa54yWz6ZYtwfxBjrMgr\r\n"
"IVe0O5SIJ99lsddgzgUkqYN2vWJW2zZ50xuXOAyo+pOnjzX0wuOcaBT3JCHWJRAb\r\n"
"VhJCf9JbswDgnddJerqFtB8pnpAOdGokLCOoM06q3s3P9mhGX+72HXdX7G8CSAuG\r\n"
"PVCGf6RaF0/G4B9R1c3du3lZRlQWfx2pxyU0LS86iFQFWqzqcWEXIcULVdcErQ==\r\n"
"-----END CERTIFICATE-----\r\n"
};

static const char *cli_crt = {
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDKTCCAhECCQDpY/MI6IXYcTANBgkqhkiG9w0BAQUFADBXMQswCQYDVQQGEwJD\r\n"
"TjEQMA4GA1UECBMHU2lDaHVhbjEVMBMGA1UEChMMTU9CSUxFVEVLLkNBMQswCQYD\r\n"
"VQQLEwJJVDESMBAGA1UEAxMJTU9CSUxFVEVLMB4XDTE4MDkxODA4MDcxNloXDTMz\r\n"
"MDkxOTA4MDcxNlowVjELMAkGA1UEBhMCQ04xEDAOBgNVBAgTB1NpQ2h1YW4xFDAS\r\n"
"BgNVBAoTC01PQklMRVRFSy5DMQswCQYDVQQLEwJJVDESMBAGA1UEAxMJTU9CSUxF\r\n"
"VEVLMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAk1EdX++SIEgEnJVm\r\n"
"T//7U8xBJBT4JL61A0MAIFSE+xLcFjKQ492ev19Y6XmrcrlrXfvaO+d9cCMvfjxc\r\n"
"xQ+aBOiNs0Sy2jHLdZJfLAKRo8Pt9eM1cX8ALaymigpmgL+7zeXNhW0ejzzCrsay\r\n"
"XLVWKZyOHNyv5kdDfRyfM1kGnPxU9qV6YNgwELz0+Nhf+b4DSNEUHDDT+eu2ZiVs\r\n"
"VoiC7Duwen2794yka1xyPrDQazW4uZMqUOZzZhrBptjjNXQ6PCHqcV5FDzFzXM7h\r\n"
"lklnl4ZOVOCSVoTCNLS3M35N+P70e4uSXM3SMJuMhFURUWrJ+BRFBBa/Oqv97lJZ\r\n"
"DLnnpQIDAQABMA0GCSqGSIb3DQEBBQUAA4IBAQAyUETpOQS6p6RAxDuX8/O7r5M0\r\n"
"ZYtzd+eUyx5tAt7H2YPCXRPBEpEppExayCihoWoEYsItfs8fQ7a6dA5goltK7bXv\r\n"
"XT3uVJnF5aBmVY3fdXiOmH5bTD7SUv7QWmXoA8qIfNcOgPL4wCwYaGdGwfP+x5+W\r\n"
"lHZK79BKli7MiABaMCmn/Ivzi1aq5iLFvCRs054ibuiz/LgVtRG912LHJbmRin0R\r\n"
"jtIzguJ7ViGW2eCFhlnpA8oACh2G7JQp6ifw8Mzi0N7rCVV8BjIc2Dfh0zWjbQ6D\r\n"
"uhiuzXYukHoEM/DRIbn66ffxb/R/tU6u2UewW3+Bv/h1vWzfKJdbqIIgwNbY\r\n"
"-----END CERTIFICATE-----\r\n"
};

static const char *cli_key = {
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAk1EdX++SIEgEnJVmT//7U8xBJBT4JL61A0MAIFSE+xLcFjKQ\r\n"
"492ev19Y6XmrcrlrXfvaO+d9cCMvfjxcxQ+aBOiNs0Sy2jHLdZJfLAKRo8Pt9eM1\r\n"
"cX8ALaymigpmgL+7zeXNhW0ejzzCrsayXLVWKZyOHNyv5kdDfRyfM1kGnPxU9qV6\r\n"
"YNgwELz0+Nhf+b4DSNEUHDDT+eu2ZiVsVoiC7Duwen2794yka1xyPrDQazW4uZMq\r\n"
"UOZzZhrBptjjNXQ6PCHqcV5FDzFzXM7hlklnl4ZOVOCSVoTCNLS3M35N+P70e4uS\r\n"
"XM3SMJuMhFURUWrJ+BRFBBa/Oqv97lJZDLnnpQIDAQABAoIBADv70kcm6EwGpwIO\r\n"
"/xrh8zb6uydy9lsX3+KVHG8NmxwUYNMVAdQWgDYSH8TpSrsq7qHVE9ZCetv3J7Zx\r\n"
"46BFqwQGoPP2rBCWJ+8Wi3QDE8Qn2jO9sRwmhy8R1rnsZDJWX6UFxjpm5QOTzP2X\r\n"
"YDbJi2zkFyV/YowURlGiHtysijnPEq0PcYlKrOMp7ee4znCKmcyEWU9EjvkVvBBS\r\n"
"WSgGYsMCePh4AzlfyhoxcO8NLWM7XTl32ov5IjUEqwV0oIVqB5yFFRxPS1gN5nSn\r\n"
"N6uggVrzwLjK4bgTGwbMAHkZhfXPeNvpSVbWLXqsgTYeyqLyikigzZrMF+PmMbKT\r\n"
"hgsOmr0CgYEAxFhLo8zMkoPmKc3Rfv3lpE6AtSIqdQfaJhVxbE+tiRzt4xpbfFtb\r\n"
"LbWGAUwcewesxMWyMFzl2nBixBPkIJkDYMkAo8dR26oL4QorWtb96xLQceDCF3L5\r\n"
"uI6Tm/3tCd1iNCEzg4yqCTtljelkcIyLwOClcscBlqIYe+ac6tj56PcCgYEAwBNr\r\n"
"CI/+s3QUiT7b9Na6WZhCItRmnA8Z3sUCdcDirm48UEwaWLFzuvolpxj9h9YrP/GQ\r\n"
"6ODnqZpaMgeTr0yW8c/LrXkW5o6RmRcnOLTWHHvphlaEgrU33xpmr0XbabOCl+TS\r\n"
"yKyrArYpyIL5wPAvCtOKHzDuQqRk+9lwBp3YyUMCgYEAramjDXghIR5Ev4jp2Tbc\r\n"
"nN1KfeuAPg755mFk9vXqebH6vrobXPy4ws8hfZhCQJdjOo/ZsWkZbIGm+eLWDfcI\r\n"
"w9xFEpdDUdUvuJX0Dt7Fq9vyPicbxP3O3mxNJtYLiIQlThJnq90IsC22/zQFwH6v\r\n"
"RaPs8n4Oa5tnqH8rH5VWRbUCgYAY/+4EdUl/bfJmUqoqWfBzTN/+zIp2cXi6iAXj\r\n"
"8bEPZwWupdkgBii8A2b3Msd88KE8d2KXDP7aEl7++AF+5YcX/iXSdFpIs/G7bUoL\r\n"
"lW3w0yf39jbVFGUrQrJuybbfMSAkSQIuYFr5xV/22yVKuXhF/naRzLqWLfN+3DQ6\r\n"
"iDz5JQKBgQCC+SK63r5sXt8xKdHy7KXNo9WcjBgDumiF3VuaBunqSasQHpYlUBol\r\n"
"M/ZvR2RLOuZWCM6XZcU84bowdSXMfWEwhMPbXvuVgMiYVfVGe9unomtRVOZdUqct\r\n"
"ULwV8cGeNE6fwrLN8hkXTmEjmL3ESNXKjinJp46U0IJ/4iVtdzfiNg==\r\n"
"-----END RSA PRIVATE KEY-----\r\n"
};



void mqtt_demo(void)
{
	mqtt_client_t *client = NULL;
    int i = 0,ret = 0;
		
	op_uart_printf("mqtt demo start!!\r\n");

	if(mqtt_demo_wait_network() != 0)
	{
		op_uart_printf("mqtt_demo socket_demo_wait_network fail\n");
		return ;
	}
	
    client = ol_mqtt_lease();
	if(client){
		ol_mqtt_set_ca(client, (char*)ca_crt);
		ol_mqtt_set_cli_crt(client, (char*)cli_crt);
		ol_mqtt_set_cli_key(client, (char*)cli_key);
		ol_mqtt_set_host(client, "118.114.239.159");
		ol_mqtt_set_port(client, "30167");
		ol_mqtt_set_user_name(client, "mbtk_test");
		ol_mqtt_set_password(client, "test");
		ol_mqtt_set_client_id(client, "test");
		ol_mqtt_set_clean_session(client, 1);
        ol_mqtt_set_error_callback(client, mqtt_error_callback);

		ret = ol_mqtt_connect(client);
		op_uart_printf("mqtt_connect ret = %d\r\n",ret);
		ol_os_task_sleep(2*200);
		ret = ol_mqtt_subscribe(client, "mbtk_test", QOS0, sub_topic_handle1);
		op_uart_printf("mqtt_subscribe ret = %d\r\n",ret);
		ol_os_task_sleep(4*200);

		for(i = 0; i < 5 ;i++)
		{
			ret = mqtt_publish_handle1(client);
			op_uart_printf("%d time mqtt_publish_handle1 ret = %d\r\n",i,ret);
			ol_os_task_sleep(4 * 1000);
		}

		ol_mqtt_disconnect(client);
		do{
			ret = ol_mqtt_release(client);
			ol_os_task_sleep(200);
		}while(ret!=0);
		
		ol_free(client);
	}
	op_uart_printf("mqtt demo end!!\r\n");
}






static void mqtt_sub_Callback(void* client, message_data_t* msg)
{
    (void) client;
	op_uart_printf("sub_topic_handle1\r\n");
	op_uart_printf("topic: %s\r\n",msg->topic_name);
	op_uart_printf("message:%s\r\n",(char*)msg->message->payload);
}

void mqtt_Callback(void* client, mqtt_cmd_id_t cmd_id, ol_mqtt_error_t error)
{
	op_uart_printf("[mqtt_Callback]");
    if(cmd_id != MQTT_CMDID_RESEALE)
    {
        op_uart_printf("[mqtt_Callback]cmd_id:%d, error: %d state %d\n",  cmd_id,error , ((mqtt_client_t *)client)->mqtt_client_state);
    }
	
	mqtt_message_t pub_data;
	
    if(error != 0)
    {
        return ;
    }

	if(cmd_id == MQTT_CMDID_CONNECT)
	{
        ol_mqtt_subscribe_asyn((mqtt_client_t *)client,"mobiletk",1,(void*)mqtt_sub_Callback);
	}
	else if(cmd_id == MQTT_CMDID_SUBSCRIBE)
	{
        memset(&pub_data, 0, sizeof(mqtt_message_t));
        pub_data.payloadlen = 5;
        pub_data.payload = (void *)"1234";
        ol_mqtt_publish_asyn((mqtt_client_t *)client,"mobiletk",&pub_data);
	}
	else if(cmd_id == MQTT_CMDID_PUBLISH)
	{
        ol_mqtt_unsubscribe_asyn((mqtt_client_t *)client,"mobiletk");
	}
	else if(cmd_id == MQTT_CMDID_UNSUBSCRIBE)
	{
        ol_mqtt_keep_alive_asyn((mqtt_client_t *)client);
	}
	else if(cmd_id == MQTT_CMDID_KEEP_ALIVE)
	{
        ol_mqtt_disconnect_asyn((mqtt_client_t *)client);
	}
	else if(cmd_id == MQTT_CMDID_DISCONNECT)
	{
	    ol_mqtt_release_asyn((mqtt_client_t *)client);
	}
	else if(cmd_id == MQTT_CMDID_RESEALE)
	{
        op_uart_printf("[mqtt_Callback] success \n");
	}
}

char host[] = "118.114.239.159";
char port[] = "30073";
char user_name[] = "mbtk_test1";
char password[] = "test";
char client_id[] = "test";

void mqtt_test_non_block(void)
{
	mqtt_client_t *client = NULL;
    int ret = 0;
	op_uart_printf("mqtt demo start!!\r\n");

	if(mqtt_demo_wait_network() != 0)
	{
		op_uart_printf("mqtt_demo socket_demo_wait_network fail\n");
		return;
	}
    client = ol_mqtt_lease();

    if(client){
        ol_mqtt_set_host(client, host);
        ol_mqtt_set_port(client, port);
        ol_mqtt_set_user_name(client, user_name);
        ol_mqtt_set_password(client, password);
        ol_mqtt_set_client_id(client, client_id);
        ol_mqtt_set_clean_session(client, 1);
        
        ret = ol_mqtt_connect_asyn(client,mqtt_Callback);
        if(ret != 0)
        {
		   ol_mqtt_disconnect(client);
           ol_mqtt_release(client);
           ol_free(client);
        }
    }

	
   
    
    
}


