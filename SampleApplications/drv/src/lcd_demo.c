
#include "mbtk_gpio.h"
#include "mbtk_api.h"
#include "mbtk_comm_api.h"

#define   BLACK     0x0000       //   o����?    
#define   NAVY      0x000F      //    ��?��?��?  
#define   DGREEN    0x03E0        //  ��??����?  
#define   DCYAN     0x03EF       //   ��??����?  
#define   MAROON    0x7800       //   ��?o����?      
#define   PURPLE    0x780F       //   ��?��?  
#define   OLIVE     0x7BE0       //   ��?��-?��      
#define   LGRAY     0xC618        //  ?�������?
#define   DGRAY     0x7BEF        //  ��??����?      
#define   BLUE      0x001F        //  ��?��?    
#define   GREEN     0x07E0        //  ?����?          
#define   CYAN      0x07FF        //  ?����?  
#define   RED       0xF800        //  o����?       
#define   MAGENTA   0xF81F        //  ?��o��    
#define   YELLOW    0xFFE0        //  ??��?        
#define   WHITE     0xFFFF        //  �����?  

static mbtk_lcd_color_struct test_color[240*240] = {};//[240*240];

void lcd_color_set(u16 color)
{
    int i = 0;
    for(i = 0; i < sizeof(test_color) / 2; i++)
    {
        test_color[i].full = color;
    }
}

int lcd_demo(void)
{
    u16 width, height;
    op_uart_printf("ol_lcd_power_switch : %d \n", ol_lcd_power_switch(mbtk_lcd_pmic_power_on));
    ol_lcd_get_dimension(&width, &height);

    op_uart_printf("ol_lcd_get_dimension : %d,%d \n", width, height);
    op_uart_printf("ol_lcd_wakeup\n");
    ol_lcd_wakeup();
    lcd_color_set(BLUE);
    ol_lcd_set_backlight_level(5);
    ol_lcd_flush(test_color);
    lcd_color_set(RED);
    ol_os_task_sleep(200);
    ol_lcd_flush(test_color);
    lcd_color_set(GREEN);
    ol_os_task_sleep(200);
    ol_lcd_flush(test_color);
    ol_os_task_sleep(200);
    ol_lcd_sleep();
    ol_lcd_power_switch(mbtk_lcd_pmic_power_off);

    return 0;
}

