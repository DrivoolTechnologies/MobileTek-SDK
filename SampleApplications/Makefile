################### makefile ########################
############## common for make app ##################
#####################################################

include ${SOFT_WORKDIR}/build/buildenv.mk


LOCAL_NAME := demo

LOCAL_API_DEPENDS :=  ${API_PLATFORM_DEPENDS} 
			
LOCAL_MODULE_DEPENDS := demo/test
# LOCAL_MODULE_DEPENDS += demo/menu_demo
# LOCAL_MODULE_DEPENDS += demo/fs_demo
# LOCAL_MODULE_DEPENDS += demo/nw_demo
# LOCAL_MODULE_DEPENDS += demo/drv
# LOCAL_MODULE_DEPENDS += demo/device
# LOCAL_MODULE_DEPENDS += demo/audio_demo
# LOCAL_MODULE_DEPENDS += demo/visual_at_demo
# LOCAL_MODULE_DEPENDS += demo/os
# LOCAL_MODULE_DEPENDS += demo/mqtt_demo
# LOCAL_MODULE_DEPENDS += demo/gps_demo
# LOCAL_MODULE_DEPENDS += demo/http_demo
# LOCAL_MODULE_DEPENDS += demo/fota_demo
# LOCAL_MODULE_DEPENDS += demo/ftp_demo
# LOCAL_MODULE_DEPENDS += demo/sms_demo
# LOCAL_MODULE_DEPENDS += demo/wifi_demo
# LOCAL_MODULE_DEPENDS += demo/ntp_demo
# LOCAL_MODULE_DEPENDS += demo/ppp_demo
# LOCAL_MODULE_DEPENDS += demo/aliyun_demo
# LOCAL_MODULE_DEPENDS += demo/lbs_demo
LOCAL_MODULE_DEPENDS += demo/drivool
LOCAL_EXPORT_FLAG += \
	MBTK_TEST_FLAG
	

C_SRC := ${wildcard *.c}
C_SRC +=  uart_demo/uart_demo.c \
		   cam_demo/cam_demo.c
#####################################################
include ${SOFT_WORKDIR}/build/makerules.mk

