/*
 * This file is part of ToyOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2016 Fabien Siron <fabien.siron@epita.fr>
 *
 * vfs_flock
 *
 * Maintain a file lock list for each inode
 */

#include <system.h>
#include <logging.h>
#include <fs.h>

/* fcntl.h */
#define F_RDLCK    0
#define F_WRLCK    1
#define F_UNLCK    2

/* fcntl.h */
#define LOCK_SH    1
#define LOCK_EX    2
#define LOCK_NB    4

#define LOCK_UN    8

struct vfs_flock {
	list_t *blocked;
	list_t *wait;
	int type;
	fs_node_t *file;
};

static list_t *flock_list;
static spin_lock_t flock_list_lock = { 0 };

static int flock_make_lock(fs_node_t *node, struct vfs_flock *caller,
			   unsigned int cmd);
static struct vfs_flock *flock_alloc_lock(struct vfs_flock *flock);
static int flock_lock_file(fs_node_t *node, struct vfs_flock *caller,
			   int wait);
static void flock_delete_lock(node_t *node, list_t *list, int wait);

static inline int
flock_locks_conflict(struct vfs_flock *caller, struct vfs_flock *sys) {
	return (caller->type == F_WRLCK) ||
		(caller->type == F_RDLCK && sys->type == F_WRLCK);
}

static int flock_make_lock(fs_node_t *node, struct vfs_flock *caller,
			   unsigned int cmd) {
	switch (cmd & ~LOCK_NB) {
	case LOCK_SH:
		caller->type = F_RDLCK;
		break;
	case LOCK_EX:
		caller->type = F_WRLCK;
		break;
	case LOCK_UN:
		caller->type = F_UNLCK;
		break;
	default:
			return 0;
	}
	caller->blocked = list_create();
	caller->wait = list_create();
	caller->file = node;

	return 1;
}

static struct vfs_flock *flock_alloc_lock(struct vfs_flock *flock) {
	struct vfs_flock *tmp;

	if ((tmp = (struct vfs_flock *)malloc(sizeof(struct vfs_flock)))
					       == NULL)
	    return tmp;

	tmp->blocked = flock->blocked;
	tmp->wait = flock->wait;
	tmp->type = flock->type;
	tmp->file = flock->file;

	return tmp;
}

static void flock_free_lock_elt(struct vfs_flock *flock) {
	list_free(flock->blocked);
	list_free(flock->wait);
}

static void flock_free_lock(struct vfs_flock *flock) {
	flock_free_lock_elt(flock);
	free(flock);
}

static int flock_lock_file(fs_node_t *file, struct vfs_flock *caller,
			   int wait) {
/*	list_t *locks_list = node->locks;*/
	struct vfs_flock *lock, *new_lock;

	if (!flock_list) {
		flock_free_lock_elt(caller);
		return -1;
	}

	/* XXX identify flock change */

	spin_lock(flock_list_lock);
	foreach(node, flock_list) {
		lock = ((struct vfs_flock *)node->value);
		if (lock->file == file) {
			if (caller->type == F_UNLCK) {
				/* delete this lock */
				flock_delete_lock(node, flock_list, 0);
			} else {
				/* doesn't not manage type change yet */
				spin_unlock(flock_list_lock);
				flock_free_lock_elt(caller);
				return 0;
			}
		}
	}
	spin_unlock(flock_list_lock);

	if (caller->type == F_UNLCK) {
		return 0;
	}

	if ((new_lock = flock_alloc_lock(caller)) == NULL) {
		flock_free_lock_elt(caller);
		debug_print(WARNING, "error alloc");
		return -1;
	}

	flock_free_lock_elt(caller);

	spin_lock(flock_list_lock);
	foreach(node, flock_list) {
		lock = ((struct vfs_flock *)node->value);
		/* looking for conflicts */
		if ((strcmp(lock->file->name,new_lock->file->name) == 0) && flock_locks_conflict(new_lock, lock)) {
			/* XXX manage waiting */
//			if (!wait) {
				/* free lock */
			        spin_unlock(flock_list_lock);
				flock_free_lock(new_lock);
				debug_print(WARNING, "Conflict...");
				return -1;
//			}

		}
	}

	/* insert new_lock */
	debug_print(WARNING, "attempt to insert in list");
	list_insert(flock_list, new_lock);
	spin_unlock(flock_list_lock);

	return 0;
}

static void flock_delete_lock(node_t *node, list_t *list, int wait) {
	struct vfs_flock *flock = ((struct vfs_flock *)node->value);
/*	struct vfs_flock *blocked_flock;*/
	list_delete(list, node);

	/* XXX what do we do with flock->flocked */
	#if 0
	foreach(node_, flock->blocked) {
		blocked_flock = ((struct vfs_flock *)node_->value);
		wakeup_queue(blocked_flock->wait);
	}
	#endif

	list_free(flock->blocked);
	list_free(flock->wait);
	free(flock);
}

int do_flock(fs_node_t *node, unsigned int cmd) {
	struct vfs_flock file_lock;
	int wait = 0;

	if (!flock_make_lock(node, &file_lock, cmd)) {
		return -1;
	}

	/* XXX: manage mode case */

	/* Only *flock(fd, LOCK_EX)*flock(fd, LOCK_SH)* calls needs wait flag */
	if (!((cmd & LOCK_UN) || (cmd & LOCK_NB))) {
		wait = 1;
	}

	return flock_lock_file(node, &file_lock, wait);
}

void flock_install(void) {
	flock_list = list_create();
}
