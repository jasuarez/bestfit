/* bestfit.h - Global stuff for bestfit
 * 
 * Copyright (C) 1998-2001  Oskar Liljeblad
 *
 * This file is part of bestfit.
 *
 * This software is copyrighted work licensed under the terms of the
 * GNU General Public License. Please consult the file "LICENSE" for
 * details.
 */

#ifndef __BESTFIT_H__
#define __BESTFIT_H__

#include "../config.h"
#include "support.h"

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/* Number of 2048 byte block on a 74-min CD */
#define ISO_BLOCKS_74_MIN_CD	333000
/* Number of 2048 byte block on a 80-min CD */
#define ISO_BLOCKS_80_MIN_CD	360000
/* Number of 2048 byte block on a 4,7 GB Single Layer DVD */
#define ISO_BLOCKS_DVD_SL	2295104
/* Number of 2048 byte block on a 8,5 GB Double Layer DVD */
#define ISO_BLOCKS_DVD_DL	4171712
/* Default number of blocks to reserve on CDs */
#define DEFAULT_RESERVED_BLOCKS 200
/* The default (and unmodifyable) block size */
#define DEFAULT_BLOCK_SIZE  	2048

typedef void (*KnapsackStatusCallback)(uint32_t current, uint32_t total);

/* sizescan.c */
uint64_t get_simple_file_size(char *name, int blocksize);
char *get_error_file();

/* knapsack.c */
bool *knapsack(uint32_t *weights, int items, int space,
    	KnapsackStatusCallback status_cb, int reportdelay);

#endif /* __BESTFIT_H__ */
