/******************************************************************************
* Copyright (C) 2011 by Jonathan Appavoo, Boston University
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

char mypidfile[80];

static void
pidfile_rm(void)
{
  unlink(mypidfile);
}

int
pidfile_get(char *path, pid_t *pid)
{
  FILE *f = fopen(path, "r");

  if (f == NULL) {
    //   perror(path);
    return 0;
  }
  
  if (fscanf(f, "%d", pid)!=1) {
    fclose(f);
    return 0;
  }
  return 1;
}

void
pidfile_init(char *path)
{
  FILE *f;
  pid_t pid;

  if (pidfile_get(path, &pid) && pid != getpid()) {
    fprintf(stderr, "There is already an instance of this"
	    " program running with pid=%d. If not then remove"
            " %s by hand. Exiting\n", pid, path);
    exit(-1);
  }
	    
  f = fopen(path, "w");
  
  if (f == NULL) {
    perror(path);
    exit(-1);
  }

  assert(strncpy(mypidfile, path, sizeof(mypidfile)) == mypidfile);

  atexit(pidfile_rm);

  if (fprintf(f, "%d", getpid())<0) {
    perror(__func__);
    exit(-1);
  }

  fclose(f);
}
