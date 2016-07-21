#ifndef _ZEBRA_I2RS_ZEBRA_H
#define _ZEBRA_I2RS_ZEBRA_H

#include "vty.h"

#define EXTERNAL_METRIC_TYPE_1      0
#define EXTERNAL_METRIC_TYPE_2      1

#define DEFAULT_ROUTE		    ZEBRA_ROUTE_MAX
#define DEFAULT_ROUTE_TYPE(T) ((T) == DEFAULT_ROUTE)

/* Prototypes */
extern void i2rs_vty_init(void);

extern int i2rs_route_add (struct prefix_ipv4 *);
extern int i2rs_route_delete (struct prefix_ipv4 *);

extern void i2rs_zebra_init (struct thread_master *);

#endif /* _ZEBRA_I2RS_ZEBRA_H */

