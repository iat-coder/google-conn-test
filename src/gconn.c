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

static size_t consume_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

rscTimings *gconn_rsc_timings_http_get(char **httpHeaders, unsigned long numHead, unsigned long numReq, unsigned long reqInterval)
{
	CURL *curl;
	CURLcode res;
	rscTimings timings, *pTimings = NULL;
	int i;

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *slist = NULL;

		// Set google.com as the target
		curl_easy_setopt(curl, CURLOPT_URL, "http://google.com/");

		// In case google.com is redirected we tell libcurl
		//  to follow redirection
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		// Avoid incoming data to be sent to stdout
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, consume_data);

		// Set custom set of headers
		for (i = 0; i < numHead; i++)
		{
			struct curl_slist *tmpSlist = NULL;

			tmpSlist = curl_slist_append(slist, httpHeaders[i]);
			if (tmpSlist == NULL)
			{
				printf("Error: headers failed to apply\n");
				goto out;
			}

 			slist = tmpSlist;
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);
		// Check for errors
		if (res == CURLE_OK)
		{
			res = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &timings.remote_ip);
			if (res != CURLE_OK || !timings.remote_ip)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_PRIMARY_IP) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &timings.http_code);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_RESPONSE_CODE) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &timings.time_namelookup);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_NAMELOOKUP_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &timings.time_connect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_CONNECT_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &timings.time_appconnect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_APPCONNECT_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &timings.time_pretransfer);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_PRETRANSFER_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &timings.time_starttransfer);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_STARTTRANSFER_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &timings.time_total);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_TOTAL_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &timings.time_redirect);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "Error: curl_easy_getinfo(CURLINFO_REDIRECT_TIME) failed: %s\n",
					curl_easy_strerror(res));
				goto out;
			}

			pTimings = &timings;
		} else
		{
			fprintf(stderr, "Error: curl_easy_perform() failed: %s\n",
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
