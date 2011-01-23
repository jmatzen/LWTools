/*
lwbasic.h

Copyright © 2011 William Astle

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

/*
definitions used throughout lwbasic
*/

#ifndef __lwbasic_h_seen__
#define __lwbasic_h_seen__

typedef struct
{
	char *output_file;
	char *input_file;
	
	int debug_level;
	
	void *input_state;
} cstate;

#ifndef __input_c_seen__
extern int input_getchar(cstate *state);
#endif

#endif /* __lwbasic_h_seen__ */
