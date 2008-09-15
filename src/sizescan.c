/* sizescan.c - Routines for figuring out size of files
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef STDC_HEADERS
#include <string.h>
#endif

static char errorfile[PATH_MAX+1];

/* If get_simple_file_size returned -1, this function will
 * return a pointer to a static buffer containing the name
 * of the file that caused the error.
 */
char *
get_error_file()
{
    return errorfile;
}

/* Return size of file denoted by /name/. If it is a directory,
 * scan it recursively and return its total size. All returned
 * sizes are rounded upwards to the nearest /blocksize/ multiple.
 *
 * -1 is returned if there was an error.
 */
uint64_t
get_simple_file_size(char *name, int blocksize)
{
    char namebuf[PATH_MAX+1];
    uint64_t size = 0;
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;

    if (stat(name, &statbuf) == -1) {
    	if (lstat(name, &statbuf) == -1) {
	    strcpy(errorfile, name);
    	    return -1;
	}
    }

    size = statbuf.st_size;
    if (size % blocksize != 0)
    	size = (size/blocksize + 1) * blocksize;

    if (S_ISDIR(statbuf.st_mode)) {
	if ((dir = opendir(name)) == NULL) {
	    strcpy(errorfile, name);
    	    return -1;
	}

   	errno = 0;
    	while ((ent = readdir(dir)) != NULL) {
   	    if (strcmp(ent->d_name, ".") != 0
	    	    && strcmp(ent->d_name, "..") != 0) {
    	    	uint64_t tsize;
	    	int needsslash = (name[strlen(name)-1] != '/');

    	    	/* If the new filename is too long, bail out.
		 * Try to make a reasonable name for the errorfile though.
		 */
    	    	if (strlen(name)+needsslash+strlen(ent->d_name) > PATH_MAX) {
	    	    strcpy(errorfile, name);
		    if (needsslash && strlen(name)+needsslash <= PATH_MAX)
		    	strcat(errorfile, "/");
		    if (strlen(name)+needsslash+2 <= PATH_MAX)
		    	strcat(errorfile, "..");
		    errno = ENAMETOOLONG;
		    return -1;
		}

    	    	strcpy(namebuf, name);
		if (needsslash)
	    	    strcat(namebuf, "/");
	    	strcat(namebuf, ent->d_name);

    	    	tsize = get_simple_file_size(namebuf, blocksize);
	    	if (tsize == -1) {
		    closedir(dir);
		    return -1;
		}
	    	size += tsize;
	    }
    	    errno = 0;
	}

	if (errno != 0)
    	    size = -1;
	closedir(dir);
    }

    return size;
}
