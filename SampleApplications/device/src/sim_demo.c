

#include "mbtk_comm_api.h"
#include "mbtk_sim_api.h"
#include "mbtk_api.h"


int get_sim_status_demo(void)
{
    uint8_t status = 0;
    int iret = 0;

    if(iret != ol_get_sim_status(&status) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf(" get_sim_status_demo status[%d]\n", status);
        return 0;
    }
}

int get_sim_imsi_demo(void)
{
    int iret = 0;
    char imsi[100] = {0};

    if(iret = ol_get_sim_imsi(imsi) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_sim_imsi_demo imsi[%s]\n", imsi);
        return 0;
    }
}

int get_sim_iccid_demo(void)
{
    int iret = 0;
    char iccid[100] = {0};

    if(iret = ol_get_sim_iccid(iccid) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_iccid_demo iccid[%s]\n", iccid);
        return 0;
    }
}

int get_sim_phone_number_demo(void)
{
    int iret = 0;
    char phbuf[100] = {0};

    if(iret = ol_get_sim_phonenumber(phbuf) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_sim_phone_number_demo phonenumber[%s]\n", phbuf);
        return 0;
    }
}

int read_phone_book_record_demo(void)
{
    int iret = 0;
    mbtk_phone_book_info_struct *phbook = NULL;
    char *temp = NULL;
    char username[10] = "Name";
    op_uart_printf("read_phone_book_record_demo\r\n");
    
    phbook = ol_malloc(1024);
    mbtk_phone_book_storge_enum storge = mbtk_ph_book_storge_sm;    

    if(phbook == NULL)
    {
        return -1;
    }

    temp = phbook;
    memset(phbook, 0, 1024);

    op_uart_printf("read_phone_book_record_demo\r\n");
    //use index
    if(iret = ol_read_phonebook_record(storge, username, 1, 500, phbook) != mbtk_sim_api_err_none)
    {
        iret = -1;
    }
    else 
    {
        while(phbook->index != 0)
        {
            op_uart_printf("read_phone_book_record_demo inde[%d], username[%s], phnumber[%s]\n", phbook->index, phbook->username, phbook->phonenum);
            phbook++;
        }
        iret = 0;
    }
    
    phbook = temp;
    memset(phbook, 0, 1024);
    //use name
    
    if(iret = ol_read_phonebook_record(storge, username, 0, 500, phbook) != mbtk_sim_api_err_none)
    {
        iret = -1;
    }
    else 
    {
        while(phbook->index != 0)
        {
            op_uart_printf("read_phone_book_record_demo inde[%d], username[%s], phnumber[%s]\n", phbook->index, phbook->username, phbook->phonenum);
            phbook++;
        }
        iret = 0;
    }

    phbook = temp;
    ol_free(phbook);
    phbook = NULL;
    temp = NULL;
    return iret;
}

int write_phone_book_record_demo(void)
{
    int iret = 0;
    mbtk_phone_book_info_struct phbook;
    mbtk_phone_book_storge_enum storge = mbtk_ph_book_storge_sm;
    phbook.index = 14;
    memcpy(phbook.username, "demo_test", strlen("demo_test"));
    memcpy(phbook.phonenum, "+18400001111", strlen("+18400001111"));

    // write ph_book_info
    if(iret = ol_write_phonebook_record(storge, phbook.index, &phbook) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("write_phone_book_record write success, real index = %d\n", phbook.index);
    }

    // cancel ph_book_info
    memset(phbook.phonenum, 0, MBTK_SIM_PH_BOOK_PH_NUM_LEN);
    memset(phbook.username, 0, MBTK_SIM_PH_BOOK_USER_LEN);

    if(iret = ol_write_phonebook_record(storge, phbook.index, &phbook) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf(" write_phone_book_record clear success, clear index = %d\n", phbook.index);
    }
    
    return 0;
}

int get_sim2_status_demo(void)
{
    uint8_t status = 0;
    int iret = 0;

    if(iret != ol_get_sim2_status(&status) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf(" get_sim2_status_demo status[%d]\n", status);
        return 0;
    }
}

int get_sim2_imsi_demo(void)
{
    int iret = 0;
    char imsi[100] = {0};

    if(iret = ol_get_sim2_imsi(imsi) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_sim2_imsi_demo imsi[%s]\n", imsi);
        return 0;
    }
}

int get_sim2_iccid_demo(void)
{
    int iret = 0;
    char iccid[100] = {0};

    if(iret = ol_get_sim2_iccid(iccid) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_sim2_iccid_demo iccid[%s]\n", iccid);
        return 0;
    }
}

int get_sim2_phone_number_demo(void)
{
    int iret = 0;
    char phbuf[100] = {0};

    if(iret = ol_get_sim2_phonenumber(phbuf) != mbtk_sim_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_sim2_phone_number_demo phonenumber[%s]\n", phbuf);
        return 0;
    }
}

int sitch_sim_demo(uint8 simId)
{
    int iret = 0;
    char phbuf[100] = {0};

    if(iret = ol_switch_sim(simId) != mbtk_sim_api_err_none)
    {
        return iret;
    }
    else 
    {
        return iret;
    }
}

int get_current_master_sim(void)
{
    int currentSimId = ol_get_current_master_sim();
    op_uart_printf("get_current_master_sim [%d]\n", currentSimId);
    return currentSimId;
}

int switch_sim_witch_check(void)
{
    int currentSimId = get_current_master_sim();
    int simId = (currentSimId == 0? 1 : 0);
    if(simId != currentSimId)
    {
        int counts = 0;
        while(1)
        {
            int iret = 0;
            iret = sitch_sim_demo(simId);
            counts++;
            ol_os_task_sleep(1*200);
            op_uart_printf("sim[%d]->sim[%d], counts:[%d]", currentSimId, simId, counts);
            if(iret != mbtk_sim_api_err_none || counts > 30)
            {
                op_uart_printf("return because switch_sim_witch_check count(%d) or ret(%d) error", counts, iret);
                return iret;
            }
            if(simId == get_current_master_sim())
            {
                op_uart_printf("return because switch_sim_witch_check succ[%d]", get_current_master_sim());
                return mbtk_sim_api_err_none;
            }
        }
    }

    return 0;
}

#if 0
static void sim_verify_demo(void)
{
    mbtk_sim_pin_struct pin_buf;
    char * pin_code = "1234";
    memcpy(pin_buf.mbtk_sim_pin_buf,pin_code,strlen(pin_code)+1);
    ol_sim_verify_pin(&pin_buf);
}



static void sim_change_demo(void)
{
    mbtk_change_sim_pin_struct pin_buf;
    char * old_pin_code = "1234";
    char * new_pin_code = "4567";
    
    memcpy(pin_buf.mbtk_old_sim_pin_buf,old_pin_code,strlen(old_pin_code)+1);
    memcpy(pin_buf.mbtk_new_sim_pin_buf,new_pin_code,strlen(new_pin_code)+1);
    ol_sim_change_pin(&pin_buf);
}

static void sim_unblock_demo(void)
{
    mbtk_unblock_sim_pin_struct pin_buf;
    char * pin_code = "1234";
    char * puk_code = "3231212"; /*puk code */
    
    memcpy(pin_buf.mbtk_sim_pin_buf,pin_code,strlen(pin_code)+1);
    memcpy(pin_buf.mbtk_sim_puk_buf,puk_code,strlen(puk_code)+1);

    ol_sim_unblock_pin(&pin_buf);
}

static void sim_disable_demo(void)
{
    mbtk_sim_pin_struct pin_buf;
    char * pin_code = "1234";
    memcpy(pin_buf.mbtk_sim_pin_buf,pin_code,strlen(pin_code)+1);
    ol_sim_diable_pin(&pin_buf);
}

static void sim_enable_demo(void)
{
    mbtk_sim_pin_struct pin_buf;
    char * pin_code = "1234";
    memcpy(pin_buf.mbtk_sim_pin_buf,pin_code,strlen(pin_code)+1);
    ol_sim_enable_pin(&pin_buf);
}
#endif

typedef int (*sim_demo_func)(void);
sim_demo_func sim_demo_func_buf[] = 
{
    get_sim2_status_demo, 
    get_sim2_imsi_demo,
    get_sim2_iccid_demo,
    get_sim2_phone_number_demo,
    get_sim_status_demo, 
    get_sim_imsi_demo,
    get_sim_iccid_demo,
    get_sim_phone_number_demo,
    read_phone_book_record_demo,
    write_phone_book_record_demo,
    get_current_master_sim,
};

void sim_demo(void)
{
    int demo_index = 0;
    op_uart_printf("sim_demo enter\n");
    for(; demo_index < (sizeof(sim_demo_func_buf) / sizeof(sim_demo_func)); demo_index++)
    {
        op_uart_printf("sim_demo run sim_demo_func_buf[%d] \n", demo_index);
        sim_demo_func_buf[demo_index]();
    }
    op_uart_printf("sim_demo end\n");
}

