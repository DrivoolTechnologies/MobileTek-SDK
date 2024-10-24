#include "mbtk_comm_api.h"
#include "ol_lbs.h"

void lbs_demo(void)
{
    int ret;
    mbtk_lbs_info_t info = {0};
    ret = ol_lbs_get_info(NULL, 0, &info, 10);
    op_uart_printf("ret = %d", ret);
    op_uart_printf("lbs_info = %s, %s", info.longitude_str,info.latitude_str);
}

