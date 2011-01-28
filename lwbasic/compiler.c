/*
compiler.c

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
This is the actual compiler bit; it drives the parser and code generation
*/

#include <stdio.h>

#include <lw_alloc.h>
#include <lw_string.h>

#include "lwbasic.h"

/* parse a type; the next token will be acquired as a result */
/* the token advancement is to provide consistency */
static int parse_type(cstate *state)
{
	int pt = -1;
	
	switch (state -> lexer_token)
	{
	case token_kw_integer:
		pt = 1;
		break;

	default:
		lwb_error("Invalid type specification");
	}
	lexer(state);
	/* look for "unsigned" modifier for integer types */
	return pt;
}


/* issub means RETURNS is not allowed; !issub means RETURNS is required */
static void parse_subfunc(cstate *state, int issub)
{
	int pt;
	char *subname;
	int vis = 0;
			
	lexer(state);
	if (state -> lexer_token != token_identifier)
	{
		lwb_error("Invalid sub name '%s'", state -> lexer_token_string);
	}
	
	subname = lw_strdup(state -> lexer_token_string);
	
	lexer(state);
	if (state -> lexer_token == token_kw_public || state -> lexer_token == token_kw_private)
	{
		if (state -> lexer_token == token_kw_public)
			vis = 1;
		lexer(state);
	}

	/* ignore the "PARAMS" keyword if present */
	if (state -> lexer_token == token_kw_params)
		lexer(state);
	
	if (state -> lexer_token == token_eol || state -> lexer_token == token_kw_returns)
		goto noparms;

paramagain:
	if (state -> lexer_token != token_identifier)
	{
		lwb_error("Parameter name expected, got %d, %s\n", state -> lexer_token, state -> lexer_token_string);
	}
	printf("Got <param> = %s\n", state -> lexer_token_string);
	lexer(state);
	
	if (state -> lexer_token != token_kw_as)
		lwb_error("Expecting AS\n");
	lexer(state);
	
	pt = parse_type(state);
	printf("Got <type> = %d\n", pt);
	
	if (state -> lexer_token == token_char && state -> lexer_token_string[0] == ',')
	{
		lexer(state);
		goto paramagain;
	}

noparms:	
	if (!issub)
	{
		int rt;
		
		if (state -> lexer_token != token_kw_returns)
		{
			lwb_error("FUNCTION must have RETURNS\n");
		}
		lexer(state);
		if (state -> lexer_token == token_identifier)
		{
			printf("Return value named: %s\n", state -> lexer_token_string);
			lexer(state);
			if (state -> lexer_token != token_kw_as)
				lwb_error("Execting AS after RETURNS");
			lexer(state);
		}
		rt = parse_type(state);
		printf("Return type: %d\n", rt);
	}
	else
	{
		if (state -> lexer_token == token_kw_returns)
		{
			lwb_error("SUB cannot specify RETURNS\n");
		}
	}

	
	if (state -> lexer_token != token_eol)
	{
		lwb_error("EOL expected; found %d, %s\n", state -> lexer_token, state -> lexer_token_string);
	}

	
	printf("Sub/Func %s, vis %s\n", subname, vis ? "public" : "private");

	state -> currentsub = subname;

	/* consume EOL */
	lexer(state);
	
	/* variable declarations */
	/* parse_decls(state); */
	
	/* output function/sub prolog */
	emit_prolog(state, vis, 0);
	
	/* parse statement block  */
	/* parse_statemetns(state); */
	
	if (issub)
	{
		if (state -> lexer_token != token_kw_endsub)
		{
			lwb_error("Expecting ENDSUB, got %d (%s)\n", state -> lexer_token, state -> lexer_token_string);
		}
	}
	else
	{
		if (state -> lexer_token != token_kw_endfunction)
		{
			lwb_error("Expecting ENDFUNCTION, got %d (%s)\n", state -> lexer_token, state -> lexer_token_string);
		}
	}
	/* output function/sub epilog */
	emit_epilog(state);
	
	lw_free(state -> currentsub);
	state -> currentsub = NULL;
	
}

void compiler(cstate *state)
{
	state -> lexer_curchar = -1;
	
	/* now look for a global declaration */
	for (;;)
	{
		state -> parser_state = parser_state_global;
		lexer(state);
		switch (state -> lexer_token)
		{
		case token_kw_function:
			printf("Function\n");
			parse_subfunc(state, 0);
			break;
			
		case token_kw_sub:
			printf("Sub\n");
			parse_subfunc(state, 1);
			break;

		/* blank lines are allowed */
		case token_eol:
			continue;
		
		/* EOF is allowed - end of parsing */
		case token_eof:
			return;

		default:
			lwb_error("Invalid token %d, %s in global state\n", state -> lexer_token, state -> lexer_token_string);
		}
	}	
}
