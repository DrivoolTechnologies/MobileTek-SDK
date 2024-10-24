#include "ol_nw_api.h"
#include "mbtk_comm_api.h"
#include "stdio.h"


void nw_cell_api_info_dump(ol_CELL_INFO_EX *cell_info)
{
	char mode;
	unsigned char i;
	char cell_num;
	char net_mode[8] = {0};

	if(cell_info->vaild_type == GSM_CELL_VALID){
		mode = GSM_CELL_VALID;
		sprintf(net_mode,"%s","GSM");
	}else{
		mode = LTE_CELL_VALID;
		sprintf(net_mode,"%s","LTE");
		op_uart_printf("[%s]:cell t3402[%d],t3412[%d],qrxlevmin[%d]",net_mode,cell_info->t3402,
			cell_info->t3412,cell_info->qrxlevmin);
	}

	op_uart_printf("==List [%s] cell info start!!==\n",net_mode);
	if(mode == GSM_CELL_VALID){
		op_uart_printf("[%s]:cell id[%d],rssi[%d],ber[%d],mcc[%d],mnc[%d],band[%d]\r\n",net_mode,
			cell_info->present_cell_info.gsm.ci,cell_info->present_cell_info.gsm.rssi,
			cell_info->present_cell_info.gsm.ber,cell_info->present_cell_info.gsm.mcc,
			cell_info->present_cell_info.gsm.mnc,cell_info->present_cell_info.gsm.band);
		op_uart_printf("[%s]:lac[%d],arfcn[%d]\r\n",net_mode,cell_info->present_cell_info.gsm.lac,
			cell_info->present_cell_info.gsm.arfcn);
		for(i = 0; i < cell_info->nb_cell_num.of_neighboring; i++){
			op_uart_printf("[%s]:cell id[%d],rssi[%d]",net_mode,cell_info->nb_cell_info[i].gsm.ci,
				cell_info->nb_cell_info[i].gsm.rssi);
		}
	}else{
		op_uart_printf("[%s]:cell id[%d],rssi[%d],ber[%d],mcc[%d],mnc[%d],band[%d]\r\n",net_mode,
			cell_info->present_cell_info.lte.ci,cell_info->present_cell_info.lte.rssi,
			cell_info->present_cell_info.lte.ber,cell_info->present_cell_info.lte.mcc,
			cell_info->present_cell_info.lte.mnc,cell_info->present_cell_info.lte.band);
		op_uart_printf("[%s]:rsrp[%d],rsrq[%d],snr[%d],pci[%d],tac[%d],earfcn[%d]\r\n",net_mode,
			cell_info->present_cell_info.lte.rsrp,cell_info->present_cell_info.lte.rsrq,
			cell_info->present_cell_info.lte.snr,cell_info->present_cell_info.lte.pci,
			cell_info->present_cell_info.lte.tac,cell_info->present_cell_info.lte.earfcn);
		for(i = 0; i < cell_info->nb_cell_num.lte.of_intra + cell_info->nb_cell_num.lte.of_inter; i++){
			op_uart_printf("[%s]:cell id[%d],pic[%d],rsrp[%d],rssi[%d]",net_mode,cell_info->nb_cell_info[i].lte.ci,
			cell_info->nb_cell_info[i].lte.pci, cell_info->nb_cell_info[i].lte.rsrp,cell_info->nb_cell_info[i].lte.rssi);
		}
	}
	op_uart_printf("==List [%s] cell info end!!==\n",net_mode);
}

void nw_bandbitmap_dump(char *mode_name,int bitmap)
{
	char i = 0;
	char log_buff[128] = {0};
	char len = 0;

	len+=sprintf(log_buff+len,"[%s band:] ",mode_name);
	for(i = 0;i<32;i++)
	{
		if((bitmap >> i) & 0x1){
			len+=sprintf(log_buff+len,"%d ",i+1);
		}
	}	
	op_uart_printf("%s",log_buff);
}


void nw_demo(void)
{
    ol_NW_CONFIG_INFO config_info = {0};
    ol_NITZ_TIME_INFO nitz_info = {0};
    ol_OPERATOR_INFO operator_info = {0};
    ol_REG_STATUS_INFO reg_info = {0};
    ol_CELL_INFO cell_info = {0};
    ol_SIGNAL_STRENGTH_INFO signal_info;
    int csq = 0;
		ol_CELL_INFO_EX cell_info_ex = {0};
		char net_mode = 0;
		char loop = 0;

    op_uart_printf("==ol nw api test start!!==\n");
    
    //get config
    if(ol_nw_get_config(&config_info) != 0)
    {
        op_uart_printf("get nw_config fail!!\n");
        return ;
    }
    op_uart_printf("get preferred_nw_mode = %d\n", config_info.preferred_nw_mode);
    op_uart_printf("get roaming_pref = %d\n", config_info.roaming_pref);
		nw_bandbitmap_dump("GSM",config_info.gsmband_bitmap);
		nw_bandbitmap_dump("FD_LTE",config_info.ltebandh_bitmap);
		nw_bandbitmap_dump("TDD_LTE",config_info.ltebandl_bitmap);
/*
		if((config_info.ltebandl_bitmap & 0x100) != 4){//set lte band3
			config_info.ltebandl_bitmap = 4;//0x100 band3 in bitmap
			ol_nw_set_config(&config_info);
		}

*/

		if(config_info.preferred_nw_mode == OL_NW_GSM_MODE)
			net_mode = 0;
		else
			net_mode = 1;

    do
    {
    	//get csq
     	ol_os_task_sleep(2000);
     	if(ol_nw_get_csq(&csq) != 0){
      	op_uart_printf("get csq fail!!\n");
      	return ;
    	}
    	op_uart_printf("get csq = %d\n", csq);
        
     	//get reg status
    	if(ol_nw_get_reg_status(&reg_info, net_mode) != 0)//get lte reg status
    	{
     		op_uart_printf("get reg_status fail!!\n");
      	return ;
     	}
    	op_uart_printf("get reg info reg status = %d\n", reg_info.state);
			op_uart_printf("get reg info tac = %x,cid = %x",reg_info.lac,reg_info.cid);
    	op_uart_printf("get reg info act = %d\n", reg_info.act);
			op_uart_printf("get reg info t3324 = %d,t3412ext2 = %d",reg_info.t3324,reg_info.t3412ext2);
    }while(reg_info.state != OL_NW_REG_STA_REG_HPLMN );

    //get nitz time
    if (ol_nw_get_nitz_time(&nitz_info) != 0)
    {
    	op_uart_printf("get nitz fail!!\n");
     	return;
    }
    op_uart_printf("time = %s\n", nitz_info.nitz_time);
    op_uart_printf("abs time = %d\n", nitz_info.abs_time);

    op_uart_printf("ol_nw_get_signal_strength = %d\n", ol_nw_get_signal_strength(&signal_info));
    op_uart_printf("ol_nw_get_signal_strength = %d\n", signal_info.CW_SignalStrength.rssi, signal_info.LTE_SignalStrength.rsrq);

    //get operator info
    if (ol_nw_get_operator_info(&operator_info) != 0)
    {
    	op_uart_printf("get operator_info fail!!\n");
     	return;
    }
    op_uart_printf("operator short name = %s\n,long name %s", operator_info.short_eons,operator_info.long_eons);
    op_uart_printf("mcc = %s,mnc = %s\n", operator_info.mcc,operator_info.mnc);
		
#if 0
    //get cell info
    if (ol_nw_get_cell_info(&cell_info) != 0)
    {
        op_uart_printf("get cell_info fail!!\n");
        return;
    }
    op_uart_printf("lte_info_valid = %d\n", cell_info.lte_info_valid);
    op_uart_printf("lte_info_num = %d\n", cell_info.lte_info_num);
    op_uart_printf("lte_info_cid = %d\n", cell_info.lte_info[0].cid);
#else
		//get cell info use EX
		while(loop < 20)
		{
    	//get cell info
    	ol_os_task_sleep(5*200);
    	if (ol_nw_get_cell_info_ex(&cell_info_ex) != 0)
    	{
    		op_uart_printf("get cell_info fail!!\n");
    		return;
    	}
			nw_cell_api_info_dump(&cell_info_ex);
			loop++;
		}
#endif
    op_uart_printf("==ol nw api test end!!==\n");
    return ;
}

/*
Note1: GPRS Timer3 defined in 24.008 (example T3412)
Bits 5 to 1 represent the binary coded timer value.
Bits 6 to 8 defines the timer value unit for the GPRS timer as follows: Bits
8 7 6
0 0 0 value is incremented in multiples of 10 minutes
0 0 1 value is incremented in multiples of 1 hour
0 1 0 value is incremented in multiples of 10 hours
0 1 1 value is incremented in multiples of 2 seconds
1 0 0 value is incremented in multiples of 30 seconds
1 0 1 value is incremented in multiples of 1 minute
1 1 0 value is incremented in multiples of 320 hours
1 1 1 value indicates that the timer is deactivated
Note2: GPRS Timer defined in 24.008(example T3324)
Timer value
Bits 5 to 1 represent the binary coded timer value.
Bits 6 to 8 defines the timer value unit for the GPRS timer as follows: Bits
8 7 6
0 0 0  value is incremented in multiples of 2 seconds
0 0 1  value is incremented in multiples of 1 minute
0 1 0  value is incremented in multiples of decihours
1 1 1  value indicates that the timer is deactivated.
Other values shall be interpreted as multiples of 1 minute.
*/
void psm_demo(void)
{
    int ret = 0;
    ol_psm_info psm;

    memset(&psm,0x0,sizeof(psm));
    psm.mode = 1;
    /*T3412 10mins T3324 2mins*/
    sprintf(psm.T3412,"%s","00000001");
    sprintf(psm.T3324,"%s","00100010");

    ret = ol_set_psm_config(&psm);
    op_uart_printf("ol_set_psm_config ret = %d",ret);
}

void psm_off(void)
{
    int ret = 0;
    ol_psm_info psm;

    memset(&psm,0x0,sizeof(psm));
    psm.mode = 0;

    ret = ol_set_psm_config(&psm);
    op_uart_printf("psm_off ret = %d",ret);
}

void psm_get(void)
{
    int ret = 0;
    ol_psm_info psm;

    memset(&psm,0x0,sizeof(psm));

    ret = ol_get_psm_config(&psm);
    op_uart_printf("ol_set_psm_config ret = %d",ret);
    op_uart_printf("mode = %d",psm.mode);
    op_uart_printf("T3312 = %s",psm.T3312);//cat1 not support
    op_uart_printf("T3314 = %s",psm.T3314);//cat1 not support
    op_uart_printf("T3412 = %s",psm.T3412);
    op_uart_printf("T3324 = %s",psm.T3324);
}

