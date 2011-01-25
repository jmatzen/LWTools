/*
lexer.c

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
This handles the gritty details of parsing tokens
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lw_alloc.h>
#include <lw_string.h>

#define __lexer_c_seen__
#include "lwbasic.h"

/*
A token idenfier is returned by lexer(). The actual string value
is found in state->lexer_lexer_token_string; if the token as an integer value,
it will be found in state->lexer_token_number in the appropriate "value"
slot.
*/

struct token_list
{
	char *string;
	int token;
};

static struct token_list lexer_global_tokens[] = 
{
	{ "function",		token_kw_function },
	{ "sub",			token_kw_sub },
	{ "public",			token_kw_public },
	{ "private",		token_kw_private },
	{ "as",				token_kw_as },
	{ "params",			token_kw_params },
	{ "returns",		token_kw_returns },
	{ NULL }
};

static int lexer_getchar(cstate *state)
{
	int c;
	c = input_getchar(state);
	if (c == -2)
	{
		lwb_error("Error reading input stream.");
	}
	return c;
}

static void lexer_nextchar(cstate *state)
{
	state -> lexer_curchar = lexer_getchar(state);
	if (state -> lexer_curchar == state -> lexer_ignorechar)
		state -> lexer_curchar = lexer_getchar(state);
	state -> lexer_ignorechar = 0;
}

static int lexer_curchar(cstate *state)
{
	if (state -> lexer_curchar == -1)
	{
		lexer_nextchar(state);
	}
	
	return state -> lexer_curchar;
}

static void lexer_skip_white(cstate *state)
{
	int c;
	
	for (;;)
	{
		c = lexer_curchar(state);
		if (!(c == 0 || c == ' ' || c == '\t'))
			return;
		lexer_nextchar(state);
	}
}

/* must not be called unless the word will be non-zero length */
static void lexer_word(cstate *state)
{
	int wordlen = 0;
	int wordpos = 0;
	char *word = NULL;
	int c;
	struct token_list *tok = NULL;
	
	for (;;) {
		c = lexer_curchar(state);
		if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0x80)
		{
			/* character is part of word */
			if (wordpos >= wordlen)
			{
				word = lw_realloc(word, wordlen + 32);
				wordlen += 32;
			}
			word[wordpos++] = c;
		}
		else
			break;
		
		lexer_nextchar(state);
	}
	
	word[wordpos] = 0;
	lw_free(state -> lexer_token_string);
	state -> lexer_token_string = lw_strdup(word);
	
	switch (state -> parser_state)
	{
	default:
		tok = lexer_global_tokens;
	}
	
	/* check for tokens if appropriate */
	/* force uppercase */
	if (tok)
	{
		for (c = 0; word[c]; c++)
			if (word[c] >= 'A' && word[c] <= 'Z')
				word[c] = word[c] + 0x20;

		while (tok -> string)
		{
			if (strcmp(tok -> string, word) == 0)
				break;
			tok++;
		}
	}
	
	lw_free(word);
	if (tok && tok -> string)
		state -> lexer_token = tok -> token;
	else
		state -> lexer_token = token_identifier;
}

static void lexer_empty_token(cstate *state)
{
	lw_free(state -> lexer_token_string);
	state -> lexer_token_string = NULL;
}

void lexer(cstate *state)
{
	int c;

	lexer_skip_white(state);
	
	lexer_empty_token(state);
	
	c = lexer_curchar(state);
	if (c == -1)
	{
		state -> lexer_token = token_eof;
		return;
	}

	if (c == '\n')
	{
		/* LF */
		lexer_nextchar(state);
		state -> lexer_ignorechar = '\r';
		state -> lexer_token = token_eol;
		return;
	}
	
	if (c == '\r')
	{
		/* CR */
		lexer_nextchar(state);
		state -> lexer_ignorechar = '\n';
		state -> lexer_token = token_eol;
		return;
	}
	
	if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c >= 0x80)
	{
		/* we have a word here; identifier, keyword, etc. */
		lexer_word(state);
		return;
	}
	
	/* return the character if all else fails */
	state -> lexer_token_string = lw_realloc(state -> lexer_token_string, 2);
	state -> lexer_token_string[0] = c;
	state -> lexer_token_string[1] = 0;
	lexer_nextchar(state);
	state -> lexer_token = token_char;
	return;
}
