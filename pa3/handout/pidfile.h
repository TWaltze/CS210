#ifndef __PIDFILE_H__
#define __PIDFILE_H__

int pidfile_get(char *path, pid_t *pid);
void pidfile_init(char *path);

#endif
