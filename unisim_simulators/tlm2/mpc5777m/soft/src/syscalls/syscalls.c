/*
 *  Copyright (c) 2017,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <sys/stat.h>
#include "console.h"
#include <errno.h>
#include <unistd.h>

#undef errno
extern int  errno;

extern struct con_t con;

int close(int file)
{
	return 0;
}
 
int fstat(int fd, struct stat *buf)
{
	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO) || (fd == STDIN_FILENO))
	{
		buf->st_mode = S_IFCHR;
		return 0;
	}
	
	errno = EBADF;
	return -1;
}
 
int isatty(int fd)
{
	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO) || (fd == STDIN_FILENO))
	{
		return 1;
	}
	
	errno = EBADF;
	return -1;
}
 
off_t lseek(int fd, off_t offset, int whence)
{
	return -1;
}
 
int open(const char *name, int flags, int mode)
{
	return -1;
}
 
int read(int fd, void *buf, size_t nbytes)
{
	if(fd == STDIN_FILENO)
	{
		size_t i;
		char *p = (char *) buf;
		
		for(i = 0; i < nbytes; i++)
		{
			con_read(&con, &p[i], 1);
			
			if(p[i] == '\n')
			{
				i++;
				break;
			}
		}

		return i;
    }
    
	errno = EBADF;
	return -1;
}

int write(int fd, const void *buf, size_t nbytes)
{
	if((fd == STDOUT_FILENO) || (fd == STDERR_FILENO))
	{
		return con_write(&con, buf, nbytes);
    }
    
	errno = EBADF;
	return -1;
}

static char *heap_end = 0;
void *sbrk(ptrdiff_t incr)
{
	extern char __HEAP;
	extern char __HEAP_END;
	char *prev_heap_end;

	if(heap_end == 0)
	{
		heap_end = &__HEAP;
	}
	prev_heap_end = heap_end;

	if((heap_end + incr) > &__HEAP_END)
	{
		/* Heap and stack collision */
		return 0;
	}

	heap_end += incr;
	
	return prev_heap_end;
}

