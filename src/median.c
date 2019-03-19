// SPDX-License-Identifier: Unlicense

/**
 * @file median.c
 * @author Igor Talzi
 * @date 19 Mar 2019
 * @brief Solution to SamKnows Technical Test.
 *
 * @see https://github.com/SamKnows/tests-and-metrics-test
 * @see https://curl.haxx.se/libcurl/
 */
#include <stdlib.h>

#include "median.h"

/**
 * @brief Comparison fucntion for type double
 * @param a First value
 * @param b Second value
 * @return Negative if the first argument is “less” than the second,
 *         zero if they are “equal”,
 *         and positive if the first argument is “greater”.
 */
static int compare_doubles(const void *a, const void *b)
{
	const double *da = (const double *) a;
	const double *db = (const double *) b;

	return (*da > *db) - (*da < *db);
}

/**
 * @brief Find median value in an array. Median of a sorted array
 *        of size N is defined as:
 *        1) it is middle element when N is odd.
 *        2) it is an average of middle two elements when N is even.
 * @param arr Array of values to find median of
 * @return Median value
 */
double find_median(double arr[], unsigned size)
{
	// First sort the array (order does not matter)
	// Use QuickSort (most of the elements are random and array size is small)
	qsort(arr, size, sizeof(double), compare_doubles);

	// Check for even case
	if (size % 2)
		return arr[size/2];

	return (arr[(size-1)/2] + arr[size/2])/2.0;
}
