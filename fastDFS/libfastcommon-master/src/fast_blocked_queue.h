/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//fast_blocked_queue.h

#ifndef _FAST_BLOCKED_QUEUE_H
#define _FAST_BLOCKED_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common_define.h"
#include "fast_task_queue.h"

struct fast_blocked_queue
{
	struct fast_task_info *head;
	struct fast_task_info *tail;
	pthread_mutex_t lock;
	pthread_cond_t cond;
};

#ifdef __cplusplus
extern "C" {
#endif

int blocked_queue_init(struct fast_blocked_queue *pQueue);
void blocked_queue_destroy(struct fast_blocked_queue *pQueue);

static inline void blocked_queue_terminate(struct fast_blocked_queue *pQueue)
{
     pthread_cond_signal(&(pQueue->cond));
}

int blocked_queue_push(struct fast_blocked_queue *pQueue,
		struct fast_task_info *pTask);
struct fast_task_info *blocked_queue_pop(struct fast_blocked_queue *pQueue);

#ifdef __cplusplus
}
#endif

#endif

