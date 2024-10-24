/*************************************************************
Description:
    MBTK Open Voice Call Demo Program
*************************************************************/
#include "mbtk_comm_api.h"
#include "ol_nw_api.h"
#include "mbtk_voicecall_api.h"


int call_event_flag = 0;


/*************************************************************
    Function Definitions
*************************************************************/
void mbtk_voice_call_demo_callback(int event_id,void *msg)
{
    op_uart_printf("mbtk voice call callback event[%d] \n", event_id);

    switch(event_id)
    {
        case MBTK_VC_EVT_NO_CARRIER:
        case MBTK_VC_EVT_DIAL_END:
            call_event_flag = 0;
            break;
        case MBTK_VC_EVT_RING:
						if(msg)
						{
							mbtk_voicecall_info *caller_info = (mbtk_voicecall_info *)msg;
							op_uart_printf("caller %s %d",caller_info->caller_id,caller_info->addr_type);
						}
            call_event_flag = 1;
            break;
        case MBTK_VC_EVT_DIAL_CONNECT:
            call_event_flag = 2;
            break;
        case MBTK_VC_EVT_DIAL_DIALING:
            op_uart_printf("MBTK_VC_EVT_DIAL_DIALING");
            break;
        case MBTK_VC_EVT_DIAL_ALERTING:
            op_uart_printf("MBTK_VC_EVT_DIAL_ALERTING");
            break;
    }
}

void voice_call_demo(void)
{
    ol_REG_STATUS_INFO reg_info = {0};
    int csq = 0;
    int index = 0;

    op_uart_printf("voice_call_demo: Start\n");
    ol_vc_event_register(mbtk_voice_call_demo_callback);

    do
    {
        ol_os_task_sleep(2000);
        if(ol_nw_get_csq(&csq) != 0)
        {
            op_uart_printf("voice_call_demo: get csq fail!!\n");
            return ;
        }

        if(ol_nw_get_reg_status(&reg_info,1) != 0)
        {
            op_uart_printf("voice_call_demo: get reg_status fail!!\n");
            return ;
        }

        op_uart_printf("voice_call_demo: get csq = %d\n",csq);
        op_uart_printf("voice_call_demo: get reg info reg status = %d\n",reg_info.state);
    }while(reg_info.state != OL_NW_REG_STA_REG_HPLMN);

    op_uart_printf("voice_call_demo: check net reg success\n");

    ol_vc_dial("10086");

    while(1)
    {
        ol_os_task_sleep(1000);

        if(call_event_flag == 1 && index == 5)
            ol_vc_answer();

        if(call_event_flag == 2 && index == 5)
            ol_vc_hangup();

        if(index++ == 5)
            index = 0;
    }
    op_uart_printf("voice_call_demo: End\n");
}


