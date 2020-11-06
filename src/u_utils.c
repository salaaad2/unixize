/*
 * ========================
 * =====    ===============
 * ======  ================
 * ======  ================
 * ======  ====   ====   ==
 * ======  ===     ==  =  =
 * ======  ===  =  ==     =
 * =  ===  ===  =  ==  ====
 * =  ===  ===  =  ==  =  =
 * ==     =====   ====   ==
 * ========================
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2020 Joe
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the organization nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JOE ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL JOE BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * unixize: src/u_utils.c
 * Joe
 * 2020-11-05 19:27
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "c_opts.h"
#include "c_unixize.h"

void
u_memdel(void** ptr)
{
	free(*ptr);
	*ptr = NULL;
}

void
u_dump_errno(void)
{
	dprintf(
		STDERR_FILENO,
		"unixize: %s\n",
		strerror(errno)
	);
}

void
u_dump_errno_path(const char path[])
{
	dprintf(
		STDERR_FILENO,
		"unixize: %s: %s\n",
		path,
		strerror(errno)
	);
}

void
u_del_nargv(char** nargv)
{
	char** ptr;

	ptr = nargv;
	while (*ptr != NULL) {
		u_memdel((void*)&nargv[ptr - nargv]);
		ptr++;
	}
	u_memdel((void*)&nargv);
}

static char
u_get_extra_args(char args[], struct opts_s* opts)
{
	char i;

	i = 0;
	args[0] = 0x00;
	if (opts->hidden == TRUE) {
		memcpy((char*)args + (i * 3), "-a:", 4 * sizeof(char));
		i++;
	}
	if (opts->confirm == TRUE) {
		memcpy((char*)args + (i * 3), "-i:", 4 * sizeof(char));
		i++;
	}
	if (opts->hyphen == TRUE) {
		memcpy((char*)args + (i * 3), "-n:", 4 * sizeof(char));
		i++;
	}
	if (opts->pretend == TRUE) {
		memcpy((char*)args + (i * 3), "-p:", 4 * sizeof(char));
		i++;
	}
	if (opts->verbose == TRUE) {
		memcpy((char*)args + (i * 3), "-v:", 4 * sizeof(char));
		i++;
	}
	if (i > 0) {
		*(args + (i * 3) - 1) = 0x00;
	}
	return (i);
}

char**
u_get_nargv(struct opts_s* opts)
{
	short i;
	char args[19];
	char** nargv;
	char* tok;

	i = u_get_extra_args(args, opts) + 2;
	nargv = (char**)malloc((i + 2) * sizeof(char*));
	if (nargv == NULL) {
		u_dump_errno();
		return (NULL);
	}
	nargv[0] = (char*)malloc(2 * sizeof(char));
	nargv[1] = NULL;
	if (nargv[0] == NULL) {
		u_memdel((void*)&nargv);
		return (NULL);
	}
	nargv[0][0] = C_RECURSIVE_CHAR;
	nargv[0][1] = 0x00;
	nargv[1] = strdup("-R");
	nargv[2] = NULL;
	if (nargv[1] == NULL) {
		u_memdel((void*)&nargv[0]);
		u_memdel((void*)&nargv);
		return (NULL);
	}
	if (i < 3) {
		return (nargv);
	}
	tok = strtok(args, ":");
	nargv[2] = strdup(tok);
	if (nargv[2] == NULL) {
		u_del_nargv(nargv);
		return (NULL);
	}
	nargv[3] = NULL;
	i = 3;
	while ((tok = strtok(NULL, ":")) != NULL) {
		nargv[i] = strdup(tok);
		if (nargv[i] == NULL) {
			u_del_nargv(nargv);
			return (NULL);
		}
		nargv[i + 1] = NULL;
		i++;
	}
	return (nargv);
}
