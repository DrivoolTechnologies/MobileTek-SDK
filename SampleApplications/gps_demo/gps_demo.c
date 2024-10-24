#include "ol_flash_fs.h"
#include "mbtk_comm_api.h"
#include "mbtk_gps.h"
#include "stdio.h"
void gps_demo(void)
{
    int ret;
    mbtk_gps_info *info;
	char aaa[100];
	int abdd= 2;
	int status = 0;

    ol_gps_power(MBTK_GNSS_POWERON);

		while((status = ol_gps_get_status()) != ASR_GPS_STATE_ACTIVE)
		{
			op_uart_printf("gps status %d",status);
			ol_os_task_sleep(2*200);
		}

/*
		ret = ol_gps_agps_open();
		if(ret != 0)
		{
			op_uart_printf("download agps data fail,ret = %d\n",ret);
			return;
		}
*/		
    info = ol_get_gps_info();
    if (!info)
    {
        return;
    }

    while (1)
    {
        ol_os_task_sleep(1*200);

        op_uart_printf("\r\n %c %d, %c %d, \n", info->nmea.nshemi,info->nmea.latitude,
                                                info->nmea.ewhemi,info->nmea.longitude);

		sprintf(aaa, "%04d", abdd);

		op_uart_printf("%s", aaa);

        op_uart_printf("\r\n %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", info->nmea.possl[0],
                                                             info->nmea.possl[1],
                                                             info->nmea.possl[2],
                                                             info->nmea.possl[3],
                                                             info->nmea.possl[4],
                                                             info->nmea.possl[5],
                                                             info->nmea.possl[6],
                                                             info->nmea.possl[7],
                                                             info->nmea.possl[8],
                                                             info->nmea.possl[9],
                                                             info->nmea.possl[10],
                                                             info->nmea.possl[11]);
        op_uart_printf("\r\n %d\n", info->nmea.beidou_slmsg[0].beidou_num);

				op_uart_printf("\r\n %d,%d,%d",info->nmea.valid_states,info->nmea.cog,info->nmea.speed);
    }
    return;
}

