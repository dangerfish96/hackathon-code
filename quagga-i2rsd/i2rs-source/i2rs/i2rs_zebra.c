
#include <zebra.h>

#include "thread.h"
#include "command.h"
#include "network.h"
#include "prefix.h"
#include "routemap.h"
#include "table.h"
#include "stream.h"
#include "memory.h"
#include "zclient.h"
#include "filter.h"
#include "plist.h"
#include "log.h"

#include "i2rs/i2rs_zebra.h"


/* Zebra structure to hold current status. */
struct zclient *zclient = NULL;

/* For registering threads. */
extern struct thread_master *master;
struct in_addr router_id_zebra;



int
i2rs_route_add (struct prefix_ipv4 *p)
{
//std:raise(SIGINT);
/*
  struct zapi_ipv4 api;
printf("ERROR number socket %d\n", zclient->sock);	
printf("ERROR number connection%d\n", zclient->t_connect);
		
	
 //int test = zclient_start(&zclient);
	
	
 printf("i2rs zebra ADD start\n");
  api.vrf_id = VRF_DEFAULT;
  api.type = ZEBRA_ROUTE_OSPF;
  api.flags =0;
  api.message = 0;
  api.safi = SAFI_UNICAST;
  SET_FLAG (api.message, ZAPI_MESSAGE_NEXTHOP);
  api.nexthop_num = 0;
  api.ifindex_num = 0;
	
	printf("zapi ip4 route call\n");// zapi_ipv4_route (ZEBRA_IPV4_ROUTE_ADD, zclient, p, &api); 
  zapi_ipv4_route (7, zclient, p, &api); 
     	printf("zapi ip4 route call END\n");

*/
printf("add route\n");	
return 0;

}

int i2rs_route_delete(struct prefix_ipv4 *p){
	printf("delete route\n");	
	return 0;
}

int i2rs_interface_add(struct zclient *zclient){
	printf("i2rs interface add\n");
	struct interface *ifp;
	ifp = zebra_interface_add_read(zclient->ibuf,VRF_DEFAULT);
	return 0;

}

int i2rs_interface_delete(struct zclient *zclient){
	printf("i2rs interface delete\n");
	struct interface *ifp;
	ifp = zebra_interface_add_read(zclient->ibuf,VRF_DEFAULT);
	return 0;
	//if_delete(ifp);

}

int i2rs_interface_state_up(struct zclient *zclient){
	struct interface *ifp;
	printf("i2rs interface up?\n");
	ifp= zebra_interface_state_read(zclient->ibuf,VRF_DEFAULT);
	return 0;

}
static
int i2rs_interface_state_down(struct zclient *zclient){
	struct interface *ifp;
	printf("i2rs interface down?\n");
	ifp= zebra_interface_state_read(zclient->ibuf,VRF_DEFAULT);
	return 0;

}
	
int i2rs_interface_address_add(int command, struct zclient *zclient,zebra_size_t lenght)
{
	struct connected *c;
	struct prefix *p;
	
	c = zebra_interface_address_add_read(zclient->ibuf,VRF_DEFAULT);
	if(c==NULL)
		return 0;
	
	p = c->address;
	printf("i2rs interface address add");
	connected_free(c);
	return 0;
	
}

int i2rs_interface_address_delete(int command, struct zclient *zclient,zebra_size_t lenght)
{
	struct connected *c;
	struct prefix *p;
	
	c = zebra_interface_address_add_read(zclient->ibuf, VRF_DEFAULT);
	if(c==NULL)
		return 0;

	printf("i2rs interface address Delete");
	connected_free(c);
	return 0;
	
}

int zclient_read_zapi_ipv4(struct zclient* zclient, struct zapi_ipv4 *zapi, struct prefix_ipv4* p, unsigned long* ifindex, struct in_addr* nexthop)
{
	struct stream *s;

	s = zclient->ibuf;
	
	zapi->type = stream_getc(s);
	zapi->flags = stream_getc(s);
	zapi->message = stream_getc(s);
	
	memset(p,0,sizeof(struct prefix_ipv4));
	
	p->family = AF_INET;
	p->prefixlen = stream_getc(s);	
	stream_get(&p->prefix,s,PSIZE(p->prefixlen));
	if(CHECK_FLAG(zapi->message,ZAPI_MESSAGE_NEXTHOP))
		{
			zapi->nexthop_num=stream_getc(s);
			nexthop->s_addr=stream_get_ipv4(s);
		}
	if(CHECK_FLAG(zapi->message, ZAPI_MESSAGE_IFINDEX))
		{
			zapi->ifindex_num = stream_getc(s);
			*ifindex = stream_getl(s);
		}
	if(CHECK_FLAG(zapi->message,ZAPI_MESSAGE_DISTANCE))
		zapi->distance=stream_getc(s);
	if(CHECK_FLAG(zapi->message,ZAPI_MESSAGE_METRIC))
		zapi->metric = stream_getl(s);	
	return 0;
}

int i2rs_zebra_route_manage(int command, struct zclient *zclient, zebra_size_t length){
	struct prefix_ipv4 p;
	struct zapi_ipv4 zapi;
	unsigned long ifindex;
	struct in_addr nexthop;
	
	zclient_read_zapi_ipv4(zclient,&zapi,&p,&ifindex,&nexthop);
return 0;
}

void
i2rs_zebra_init (struct thread_master *master)
{
  /* Allocate zebra structure. */
  zclient = zclient_new (master);
  zclient_init (zclient, ZEBRA_ROUTE_OSPF);
  //zclient->zebra_connected = ;
  //zclient->router_id_update = ;
  zclient->interface_add = i2rs_interface_add;
  zclient->interface_delete = i2rs_interface_delete;
  zclient->interface_up = i2rs_interface_state_up;
  zclient->interface_down = i2rs_interface_state_down;
  zclient->interface_address_add = i2rs_interface_address_add;
  zclient->interface_address_delete =i2rs_interface_address_delete;
  zclient->ipv4_route_add =i2rs_route_add;
  zclient->ipv4_route_delete = i2rs_route_delete;
}
