#include "ol_flash_fs.h"
#include "mbtk_comm_api.h"
#include "string.h"


#define FILE_NAME							"testFile.txt"
#define FILE_RENAME						"newtestFile.txt"
#define FILE_PRNAME						"printfFile.txt"
#define FILE_CONTENT 					"abcdefghi242102asd"
#define DIR_NAME							"testDir"
#define SUB_DIR_NAME					"SubDir"

#define DISK									"C:"
#define ROOT_PATH							""DISK"/"
#define FILE_UNDER_ROOT 			""DISK"/"FILE_NAME""
#define RNFILE_UNDER_ROOT			""DISK"/"FILE_RENAME""
#define PRFILE_UNDER_ROOT			""DISK"/"FILE_PRNAME""
#define DIR_PATH 							""DISK"/"DIR_NAME""
#define SUB_DIR_PATH					""DISK"/"DIR_NAME"/"SUB_DIR_NAME""
#define FILE_UNDER_DIR				""DIR_PATH"/"FILE_NAME""


void filefind_printf(ol_PFS_FIND_DATA find_data)
{
    int year = find_data->date & 0xFFF;
    int month = find_data->date >> 12 & 0xF;
    int day = find_data->date >> 16;
    int hour = find_data->time >> 12;
    int minutes = find_data->time >> 6 & 0x3F;
    int seconds = find_data->time & 0x3F;

    op_uart_printf("find %s\n",find_data->file_name);
		op_uart_printf("permissions = %d",find_data->permissions);
    op_uart_printf("find file size = %d\n",find_data->size);
    op_uart_printf("modify data = %d:%d:%d\n",year,month,day);
    op_uart_printf("modify time = %d:%d:%d\n",hour,minutes,seconds);
}

void flash_dir_demo(void)
{
  int ret = 0;

  /*create dir*/
  ret = ol_ffs_createdir(DIR_PATH);
  op_uart_printf("ol_ffs_createdir(%s) ret:%d\r", DIR_PATH, ret);
  ret = ol_ffs_createdir(SUB_DIR_PATH);
  op_uart_printf("ol_ffs_createdir(%s) ret:%d\r", SUB_DIR_PATH, ret);

  /*open dir*/
  unsigned int dirHandle = ol_ffs_opendir(DIR_PATH);
  op_uart_printf("ol_ffs_opendir ret:%u\r", dirHandle);

  /*read dir*/
  ol_FS_FIND_DATA fs_info = {0};
	while(1){
		memset(&fs_info, 0, sizeof(fs_info));
		ret = ol_ffs_readdir(dirHandle, &fs_info);
		if(ret == 0)
  		filefind_printf(&fs_info);
		else
			break;
	}
  /*close dir*/
  ret = ol_ffs_closedir(dirHandle);
  op_uart_printf("ol_ffs_closedir ret:%d\n", ret);

  /*delete dir*/
  ret = ol_ffs_deletedir(SUB_DIR_PATH);
  op_uart_printf("ol_ffs_deletedir(%s) ret:%d\n", SUB_DIR_PATH, ret);
  ret = ol_ffs_deletedir(DIR_PATH);
  op_uart_printf("ol_ffs_deletedir(%s) ret:%d\n", DIR_PATH, ret);
}

void flash_fs_demo(void)
{
  int ret;
  int handle;
  char buff[256] = {0};
  char printf_buffer[20] = {0};
  int printf_value = 0;
  ol_FS_FIND_DATA find_data = {0};

  //get free/used space
  op_uart_printf("free space  = %lld\n", ol_ffs_getfreespace(ROOT_PATH));
  op_uart_printf("used space  = %lld\n",ol_ffs_getusedspace(ROOT_PATH));
  op_uart_printf("total space = %lld\n",ol_ffs_gettotalspace(ROOT_PATH));
	
  //create a file
  handle = ol_ffs_open(FILE_UNDER_ROOT, "wb");
  if(handle < 0 ){
    op_uart_printf("open file err,errno = %d\n", handle);
    return ;
  }	
  op_uart_printf("open %s mode:wb handle:%x\n", FILE_UNDER_ROOT,handle);
	
  //write data
  ret = ol_ffs_write(handle,FILE_CONTENT,strlen(FILE_CONTENT));
  op_uart_printf("write file %d\n",ret);
  op_uart_printf("after write,free space  = %lld\n", ol_ffs_getfreespace(ROOT_PATH));
  op_uart_printf("after write,used space  = %lld\n",ol_ffs_getusedspace(ROOT_PATH));
  //seek
  ol_ffs_seek(handle, 0,OL_FS_SEEK_SET);
  ol_ffs_write(handle,"456",strlen("456"));
  ol_ffs_close(handle);

  //get size
  ret = ol_ffs_getsize(FILE_UNDER_ROOT);
  op_uart_printf("%s file size is :%d\n", FILE_UNDER_ROOT,ret);
	
  //rename
  ret = ol_ffs_rename(FILE_UNDER_ROOT, RNFILE_UNDER_ROOT);
  if(ret != 0){
    op_uart_printf("fs_test.txt rename fail,errno = %d\n", ret);
    return ;
  }
  op_uart_printf("fs_test.txt rename success!\n");	
  //read data
  memset(buff,0x0,256);
  handle = ol_ffs_open(RNFILE_UNDER_ROOT, "rb");
  ret = ol_ffs_read(handle, buff,256);
  if(ret < 0){
    op_uart_printf("fs_name.txt read fail,errno = %d\n",ret);
    ol_ffs_close(handle);
    return ;
  }
  op_uart_printf("%s read success,buff = %s,size = %d\n",RNFILE_UNDER_ROOT,buff,ret);
  ol_ffs_close(handle);
	//printf test
  handle = ol_ffs_open(PRFILE_UNDER_ROOT, "wb+");
  ol_ffs_fprintf(handle,"TEST:%s = %d","value",50);
  ol_ffs_seek(handle, 0,OL_FS_SEEK_SET);
  ol_ffs_fscanf(handle,"TEST:%s = %d",printf_buffer,&printf_value);
  op_uart_printf("fscanf get :%s = %d",printf_buffer,printf_value);
  ol_ffs_close(handle);
  ol_ffs_delete(PRFILE_UNDER_ROOT);

  //file find
  ret = ol_ffs_findfirst(""ROOT_PATH"*.txt",&find_data);
  if(ret != 0){
    op_uart_printf("can't find *.txt file\n");
    return ;
  }
  filefind_printf(&find_data);
  //find next
  while(1){
    memset(&find_data,0x0,sizeof(ol_FS_FIND_DATA));
    ret = ol_ffs_findnext(&find_data);
    if(ret == 0){
    	filefind_printf(&find_data);
    }
    else
     	break;
  }
	ol_ffs_findclose();
  op_uart_printf("find file end!\n");
  ol_ffs_delete(RNFILE_UNDER_ROOT);

  return;
}

