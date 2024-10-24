
#include "mbtk_gpio.h"
#include "mbtk_api.h"
#include "mbtk_comm_api.h"

#define   BLACK     0x0000       //   oи▓иж?    
#define   NAVY      0x000F      //    иж?ид?иж?  
#define   DGREEN    0x03E0        //  иж??имиж?  
#define   DCYAN     0x03EF       //   иж??идиж?  
#define   MAROON    0x7800       //   иж?oимиж?      
#define   PURPLE    0x780F       //   б┴?иж?  
#define   OLIVE     0x7BE0       //   иж?иж-?им      
#define   LGRAY     0xC618        //  ?и░буб┴иж?
#define   DGRAY     0x7BEF        //  иж??и░иж?      
#define   BLUE      0x001F        //  ид?иж?    
#define   GREEN     0x07E0        //  ?имиж?          
#define   CYAN      0x07FF        //  ?идиж?  
#define   RED       0xF800        //  oимиж?       
#define   MAGENTA   0xF81F        //  ?бдoим    
#define   YELLOW    0xFFE0        //  ??иж?        
#define   WHITE     0xFFFF        //  буб┴иж?  

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

