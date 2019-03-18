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
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "gconn.h"

#define GCONN_NUM_REQ 10
#define GCONN_REQ_INTERVAL_MS 0
#define GCONN_NUM_HTTP_HEADERS 50
#define GCONN_HTTP_HEADER_LEN 256

static int validate_int_option(char *opt, unsigned long *val)
{
	char *endptr = NULL;

	errno = 0;
	*val = strtol(opt, &endptr, 10);
	if (opt == endptr)
	{
		printf ("Error: option %s: %lu invalid value\n", opt, *val);
		return EINVAL;
	} else if (errno == ERANGE && (*val == LONG_MIN || *val == LONG_MAX))
	{
		printf ("Error: option %s: %lu out-of-range value\n", opt, *val);
		return ERANGE;
	} else if (errno == EINVAL)  /* not in all c99 implementations - gcc OK */
	{
		printf ("Error: option %s: %lu invalid base\n", opt, *val);
		return EINVAL;
	} else if (*val < 0)
	{
		printf ("Error: option %s: %lu negative value\n", opt, *val);
		return EINVAL;
	} else if (errno != 0 && *val == 0)
	{
		printf ("Error: option %s: %lu invalid (unspecified error occurred)\n", opt, *val);
        return EINVAL;
	} else
	{
		return 0;
	}
}

int main(int argc, char *argv[])
{
	int opt, res, i, curHeader = 0;
	unsigned long numReq = GCONN_NUM_REQ, reqInterval = GCONN_REQ_INTERVAL_MS;
	char **httpHeaders;

	// Allocate all possible HTTP headers
	httpHeaders = malloc(sizeof(char *) * GCONN_NUM_HTTP_HEADERS);
	for (i = 0; i < GCONN_NUM_HTTP_HEADERS; i++)
		httpHeaders[i] = malloc(GCONN_HTTP_HEADER_LEN * sizeof(char));

	// Process command line options
	while((opt = getopt(argc, argv, ":H:n:i:")) != -1)
	{
		switch(opt)
		{
            case 'H':
            	if (strlen(optarg) <= GCONN_HTTP_HEADER_LEN-1)
            	{
            		printf("Error: HTTP header \'%s\' is too long (>%d) and will be skipped\n", optarg, GCONN_HTTP_HEADER_LEN);
            	} else
            	{
		        	if (curHeader >= GCONN_NUM_HTTP_HEADERS)
		        	{
						printf("Warning: too many HTTP headers (>%d), \'%s\' will not be applied\n", GCONN_NUM_HTTP_HEADERS, optarg);
		        	} else
		        	{
						strcpy(httpHeaders[curHeader], optarg);
						curHeader++;
					}
				}
				break;
			case 'n':
				res = validate_int_option(optarg, &numReq);
				if (!res) // all went fine, go on
				{
					if (!numReq)
						numReq = GCONN_NUM_REQ;
					printf("Info: number of requests set to %lu\n", numReq);
				} else
				{
					exit(res);
				}
				break;
			case 'i':
				res = validate_int_option(optarg, &reqInterval);
				if (!res) // all went fine, go on
					printf("Info: interval between requests set to %lu\n", reqInterval);
				else
					exit(res);
				break;
			case ':':
                printf("Error: option %s needs a value\n", optarg);
                break;
			case '?':
				printf("Error: unknown option %c\n", optopt);
			break;
		}
	}

	// Deal with unsupported arguments if any
	for(; optind < argc; optind++)
		printf("Warning: extra argument \'%s\' ignored\n", argv[optind]);

	// Call the library function
	rscTimings *timings = gconn_rsc_timings_http_get(httpHeaders, curHeader, numReq, reqInterval);

	// Display timing stats
	printf("SKTEST;%s;%lu;%.6lf;%.6lf;%.6lf;%.6lf\n", timings->remote_ip, timings->http_code,
		timings->time_namelookup, timings->time_connect, timings->time_starttransfer, timings->time_total);

	// Free the options (custom headers)
	for (i = 0; i < GCONN_NUM_HTTP_HEADERS; i++)
		free(httpHeaders[i]);
	free(httpHeaders);

	return 0;
}
