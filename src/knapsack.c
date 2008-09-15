/* knapsack.c - Algorithm for solving the 0-1 knapsack problem
 * 
 * Copyright (C) 1998-2001  Oskar Liljeblad
 *
 * This file is part of bestfit.
 *
 * This software is copyrighted work licensed under the terms of the
 * GNU General Public License. Please consult the file "LICENSE" for
 * details.
 */

#include "bestfit.h"

#define C(i,j)  (c[(i)*(W+1) + (j)])

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

/* Run the knapsack algorithm.
 *
 * /weights/ is an array with at least /items/ item weights.
 * /space/ is the maximum total size.
 * /status_cb/ is a function that will be called every /reportdelay/ units.
 * (A /reportdelay/ of MAX_INT and /status_cb/ of NULL disables reporting.)
 *
 * This function allocates and returns an array where cell /i/ indicates
 * whether item /i/ is included in the knapsack (TRUE) or not (FALSE).
 * This array should be freed with /free/ when no longer needed.
 */
bool *
knapsack(uint32_t *w, int n, int W, KnapsackStatusCallback status_cb,
    	int reportdelay)
{
    register int i;
    register int j;
    int *c;
    int minsize;
    bool *results;

    c = xmalloc(sizeof(int) * (n+1) * (W+1));
    results = xmalloc(sizeof(bool) * n);

    minsize = INT_MAX;

    for (j = 0; j <= W; j++) {
    	C(0, j) = 0;
    }
    for (i = 1; i <= n; i++) {
    	minsize = MIN(minsize, w[i-1]);
	minsize = MIN(minsize, W);
	minsize = MAX(minsize, 1);

    	for (j = 0; j < minsize; j++) {
    	    C(i, j) = 0;
	}

	for (j = minsize; j <= W; j++) {
	    if (j % reportdelay == 0 && status_cb != NULL)
	    	status_cb(j+W*(i-1), W*n);

	    if (w[i-1] <= j && w[i-1] + C(i-1, j-w[i-1]) > C(i-1, j)) {
	    	C(i, j) = w[i-1] + C(i-1, j-w[i-1]);
	    } else {
    	    	C(i, j) = C(i-1, j);
	    }
	}
    }

    j = W;
    for (i = n; i > 0; i--) {
	if (w[i-1] <= j && C(i, j) == w[i-1] + C(i-1, j-w[i-1])) {
	    results[i-1] = TRUE;
	    j -= w[i-1];
	} else {
	    results[i-1] = FALSE;
	}
    }

    return results;
}
