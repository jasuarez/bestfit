/* bestfit.c - The main source file
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

#define _GNU_SOURCE

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef STDC_HEADERS
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "getopt.h"

/* Verbosity level: 0 = quiet, 1 = normal, 2 = verbose */
static int verbosity = 1;
/* Program name: same as argv[0] */
static char *program_name;

/* VERSION_MESSAGE:
 * Message to display when the --version option is used.
 */
#define VERSION_MESSAGE \
PACKAGE " " VERSION "\n\
Written by Oskar Liljeblad.\n\
\n\
Copyright (C) 2001 Oskar Liljeblad\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"

/* HELP_MESSAGE:
 * Message to display when the --help option is used.
 */
#define HELP_MESSAGE "\
Usage: %s [OPTION]... FILE... [-e COMMAND...]\n\
Determine which FILEs should be put on a CD (or other disk) so that as\n\
little space as possible is wasted. Output the name of those files,\n\
or if --execute specified, execute a command for each.\n\
\n\
  -8, --80min               space is that of an empty 80 min CD (default)\n\
  -7, --74min               space is that of an empty 74 min CD\n\
  -d, --dirs-only           ignore non-directory arguments\n\
  -s, --space=BLOCKS        maximum amount of space allowed to use\n\
  -r, --reserve=BLOCKS      blocks to reserve (%d by default)\n\
  -e, --execute COMMAND...  execute commands for each selected file\n\
  -i, --invert              print/execute for non-fitting files instead\n\
  -v, --verbose             explain what is being done\n\
  -q, --quiet               output no diagnostic/information messages\n\
      --help                display this help and exit\n\
      --version             output version information and exit\n\
\n\
The block-size is fixed to 2048 bytes.\n\
\n\
All following arguments after --execute are taken to be arguments\n\
to the command to execute for each selected file. The string `{}'\n\
is replaced by the current file name being processed.\n\
\n\
Report bugs to <" BUG_EMAIL ">.\n"

static void print_knapsack_status(uint32_t current, uint32_t total);
static void verbose_printf(int minlvl, char *format, ...);
static void execute_command(char *file, char **argv);

static struct option long_options[] = {
    { "80min",	    	no_argument,	    NULL, '8' },
    { "74min",	    	no_argument,	    NULL, '7' },
    { "dirs-only",   	no_argument,	    NULL, 'd' },
    { "space",          required_argument,  NULL, 's' },
    { "reserve",    	required_argument,  NULL, 'r' },
    { "help",           no_argument,        NULL, 151 },
    { "version",        no_argument,        NULL, 150 },
    { "quiet",          no_argument,        NULL, 'q' },
    { "silent",         no_argument,        NULL, 'q' },
    { "invert",         no_argument,        NULL, 'i' },
    { "execute",        no_argument,        NULL, 'e' },
    { 0, 0, 0, 0 }
};

/* Print an error message to the standard error stream.
 */
static void
verbose_printf(int minlvl, char *format, ...)
{
    va_list args;

    va_start(args, format);
    if (verbosity >= minlvl) {
	vfprintf(stderr, format, args);
    	fflush(stderr);
    }
    va_end(args);
}

/* Called by knapsack function to display status information.
 */
static void
print_knapsack_status(uint32_t current, uint32_t total)
{
    verbose_printf(1, "  %4.1f%%\r", current*100.0/total);
}

/* Execute a command with specified arguments for the specified file.
 * Replace all occurences of {} in /argv/ with /file/.
 */
static void
execute_command(char *file, char **argv)
{
    pid_t child;

    child = fork();
    if (child == 0) {	/* in child */
    	int i, j, k;
	char *t;

    	/* Replace all occurences of {} with /file/ */
    	for (i = 1; argv[i] != NULL; i++) {
	    t = argv[i];
	    for (j = 0; (t = strstr(t, "{}")) != NULL; j++)
	    	t++;

    	    if (j > 0) {
    	    	t = xmalloc(strlen(argv[i]) - j*2 + j*strlen(file) + 1);
		k = 0;
	    	for (j = 0; argv[i][j] != 0; j++) {
		    if (argv[i][j] == '{' && argv[i][j+1] == '}') {
		    	strcpy(&t[k], file);
			k += strlen(file);
		    	j++;
		    } else {
		    	t[k++] = argv[i][j];
		    }
		}
		t[k] = '\0';
	    	argv[i] = t;
	    }
	}
    
    	execvp(argv[0], argv);
    	perror(program_name);
    	exit(1);
    } else if (child != -1) { /* in parent */
    	int status = 0;
    	if (waitpid(child, &status, 0) == -1) {
	    perror(program_name);
	    exit(1);
	}
	if (!WIFEXITED(status)) {
	    fprintf(stderr, "%s: child did not exit normally\n", program_name);
	    exit(1);
	}
    } else {
    	perror(program_name);
	exit(1);
    }
}

int
main (int argc, char **argv)
{
    int i;
    int j;
    int nonoptscount;
    uint64_t total;
    uint32_t used;
    uint32_t *weights;
    bool *results;
    char **nonopts;
    int space = ISO_BLOCKS_80_MIN_CD;
    int reserve = DEFAULT_RESERVED_BLOCKS;
    bool directories_only = FALSE;
    bool invert = FALSE;
    bool execute = FALSE;

    program_name = argv[0];
    nonopts = (argc <= 1 ? NULL : xmalloc(sizeof(char *) * (argc-1)));
    nonoptscount = 0;

    while (TRUE) {
    	int option_index = 0;
	i = getopt_long(argc, argv, "-87dqver:s:", long_options, &option_index);
    	if (i == EOF)
	    break;

    	if (i == 'e') {
	    if (argc <= optind) {
	    	printf("%s: missing arguments for --execute\n", program_name);
		exit(1);
	    }
	    execute = TRUE;
	    break;
	}

    	switch (i) {
	case '8': /* --80min */
	    space = ISO_BLOCKS_80_MIN_CD;
	    break; 

	case '7': /* --74min */
	    space = ISO_BLOCKS_74_MIN_CD;
	    break; 

    	case 'd': /* --dirs-only */
	    directories_only = TRUE;
	    break;

    	case 'q': /* --quiet */
	    verbosity = 0;
	    break;

	case 'v': /* --verbose */
	    verbosity = 2;
	    break;

    	case 's': /* --space */
	    if (!xstrtoi(optarg, &space)) {
		fprintf(stderr, "%s: invalid integer: %s\n", program_name, optarg);
		exit(1);
	    }
    	    break;

    	case 'r': /* --reserve */
	    if (!xstrtoi(optarg, &reserve)) {
		fprintf(stderr, "%s: invalid integer: %s\n", program_name, optarg);
		exit(1);
	    }
    	    break;

    	case 'i': /* --invert */
	    invert = TRUE;
	    break;

	case 150: /* --version */
	    printf(VERSION_MESSAGE);
	    exit(0);

	case 151: /* --help */
	    printf(HELP_MESSAGE, program_name, DEFAULT_RESERVED_BLOCKS);
	    exit(0);

    	case 1: /* non-option arguments */
	    nonopts[nonoptscount++] = optarg;
	    break;

	case '?':
	default:
	    exit(1);
    	}
    }

    if (verbosity == 1 && !isatty(STDERR_FILENO))
    	verbosity = 0;

    if (nonoptscount == 0) {
	fprintf(stderr, "%s: missing arguments\n", program_name);
	fprintf(stderr, "Try `%s --help' for more information.\n", program_name);
	exit(1);
    }

    weights = xmalloc(sizeof(uint32_t) * nonoptscount);
    for (i = 0; i < nonoptscount; i++) {
    	uint64_t size;

	if (directories_only && !is_directory(nonopts[i])) {
	    verbose_printf(2, "ignored file `%s'\n", nonopts[i]);
    	    nonoptscount--;
    	    for (j = i; j < nonoptscount; j++)
	    	nonopts[j] = nonopts[j+1];
	    i--;
	    continue;
	}

        verbose_printf(2, "scanning `%s': ", nonopts[i]);
    	size = get_simple_file_size(nonopts[i], DEFAULT_BLOCK_SIZE);
	if (size == -1) {
	    verbose_printf(2, "failed\n");
	    fprintf(stderr, "%s: %s\n", get_error_file(), strerror(errno));
	    exit(1);
	}
    	verbose_printf(2, "%lld %s\n", size, (size == 1 ? "byte" : "bytes"));

    	if ((uint64_t) (size/DEFAULT_BLOCK_SIZE) > (uint64_t) UINT_MAX) {
	    verbose_printf(2, "`%s' has more than 2^32 block - ignoring\n", nonopts[i]);
    	    nonoptscount--;
    	    for (j = i; j < nonoptscount; j++)
	    	nonopts[j] = nonopts[j+1];
	    i--;
	    continue;
    	}

    	weights[i] = size/DEFAULT_BLOCK_SIZE;
    }

    if (nonoptscount == 0) {
	fprintf(stderr, "%s: no remaining arguments\n", program_name);
	exit(1);
    }

    space -= reserve;
    results = knapsack(weights, nonoptscount, space, print_knapsack_status,
    	    	    (verbosity == 0 ? INT_MAX : 10000));
    verbose_printf(1, "       \r");

    used = 0;
    total = 0;
    for (i = 0; i < nonoptscount; i++) {
    	if (invert ^ results[i]) {
	    if (execute) {
    	    	verbose_printf(2, "%s\n", nonopts[i]);
		execute_command(nonopts[i], &argv[optind]);
	    } else {
	    	printf("%s\n", nonopts[i]);
	    }
	    used += weights[i];
	} else if (invert) {
	    verbose_printf(2, "include `%s'\n", nonopts[i]);
	} else {
	    verbose_printf(2, "did not include `%s'\n", nonopts[i]);
	}
	total += weights[i];
    }

    verbose_printf(1, "used %d of %lld %s (%.1fM of %.1fM)\n",
    	    used,
    	    total,
	    (total == 1 ? "block" : "blocks"),
	    (used*DEFAULT_BLOCK_SIZE) / 1048576.0,
	    (total*DEFAULT_BLOCK_SIZE) / 1048576.0);

    verbose_printf(1, "wasted %d of %d %s (%.1fM of %.1fM)\n",
	    space-used,
	    space,
	    (space == 1 ? "block" : "blocks"),
	    (space-used)*DEFAULT_BLOCK_SIZE / 1048576.0,
	    (space*DEFAULT_BLOCK_SIZE) / 1048576.0);

    free(results);
    free(weights);
    free(nonopts);

    exit(0);
}
