// SPDX-License-Identifier: Unlicense

#ifndef _GCONN_H_
#define _GCONN_H_

#include <stdbool.h>

#define GCONN_NUM_REQ 10
#define GCONN_MAX_NUM_REQ 100
#define GCONN_REQ_INTERVAL_MS 0
#define GCONN_MAX_REQ_INTERVAL_MS 60000 // =1min

struct rscTimings
{
	char *remote_ip;
	long http_code;
	double time_namelookup;
	double time_connect;
	double time_appconnect;
	double time_pretransfer;
	double time_starttransfer;
	double time_total;
	double time_redirect;
};
typedef struct rscTimings rscTimings;

extern void gconn_init(void);
extern bool gconn_add_http_header(char *httpHeader);
extern bool gconn_set_num_req(unsigned long numReq);
extern bool gconn_set_interval_req(unsigned long reqInterval);
extern rscTimings *gconn_rsc_timings_http_get();

#endif
