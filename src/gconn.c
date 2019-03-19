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
#include <curl/curl.h>

#include "gconn.h"

static struct curl_slist *slist;
static unsigned long _numReq, _reqInterval;

/**
 * @brief Internal function used to consume HTTP response body
 *        (by default Curl sends it to stdout)
 */
static size_t gconn_consume_data(void *buffer, size_t size, size_t nmemb, void *userp)
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
	if (tmpSlist == NULL)
	{
		fprintf(stderr, "Error: HTTP header %s failed to apply\n", httpHeader);
		curl_slist_free_all(slist);
		return false;
	}

 	slist = tmpSlist;

 	return true;
}

/**
 * @brief Set number of requests to make
 * @param numReq Number of requests
 * @return Success/fail
 */
bool gconn_set_num_req(unsigned long numReq)
{
	if (numReq >= 0 && numReq <= GCONN_MAX_NUM_REQ)
	{
		_numReq = numReq ? numReq : GCONN_NUM_REQ;
		fprintf(stderr, "Info: number of requests set to %lu\n", _numReq);
		return true;
	}

	fprintf(stderr, "Error: invalid number of requests: %lu\n", numReq);
	return false;
}

/**
 * @brief Set interval between requests
 * @param reqInterval Interval between requests
 * @return Success/fail
 */
bool gconn_set_interval_req(unsigned long reqInterval)
{
	if (reqInterval >= 0 && reqInterval <= GCONN_MAX_REQ_INTERVAL_MS)
	{
		_reqInterval = reqInterval;
		fprintf(stderr, "Info: interval between requests set to %lu\n", _reqInterval);
		return true;
	}

	fprintf(stderr, "Error: invalid interval between requests: %lu\n", reqInterval);
	return false;
}

rscTimings *gconn_rsc_timings_http_get()
{
	CURL *curl;
	CURLcode res;
	rscTimings timings, *pTimings = NULL;

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

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);
		// Check for errors
		if (res == CURLE_OK)
		{
			res = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &timings.remote_ip);
			if (res != CURLE_OK || !timings.remote_ip)
			{
				fprintf(stderr, "Error: couldn't retrieve IP address of the HTTP server: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &timings.http_code);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve HTTP response code: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &timings.time_namelookup);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve name lookup time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &timings.time_connect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve connect time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &timings.time_appconnect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve application connect time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &timings.time_pretransfer);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve pre-transfer time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &timings.time_starttransfer);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve start-transfer time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &timings.time_total);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve total time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &timings.time_redirect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: couldn't retrieve redirect time: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			pTimings = &timings;
		} else
		{
			fprintf(stderr, "Error: couldn't make HTTP request: %s\n",
				curl_easy_strerror(res));
		}
out:
		// Free the custom headers
		curl_slist_free_all(slist);

		// General cleanup
		curl_easy_cleanup(curl);
	}

	return pTimings;
}
