#ifndef _FILE_H_
# define _FILE_H_

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_UN 8

#define LOCK_NB 4

extern int flock(int __fd, int __operation);

#endif /* _FILE_H_ */
