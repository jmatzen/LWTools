/*
symtab.h

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
Symbol table handling
*/

#ifndef __symtab_h_seen__
#define __symtab_h_seen__

#ifndef __symtab_c_seen__
#define __E extern
#else
#define __E
#endif

/*
the meaning of "addr" and "symtype" is defined by the particular context
in which the symbol table is used
*/
typedef struct symtab_entry_s symtab_entry_t;
struct symtab_entry_s
{
	char *name;				/* name of the symbol */
	int addr;				/* address of symbol */
	int symtype;			/* type of symbol */
	
	symtab_entry_t *next;	/* next in the list */
};

typedef struct symtab_s
{
	symtab_entry_t *head;
} symtab_t;

__E symtab_t *symtab_init(void);
__E void symtab_destroy(symtab_t *st);
__E symtab_entry_t *symtab_find(symtab_t *st, char *name);
__E void symtab_register(symtab_t *st, char *name, int addr, int symtype);

#undef __E

#endif /* __symtab_h_seen__ */
