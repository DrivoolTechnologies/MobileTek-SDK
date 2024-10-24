#include "mbtk_cam.h"
#include "mbtk_comm_api.h"

#include "mbtk_lcd.h"

#define CAMERA_PREVIEW_WIDTH     240
#define CAMERA_PREVIEW_HEIGHT    240
#define CAMERA_PREVIEW_TIME    15  //秒
#define CAMERA_PREVIEW_FRAME_RATE    10  //帧

mbtk_lcd_color_struct test_color[CAMERA_PREVIEW_WIDTH+1][CAMERA_PREVIEW_HEIGHT+1]={0};

void camera_demo(void)
{
    int ret;
    int i =0;
    int sleep_period = (1000/5)/10;  //sleep函数1表示5毫秒
    int max_i = CAMERA_PREVIEW_TIME*CAMERA_PREVIEW_FRAME_RATE;
    op_uart_printf("\r\ncamera_demo finish");
}

