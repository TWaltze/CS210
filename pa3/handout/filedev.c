/******************************************************************************
* Copyright (C) 2013 by Jonathan Appavoo, Boston University
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
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

#include "machine.h"
#include "mem.h"
#include "instruction.h"
#include "interrupt.h"
#include "misc.h"
#include "pidfile.h"

#define FILEDEV_MAXPATHLEN 80
#define FILEDEV_RETRY_PERIOD 3

struct filedev {
  char *start;
  int  size;
  pid_t pid;
  int  fd;
  address addr;
  char path[FILEDEV_MAXPATHLEN];
  struct filedev *next;
};

struct filedev *devs = NULL;

static void
killdevs(void)
{
  struct filedev *d;
  
  while (devs) {
    d = devs;
    devs = d->next;
    kill(d->pid, SIGTERM);
    free(d);
  }
}

static int
filedev_setup(struct filedev **f, char *pidpath, char *mempath, address addr, int wait) 
{
  struct stat finfo;
  pid_t pid;

  *f = NULL;
  
 retry:
  fprintf(stderr, "%s: probing\n", mempath); 
  if (stat(mempath, &finfo) != 0 || finfo.st_size == 0) {
    if (wait) {
      fprintf(stderr, "%s: not found ... will retry in %d secs\n", 
	      mempath, FILEDEV_RETRY_PERIOD);
      sleep(FILEDEV_RETRY_PERIOD);
      goto retry;
    } else {
      fprintf(stderr, "%s: not found or zero size... giving up\n", mempath);
      return 0;
    }
  }
  
  if (!pidfile_get(pidpath, &pid)) {
    fprintf(stderr, "%s: not able to determine pid of device (from %s)\n",
	    mempath, pidpath);
    return 0;
  }
  
  fprintf(stderr, "%s: found size=%d pid=%d\n", mempath, (int) finfo.st_size, pid);

  struct filedev *fdev = malloc(sizeof(struct filedev));
  if (fdev == NULL) {
    perror(__func__);
    return 0;
  }

  fdev->pid = pid;

  if (strncpy(fdev->path, mempath, FILEDEV_MAXPATHLEN) != fdev->path) {
    fprintf(stderr, "strncpy: path length exceeds max path length of %d\n", 
	    FILEDEV_MAXPATHLEN);
    free(fdev);
    return 0;
  }

  if ((fdev->fd = open (fdev->path, O_RDWR)) < 0) {
   perror(fdev->path);
   free(fdev);
   return 0;
  }

  fdev->size = finfo.st_size;
  fdev->addr = addr;

 /* mmap the output file */
 if ((fdev->start = mmap (0, fdev->size, PROT_READ | PROT_WRITE,
			  MAP_SHARED, fdev->fd, 0)) == (caddr_t) -1) {
   perror(fdev->path);
   close(fdev->fd);
   free(fdev);
   return 0;
 }

 if (devs == NULL) {
   atexit(killdevs);
 }

 fdev->next = devs;
 devs = fdev;

  *f = fdev;
  
  return 1;
}

static void
filedev_store(void *ptr, struct machine *m, address addr, byte v)
{
  //  fprintf(stderr, "%s\n", __func__);
  struct filedev *fdev = ptr;
  int offset = addr - fdev->addr;
  
  assert(offset>=0 && offset<fdev->size);
  fdev->start[offset] = v;
}

static void
filedev_load(void *ptr, struct machine *m, address addr, byte *v)
{
  // fprintf(stderr, "%s\n", __func__);
  struct filedev *fdev = ptr;
  int offset = addr - fdev->addr;
  
  assert(offset>=0 && offset<fdev->size);
  *v = fdev->start[offset];
}

int 
filedev_init(struct machine *m, char *pidpath, char *mempath, address addr, int wait)
{
  struct filedev *f;

  if (filedev_setup(&f, pidpath, mempath, addr, wait)) {
    return mem_map_device(addr, f, filedev_load, filedev_store, f->size);
  }
  return -1;
}
