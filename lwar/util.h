/*
util.h
Copyright © 2009 William Astle

This file is part of LWAR.

LWAR is free software: you can redistribute it and/or modify it under the
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

#ifndef __util_h_seen__
#define __util_h_seen__

#ifndef __util_c_seen__
#define __util_E__ extern
#else
#define __util_E__
#endif

// allocate memory
__util_E__ void *lw_malloc(int size);
__util_E__ void lw_free(void *ptr);
__util_E__ void *lw_realloc(void *optr, int size);

// string stuff
__util_E__ char *lw_strdup(const char *s);

#undef __util_E__

#endif // __util_h_seen__
