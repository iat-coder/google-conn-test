// SPDX-License-Identifier: Unlicense

#ifndef _GCONN_H_
#define _GCONN_H_

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

extern rscTimings *gconn_rsc_timings_http_get(char **httpHeaders, unsigned long numHead, unsigned long numReq, unsigned long reqInterval);

#endif
