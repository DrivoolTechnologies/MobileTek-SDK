

#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "ol_nw_api.h"
#include "mbtk_api.h"
#include <stdio.h>
#include "string.h"

#define MBTK_TEST_SOC_SER_IPV6

#define SOCKET_LOCAL_CID mbtk_cid_index_2
#define LOCAL_CID_PDN_TYPE mbtk_data_call_v4v6

mbtk_socket_t soc_fd = 0;


/********************************************************************************************
       This function is only used to tell the user how to work, users are advised to use osmsg ect.  
     to forward socket events to user's tasks
*********************************************************************************************/
void socket_callback_func(mbtk_socket_t socket, int event, uint16_t len)
{
    op_uart_printf("socket_callback_func enter param socket[%d], event[%d], len[%d] \n", socket, event, len);
    switch(event)
    {
        case OL_SOCKET_EVT_ACCEPTED :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_ACCEPTED event \n", socket);
            break;
        }

        case OL_SOCKET_EVT_CONNECTED :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_CONNECTED event \n", socket);
            /*  creat msg; 
                msg.param = OL_SOCKET_EVT_CONNECTED;
                send msg to user's task;
                callback can't do any long time work;
            */
            break;
        }

        case OL_SOCKET_EVT_DATA_SEND :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_DATA_SEND event sentlen = %d \n", socket, len);
            /*  it's only ues for tcp connection, the data is already send to lte protocol stack  */
            break;
        }

        case OL_SOCKET_EVT_RECV_DATA :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_RECV_APP_DATA event recvlen = %d \n", socket, len);
            
            if(len > 0)
            {
                /*  creat msg; 
                    msg.param1 = OL_SOCKET_EVT_RECV_APP_DATA;
                    msg.param2 = datalen;
                    send msg to user's task;
                    callback can't do any long time work;
                */
            }
            else 
            {
                /*  read err
                    err = ol_get_socket_errno;
                    switch(err)
                    {
                        //TDO
                    }

                */
            }
            break;
        }

        case OL_SOCKET_EVT_CLOSER_IND :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_RECV_DATA event recvlen = %d \n", socket, len);
            /*  creat msg; 
                msg.param = OL_SOCKET_EVT_CLOSER_IND;
                send msg to user's task;
                callback can't do any long time work;
            */
            break;
        }
    
        case OL_SOCKET_EVT_RST     :
        case OL_SOCKET_EVT_ABRT  :
        
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_RST / OL_SOCKET_EVT_ABRT event \n", socket);
            /* close local socket */
            break;
        }
        
        case OL_SOCKET_EVT_SEND_ACKED :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_SEND_ACKED event \n", socket);
            /* the data has already send to remote, and the ack of remote has recved */
            break;
        }

        case OL_SOCKET_EVT_CLOSED :
        {
            op_uart_printf("socket_callback_func socket[%d] recv OL_SOCKET_EVT_SEND_ACKED event \n", socket);
            /* socket has closed normally */
            break;
        }
        
        default :
        {
            op_uart_printf("socket_callback_func socket[%d] recv unknown event[%d] \n", socket, event);
              /* may be some protocol stack event, doesn't care just break  */
            break;
        }
    }
}

int socket_demo_wait_network(void)
{
    if(ol_wait_network_regist(120) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_wait_network_regist time out\n");
        return -1;
    }
    op_uart_printf("socket_demo_wait_network execute ol_data_call_start \n");
    if(ol_data_call_start(SOCKET_LOCAL_CID, LOCAL_CID_PDN_TYPE, "cmnet", "aaa", "bbb", 1) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_data_call_start fail\n");
        return -1;
    }

    return 0;
}

int socket_demo(void) 
{
    int iret = 0;
    int onoff = 1;
    ol_fd_set readfd, writefd;
    mbtk_ipaddr_struct addr_info = {0};
		mbtk_ipaddr_struct_ex addr_info_ex = {0};
    char ip_buf[128] = {0};
    ol_timeval tv;
    int value = 0;
    int value_len = 0;
    char recv_buf[50] = {0};
    uint8_t is_ipv6=0;

    if(socket_demo_wait_network() != 0)
    {
        op_uart_printf("socket_demo socket_demo_wait_network fail\n");
        return -1;
    }

    if(is_ipv6)
    {
        soc_fd = ol_socket(OL_AF_INET6, OL_SOCK_STREAM, OL_IPPROTO_IP);
    }
    else
    {
        soc_fd = ol_socket(OL_AF_INET, OL_SOCK_STREAM, OL_IPPROTO_IP);
    }
    if(soc_fd  < 0)
    {
        op_uart_printf("socket_demo ol_socket err, soc_fd = %d \n", soc_fd);
        return -1;
    }
    op_uart_printf("socket_demo soc_fd = %d \n", soc_fd);

    if((iret = ol_gethostbyname_ex("www.sina.com.cn", &addr_info_ex)) == OL_ERROK)
    {
    		if(addr_info_ex.ipv4_sta){
					ol_inet_ntop(OL_AF_INET, &addr_info_ex.ipv4, ip_buf, sizeof(ip_buf));
					op_uart_printf("socket_demo resolve dns ipv4[%s] \n", ip_buf);
				}

				if(addr_info_ex.ipv6_sta){
					ol_inet_ntop(OL_AF_INET6, &addr_info_ex.ipv6, ip_buf, sizeof(ip_buf));
					op_uart_printf("socket_demo resolve dns ipv6[%s] \n", ip_buf);
				}
#if 0
        if(addr_info.iptype == OL_AF_INET)
        {
            ol_inet_ntop(OL_AF_INET, &addr_info.mbtk_ip_addr.ipv4, ip_buf, sizeof(ip_buf));
        }
        else 
        {
            ol_inet_ntop(OL_AF_INET6, &addr_info.mbtk_ip_addr.ipv6, ip_buf, sizeof(ip_buf));
        }

        if(strlen(ip_buf) != 0)
        {
            op_uart_printf("socket_demo resolve baidu dns ip[%s] \n", ip_buf);
        }
#endif

    }
    else
    {
        op_uart_printf("socket_demo ol_gethostbyname err, iret = %d \n", iret);
        ol_close(soc_fd);
        return -1;
    }



#if 0
    OL_FD_ZERO(&readfd);
    OL_FD_ZERO(&writefd);
    OL_FD_SET(soc_fd, &readfd);
    OL_FD_SET(soc_fd, &writefd);


    ol_ioctl(soc_fd, OL_FIONBIO, &onoff);

    tv.tv_sec = 10; // 10s for connect
    tv.tv_usec = 0;

    if(is_ipv6)
    {
        iret = ol_connect(soc_fd, "240E:474:B808:416F:0:0:0:1", 456);
    }
    else
    {
        iret = ol_connect(soc_fd, "118.114.239.159", 30063);
    }
    if(iret == OL_ERROK)
    {
        op_uart_printf("socket_demo connect success \n");
    }
    else if(ol_get_socket_errno() == OL_EINPROGRESS)
    {
        op_uart_printf("socket_demo ol_select enter\n");
        iret = ol_select(soc_fd + 1, &readfd, &writefd, NULL, &tv);
        op_uart_printf("socket_demo ol_select iret = %d\n", iret);
        switch(iret)
        {
            case -1:
            {
                ol_close(soc_fd);
                op_uart_printf("socket_demo ol_select fail \n");
                return -1;
            }

            case 0:
            {    
                ol_close(soc_fd);
                op_uart_printf("socket_demo ol_select timeout \n");
                return -1;
            }

            default:
            {
                // before connect read err
                value_len = sizeof(value);
                ol_getsocketopt(soc_fd, OL_SOL_SOCKET, OL_SO_ERROR, &value, (int *)&value_len);
                if(value != OL_ERROK)
                {
                    ol_close(soc_fd);
                    op_uart_printf("socket_demo connect recv rst %d\n", value);
                    return -1;
                }
                else
                {
                    op_uart_printf("socket_demo connect success \n");        
                    OL_FD_CLR(soc_fd, &readfd);
                    OL_FD_CLR(soc_fd, &writefd);
                    break;
                }
             }
        }
    }
    else
    {
        op_uart_printf("socket_demo connect iret = %d, errno = %d \n", iret, ol_get_socket_errno());
        ol_close(soc_fd);
        return -1;
    }

    if((iret = ol_send(soc_fd, (uint8_t *)"hello server", strlen("hello server"), 0)) < 0)
    {
        
        op_uart_printf("socket_demo ol_send fail iret = %d\n", iret);
        ol_close(soc_fd);
        return -1;
    }
    op_uart_printf("socket_demo ol_send success \n");

    tv.tv_sec = 30; // 30s for recv
    tv.tv_usec = 0;

    OL_FD_SET(soc_fd, &readfd);
    while(1)
    {
        iret = ol_select(soc_fd + 1, &readfd, NULL, NULL, &tv);
        op_uart_printf("socket_demo ol_select iret = %d \n", iret);
        if(iret <= 0)
        {
            op_uart_printf("socket_demo prefer recv ol_select fail iret = %d, socket err = %d\n", iret, ol_get_socket_errno());
            ol_close(soc_fd);
            return -1;
        }
        else 
        {
            if(OL_FD_ISSET(soc_fd, &readfd))
            {
                if((iret = ol_recv(soc_fd, (uint8_t *)recv_buf, sizeof(recv_buf), 0)) < 0)
                {
                    ol_close(soc_fd);
                    op_uart_printf("socket_demo ol_recv fail iret = %d\n", iret);
                    return -1;
                }    
                else 
                {
                    op_uart_printf("socket_demo ol_recv recv_buf[%s]\n", recv_buf);
                    ol_close(soc_fd);
                    return 0;
                }
            }
            else
            {
                ol_close(soc_fd);
                op_uart_printf("socket_demo ol_recv readfd is not set\n");
                return 0;
            }
        }
    }
#endif
	return 1;
}

typedef struct
{
    unsigned char ipaddr[128];
    unsigned short port;
    mbtk_taskref task_ref;
    mbtk_socket_t cli_soc_fd;
    void *task_stack_ptr;
}soc_cli_info;

void soc_incoming_cli_task(void *argv)
{
    ol_fd_set readfd;
    int iret = 0;
    soc_cli_info *info = (soc_cli_info *)argv;
    char recv_buf[50] = {0};
		mbtk_taskref curr_task_ref = NULL;
			
    op_uart_printf("soc_incoming_cli_task  cli_soc_fd = %d \n", info->cli_soc_fd);
		
    while(1)
    {
    		OL_FD_ZERO(&readfd);
    		OL_FD_SET(info->cli_soc_fd, &readfd);
				
        op_uart_printf("soc_incoming_cli_task  cli_soc_fd = %d \n", info->cli_soc_fd);
        iret = ol_select(info->cli_soc_fd + 1, &readfd, NULL, NULL, NULL);
        op_uart_printf("socket_demo ol_select iret = %d \n", iret);
        if(iret <= 0)
        {
            op_uart_printf("socket_demo prefer recv ol_select fail iret = %d, socket err = %d\n", iret, ol_get_socket_errno());
            ol_close(info->cli_soc_fd);
            break;
        }
        else
        {
            if(OL_FD_ISSET(info->cli_soc_fd, &readfd))
            {
                if((iret = ol_recv(info->cli_soc_fd, (uint8_t *)recv_buf, sizeof(recv_buf), 0)) < 0)
                {
                    ol_close(info->cli_soc_fd);
                    op_uart_printf("socket_demo ol_recv fail iret = %d\n", iret);
                    break;
                }
                else
                {
                    op_uart_printf("socket_demo ol_recv recv_buf[%s]\n", recv_buf);
                    //收到什么发什么
                    if((iret = ol_send(info->cli_soc_fd, (uint8_t *)recv_buf, iret, 0)) < 0)
                    {
                        op_uart_printf("socket_demo ol_send fail iret = %d\n", iret);
                    }

                    op_uart_printf("socket_demo ol_send success \n");
                }
            }
        }
    }

		curr_task_ref = info->task_ref;
		if(info) ol_free(info);
    if(curr_task_ref) ol_os_task_delete(curr_task_ref);   
}

#define MBTK_SOC_CLI_STACK_SIZE  4*1024
void socket_new_client_create(unsigned char *ipaddr, unsigned short port,  mbtk_socket_t cli_soc_fd)
{
    static int client_num=0;
    char task_name[20] = {0};
    soc_cli_info *info = ol_malloc(sizeof(soc_cli_info));
    memset(info, 0, sizeof(soc_cli_info));
    
    sprintf(task_name,"tcp_ser_cli%d", client_num++);

    if(!info)
    {
        op_uart_printf("\r\nsocket_new_client_create info nulllllll!!!\n");
    }

    strcpy((char *)info->ipaddr, (const char *)ipaddr);
    info->port = port;
    info->task_stack_ptr = NULL;
    info->cli_soc_fd = cli_soc_fd;

    ol_os_task_creat(&info->task_ref, NULL, MBTK_SOC_CLI_STACK_SIZE, 224,task_name, soc_incoming_cli_task, info);
}


#define TCP_SERVER_BACKLOG_NUM 5
int socket_server_demo(void) 
{
    int iret = 0;
    ol_fd_set readfd;
    char ip_buf[128] = {0};
    unsigned short port = 0;
    int reuseport = 1;    
    mbtk_socket_t client_soc_fd = 0;

    if(socket_demo_wait_network() != 0)
    {
        op_uart_printf("socket_server_demo socket_demo_wait_network fail\n");
        return -1;
    }
#ifdef MBTK_TEST_SOC_SER_IPV4
    if((soc_fd = ol_socket(OL_AF_INET, OL_SOCK_STREAM, OL_IPPROTO_IP)) < 0)
#else
    if((soc_fd = ol_socket(OL_AF_INET6, OL_SOCK_STREAM, OL_IPPROTO_IP)) < 0)
#endif
    {
        op_uart_printf("socket_server_demo ol_socket err, soc_fd = %d \n", soc_fd);
        return -1;
    }
    op_uart_printf("socket_server_demo soc_fd = %d \n", soc_fd);

    if(ol_setsocketopt(soc_fd, OL_SOL_SOCKET, OL_SO_REUSEADDR, &reuseport,  sizeof(reuseport))<0)
    {
        op_uart_printf("%s, ol_setsocketopt OL_SOL_SOCKET failed",__FUNCTION__);
        return -1;
    }

#ifdef MBTK_TEST_SOC_SER_IPV4    
    struct sockaddr_in addr_server = {0};
    ol_get_pdp_ipv4addr(ip_buf, SOCKET_LOCAL_CID);
            
    addr_server.sin_addr.s_addr = ol_inet_addr(ip_buf);
    addr_server.sin_family = OL_AF_INET;
    addr_server.sin_port = ol_htons(50001);
    addr_server.sin_len = sizeof(struct sockaddr_in);   
    op_uart_printf("socket_server_demo ol_bind nip[%s], nport[%d] \n", ol_inet_ntoa((const ip_addr_t *)&addr_server.sin_addr.s_addr), addr_server.sin_port);
		iret = ol_bind(soc_fd, (struct sockaddr *)&addr_server, sizeof(struct sockaddr_in));
#endif
    
#ifdef MBTK_TEST_SOC_SER_IPV6
    struct sockaddr_in6 addr_server;
    ol_get_pdp_ipv6addr((unsigned char *)ip_buf, mbtk_cid_index_2);
    op_uart_printf("socket_server_demo ol_inet6_ntoa %s\n", ip_buf);

    ol_get_pdp_ipv6addr((unsigned char *)ip_buf, mbtk_cid_index_1);

    op_uart_printf("socket_server_demo ol_inet6_ntoa1 %s\n", ip_buf);

    addr_server.sin6_family = OL_AF_INET6;
    addr_server.sin6_port = ol_htons(50001);
    ol_inet6_aton((const char *)ip_buf,(ip6_addr_t *)&addr_server.sin6_addr);    
    op_uart_printf("socket_server_demo ol_bind nip[%s], nport[%d] \n", ol_inet6_ntoa((const ip6_addr_t *)addr_server.sin6_addr.s6_addr), addr_server.sin6_port);
		iret = ol_bind(soc_fd, (struct sockaddr *)&addr_server, sizeof(struct sockaddr_in6));
#endif
    
    op_uart_printf("socket_server_demo ol_bind iret = %d", iret);
    if(iret < 0)
    {
        op_uart_printf("socket_server_demo ol_bind fail\n");
        return -1;
    }

    iret = ol_listen(soc_fd, TCP_SERVER_BACKLOG_NUM);
    if(iret < 0) 
    {
        ol_close(soc_fd);
        op_uart_printf("%s, listen error",__FUNCTION__);
        return -1;
    }

    op_uart_printf("socket_server_demo ol_listen iret = %d \n", iret);
    OL_FD_ZERO(&readfd);
    OL_FD_SET(soc_fd, &readfd);

    while(1)
    {
        iret = ol_select(soc_fd + 1, &readfd, NULL, NULL, NULL);
        op_uart_printf("socket_server_demo ol_select iret = %d \n", iret);
        if(iret <= 0)
        {
            op_uart_printf("socket_server_demo prefer recv ol_select fail iret = %d, socket err = %d\n", iret, ol_get_socket_errno());
            continue;
        }
        else 
        {
            if(OL_FD_ISSET(soc_fd, &readfd))
            {
                op_uart_printf("%s, Accept socket", __FUNCTION__);
                memset(ip_buf, 0, 128);
                client_soc_fd = ol_accept(soc_fd,ip_buf, &port);

                if(client_soc_fd<0)
                {
                    if(OL_EWOULDBLOCK == ol_get_socket_errno())
                    {
                        ol_os_task_sleep(50);
                        continue;
                    }
                }
                else
                {
                    socket_new_client_create((unsigned char *)ip_buf, port, client_soc_fd);
                    //这里创建一个新的task进行，如果是要客户端连接进来后立即发数据，可能会丢掉首包数据，如果想避免这种情况，可以提前建好task,
                    //分配可用的给新客户端即可
                }
            }
        }
    }
}

int socket_witch_callback_demo(void)
{
    if(socket_demo_wait_network() != 0)
    {
        op_uart_printf("socket_witch_callback_demo socket_demo_wait_network fail\n");
        return -1;
    }
    
    if((soc_fd = ol_socket_with_callback(OL_AF_INET, OL_SOCK_STREAM, OL_IPPROTO_TCP, socket_callback_func)) < 0)
    {
        op_uart_printf("socket_demo ol_socket err, soc_fd = %d \n", soc_fd);
        return -1;
    }
    op_uart_printf("socket_demo soc_fd = %d \n", soc_fd);

    ol_connect(soc_fd, "220.167.54.26", 30065);
    //after connect , call back will recv connected/err event;
    return 0;
}

int socket_udp(void)
{
    int iret = 0;
    ol_fd_set readfd;
    char ip_buf[50] = {0};
    char recv_buf[50] = {0};
    uint16_t port = 0; 
    ol_timeval tv;
    struct sockaddr_in *ptr;
    struct sockaddr_in addr_in = {0};
    struct sockaddr sock_addr = {0};
    int addr_len = 0;

    mbtk_data_call_info_strcut datacall_info = {0};

    if(socket_demo_wait_network() != 0)
    {
        op_uart_printf("socket_udp socket_demo_wait_network fail\n");
        return -1;
    }

    if((soc_fd = ol_socket(OL_AF_INET, OL_SOCK_DGRAM, OL_IPPROTO_UDP)) < 0)
    {
        op_uart_printf("socket_udp ol_socket fail\n");
        return -1;
    }
    op_uart_printf("socket_udp ol_socket socket = %d\n", soc_fd);

/*             ol_bind_local_cid is only support ipv4                   */
#if 0
    if((iret = ol_bind_local_cid(mbtk_cid_index_2, soc_fd, mbtk_data_call_v4, 50001)) < 0)
    {    
        op_uart_printf("socket_udp ol_bind_local_cid fail\n");
        ol_close(soc_fd);
        return -1;
    }
#else
    /*******************************************************************************

            this code is also to show how to bind ip and port use ol_bind

    *********************************************************************************/
    ol_get_pdp_ipv4addr((unsigned char *)ip_buf, mbtk_cid_index_2);

    addr_in.sin_addr.s_addr = ol_inet_addr(ip_buf);
    addr_in.sin_family = OL_AF_INET;
    addr_in.sin_port = ol_htons(50001);
    addr_in.sin_len = sizeof(struct sockaddr_in);
    op_uart_printf("socket_udp ol_bind nip[%d], nport[%d] fail\n", addr_in.sin_addr.s_addr, addr_in.sin_port);
    iret = ol_bind(soc_fd, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in));
    op_uart_printf("socket_udp ol_bind iret = %d", iret);
    if(iret < 0)
    {
        op_uart_printf("socket_udp ol_bind fail\n");
        return -1;
    }
#endif
    addr_len = sizeof(struct sockaddr_in); 
    iret = ol_getsocketname(soc_fd, &sock_addr, &addr_len);
    op_uart_printf("socket_udp ol_getsocketname iret = %d", iret);
    if(iret < 0)
    {
        op_uart_printf("socket_udp ol_getsocketname fail\n");
        return -1;
    }
    op_uart_printf("socket_udp ol_getsocketname addr_len = %d", addr_len);

    ptr = (struct sockaddr_in *)&sock_addr;

    op_uart_printf("socket_udp bind info ip[%d], port[%d], local ip[%d], port[%d] \n", ptr->sin_addr.s_addr, ptr->sin_port, datacall_info.v4info.ipv4addr.ip.s_addr, ol_htons(50001));

    if((iret = ol_sendto(soc_fd, "hello server", strlen("hello server"), 0, "220.167.54.26", 30065)) < 0)
    {
        op_uart_printf("socket_udp ol_sendto fail\n");
        ol_close(soc_fd);
        return -1;
    }
    OL_FD_ZERO(&readfd);
    OL_FD_SET(soc_fd, &readfd);
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    while(1)
    {
        iret = ol_select(soc_fd + 1, &readfd, NULL, NULL, &tv);
        op_uart_printf("socket_udp ol_select iret = %d\n", iret);
        if(iret <= 0)
        {
            op_uart_printf("socket_udp prefer recv ol_select fail iret = %d, socket err = %d\n", iret, ol_get_socket_errno());
            ol_close(soc_fd);
            return -1;
        }
        else 
        {
            op_uart_printf("socket_udp prefer recv check readfd set\n");
            if(OL_FD_ISSET(soc_fd, &readfd))
            {
                op_uart_printf("socket_udp call ol_recvfrom\n");
                if((iret = ol_recvfrom(soc_fd, recv_buf, sizeof(recv_buf), 0, ip_buf, &port)) < 0)
                {
                    ol_close(soc_fd);
                    op_uart_printf("socket_udp ol_recv fail iret = %d\n", iret);
                    return -1;
                }    
                else 
                {
                    op_uart_printf("socket_udp ol_recv recv_buf[%s], ip[%s], port[%d]\n", recv_buf, ip_buf, port);
                    ol_close(soc_fd);
                    return 0;
                }
            }
            else 
            {
                ol_close(soc_fd);
                op_uart_printf("socket_udp ol_recv readfd is not set\n");
                return 0;
            }
        }    
    }
}

#define IP_CHECK_MBTK(a) (a>=0 && a<=255)
int socket_ip6_dec_to_hex(char *addr_dec, char *addr_hex, int addr_hex_len)
{
    unsigned char addr[16] = {0}; 
    if(!addr_dec || !addr_hex)
    {
        return -1;
    }

    memset(addr,0, sizeof(addr));
    memset(addr_hex, 0, addr_hex_len);
    if(strlen(addr_dec)>16)
    {
        if(sscanf(addr_dec, "%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u", &addr[0],
            &addr[1], &addr[2], &addr[3],
            &addr[4], &addr[5], &addr[6],
            &addr[7], &addr[8], &addr[9],
            &addr[10], &addr[11], &addr[12],
            &addr[13], &addr[14], &addr[15]) == 16)
            if(IP_CHECK_MBTK(addr[0]) && IP_CHECK_MBTK(addr[1])
                && IP_CHECK_MBTK(addr[2]) && IP_CHECK_MBTK(addr[3])
                && IP_CHECK_MBTK(addr[4]) && IP_CHECK_MBTK(addr[5])
                && IP_CHECK_MBTK(addr[6]) && IP_CHECK_MBTK(addr[7])
                && IP_CHECK_MBTK(addr[8]) && IP_CHECK_MBTK(addr[9])
                && IP_CHECK_MBTK(addr[10]) && IP_CHECK_MBTK(addr[11])
                && IP_CHECK_MBTK(addr[12]) && IP_CHECK_MBTK(addr[13])
                && IP_CHECK_MBTK(addr[14]) && IP_CHECK_MBTK(addr[15])
                )
            {
                sprintf(addr_hex,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6],addr[7],addr[8],addr[9],
                addr[10],addr[11],addr[12],addr[13],addr[14],addr[15]);

                op_uart_printf("socket_ip6_dec_to_hex   %s\n", addr_hex);
                return 0;
            }
    }
    return -1;
}

void socket_ipv6_addr_test(void)
{
    //char ipv6[100] = {"240E:474:B808:416F:0:0:0:1"};
    char ipv6_10[100] = {"36.14.4.116.184.8.65.111.0.0.0.0.0.0.0.1"};
    char ipv6_test[100];

    op_uart_printf("socket_ipv6_addr_test   %d\n", socket_ip6_dec_to_hex(ipv6_10, ipv6_test, sizeof(ipv6_test)));
    op_uart_printf("socket_ipv6_addr_test   %s\n", ipv6_test);
}

