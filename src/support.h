/* support.h - Support routines and definitions
 * 
 * Copyright (C) 1998-2001  Oskar Liljeblad
 *
 * This file is part of bestfit.
 *
 * This software is copyrighted work licensed under the terms of the
 * GNU General Public License. Please consult the file "LICENSE" for
 * details.
 */

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include "../config.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* bool:
 * Boolean type (use TRUE and FALSE).
 */
typedef int8_t bool;

/* FALSE:
 * TRUE:
 * Boolean values.
 */
#define FALSE	    0
#define TRUE	    (!FALSE)

bool xstrtoi(char *str, int *result);
void *xmalloc(size_t size);
void assert_memory(bool expression);
bool is_directory(char *filename);

#endif /* __SUPPORT_H__ */
