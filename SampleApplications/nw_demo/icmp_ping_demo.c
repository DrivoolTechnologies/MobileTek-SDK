

#include "mbtk_comm_api.h"
#include "mbtk_socket_api.h"
#include "mbtk_datacall_api.h"
#include "mbtk_api.h"

#include "stdio.h"
#include "string.h"


#define MBTK_TEST_SOC_SER_IPV6

#define SOCKET_LOCAL_CID mbtk_cid_index_1
#define LOCAL_CID_PDN_TYPE mbtk_data_call_v4v6


#define PING_DATA_LEN 64
#define PING_TTL   255
#define PING_ICMD_ID                         0xB8B8
#define PING_NUM   4
#define PING_ADDR     "www.sina.com.cn"
#define PINT_TIMEOUT   3
#define PINT_INTEVAL   2


static mbtk_socket_t soc_fd = -1;
static uint_8 ping_send_num = 0;
static uint_8 ping_running = 0;
static uint_8 ping_recv_num = 0;

int ping_demo_wait_network(void)
{
    if (ol_wait_network_regist(120) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_wait_network_regist time out\n");
        return -1;
    }
    op_uart_printf("socket_demo_wait_network execute ol_data_call_start \n");
    if (ol_data_call_start(SOCKET_LOCAL_CID, LOCAL_CID_PDN_TYPE, "ctlte", NULL, NULL, 0) != mbtk_data_call_ok)
    {
        op_uart_printf("socket_demo_wait_network ol_data_call_start fail\n");
        return -1;
    }
    ol_os_task_sleep(2*200);
    return 0;
}

unsigned short mbtk_ping_cal_cksum(unsigned short *addr, int len)
{
    int nleft = len;
    u32_t sum = 0;
    u16_t *w = addr;
    u16_t answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w ;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return(answer);
}

unsigned short mbtk_ping6_cal_cksum(void *data_ptr, unsigned char proto, unsigned short proto_len, ip6_addr_t *src, ip6_addr_t *dest)
{
    unsigned int acc       = 0;
    unsigned int addr      = 0;
    unsigned char addr_part  = 0;
    unsigned char swapped    = 0;

    for (addr_part = 0; addr_part < 4; addr_part++)
    {
        addr = src->addr[addr_part];
        acc += (addr & 0xffffUL);
        acc += ((addr >> 16) & 0xffffUL);
        addr = dest->addr[addr_part];
        acc += (addr & 0xffffUL);
        acc += ((addr >> 16) & 0xffffUL);
    }

    /* fold down to 16 bits */
    acc = FOLD_U32T(acc);
    acc = FOLD_U32T(acc);

    /* iterate through all pbuf in chain */
    acc += ol_ip_standard_chksum(data_ptr, proto_len);
    acc = FOLD_U32T(acc);

    if (proto_len % 2 != 0)
    {
        swapped = 1 - swapped;
        acc = SWAP_BYTES_IN_WORD(acc);
    }
    if (swapped)
    {
        acc = SWAP_BYTES_IN_WORD(acc);
    }

    acc += (unsigned int)ol_htons(proto);
    acc += (unsigned int)ol_htons(proto_len);

    /* Fold 32-bit sum to 16 bits
    calling this twice is propably faster than if statements... */
    acc = FOLD_U32T(acc);
    acc = FOLD_U32T(acc);

    return (unsigned short)~(acc & 0xffffUL); 
}

void mbtk_ping_init(void)
{
    if(soc_fd >=0)
    {
        ol_close(soc_fd);
        soc_fd = -1;
    }

    ping_send_num = 0;
    ping_recv_num = 0;
}

int mbtk_send_ping(int socket_fd, void *icmp, unsigned int data_len, uint8_t is_ipv6, char *addr)
{
    struct icmp_echo_hdr *icmp_4 = NULL;
    struct icmp6_echo_hdr *icmp_6 = NULL;
    uint8_t ip_buf[128] = {0};
    int ret = 0;
    uint32_t *tick;

    op_uart_printf("%s enter is_ipv6[%d], %s", __FUNCTION__, is_ipv6, addr);

    if(!is_ipv6)
    {
        tick = (uint32_t *)(icmp + sizeof(struct icmp_echo_hdr));
        *tick = ol_os_get_ticks();
        icmp_4 = (struct icmp_echo_hdr *)icmp;
        icmp_4->type   = ICMP_ECHO;
        icmp_4->code   = 0;
        icmp_4->chksum = 0;
        icmp_4->id     = PING_ICMD_ID;
        icmp_4->seqno  = ping_send_num;
        icmp_4->chksum = mbtk_ping_cal_cksum((unsigned short *)icmp_4, data_len);

        op_uart_printf("%s echo4[%#x] data_len[%d]", __FUNCTION__, icmp_4,data_len);
        ret = ol_sendto(socket_fd, icmp_4, (size_t)data_len, 0, addr, 0);
        if(ret < 0)
        {
            op_uart_printf("%s mbtk_tcpip_send_ping fail[%d]", __FUNCTION__, ret);
            return -1;
        }
    }
    else
    {
        ip6_addr_t src_addr;
        ip6_addr_t dest_addr;

        memset(&src_addr, 0, sizeof(ip6_addr_t));
        memset(&dest_addr, 0, sizeof(ip6_addr_t));

        ol_inet6_aton(addr, &dest_addr);
        ol_get_pdp_ipv6addr(ip_buf, SOCKET_LOCAL_CID);
        ol_inet6_aton(ip_buf, &src_addr);

        tick = (uint32_t *)(icmp + sizeof(struct icmp6_echo_hdr));
        *tick = ol_os_get_ticks();
        icmp_6 = (struct icmp6_echo_hdr *)icmp;
        icmp_6->type   = ICMP6_TYPE_EREQ;
        icmp_6->code   = 0;
        icmp_6->chksum = 0;
        icmp_6->id     = PING_ICMD_ID;
        icmp_6->seqno  = ping_send_num;
        icmp_6->chksum = mbtk_ping6_cal_cksum((void *)icmp_6, IP6_NEXTH_ICMP6, data_len, &src_addr, &dest_addr);

        if(ol_sendto(socket_fd, icmp_6, (size_t)data_len, 0, addr, 0) < 0)
        {
            op_uart_printf("%s mbtk_tcpip_send_ping fail", __FUNCTION__);
            return -1;
        }
    }
    ping_send_num++;

    op_uart_printf("%s exit ping_nsend[%d]", __FUNCTION__,ping_send_num);

    return 0;
}

int mbtk_recv_ping(int socket_fd, unsigned char *data_buf, uint8_t is_ipv6, char *addr)
{
    int iret = 0;
    int recv_data_len = 0;
    int ttl = 0;
    struct icmp_echo_hdr *icmp4_header = NULL;
    struct icmp6_echo_hdr *icmp6_header = NULL;
    uint16_t port = 0;
    unsigned char *pdata = NULL;
    char ipaddr[80]= {0};
    ol_timeval time_out = {0};
    ol_fd_set rset = {0};

    unsigned int tv_send = 0;
    unsigned int tv_recv = 0;
    unsigned int tv_sub  = 0;

    op_uart_printf("%s enter is_ipv6[%d]", __FUNCTION__,is_ipv6);
    if(!is_ipv6)
    {
        recv_data_len = sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr) + 4;
    }
    else
    {
        recv_data_len = sizeof(struct icmp6_echo_hdr) + sizeof(struct ip6_hdr) + 4;
    }
		
    memset(data_buf, 0, recv_data_len);

    time_out.tv_sec = PINT_TIMEOUT;
    time_out.tv_usec = 0;

    OL_FD_ZERO(&rset);
    OL_FD_SET(socket_fd, &rset);

    while (1)
    {
        iret = ol_select(socket_fd + 1, &rset, NULL, NULL, &time_out);
        op_uart_printf("%s enter iret[%d]", __FUNCTION__,iret);

        switch(iret)
        {
            case -1 :
            {
                return -1;
            }

            case 0 :
            {
                op_uart_printf("%s ping  timeout", __FUNCTION__);
                break;
            }

            default :
            {
                
                iret = ol_recvfrom(socket_fd, data_buf, recv_data_len, 0, addr, &port);
                op_uart_printf("%s ping  recvfrom[%d]", __FUNCTION__,iret);
                if(iret < 0)
                {
                    return -1;
                }
                break;
            }
        }

        op_uart_printf("%s ping  recvfrom[%d]", __FUNCTION__,iret);
        if (iret == 0)
        {
            break;
        }

        if (!is_ipv6)
        {
            struct ip_hdr *ip4_header = NULL;
            ip4_header = (struct ip_hdr *)data_buf;
            ttl = IPH_TTL(ip4_header);
            pdata = (unsigned char *)&ip4_header->src;
            sprintf(ipaddr, "%d.%d.%d.%d", pdata[0], pdata[1], pdata[2], pdata[3]);

            if(IPH_PROTO(ip4_header) != IP_PROTO_ICMP)
            {
                op_uart_printf("%s ping  IP_PROTO is not ICMP");
                continue;
            }

            icmp4_header = (struct icmp_echo_hdr*)(data_buf + sizeof(struct ip_hdr));
            if (icmp4_header->type == ICMP_ER)
            {
                if (icmp4_header->id == PING_ICMD_ID)
                {
                    tv_send = *(unsigned int *)(data_buf + sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr));
                    tv_recv = ol_os_get_ticks();
                    tv_sub = U32_DIFF(tv_recv, tv_send) * 5;
                    ping_recv_num++;
                    op_uart_printf("%s ping tv_send=%d tv_recv=%d \n", __FUNCTION__, tv_send, tv_recv);

                    op_uart_printf("%s ping Reply from %s: bytes=%d time=%ldms TTL=%d\n", __FUNCTION__, ipaddr, iret, tv_sub, ttl);
                    break;
                }
                else 
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            struct ip6_hdr *ip6_header = NULL;
            ip6_addr_t src_addr = {0};

            ip6_header = (struct ip6_hdr*)data_buf;
            ttl = IP6H_HOPLIM(ip6_header);            
            ip6_addr_copy(src_addr, ip6_header->src);
            snprintf(ipaddr,sizeof(ipaddr),"%x:%x:%x:%x:%x:%x:%x:%x",
                     IP6_ADDR_BLOCK1(&src_addr), IP6_ADDR_BLOCK2(&src_addr),
                     IP6_ADDR_BLOCK3(&src_addr), IP6_ADDR_BLOCK4(&src_addr),
                     IP6_ADDR_BLOCK5(&src_addr), IP6_ADDR_BLOCK6(&src_addr),
                     IP6_ADDR_BLOCK7(&src_addr), IP6_ADDR_BLOCK8(&src_addr));

            if(IP6H_NEXTH(ip6_header) != IP6_NEXTH_ICMP6)
            {
            
                op_uart_printf("%s ping IP_PROTO is not ICMP\n");
                continue;
            }

            icmp6_header = (struct icmp6_echo_hdr*)(data_buf + sizeof(struct ip6_hdr));
						
            if(icmp6_header->type == ICMP6_TYPE_EREP)
            {
                if(icmp6_header->id == PING_ICMD_ID)
                {
                    tv_send = *(unsigned int *)(data_buf + sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr));
                    tv_recv = ol_os_get_ticks();
                    tv_sub = U32_DIFF(tv_recv, tv_send) * 5;
                    ping_recv_num++;
                    op_uart_printf("%s ping Reply from %s: bytes=%d time=%ldms TTL=%d\n", __FUNCTION__, ipaddr, iret, tv_sub, ttl);

                    break;
                }
                else 
                {
                    continue;
                }
            }
            else 
            {
                continue;
            }
        }
    }
    return 0;
}

int ping_demo(void) 
{
    int iret = 0;
    mbtk_ipaddr_struct_ex addr_info = {0};
    char ip_buf[128] = {0};
    uint8_t *send_buf = NULL;
    uint8_t *recv_buf = NULL;
    int send_len = 0;
    int recv_len = 0;
    uint8_t is_ipv6=1;
    uint8_t *ping_data = NULL;
    uint8_t send_data_len_temp = 0;
    uint8_t curr_ping_count = 0;
    uint8_t cur_ttl = 0;    
    char *dst_addr = NULL;

    op_uart_printf("ping_demo ping_demo_wait_network\n");

    if(ping_demo_wait_network() != 0)
    {
        op_uart_printf("ping_demo ping_demo_wait_network fail\n");
        return -1;
    }

    if(ping_running)
    {
        op_uart_printf("ping_demo ping_running!!!\n");
        return 1;
    }

    mbtk_ping_init();
    if(is_ipv6)
    {
        soc_fd = ol_socket(OL_AF_INET6, OL_SOCK_RAW, IP6_NEXTH_ICMP6);
    }
    else
    {
        soc_fd = ol_socket(OL_AF_INET, OL_SOCK_RAW, OL_IPPROTO_ICMP);
    }
    if(soc_fd  < 0)
    {
        op_uart_printf("ping_demo ol_socket err, soc_fd = %d \n", soc_fd);
        return -1;
    }
    op_uart_printf("ping_demo soc_fd = %d \n", soc_fd);

//  构建ping
    recv_len = send_len = PING_DATA_LEN;
    if (!is_ipv6)
    {
        send_len += (sizeof(struct icmp_echo_hdr));
        recv_len += (sizeof(struct icmp_echo_hdr) + sizeof(struct ip_hdr));
    }
    else
    {
        send_len += (sizeof(struct icmp6_echo_hdr));
        recv_len += (sizeof(struct icmp6_echo_hdr) + sizeof(struct ip6_hdr));
    }

    op_uart_printf("ping_demo send_len[%d], recv_len[%d]", send_len, recv_len);

    send_buf = ol_malloc(send_len);
    recv_buf = ol_malloc(recv_len);
    if (send_buf == NULL || recv_buf == NULL)
    {
        op_uart_printf("%s malloc data_buf fail",__FUNCTION__);
        return -1;
    }

    memset(send_buf, 0, send_len);
    memset(recv_buf, 0, recv_len);

    if (!is_ipv6)
    {
        ping_data = (unsigned char *)send_buf + sizeof(struct icmp_echo_hdr);
    }
    else
    {
        ping_data = (unsigned char *)send_buf + sizeof(struct icmp6_echo_hdr);
    }

    send_data_len_temp = PING_DATA_LEN - 4; // ping_datalen - offset;
    ping_data += 4;                 // pdata + offset

    unsigned char build_data = 'a';

    op_uart_printf("%s send_data_len_temp[%d]",__FUNCTION__, send_data_len_temp);
    while (send_data_len_temp > 0)
    {
        *ping_data++ = build_data;
        build_data++;
        if(build_data > 'w')
        {
            build_data = 'a';
        }
        send_data_len_temp--;
    }

    /*******************************************************************************/
    op_uart_printf("%s socket_fd[%d] ttl[%d]", __FUNCTION__, soc_fd, PING_TTL);
    if ((iret = ol_gethostbyname_ex(PING_ADDR, &addr_info)) == OL_ERROK)
    {
      if (addr_info.ipv4_sta && !is_ipv6){
     		ol_inet_ntop(OL_AF_INET, &addr_info.ipv4, ip_buf, sizeof(ip_buf));				
      }
			else if(addr_info.ipv6_sta && is_ipv6){
				ol_inet_ntop(OL_AF_INET6, &addr_info.ipv6, ip_buf, sizeof(ip_buf));
			}
    }

		if (strlen(ip_buf) != 0){
      op_uart_printf("dns resolve ip[%s] \n", ip_buf);
    }else{
			op_uart_printf("dns resolve ip is null \n");
			return -1;
		}

    cur_ttl = ol_get_ip_default_ttl();
    ol_set_ip_default_ttl(PING_TTL);
    while (curr_ping_count < PING_NUM)
    {
        op_uart_printf("%s curr_ping_count[%d]  ping_num[%d]", __FUNCTION__, curr_ping_count, PING_NUM);
        if ((iret = mbtk_send_ping(soc_fd, send_buf, send_len,is_ipv6, ip_buf)) != 0)
        {
            break;
        }

        if ((iret = mbtk_recv_ping(soc_fd, recv_buf, is_ipv6, dst_addr)) != 0)
        {
            break;
        }
        ol_os_task_sleep(PINT_INTEVAL*200);
        curr_ping_count++;
    }
    ol_set_ip_default_ttl(cur_ttl);
    ol_close(soc_fd);
    ol_free(send_buf);
    ol_free(recv_buf);
    return 1;
}

