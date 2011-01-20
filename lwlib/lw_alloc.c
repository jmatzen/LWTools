/*
lw_alloc.c

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

#include <stdlib.h>

#define ___lw_alloc_c_seen___
#include "lw_alloc.h"

void lw_free(void *P)
{
	if (P)
		free(P);
}

void *lw_alloc(int size)
{
	void *r;
	
	r = malloc(size);
	if (!r)
	{
		abort();
	}
	return r;
}

void *lw_realloc(void *P, int S)
{
	void *r;

	if (!P)
	{
		return lw_alloc(S);
	}
	
	if (!S)
	{
		lw_free(P);
		return NULL;
	}
	
	r = realloc(P, S);
	if (!r)
	{
		abort();
	}
	return r;
}
