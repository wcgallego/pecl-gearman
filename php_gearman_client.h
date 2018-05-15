/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke <contact@jamesluedke.com>,
 *			Eric Day <eday@oddments.org>
 * All rights reserved.
 *
 * Use and distribution licensed under the PHP license.  See
 * the LICENSE file in this directory for full text.
 */

#ifndef __PHP_GEARMAN_CLIENT_H
#define __PHP_GEARMAN_CLIENT_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_gearman.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>

extern zend_class_entry *gearman_client_ce;
extern zend_object_handlers gearman_client_obj_handlers;

zend_object *gearman_client_obj_new(zend_class_entry *ce);

typedef enum {
	GEARMAN_CLIENT_OBJ_CREATED = (1 << 0)
} gearman_client_obj_flags_t;

typedef struct {
	gearman_return_t ret;
	gearman_client_obj_flags_t flags;
	gearman_client_st client;
	/* used for keeping track of task interface callbacks */
	zval zworkload_fn;
	zval zcreated_fn;
	zval zdata_fn;
	zval zwarning_fn;
	zval zstatus_fn;
	zval zcomplete_fn;
	zval zexception_fn;
	zval zfail_fn;

	zend_ulong created_tasks;
	zval task_list;

	zend_object std;
} gearman_client_obj;

gearman_client_obj *gearman_client_fetch_object(zend_object *obj);

#define Z_GEARMAN_CLIENT_P(zv) gearman_client_fetch_object(Z_OBJ_P((zv)))

/* NOTE: It seems kinda weird that GEARMAN_WORK_FAIL is a valid
 * return code, however it is required for a worker to pass status
 * back to the client about a failed job, other return codes can
 * be passed back but they will cause a docref Warning. Might
 * want to think of a better solution XXX */
#define PHP_GEARMAN_CLIENT_RET_OK(__ret) ((__ret) == GEARMAN_SUCCESS || \
					  (__ret) == GEARMAN_PAUSE || \
					  (__ret) == GEARMAN_IO_WAIT || \
					  (__ret) == GEARMAN_WORK_STATUS || \
					  (__ret) == GEARMAN_WORK_DATA || \
					  (__ret) == GEARMAN_WORK_EXCEPTION || \
					  (__ret) == GEARMAN_WORK_WARNING || \
					  (__ret) == GEARMAN_WORK_FAIL)

PHP_FUNCTION(gearman_client_create);
PHP_METHOD(GearmanClient, __construct);
PHP_METHOD(GearmanClient, __destruct);
PHP_FUNCTION(gearman_client_return_code);
PHP_FUNCTION(gearman_client_error);
PHP_FUNCTION(gearman_client_get_errno);
PHP_FUNCTION(gearman_client_options);
PHP_FUNCTION(gearman_client_set_options);
PHP_FUNCTION(gearman_client_add_options);
PHP_FUNCTION(gearman_client_remove_options);
PHP_FUNCTION(gearman_client_timeout);
PHP_FUNCTION(gearman_client_set_timeout);
PHP_FUNCTION(gearman_client_add_server);
PHP_FUNCTION(gearman_client_add_servers);
PHP_FUNCTION(gearman_client_wait);
PHP_FUNCTION(gearman_client_do_normal);
PHP_FUNCTION(gearman_client_do_high);
PHP_FUNCTION(gearman_client_do_low);
PHP_FUNCTION(gearman_client_do_background);
PHP_FUNCTION(gearman_client_do_high_background);
PHP_FUNCTION(gearman_client_do_low_background);
PHP_FUNCTION(gearman_client_do_job_handle);
PHP_FUNCTION(gearman_client_do_status);
PHP_FUNCTION(gearman_client_job_status);
PHP_FUNCTION(gearman_client_job_status_by_unique_key);
PHP_FUNCTION(gearman_client_ping);
PHP_FUNCTION(gearman_client_add_task);
PHP_FUNCTION(gearman_client_add_task_high);
PHP_FUNCTION(gearman_client_add_task_low);
PHP_FUNCTION(gearman_client_add_task_background);
PHP_FUNCTION(gearman_client_add_task_high_background);
PHP_FUNCTION(gearman_client_add_task_low_background);
PHP_FUNCTION(gearman_client_run_tasks);
PHP_FUNCTION(gearman_client_add_task_status);
PHP_FUNCTION(gearman_client_set_workload_callback);
PHP_FUNCTION(gearman_client_set_created_callback);
PHP_FUNCTION(gearman_client_set_data_callback);
PHP_FUNCTION(gearman_client_set_warning_callback);
PHP_FUNCTION(gearman_client_set_status_callback);
PHP_FUNCTION(gearman_client_set_complete_callback);
PHP_FUNCTION(gearman_client_set_exception_callback);
PHP_FUNCTION(gearman_client_set_fail_callback);
PHP_FUNCTION(gearman_client_clear_callbacks);
PHP_FUNCTION(gearman_client_context);
PHP_FUNCTION(gearman_client_set_context);
PHP_FUNCTION(gearman_client_enable_exception_handler);

#endif  /* __PHP_GEARMAN_CLIENT_H */
