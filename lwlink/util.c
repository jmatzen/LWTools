/*
util.c
Copyright © 2009 William Astle

This file is part of LWLINK.

LWLINK is free software: you can redistribute it and/or modify it under the
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

/*
Utility functions
*/

#define __util_c_seen__
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *lw_malloc(int size)
{
	void *ptr;
	
	ptr = malloc(size);
	if (!ptr)
	{
		// bail out; memory allocation error
		fprintf(stderr, "lw_malloc(): Memory allocation error\n");
		exit(1);
	}
	return ptr;
}

void *lw_realloc(void *optr, int size)
{
	void *ptr;
	
	if (size == 0)
	{
		lw_free(optr);
		return;
	}
	
	ptr = realloc(optr, size);
	if (!ptr)
	{
		fprintf(stderr, "lw_realloc(): memory allocation error\n");
		exit(1);
	}
}

void lw_free(void *ptr)
{
	if (ptr)
		free(ptr);
}

char *lw_strdup(const char *s)
{
	char *d;
	
	if (!s)
		return NULL;

	d = strdup(s);
	if (!d)
	{
		fprintf(stderr, "lw_strdup(): memory allocation error\n");
		exit(1);
	}
	
	return d;
}
