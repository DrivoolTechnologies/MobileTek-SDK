#include "mbtk_comm_api.h"
#include "mbtk_device_api.h"
#include "mbtk_api.h"

int get_device_imei_demo(void)
{
    int iret = 0;
    char imeibuf[20] = {0};

    if (iret = ol_get_device_imei(imeibuf) != mbtk_device_api_err_none)
    {
        return -1;
    }
    else 
    {
        if (strlen(imeibuf) != 0)
        {
            op_uart_printf("get_device_imei_demo imei[%s]\n", imeibuf);
        }
        return 0;
    }
}

int get_firmware_version_demo(void)
{
    int iret = 0;

    mbtk_device_firmware_ver_struct *version;

    if (iret = ol_get_firmware_version(&version) != mbtk_device_api_err_none)
    {
        iret = -1;
    }
    else 
    {
        op_uart_printf("get_firmware_version_demo company[%s]\n", version->company);
        op_uart_printf("get_firmware_version_demo projectname[%s]\n", version->projectname);
        op_uart_printf("get_firmware_version_demo realsedate[%s]\n", version->realsedate);
        op_uart_printf("get_firmware_version_demo softversion[%s]\n", version->softversion);
        iret = 0;
    }
    return iret;
}

int get_device_current_time_demo(void)
{
    int iret = 0;
    mbtk_device_time_struct cur_time = {0};
    
    if (iret = ol_get_device_current_time(&cur_time) != mbtk_device_api_err_none)
    {
        return -1;
    }
    else 
    {
        op_uart_printf("get_device_current_time_demo time[%d:%d:%d:%d:%d:%d]\n", cur_time.year, cur_time.month, cur_time.day, cur_time.hour, cur_time.min, cur_time.sec);
        return 0;
    }
}

int get_device_sn_demo(void)
{
    int iret = 0;
    char snbuf[20] = {0};

    if (iret = ol_get_device_sn(snbuf) != mbtk_device_api_err_none)
    {
        return -1;
    }
    else
    {
        if (strlen(snbuf) != 0)
        {
            op_uart_printf("get_device_sn_demo sn[%s]\n", snbuf);
        }
        return 0;
    }
}

int set_modem_function_demo(void)
{
    int fun = 0;
    int rst = 0;  // close fun and no reset
    int iret = 0;
    
    iret = ol_set_modem_function(fun, rst);
    if (iret != mbtk_device_api_err_none)
    {
        return -1;
    }
    else
    {
        op_uart_printf("demo_mbtk_set_modem_function sucess, fun[%d], rst[%d]\n", fun, rst);
        return 0;
    }
}

int get_modem_function_demo(void)
{
    int fun = 0;
    int iret = 0;

    if (iret = ol_get_modem_function(&fun) != mbtk_device_api_err_none)
    {
        return -1;
    }
    else
    {
        op_uart_printf("demo_mbtk_get_modem_function sucess, fun[%d]\n", fun);
        return 0;
    }
}

int time_demo(void)
{
    struct tm* lt;
    struct tm lt2 = {0};
    uint32 cnt=0;
    unsigned int t;
    lt = &lt2;

    uint8_t timezone = ol_get_timezone();

    op_uart_printf("ol_gmtime2 %d\n",timezone);
    t = ol_time(&t);
    lt = ol_gmtime(&t);

    op_uart_printf("ol_gmtime1 %d,%d,%d,%d,%d,%d\n",(lt->tm_year), lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

    int temp_time = 1650450111;

    ol_set_time(&temp_time, 1, 8);

    t = ol_time(&t);
    lt = ol_gmtime(&t);

    op_uart_printf("ol_gmtime2 %d,%d,%d,%d,%d,%d\n",(lt->tm_year), lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

    lt->tm_year = 2022;
    lt->tm_mon = 5;
    lt->tm_mday = 7;
    lt->tm_hour = 10;
    lt->tm_min = 10;
    lt->tm_sec = 5;

    ol_set_time(lt,0, 8);

    memset(lt,0, sizeof(struct tm));
    t = ol_time(&t);
    lt = ol_gmtime(&t);
    op_uart_printf("ol_gmtime3 %d,%d,%d,%d,%d,%d\n",(lt->tm_year), lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    ol_set_nitz_enable(0);
    return 0;
}


int mbtk_factory_demo(void)
{
    char data_buffer[16]={0};
    int ret = 0;

    //OL_FACTORY_R用来读取，OL_FACTORY_W设置， OL_FACTORY_D为删除
    ret = ol_factory_operation(OL_FACTORY_R,data_buffer, 16, 0);
    if (ret)
    {
        op_uart_printf("ol_factory_operation ret[%d]\n", ret);
    }
    else
    {
        op_uart_printf("ol_factory_operation data_buffer[%s]\n", data_buffer);
    }
    ol_factory_operation(OL_FACTORY_W,"this is test123",strlen("this is test123"), 0 );
    
    ret = ol_factory_operation(OL_FACTORY_R,data_buffer, 16, 0);
    if (ret)
    {
        op_uart_printf("ol_factory_operation ret[%d]\n", ret);
    }
    else
    {
        op_uart_printf("ol_factory_operation data_buffer[%s]\n", data_buffer);
    }
    return 0;
}

typedef int (*device_demo_func)(void);
device_demo_func demo_func_buf[] = 
{
    get_device_imei_demo, 
    get_firmware_version_demo, 
    get_device_current_time_demo, 
    get_device_sn_demo, 
    set_modem_function_demo, 
    get_modem_function_demo
};

void device_demo(void)
{
    int demo_index = 0;
    op_uart_printf("device_demo enter\n");
    for(; demo_index < (sizeof(demo_func_buf) / sizeof(device_demo_func)); demo_index++)
    {
        op_uart_printf("device_demo run demofunc_buf[%d] \n", demo_index);
        demo_func_buf[demo_index]();
    }
    op_uart_printf("device_demo end\n");
}

