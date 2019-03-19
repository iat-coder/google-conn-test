// SPDX-License-Identifier: Unlicense

/**
 * @file gconn.c
 * @author Igor Talzi
 * @date 16 Mar 2019
 * @brief Solution to SamKnows Technical Test.
 *
 * @see https://github.com/SamKnows/tests-and-metrics-test
 * @see https://curl.haxx.se/libcurl/
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curl/curl.h>

#include "gconn.h"
#include "median.h"

static struct curl_slist *slist;
static int _numReq, _reqInterval;
static resourceTiming_t medianTiming, *pMedianTiming;

/**
 * @brief Internal function used to consume HTTP response body
 *        (by default Curl sends it to stdout)
 */
static size_t gconn_consume_data(void *buffer, size_t size, size_t nmemb,
								 void *userp)
{
	return size * nmemb;
}

/**
 * @brief Initialize/reset internal structures
 * @return Void
 */
void gconn_init(void)
{
	slist = NULL;
	_numReq = GCONN_NUM_REQ;
	_reqInterval = GCONN_REQ_INTERVAL_MS;
}

/**
 * @brief Add custom HTTP header to the request
 * @param httpHeader HTTP header (string)
 * @return Success/fail
 */
bool gconn_add_http_header(char *httpHeader)
{
	struct curl_slist *tmpSlist = NULL;

	tmpSlist = curl_slist_append(slist, httpHeader);
	if (!tmpSlist) {
		fprintf(stderr, "Error: HTTP header \"%s\" failed to apply\n",
				httpHeader);
		curl_slist_free_all(slist);
		return false;
	} else {
		fprintf(stderr, "Info: HTTP header \"%s\" applied successfully\n",
				httpHeader);
	}

	slist = tmpSlist;

	return true;
}

/**
 * @brief Set number of requests to make
 * @param numReq Number of requests
 * @return Success/fail
 */
bool gconn_set_num_req(int numReq)
{
	if (numReq >= 0 && numReq <= GCONN_MAX_NUM_REQ) {
		_numReq = numReq ? numReq : GCONN_NUM_REQ;
		fprintf(stderr, "Info: number of requests set to %d\n",
				_numReq);
		return true;
	}

	fprintf(stderr, "Error: invalid number of requests: %d\n", numReq);
	fprintf(stderr, "Info: number of requests defaults to %d\n", _numReq);
	return false;
}

/**
 * @brief Set interval between requests
 * @param reqInterval Interval between requests
 * @return Success/fail
 */
bool gconn_set_interval_req(int reqInterval)
{
	if (reqInterval >= 0 && reqInterval <= GCONN_MAX_REQ_INTERVAL_MS) {
		_reqInterval = reqInterval;
		fprintf(stderr, "Info: interval between requests set to %d\n",
				_reqInterval);
		return true;
	}

	fprintf(stderr, "Error: invalid interval between requests: %d\n",
			reqInterval);
	fprintf(stderr, "Info: interval between requests defaults to %d\n",
			_reqInterval);
	return false;
}

resourceTiming_t *gconn_resource_timing_http_get()
{
	CURL *curl;
	CURLcode res;
	int req, timingType;
	resourceTiming_t timing, *pTimings = NULL;
	double *timingSeries;

	struct timespec ts;
	ts.tv_sec = _reqInterval / 1000;
	ts.tv_nsec = (_reqInterval % 1000) * 1000000;

	pTimings = (resourceTiming_t *)malloc(sizeof(resourceTiming_t) * _numReq);
	if (!pTimings) {
		fprintf(stderr, "Error: couldn't allocate the resource timing set\n");
		goto out;
	}

	timingSeries = (double *)malloc(sizeof(double) * _numReq * GCONN_NUM_TIMING_TYPE);
	if (!timingSeries) {
		fprintf(stderr, "Error: couldn't allocate the space for timing series\n");
		free(pTimings);
		goto out;
	}

	pMedianTiming = NULL;
	curl = curl_easy_init();
	if (curl) {
		// Set google.com as the target
		curl_easy_setopt(curl, CURLOPT_URL, "http://google.com/");

		// In case google.com is redirected we tell libcurl
		//  to follow redirection
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		// Avoid incoming data to be sent to stdout
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gconn_consume_data);

		// Populate HTTP request with custom headers (use defaults otherwise)
		if (slist)
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

		// Use persistent HTTP connection to make all requests
		for (req = 0; req < _numReq; req++) {
			// Perform the request, res will get the return code
			fprintf(stderr, "Info: making HTTP request #%d\n", req + 1);
			res = curl_easy_perform(curl);
			// Check for errors
			if (res == CURLE_OK) {
				res = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &timing.remote_ip);
				if (res != CURLE_OK || !timing.remote_ip) {
					fprintf(stderr,
							"Error: couldn't retrieve IP address of the HTTP server: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &timing.http_code);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve HTTP response code: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &timing.time_namelookup);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve name lookup time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &timing.time_connect);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve connect time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &timing.time_appconnect);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve application connect time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &timing.time_pretransfer);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve pre-transfer time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &timing.time_starttransfer);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve start-transfer time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &timing.time_total);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve total time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &timing.time_redirect);
				if (res != CURLE_OK) {
					fprintf(stderr,
							"Error: couldn't retrieve redirect time: %s\n",
							curl_easy_strerror(res));
					goto cleanup;
				}

				pTimings[req] = timing;
			} else {
				fprintf(stderr, "Error: couldn't make HTTP request: %s\n",
						curl_easy_strerror(res));
				goto cleanup;
			}

			// Delay between requests
			nanosleep(&ts, NULL);
		}

		// Extract timing series
		for (req = 0; req < _numReq; req++) {
			/*printf("%d | %s | %lu | %.6lf | %.6lf | %.6lf | %.6lf | %.6lf | %.6lf | %.6lf\n",
				req+1,
				pTimings[req].remote_ip,
				pTimings[req].http_code,
				pTimings[req].time_namelookup,
				pTimings[req].time_connect,
				pTimings[req].time_appconnect,
				pTimings[req].time_pretransfer,
				pTimings[req].time_starttransfer,
				pTimings[req].time_total,
				pTimings[req].time_redirect);*/

			timingSeries[req] = pTimings[req].time_namelookup;
			timingSeries[req + _numReq] = pTimings[req].time_connect;
			timingSeries[req + _numReq * 2] = pTimings[req].time_appconnect;
			timingSeries[req + _numReq * 3] = pTimings[req].time_pretransfer;
			timingSeries[req + _numReq * 4] = pTimings[req].time_starttransfer;
			timingSeries[req + _numReq * 5] = pTimings[req].time_total;
			timingSeries[req + _numReq * 6] = pTimings[req].time_redirect;
		}

		// Calculate medians for all timing series
		for (timingType = 0; timingType < GCONN_NUM_TIMING_TYPE; timingType++) {
			double m = find_median(timingSeries + _numReq * timingType, _numReq);

			switch (timingType) {
			case 0:
				medianTiming.time_namelookup = m;
				break;
			case 1:
				medianTiming.time_connect = m;
				break;
			case 2:
				medianTiming.time_appconnect = m;
				break;
			case 3:
				medianTiming.time_pretransfer = m;
				break;
			case 4:
				medianTiming.time_starttransfer = m;
				break;
			case 5:
				medianTiming.time_total = m;
				break;
			case 6:
				medianTiming.time_redirect = m;
				break;
			}
			//printf("%d median = %.6lf\n", timingType+1, find_median(timingSeries + _numReq * timingType, _numReq));
		}
		medianTiming.remote_ip = pTimings[0].remote_ip;
		medianTiming.http_code = pTimings[0].http_code;
		pMedianTiming = &medianTiming;
cleanup:
		// Free all malloc resources
		free(pTimings);
		free(timingSeries);

		// Free the custom headers
		curl_slist_free_all(slist);

		// General cleanup
		curl_easy_cleanup(curl);
	}
out:
	return pMedianTiming;
}
