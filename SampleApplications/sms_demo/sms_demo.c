#include "mbtk_os.h"
#include "string.h"
#include "ol_sms_api.h"
#include "ol_nw_api.h"
#include "mbtk_sim_api.h"
#include "mbtk_comm_api.h"
#include "stdlib.h"

//#define PDU_MODE

#define TASK_STACK_SIZE 1024*10
#define TASK_PRIORITY 100

#define DEMO_TASK_NAME "sms_demo_task"

#define DEMO_LIST_NUM    5
#define DEMO_TEST_SCA    "8613800280500"
#define DEMO_TEST_DA     "10086"
#define DEMO_TEST_SEND_DATA    "hello world"
//8613800280500 send hello word
#define PDU_MSG "0011000D91683108200805F00008011600680065006C006C006F00200077006F0072006C0064"
#define PDU_LEN ((strlen(PDU_MSG)/2)-1)

static int sms_demo_net_check(void)
{
    int ret = 0;
    unsigned char count = 0;
    unsigned char sim_status = 0;

    op_uart_printf("check sim status first\r\n");
    do{
        ol_os_task_sleep(5*200);
        ret = ol_get_sim_status(&sim_status);
        op_uart_printf("check sim status ret %d,status %d\r\n",ret,sim_status);
        count++;
        if (count >= 3)
        {
            op_uart_printf("check sim status timeout\r\n");
            return -1;
        }
    }while(sim_status != mbtk_sim_ready);
    op_uart_printf("check sms readly status\r\n");
    do{
        ret = ol_sms_readly_status();
        op_uart_printf("check sms readly status %d\r\n",ret);
        ol_os_task_sleep(5*200);
        count++;
        if (count >= 5)
        {
            op_uart_printf("check sms readly status timeout\r\n");
            return -1;
        }
    }while(!ret);

    return 0;
}

static int sms_msg_info_dump(mbtk_sms_info *msg_info)
{ 
    op_uart_printf("[MSG]:stat: %d\r\n",msg_info->stat);
    op_uart_printf("[MSG]:da: %s\r\n",msg_info->da);
    op_uart_printf("[MSG]:date: [%02d:%02d:%02d]\r\n",msg_info->timestamp.tsYear,
        msg_info->timestamp.tsMonth,msg_info->timestamp.tsDay);
    op_uart_printf("[MSG]:time: [%02d:%02d:%02d]\r\n",msg_info->timestamp.tsHour,
        msg_info->timestamp.tsMinute,msg_info->timestamp.tsSecond);
    op_uart_printf("[MSG]:timezone: %d sign %d\r\n",msg_info->timestamp.tsTimezone,msg_info->timestamp.tsZoneSign);
    op_uart_printf("[MSG]:coding type: %d\r\n",msg_info->msg.decorde_type);
    op_uart_printf("[MSG]:len: %d\r\n",msg_info->msg.msg_len);
    op_uart_printf("[MSG]:msg: %s\r\n",msg_info->msg.msg_data);

    return 0;
}

void sms_report_callback (mbtk_sms_info *msg_info)
{
    op_uart_printf("sms_report_cb");
    sms_msg_info_dump(msg_info);
}

void sms_demo_task(void *data)
{
    int ret = 0;
    unsigned int ret_index = 0;
    mbtk_sms_mem_list mem_list = {0};
    mbtk_sms_config config = {0};
    mbtk_sms_info msg_info = {0};

    op_uart_printf("===sms demo task start!!===\r\n");

    ret = sms_demo_net_check();
    if (ret != 0)
    {
        op_uart_printf("===sms network check error!!===\r\n");
        return;
    }

    op_uart_printf("report regist\r\n");
    ret = ol_sms_report_register(sms_report_callback);
    op_uart_printf("regist ret = %d\r\n",ret);

    op_uart_printf("get sms sca\r\n");
    memset(&config,0x0,sizeof(config));
    ret = ol_sms_get_sca(&config);
    op_uart_printf("get sca = %s,sca_type = %d",config.sca,config.tosca);

    op_uart_printf("set sms config\r\n");
    //memset(&config,0x0,sizeof(config));
#ifdef PDU_MODE
    config.msg_type = 0;//set msg pdu type
#else
    config.msg_type = 1;//set msg txt type
#endif
    //memcpy(config.sca, DEMO_TEST_SCA, strlen(DEMO_TEST_SCA));
    //config.tosca = 145;
    ret = ol_sms_config(0, &config);
    if (ret != 0)
    {
        goto exit;
    }

    ret = ol_sms_mem(0, &mem_list);
    if (ret != 0)
    {
        goto exit;
    }
    op_uart_printf("[read mem]type %d,used %d,total %d\r\n", mem_list.read.perform_id, mem_list.read.used, mem_list.read.total);
    op_uart_printf("[write mem]type %d,used %d,total %d\r\n", mem_list.write.perform_id, mem_list.write.used, mem_list.write.total);
    op_uart_printf("[recv mem]type %d,used %d,total %d\r\n", mem_list.recv.perform_id, mem_list.recv.used, mem_list.recv.total);

    if (mem_list.read.used >= DEMO_LIST_NUM)
    {
        mbtk_sms_info msg_list[DEMO_LIST_NUM] = {0};

        op_uart_printf("list %d msg\r\n",DEMO_LIST_NUM);
        ret = ol_sms_list(0,SMS_STAT_ALL,(void *)msg_list,0,DEMO_LIST_NUM);
        if(ret != 0)
        {
            goto exit;
        }
        else
        {
            unsigned int i = 0;
            for(i = 0;i < DEMO_LIST_NUM;i++)
            {
                sms_msg_info_dump(&msg_list[i]);
            }
        }
    }

    if(mem_list.write.used >= mem_list.write.total){
        op_uart_printf("write mem full,delete a msg for write test\r\n");
        ret = ol_sms_delete(0, 0, 1);//delete index 1 msg
        if(ret != 0)
        {
            goto exit;
        }
    }

    op_uart_printf("write a msg\r\n");
#ifdef PDU_MODE
    char pdu_da[5] = {0};
    memset(pdu_da,0x0,5);
    if(!__itoa(PDU_LEN,pdu_da,10))
    {
        goto exit;
    }
    ret = ol_sms_write(0, pdu_da, PDU_MSG, &ret_index);
#else
    ret = ol_sms_write(0, DEMO_TEST_DA, DEMO_TEST_SEND_DATA, &ret_index);
#endif
    if (ret != 0)
    {
        goto exit;
    }

    op_uart_printf("write msg success,return index %d",ret_index);
    memset(&msg_info, 0x0, sizeof(msg_info));
    ret = ol_sms_read(0, ret_index, &msg_info);
    if (ret != 0)
    {
        goto exit;
    }
    sms_msg_info_dump(&msg_info);

    ol_os_task_sleep(100);
    op_uart_printf("send the msg\r\n");
    ret = ol_sms_send(0, ret_index);
    if (ret != 0)
    {
        goto exit;
    }
    op_uart_printf("===sms demo task end!!===\r\n");
    return;
exit:
    op_uart_printf("===sms demo error %d!!===\r\n",ret);
    return;
}

void sms_demo(void * argv)
{
    mbtk_taskref demo_task_ref = NULL;
    void *mbtk_sms_task_stack = NULL;

    mbtk_sms_task_stack = ol_malloc(TASK_STACK_SIZE);
    ol_os_task_creat(&demo_task_ref, mbtk_sms_task_stack, TASK_STACK_SIZE, TASK_PRIORITY, DEMO_TASK_NAME, sms_demo_task, NULL);
}
