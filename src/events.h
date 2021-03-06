/*
  Copyright (C) 2006, 2007, 2008, 2009, 2010  Anthony Catel <a.catel@weelya.com>

  This file is part of APE Server.
  APE is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  APE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with APE ; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* events.h */

#ifndef _EVENTS_H
#define _EVENTS_H

#include "utils.h"
#include "main.h"

#include "configure.h"

#ifdef USE_KQUEUE_HANDLER
#include <sys/event.h>
#endif
#ifdef USE_EPOLL_HANDLER
#include <sys/epoll.h>
#endif
#ifdef USE_SELECT_HANDLER
#include <sys/time.h>
#define MAX_SELECT_FDS	1024
#endif

/* Generics flags */
#define EVENT_READ 0x01
#define EVENT_WRITE 0x02

/* Events handler */
typedef enum {
	EVENT_UNKNOWN,
	EVENT_EPOLL, 	/* Linux */
	EVENT_KQUEUE, 	/* BSD */
	EVENT_DEVPOLL,	/* Solaris */
	EVENT_POLL,	/* POSIX */
	EVENT_SELECT	/* Generic */
} fdevent_handler_t;

#ifdef USE_SELECT_HANDLER
typedef struct {
  int  fd;
  char read:4;		/* bitmask */
  char write:4;		/* bitmask */
} select_fd_t;
#endif

struct _fdevent {
	/* Common values */
	int *basemem;
	/* Interface */
	int (*add)(struct _fdevent *, int, int);
	int (*remove)(struct _fdevent *, int);
	int (*poll)(struct _fdevent *, int);
	int (*get_current_fd)(struct _fdevent *, int);
	void (*growup)(struct _fdevent *);
	int (*revent)(struct _fdevent *, int);
	int (*reload)(struct _fdevent *);
	
	/* Specifics values */
	#ifdef USE_KQUEUE_HANDLER
	struct kevent *events;
	int kq_fd;
	#endif
	#ifdef USE_EPOLL_HANDLER
	struct epoll_event *events;
	int epoll_fd;
	#endif
        #ifdef USE_SELECT_HANDLER
        select_fd_t fds[MAX_SELECT_FDS];
        select_fd_t **events; /* Pointers into fds */
        #endif

	fdevent_handler_t handler;
};

int events_init(acetables *g_ape, int *basemem);
void events_free(acetables *g_ape);
int events_add(struct _fdevent *ev, int fd, int bitadd);
int events_remove(struct _fdevent *ev, int fd);
int events_poll(struct _fdevent *ev, int timeout_ms);
int events_get_current_fd(struct _fdevent *ev, int i);
void events_growup(struct _fdevent *ev);
int events_revent(struct _fdevent *ev, int i);
int events_reload(struct _fdevent *ev);

int event_kqueue_init(struct _fdevent *ev);
int event_epoll_init(struct _fdevent *ev);
int event_select_init(struct _fdevent *ev);

#ifndef USE_SELECT_HANDLER
static inline ssize_t event_write(struct _fdevent *ev, int fd, const void *buf, size_t nbyte)
{
  return write(fd, buf, nbyte);
}
#endif
#endif
