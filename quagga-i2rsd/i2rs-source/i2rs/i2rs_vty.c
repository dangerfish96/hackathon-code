#include <zebra.h>

#include "memory.h"
#include "thread.h"
#include "prefix.h"
#include "table.h"
#include "vty.h"
#include "lib/command.h"
#include "plist.h"
#include "log.h"
#include "zclient.h"
#include <signal.h>
#include <bits/sockaddr.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include "i2rs/i2rs_main.h"

struct{
	struct nlmsghdr nl;
	struct rtmsg rt;
	char buf[8192];
}request;
int fd;
int nllength;
int rtl;
int rtlength = sizeof(struct rtmsg);//RT Length
char buffer[8192], dsts[24], gws[24], ifs[16], ms[24];
struct sockaddr_nl localAddr;
struct nlmsghdr *nlpointer;
struct msghdr msg;
struct iovec iov;
struct rtmsg *rtpointer;
struct rtattr *rtap;
struct rtattr *rtattrpointer; //RT Attribute Pointer


DEFUN (show_ip_i2rs_route,
       show_ip_i2rs_route_cmd,
       "show ip i2rs route",
       "I2RS information\n")
{
	fd =socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	


	bzero(&localAddr, sizeof(localAddr));

	localAddr.nl_family = AF_NETLINK;
	localAddr.nl_pad=0;
	localAddr.nl_pid=getpid();
	localAddr.nl_groups=0;

	bind (fd,(struct sockaddr*) &localAddr, sizeof(localAddr));
	
	//getroutingtable
	bzero(&request, sizeof(request));

	request.nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	request.nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	request.nl.nlmsg_type = RTM_GETROUTE;

	request.rt.rtm_family = AF_INET;
	request.rt.rtm_table = RT_TABLE_MAIN;
	//end

	//sendrequest
	bzero(&localAddr, sizeof(localAddr));

	localAddr.nl_family = AF_NETLINK;

	bzero(&msg, sizeof(msg));
	msg.msg_name = (void *) &localAddr;
	msg.msg_namelen = sizeof(localAddr);

	iov.iov_base =(void *)&request.nl;
	iov.iov_len = request.nl.nlmsg_len;
	msg.msg_iov=&iov;
	msg.msg_iovlen =1;
	int temp = sendmsg(fd,&msg,0);
	if(temp<0){
		printf("send message failure\n");
	}
	else{
		printf("send message succesfull\n");
	}
	//end

	//recvrequest
	char *pointer;
	
	bzero(buffer,sizeof(buffer));
	nllength = 0;
	pointer = buffer;
	while(1){
		int rtn = recv(fd,pointer,sizeof(buffer)-nllength,0);

		nlpointer = (struct nlmsghdr *)pointer;

		if(nlpointer->nlmsg_type == NLMSG_DONE)
			break;

		pointer += rtn;
		nllength +=rtn;

		if((localAddr.nl_groups &RTMGRP_IPV4_ROUTE) == RTMGRP_IPV4_ROUTE)
			break;
	}
	//end


	//read reply
	nlpointer = (struct nlmsghdr *) buffer;
	for(;NLMSG_OK(nlpointer,nllength);nlpointer=NLMSG_NEXT(nlpointer,nllength)){
		rtpointer = (struct rtmsg *) NLMSG_DATA(nlpointer);
		if(rtpointer->rtm_table !=RT_TABLE_MAIN)
			continue;

		bzero(dsts, sizeof(dsts));
		bzero(gws, sizeof(gws));
		bzero(ifs, sizeof(ifs));
		bzero(ms, sizeof(ms));

		rtap = (struct rtattr *) RTM_RTA(rtpointer);
		rtl = RTM_PAYLOAD(nlpointer);
		for(;RTA_OK(rtap,rtl);rtap = RTA_NEXT(rtap, rtl))
		{
			switch(rtap->rta_type)
			{
			case RTA_DST:
				inet_ntop(AF_INET,RTA_DATA(rtap),dsts,24);
				break;
			case RTA_GATEWAY:
				inet_ntop(AF_INET,RTA_DATA(rtap),gws,24);
				break;
			case RTA_OIF:
				sprintf(ifs,"%d", *((int *)RTA_DATA(rtap)));
			default:
				break;
			}
		}
		sprintf(ms,"%d", rtpointer->rtm_dst_len);
		printf("dst %s/%s gw %s if %s\n",dsts,ms,gws,ifs);
	}
	//end
	
	close(fd);
	return CMD_SUCCESS;
}
//new route add funktion
DEFUN (route_ip_i2rs_add,
	   route_ip_i2rs_add_cmd,
	   "route ip i2rs add",
	   "Add new Route to the routing table\n")
{
	printf("Add route");
	return CMD_SUCCESS;
}
/*
DEFUN (show_ip_i2rs_add,
       show_ip_i2rs_add_cmd,
       "route ip i2rs add",
       SHOW_STR
       IP_STR
       "Add new Route information\n"
       "Add new Route to the routing table\n")
{
	
	fd =socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);//AF_Netlink address rang of ip4


	bzero(&localAddr, sizeof(localAddr));// setting the attributes of the address to 0
	

	localAddr.nl_family = AF_NETLINK;//ip4 address
	localAddr.nl_pid=getpid(); // Process ID of this deamon

	dsts = "10.0.2.16";
	int prefixl = 32;//
	printf("PrefixLength adress %i\n",prefixl);
	
	int ifstemp = 3;// interface number eth0 is 2
	printf("Interface adress %i\n",ifstemp);

	bind (fd,(struct sockaddr*) &localAddr, sizeof(localAddr));
	
	//set routing informtion
	printf("set routing informtion\n");
	bzero(&request, sizeof(request));//setting request to 0
	
	
	request.nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE;//setting flags for the message
	request.nl.nlmsg_type = RTM_NEWROUTE;//request type is for adding a new route

	request.rt.rtm_family = AF_INET;//ip4 address
	request.rt.rtm_table = RT_TABLE_MAIN;//main routing table
	request.rt.rtm_protocol = RTPROT_BOOT;//this is for the protocol which adds the route
	request.rt.rtm_scope = RT_SCOPE_UNIVERSE;
	request.rt.rtm_type = RTN_UNICAST;
	request.rt.rtm_dst_len = 32;
	request.rt.rtm_src_len = 0;

	rtattrpointer = (struct rtattr *) request.buf;
	rtattrpointer->rta_type = RTA_DST;//type of the Attribute (destination)
	rtattrpointer->rta_len = sizeof(struct rtattr) +4;//length of the attribute
	inet_pton(AF_INET, dsts, ((char *)rtattrpointer) + sizeof(struct rtattr));//string dsts cast into ip address add saved in rtattrpointer
	rtlength += rtattrpointer->rta_len; //adding length
	
	//rtattrpointer = (struct rtattr *) request.buf;
	rtattrpointer->rta_type = RTA_GATEWAY;//next type is Gateway
	rtattrpointer->rta_len = sizeof(struct rtattr) +4;
	inet_pton(AF_INET, gws, ((char *)rtattrpointer) + sizeof(struct rtattr));
	rtlength += rtattrpointer->rta_len;

	//rtattrpointer = (struct rtattr *) (((char *)rtattrpointer) + rtattrpointer->rta_len);
	rtattrpointer->rta_type = RTA_OIF;//last type is Interface
	rtattrpointer->rta_len = sizeof(struct rtattr) +4;
	memcpy(((char *)rtattrpointer) + sizeof(struct rtattr), &ifstemp, 4);
	rtlength += rtattrpointer->rta_len;

	request.nl.nlmsg_len = NLMSG_LENGTH(rtlength);//calculate complete length 

	printf("set routing informtion End\n");


	//sendrequest
	printf("send request\n");
	bzero(&peerAddr, sizeof(peerAddr));//set peer address to 0
	peerAddr.nl_family=AF_NETLINK;//set to ip4
					//because everzthing else is set to 0 the mess<ge will be send to the kernel
	bzero(&msg, sizeof(msg));//set message attributes to 0
	msg.msg_name = (void *) &peerAddr;//name, where the message will be send
	msg.msg_namelen = sizeof(peerAddr);

	iov.iov_base =(void *)&request.nl;//content of the message
	iov.iov_len = request.nl.nlmsg_len;

	msg.msg_iov=&iov;
	msg.msg_iovlen =1;//only 1 io Vector

	int rtn = sendmsg(fd,&msg,0);
	if(rtn<0){
		printf("send message failure\n");
	}
	else{
		printf("send message succesfull\n");
	}
	
	


	close(fd);
	return CMD_SUCCESS;
}*/
DEFUN (show_ip_i2rs_delete,
       show_ip_i2rs_delete_cmd,
       "show ip i2rs delete",
       "Delete Route information\n")
{
	printf("Delete Route");
	return CMD_SUCCESS;
}

void
i2rs_vty_show_init (void)
{

	
  install_element (VIEW_NODE, &show_ip_i2rs_route_cmd);
  install_element (ENABLE_NODE, &show_ip_i2rs_route_cmd);

  install_element (VIEW_NODE, &route_ip_i2rs_add_cmd);
  install_element (ENABLE_NODE, &route_ip_i2rs_add_cmd);
 }


/* i2rs's interface node. */
/*static struct cmd_node interface_node =
{
  INTERFACE_NODE,
  "%s(config-if)# ",
  1
};
static struct cmd_node i2rs_node =
{
  I2RS_NODE,
  "%s(config-if)# ",
  1
};

DEFUN(router_i2rs,
		router_i2rs_cmd,
		"router i2rs",
		"Enable i2rs")
{
	vty->node = I2RS_NODE;
	//vty->index = i2rs_global;
	return CMD_SUCCESS;
}*/
static int
config_write_interface (struct vty *vty)
{
 printf("dummyfunktion\n");
  return write;
}
static int
i2rs_config_write (struct vty *vty)
{
	printf("dummyfunktion\n");
  return write;
}
/*static struct cmd_node i2rs_node =
{
  I2RS_NODE,
  "%s(config-router)# ",
  1
};*/

/* Initialization of OSPF interface. */
//static void
//i2rs_vty_init (void)
//{
//  /* Install interface node. */
//  install_node (&i2rs_node, i2rs_config_write);
//  install_node (&interface_node, config_write_interface);
//  install_element (CONFIG_NODE, &router_i2rs_cmd);
//  install_element (CONFIG_NODE,  &show_ip_i2rs_route_cmd);
//  install_element (CONFIG_NODE,  &route_ip_i2rs_add_cmd);
//  i2rs_vty_show_init();
// }



