/* SPDX-License-Identifier: Unlicense */

#ifndef _GCONN_H_
#define _GCONN_H_

#include <stdbool.h>

#define GCONN_NUM_REQ 10
#define GCONN_MAX_NUM_REQ 100
#define GCONN_REQ_INTERVAL_MS 0
#define GCONN_MAX_REQ_INTERVAL_MS 60000 // =1min

struct resourceTiming {
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

#define GCONN_NUM_TIMING_TYPE 7 // as defined in resourceTiming above
typedef struct resourceTiming resourceTiming_t;

void gconn_init(void);
bool gconn_add_http_header(char *httpHeader);
bool gconn_set_num_req(int numReq);
bool gconn_set_interval_req(int reqInterval);
resourceTiming_t *gconn_resource_timing_http_get();

#endif
