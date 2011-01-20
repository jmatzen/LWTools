/*
output.c
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


Actually output the binary
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwlink.h"

// this prevents warnings about not using the return value of fwrite()
// and, theoretically, can be replaced with a function that handles things
// better in the future
#define writebytes(s, l, c, f)	do { int r; r = fwrite((s), (l), (c), (f)); } while (0)

void do_output_decb(FILE *of);
void do_output_raw(FILE *of);
void do_output_lwex0(FILE *of);

void do_output(void)
{
	FILE *of;
	
	of = fopen(outfile, "wb");
	if (!of)
	{
		fprintf(stderr, "Cannot open output file %s: ", outfile);
		perror("");
		exit(1);
	}
	
	switch (outformat)
	{
	case OUTPUT_DECB:
		do_output_decb(of);
		break;
	
	case OUTPUT_RAW:
		do_output_raw(of);
		break;

	case OUTPUT_LWEX0:
		do_output_lwex0(of);
		break;
	
	default:
		fprintf(stderr, "Unknown output format doing output!\n");
		exit(111);
	}
	
	fclose(of);
}

void do_output_decb(FILE *of)
{
	int sn, sn2;
	int cloc, olen;
	unsigned char buf[5];
	
	for (sn = 0; sn < nsects; sn++)
	{
		if (sectlist[sn].ptr -> flags & SECTION_BSS)
		{
			// no output for a BSS section
			continue;
		}
		if (sectlist[sn].ptr -> codesize == 0)
		{
			// don't generate output for a zero size section
			continue;
		}
		
		// calculate the length of this output block
		cloc = sectlist[sn].ptr -> loadaddress;
		olen = 0;
		for (sn2 = sn; sn2 < nsects; sn2++)
		{
			// ignore BSS sections
			if (sectlist[sn2].ptr -> flags & SECTION_BSS)
				continue;
			// ignore zero length sections
			if (sectlist[sn2].ptr -> codesize == 0)
				continue;
			if (cloc != sectlist[sn2].ptr -> loadaddress)
				break;
			olen += sectlist[sn2].ptr -> codesize;
			cloc += sectlist[sn2].ptr -> codesize;
		}
		
		// write a preamble
		buf[0] = 0x00;
		buf[1] = olen >> 8;
		buf[2] = olen & 0xff;
		buf[3] = sectlist[sn].ptr -> loadaddress >> 8;
		buf[4] = sectlist[sn].ptr -> loadaddress & 0xff;
		writebytes(buf, 1, 5, of);
		for (; sn < sn2; sn++)
		{
			if (sectlist[sn].ptr -> flags & SECTION_BSS)
				continue;
			if (sectlist[sn].ptr -> codesize == 0)
				continue;
			writebytes(sectlist[sn].ptr -> code, 1, sectlist[sn].ptr -> codesize, of);
		}
		sn--;
	}
	// write a postamble
	buf[0] = 0xff;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = linkscript.execaddr >> 8;
	buf[4] = linkscript.execaddr & 0xff;
	writebytes(buf, 1, 5, of);
}

void do_output_raw(FILE *of)
{
	int nskips = 0;		// used to output blanks for BSS inline
	int sn;
	
	for (sn = 0; sn < nsects; sn++)
	{
		if (sectlist[sn].ptr -> flags & SECTION_BSS)
		{
			// no output for a BSS section
			nskips += sectlist[sn].ptr -> codesize;
			continue;
		}
		while (nskips > 0)
		{
			// the "" is not an error - it turns into a single NUL byte!
			writebytes("", 1, 1, of);
			nskips--;
		}
		writebytes(sectlist[sn].ptr -> code, 1, sectlist[sn].ptr -> codesize, of);
	}
}

void do_output_lwex0(FILE *of)
{
	int nskips = 0;		// used to output blanks for BSS inline
	int sn;
	int codedatasize = 0;
	unsigned char buf[32];
	
	// calculate items for the file header
	for (sn = 0; sn < nsects; sn++)
	{
		if (sectlist[sn].ptr -> flags & SECTION_BSS)
		{
			// no output for a BSS section
			nskips += sectlist[sn].ptr -> codesize;
			continue;
		}
		codedatasize += nskips;
		nskips = 0;
		codedatasize += sectlist[sn].ptr -> codesize;
	}

	// output the file header
	buf[0] = 'L';
	buf[1] = 'W';
	buf[2] = 'E';
	buf[3] = 'X';
	buf[4] = 0;		// version 0
	buf[5] = 0;		// low stack
	buf[6] = linkscript.stacksize / 256;
	buf[7] = linkscript.stacksize & 0xff;
	buf[8] = nskips / 256;
	buf[9] = nskips & 0xff;
	buf[10] = codedatasize / 256;
	buf[11] = codedatasize & 0xff;
	buf[12] = linkscript.execaddr / 256;
	buf[13] = linkscript.execaddr & 0xff;
	memset(buf + 14, 0, 18);
	
	writebytes(buf, 1, 32, of);
	// output the data
	// NOTE: disjoint load addresses will not work correctly!!!!!
	for (sn = 0; sn < nsects; sn++)
	{
		if (sectlist[sn].ptr -> flags & SECTION_BSS)
		{
			// no output for a BSS section
			nskips += sectlist[sn].ptr -> codesize;
			continue;
		}
		while (nskips > 0)
		{
			// the "" is not an error - it turns into a single NUL byte!
			writebytes("", 1, 1, of);
			nskips--;
		}
		writebytes(sectlist[sn].ptr -> code, 1, sectlist[sn].ptr -> codesize, of);
	}
}
