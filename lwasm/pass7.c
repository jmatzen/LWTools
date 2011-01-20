/*
pass7.c

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
#include <string.h>

#include <lw_alloc.h>
#include <lw_string.h>

#include "lwasm.h"
#include "instab.h"

/*
emit pass

Generate object code
*/
void do_pass7(asmstate_t *as)
{
	line_t *cl;

	for (cl = as -> line_head; cl; cl = cl -> next)
	{
		as -> cl = cl;
		if (cl -> insn != -1)
		{
			if (instab[cl -> insn].emit)
			{
				(instab[cl -> insn].emit)(as, cl);
			}
		}
	}
}
