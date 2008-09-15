/* support.c - Support routines and definitions
 * 
 * Copyright (C) 1998-2001  Oskar Liljeblad
 *
 * This file is part of bestfit.
 *
 * This software is copyrighted work licensed under the terms of the
 * GNU General Public License. Please consult the file "LICENSE" for
 * details.
 */

#include "support.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef STDC_HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

/* xstrtoi:
 * More simple version of strtol.
 */
bool
xstrtoi(char *str, int *result)
{
    char *endptr;
    long int lresult;

    lresult = strtol(str, &endptr, 10);
    if (str[0] == '\0' || endptr[0] != '\0')
    	return FALSE;

    *result = (int) lresult;
    return TRUE;
}

/* xmalloc:
 * Safe malloc, that will never return NULL.
 */
void *
xmalloc(size_t size)
{
    void *mem = malloc(size);
    assert_memory(mem != NULL);
    return mem;
}

/* Make sure that a memory allocation operation has succeeded.
 * Otherwise we print an message, and exit with non-zero status.
 */
void
assert_memory(bool expression)
{
    if (!expression) {
    	fprintf(stderr, "%s\n", strerror(ENOMEM));
	exit(1);
    }
}

/* is_directory:
 * Return TRUE if file exists, is readable and is a directory.
 */
bool
is_directory(char *filename)
{
    struct stat statbuf;
    if (stat(filename, &statbuf) == -1)
	return FALSE;
    return S_ISDIR(statbuf.st_mode);
}
