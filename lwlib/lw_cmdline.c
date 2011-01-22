/*
lw_cmdline.c

Copyright © 2010 William Astle

This file is part of LWTOOLS.

LWTOOLS is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lw_alloc.h"

#define ___lw_cmdline_c_seen___
#include "lw_cmdline.h"

static struct lw_cmdline_options builtin[3] =
{
	{ "help", '?', 0, 0, "give this help list" },
	{ "usage", 0, 0, 0, "give a short usage message" },
	{ "version", 'V', 0, 0, "print program version" }
};

static void lw_cmdline_usage(struct lw_cmdline_parser *parser, char *name)
{
	struct lw_cmdline_options **slist, **llist;
	int nopt;
	int i;
	int t;
	
	for (nopt = 0; parser -> options[nopt].name; nopt++)
		/* do nothing */ ;
	
	slist = lw_alloc(sizeof(struct lw_cmdline_options *) * (nopt + 3));
	llist = lw_alloc(sizeof(struct lw_cmdline_options *) * (nopt + 3));	

	for (i = 0; i < nopt; i++)
	{
		slist[i] = &(parser -> options[i]);
		llist[i] = &(parser -> options[i]);
	}
	
	/* now sort the two lists */
	
	/* now append the automatic options */
	slist[nopt] = &(builtin[0]);
	slist[nopt + 1] = &(builtin[1]);
	slist[nopt + 2] = &(builtin[2]);

	llist[nopt] = &(builtin[0]);
	llist[nopt + 1] = &(builtin[1]);
	llist[nopt + 2] = &(builtin[2]);
	
	/* now show the usage message */
	printf("Usage: %s", name);
	
	/* print short options that take no args */
	t = 0;
	for (i = 0; i < nopt + 3; i++)
	{
		if (slist[i]->key > 0x20 && slist[i]->key < 0x7f)
		{
			if (slist[i]->arg == NULL)
			{
				if (!t)
				{
					printf(" [-");
					t = 1;
				}
				printf("%c", slist[i]->key);
			}
		}
	}
	if (t)
		printf("]");
	
	/* print short options that take args */
	for (i = 0; i < nopt + 3; i++)
	{
		if (slist[i]->key > 0x20 && slist[i]->key < 0x7f && slist[i] -> arg)
		{
			if (slist[i]->flags & lw_cmdline_opt_optional)
			{
				printf(" [-%c[%s]]", slist[i]->key, slist[i]->arg);
			}
			else
			{
				printf(" [-%c %s]", slist[i]->key, slist[i]->arg);
			}
		}
	}
	
	/* print long options */
	for (i = 0; i < nopt + 3; i++)
	{
		if (llist[i]->arg)
		{
			printf(" [--%s=%s%s%s]", 
				llist[i] -> name,
				(llist[i] -> flags & lw_cmdline_opt_optional) ? "[" : "",
				llist[i] -> arg,
				(llist[i] -> flags & lw_cmdline_opt_optional) ? "]" : "");
		}
		else
		{
			printf(" [--%s]", llist[i] -> name);
		}
	}
	
	/* print "non option" text */
	if (parser -> args_doc)
	{
		printf(" %s", parser -> args_doc);
	}
	printf("\n");
	
	/* clean up scratch lists */
	lw_free(slist);
	lw_free(llist);
}

static void lw_cmdline_help(struct lw_cmdline_parser *parser, char *name)
{
	struct lw_cmdline_options **llist;
	int nopt;
	int i;
	int t;
	char *tstr;
	
	tstr = parser -> doc;
	for (nopt = 0; parser -> options[nopt].name; nopt++)
		/* do nothing */ ;
	
	llist = lw_alloc(sizeof(struct lw_cmdline_options *) * (nopt + 3));	

	for (i = 0; i < nopt; i++)
	{
		llist[i] = &(parser -> options[i]);
	}
	
	/* now sort the list */
	
	/* now append the automatic options */
	llist[nopt] = &(builtin[0]);
	llist[nopt + 1] = &(builtin[1]);
	llist[nopt + 2] = &(builtin[2]);
	
	/* print brief usage */
	printf("Usage: %s [OPTION...] %s\n", name, parser -> args_doc ? parser -> args_doc : "");
	if (tstr)
	{
		while (*tstr && *tstr != '\v')
			fputc(*tstr++, stdout);
		if (*tstr)
			tstr++;
	}
	fputc('\n', stdout);
	fputc('\n', stdout);

	/* display options - do it the naïve way for now */
	for (i = 0; i < (nopt + 3); i++)
	{
		if (llist[i] -> key > 0x20 && llist[i] -> key < 0x7F)
		{
			printf("  -%c, ", llist[i] -> key);
		}
		else
		{
			printf("      ");
		}
		
		printf("--%s", llist[i] -> name);
		if (llist[i] -> arg)
		{
			if (llist[i] -> flags & lw_cmdline_opt_optional)
			{
				printf("[=%s]", llist[i] -> arg);
			}
			else
			{
				printf("=%s", llist[i] -> arg);
			}
		}
		if (llist[i] -> doc)
		{
			printf("\t\t%s", llist[i] -> doc);
		}
		fputc('\n', stdout);
	}

	printf("\nMandatory or optional arguments to long options are also mandatory or optional\nfor any corresponding short options.\n");

	if (*tstr)
	{
		printf("\n%s\n", tstr);
	}

	/* clean up scratch lists */
	lw_free(llist);
}

int lw_cmdline_parse(struct lw_cmdline_parser *parser, int argc, char **argv, unsigned flags, int *arg_index, void *input)
{
	int i, j, r;
	int firstarg;
	int nextarg;
	char *tstr;
	int cch;
	
	/* first, permute the argv array so that all option arguments are at the start */
	for (i = 1, firstarg = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1])
		{
			/* have an option arg */
			if (firstarg == i)
			{
				firstarg++;
				continue;
			}
			tstr = argv[i];
			for (j = i; j > firstarg; j--)
			{
				argv[j] = argv[j - 1];
			}
			argv[firstarg] = tstr;
			firstarg++;
			if (argv[firstarg - 1][1] == '-' && argv[firstarg - 1][2] == 0)
				break;
		}
	}

	/* now start parsing options */
	nextarg = firstarg;
	i = 1;
	cch = 0;
	while (i < firstarg)
	{
		if (cch > 0 && argv[i][cch] == 0)
		{
			i++;
			cch = 0;
			continue;
		}

		if (cch > 0)
			goto shortopt;
		
		/* skip the "--" option */
		if (argv[i][1] == '-' && argv[i][2] == 0)
			break;
		
		if (argv[i][1] == '-')
		{
			goto longopt;
		}
		
		cch = 1;
	shortopt:
		/* handle a short option here */
		
		/* automatic options */
		if (argv[i][cch] == '?')
			goto do_help;
		if (argv[i][cch] == 'V')
			goto do_version;
		/* look up key */
		for (j = 0; parser -> options[j].name; j++)
			if (parser -> options[j].key == argv[i][cch])
				break;
		cch++;
		tstr = argv[i] + cch;
		if (!*tstr)
		{
			if (nextarg < argc)
				tstr = argv[nextarg];
			else
				tstr = NULL;
		}
		goto common;

	longopt:
		if (strcmp(argv[i], "--help") == 0)
			goto do_help;
		if (strcmp(argv[i], "--usage") == 0)
			goto do_usage;
		if (strcmp(argv[i], "--version") == 0)
			goto do_version;
		/* look up name */
		
		for (j = 2; argv[i][j] && argv[i][j] != '='; j++)
			/* do nothing */ ;
		tstr = lw_alloc(j - 1);
		strncpy(tstr, argv[i] + 2, j - 2);
		tstr[j - 1] = 0;
		if (argv[i][j] == '=')
			j++;
		cch = j;
		
		for (j = 0; parser -> options[j].name; j++)
		{
			if (strcmp(parser -> options[j].name, tstr) == 0)
				break;
		}
		lw_free(tstr);
		tstr = argv[i] + cch;
		cch = 0;
		
	common:
		/* j will be the offset into the option table when we get here */
		/* cch will be zero and tstr will point to the arg if it's a long option */
		/* cch will be > 0 and tstr points to the theoretical option, either within */
		/* this string or "nextarg" */
		if (parser -> options[j].name == NULL)
		{
			fprintf(stderr, "Unknown option. See %s --usage.\n", argv[0]);
			exit(1);
		}
		if (parser -> options[j].arg)
		{
			if (tstr && cch && argv[i][cch] == 0)
				nextarg++;
			
			if (!*tstr)
				tstr = NULL;
			
			if (!tstr && (parser -> options[j].flags & lw_cmdline_opt_optional) == 0)
			{
				fprintf(stderr, "Option %s requires argument.\n", parser -> options[j].name);
			}
		}
		r = (*(parser -> parser))(parser -> options[j].key, tstr, input);
		if (r != 0)
			return r;
	}
	/* handle non-option args */
	if (arg_index)
		*arg_index = nextarg;
	for (i = nextarg; i < argc; i++)
	{
		r = (*(parser -> parser))(lw_cmdline_key_arg, argv[i], input);
		if (r != 0)
			return r;
	}
	r = (*(parser -> parser))(lw_cmdline_key_end, NULL, input);
	return r;

do_help:
	lw_cmdline_help(parser, argv[0]);
	exit(0);

do_version:
	printf("%s\n", parser -> program_version);
	exit(0);

do_usage:
	lw_cmdline_usage(parser, argv[0]);
	exit(0);
}
