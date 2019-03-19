// SPDX-License-Identifier: Unlicense

/**
 * @file gconn_app.c
 * @author Igor Talzi
 * @date 16 Mar 2019
 * @brief Solution to SamKnows Technical Test.
 *
 * @see https://github.com/SamKnows/tests-and-metrics-test
 * @see https://curl.haxx.se/libcurl/
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

#include "gconn.h"

static int validate_int_option(char *opt, int *val)
{
	char *pEnd = NULL;

	errno = 0;
	const long sl = strtol(opt, &pEnd, 10);

	if (opt == pEnd) {
		fprintf(stderr, "Error: option %s is an invalid decimal value\n", opt);
		return -EINVAL;
	}  else if (*pEnd != '\0') {
		fprintf(stderr, "Error: option %s has extra characters at the end (%s)\n", opt, pEnd);
		return -EINVAL;
	} else if (errno == ERANGE && (sl == LONG_MIN || sl == LONG_MAX)) {
		fprintf(stderr, "Error: option %s is out-of-range value (long)\n", opt);
		return -ERANGE;
	} else if (sl > INT_MAX || sl < INT_MIN) {
		fprintf(stderr, "Error: option %s is out-of-range value (int)\n", opt);
		return -ERANGE;
	} else if (errno == EINVAL) { /* not in all c99 implementations - gcc OK */
		fprintf(stderr, "Error: option %s: %d invalid base\n", opt, *val);
		return -EINVAL;
	} else if (errno != 0 && sl == 0) {
		fprintf(stderr, "Error: option %s: %d invalid (unspecified error occurred)\n", opt, *val);
		return -EINVAL;
	}
	*val = (int)sl;

	return 0;
}

int main(int argc, char *argv[])
{
	int opt, res;
	int numReq = GCONN_NUM_REQ, reqInterval = GCONN_REQ_INTERVAL_MS;

	gconn_init();

	// Process command line options (no need to check return codes for gconn
	//  interface fucntions as parsing errors can be found in stderr and default
	//  values are applied (good enough for this app)
	while ((opt = getopt(argc, argv, ":H:n:i:")) != -1) {
		switch (opt) {
		case 'H':
			gconn_add_http_header(optarg);
			break;
		case 'n':
			res = validate_int_option(optarg, &numReq);
			if (!res) // all went fine, go on
				gconn_set_num_req(numReq);
			else
				exit(res);
			break;
		case 'i':
			res = validate_int_option(optarg, &reqInterval);
			if (!res) // all went fine, go on
				gconn_set_interval_req(reqInterval);
			else
				exit(res);
			break;
		case ':':
			fprintf(stderr, "Error: option %s needs a value\n", optarg);
			break;
		case '?':
			fprintf(stderr, "Error: unknown option %c\n", optopt);
			break;
		}
	}

	// Deal with unsupported arguments if any
	for (; optind < argc; optind++)
		fprintf(stderr, "Warning: extra argument \'%s\' ignored\n", argv[optind]);

	// Call the library function
	rscTimings *timings = gconn_rsc_timings_http_get();

	// Display timing stats
	printf("--- Output ---\n");
	printf("SKTEST;%s;%lu;%.6lf;%.6lf;%.6lf;%.6lf\n",
		   timings->remote_ip, timings->http_code, timings->time_namelookup,
		   timings->time_connect, timings->time_starttransfer, timings->time_total);

	return 0;
}
