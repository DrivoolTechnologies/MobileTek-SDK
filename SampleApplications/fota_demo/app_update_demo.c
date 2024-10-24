#include "mbtk_comm_api.h"
#include "ol_app_update.h"
#include "ol_flash_fs.h"
#include "ol_app_update.h"

void app_update_demo(void)
{
    int ret;
    int handle;
    unsigned int file_size = 0;
    unsigned int remain_size = 0;
    unsigned char buffer[4*1024] = {0};
    AppCtxPtr context = NULL;

    file_size = ol_ffs_getsize("mbtk_app.bin");
    op_uart_printf("get mbtk_fota.bin size = %d", file_size);

    handle = ol_ffs_open("mbtk_app.bin", "rb");
    if (handle < 0 )
    {
        op_uart_printf("open file err,errno = %d\n", handle);
        return ;
    }
    remain_size = file_size;
    ol_ffs_seek(handle, 0, OL_FS_SEEK_SET);

    context = (AppCtxPtr)ol_app_update_create_context();
    if(context == NULL)
    {
        ol_ffs_close(handle);
        op_uart_printf("create update context fail\n");
    }

    while (remain_size)
    {
        unsigned int read_len = 0;
        if(remain_size >= 4*1024)
        {
            read_len =  4*1024;
        }
        else
        {
            read_len = remain_size;
        }
        memset(buffer,0x0,4*1024);
        ret = ol_ffs_read(handle, buffer, 4*1024);
        if (ret < 0)
        {
            op_uart_printf("mbtk_app.bin read fail,errno = %d\n",ret);
            ol_ffs_close(handle);
            return ;
        }
        op_uart_printf("ol_ffs_read len = %d,[%x,%x,%x,%x,%x]\n",ret,
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
        ret = ol_app_update_load_image(context, buffer, read_len);
        if(ret < 0)
        {
            op_uart_printf("mbtk_app.bin write fail\n");
            ol_app_update_destory_context(context);
            ol_ffs_close(handle);
            return ;
        }

        memset(buffer, 0x0, 4*1024);
        remain_size = remain_size - read_len;
    }

    ol_ffs_close(handle);
    if(ol_app_update_image_verify(context, file_size) < 0)
    {
        op_uart_printf("mbtk_app.bin verify fail\n");
        return;
    }

    ol_ffs_delete("mbtk_app.bin");
    ol_app_update_set_flag(context);
    ol_app_update_destory_context(context);
    context = NULL;

    return;
}
