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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_gearman.h"
#include "php_gearman_client.h"
#include "php_gearman_task.h"
#include "php_gearman_worker.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>

zend_class_entry *gearman_exception_ce;
zend_class_entry *gearman_client_ce;
zend_object_handlers gearman_client_obj_handlers;
zend_class_entry *gearman_task_ce;
zend_object_handlers gearman_task_obj_handlers;
zend_class_entry *gearman_job_ce;
zend_object_handlers gearman_job_obj_handlers;
zend_class_entry *gearman_worker_ce;
zend_object_handlers gearman_worker_obj_handlers;


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_version, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_bugreport, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_verbose_name, 0, 0, 1)
	ZEND_ARG_INFO(0, verbose)
ZEND_END_ARG_INFO()

/*
 * Gearman Task arginfo
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_return_code, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_return_code, 0, 0, 0)
ZEND_END_ARG_INFO()

/* TODO: so looks like I may have implemented this incorrectly for
 * now no oo interface exist. I will need to come back to this later */
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_context, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_function_name, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_function_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_unique, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_unique, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_job_handle, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_job_handle, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_is_known, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_is_known, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_is_running, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_is_running, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_numerator, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_numerator, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_denominator, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_denominator, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_data, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_data, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_data_size, 0, 0, 1)
	ZEND_ARG_INFO(0, task_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_data_size, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_send_workload, 0, 0, 2)
	ZEND_ARG_INFO(0, task_object)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_send_workload, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_task_recv_data, 0, 0, 2)
	ZEND_ARG_INFO(0, task_object)
	ZEND_ARG_INFO(0, data_len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_recv_data, 0, 0, 1)
	ZEND_ARG_INFO(0, data_len)
ZEND_END_ARG_INFO()


/*
 * Gearman Job Functions
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_return_code, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_return_code, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_data, 0, 0, 2)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_data, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_warning, 0, 0, 2)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, warning)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_warning, 0, 0, 1)
	ZEND_ARG_INFO(0, warning)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_status, 0, 0, 3)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, numerator)
	ZEND_ARG_INFO(0, denominator)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_status, 0, 0, 2)
	ZEND_ARG_INFO(0, numerator)
	ZEND_ARG_INFO(0, denominator)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_complete, 0, 0, 2)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_complete, 0, 0, 1)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_exception, 0, 0, 2)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, exception)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_exception, 0, 0, 1)
	ZEND_ARG_INFO(0, exception)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_send_fail, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_send_fail, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_handle, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_handle, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_unique, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_unique, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_function_name, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_function_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_workload, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_workload, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_workload_size, 0, 0, 1)
	ZEND_ARG_INFO(0, job_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_workload_size, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_job_set_return, 0, 0, 2)
	ZEND_ARG_INFO(0, job_object)
	ZEND_ARG_INFO(0, gearman_return_t)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_job_set_return, 0, 0, 1)
	ZEND_ARG_INFO(0, gearman_return_t)
ZEND_END_ARG_INFO()

/*
 * Gearman Client arginfo
 */

// Procedural function for creating a GearmanClient object
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_create, 0, 0, 0)
ZEND_END_ARG_INFO()

// Objected Oriented method for creating a GearmanClient object
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_return_code, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_return_code, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_error, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_error, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_get_errno, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_get_errno, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_options, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_options, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_options, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_options, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_remove_options, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_remove_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_timeout, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_timeout, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_timeout, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_timeout, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_server, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, enableExceptionHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_server, 0, 0, 0)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, enableExceptionHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_servers, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, servers)
	ZEND_ARG_INFO(0, setupExceptionHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_servers, 0, 0, 0)
	ZEND_ARG_INFO(0, servers)
	ZEND_ARG_INFO(0, setupExceptionHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_wait, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_wait, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_normal, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_normal, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_high, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_high, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_low, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_low, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_high_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_high_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_low_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_low_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_job_handle, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_job_handle, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_do_status, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_do_status, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_job_status, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, job_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_job_status, 0, 0, 1)
	ZEND_ARG_INFO(0, job_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_job_status_by_unique_key, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, unique_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_job_status_by_unique_key, 0, 0, 1)
	ZEND_ARG_INFO(0, unique_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_ping, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, workload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_ping, 0, 0, 1)
	ZEND_ARG_INFO(0, workload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_high, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_high, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_low, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_low, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_high_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_high_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_low_background, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_low_background, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, workload)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, unique)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_task_status, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, job_handle)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_task_status, 0, 0, 1)
	ZEND_ARG_INFO(0, job_handle)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_workload_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_workload_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_created_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_created_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_data_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_data_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_warning_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_warning_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_status_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_status_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_complete_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_complete_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_exception_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_exception_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_fail_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_fail_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_clear_callbacks, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_clear_callbacks, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_context, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_context, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_set_context, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_set_context, 0, 0, 1)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_enable_exception_handler, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_enable_exception_handler, 0, 0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_run_tasks, 0, 0, 1)
	ZEND_ARG_INFO(0, client_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_run_tasks, 0, 0, 0)
ZEND_END_ARG_INFO()

/*
 * Gearman Worker arginfo
 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_return_code, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_return_code, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_error, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_error, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_errno, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_errno, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_options, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_options, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_set_options, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_set_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_add_options, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_add_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_remove_options, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_remove_options, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_timeout, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_timeout, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_set_timeout, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_set_timeout, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_set_id, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_set_id, 0, 0, 1)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_add_server, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_add_server, 0, 0, 0)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_add_servers, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, servers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_add_servers, 0, 0, 1)
	ZEND_ARG_INFO(0, servers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_wait, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_wait, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_register, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_register, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_unregister, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_unregister, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_unregister_all, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_unregister_all, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_grab_job, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_grab_job, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_add_function, 0, 0, 3)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_add_function, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_work, 0, 0, 1)
	ZEND_ARG_INFO(0, worker_object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_work, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_worker_ping, 0, 0, 2)
	ZEND_ARG_INFO(0, worker_object)
	ZEND_ARG_INFO(0, workload)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_worker_ping, 0, 0, 1)
	ZEND_ARG_INFO(0, workload)
ZEND_END_ARG_INFO()


/* }}} end arginfo */

/* Custom malloc and free calls to avoid excessive buffer copies. */
void *_php_malloc(size_t size, void *arg) {
	uint8_t *ret;
	ret = emalloc(size+1);
	ret[size]= 0;
	return ret;
}

void _php_free(void *ptr, void *arg) {
	efree(ptr);
}

/*
 * Functions from gearman.h
 */

/* {{{ proto string gearman_version()
   Returns libgearman version */
PHP_FUNCTION(gearman_version) {
	RETURN_STRING((char *)gearman_version());
}
/* }}} */

/* {{{ proto string gearman_bugreport()
   Returns bug report URL string */
PHP_FUNCTION(gearman_bugreport) {
	RETURN_STRING((char *)gearman_bugreport());
}
/* }}} */

/* {{{ proto string gearman_verbose_name(constant verbose)
   Returns string with the name of the given verbose level */
PHP_FUNCTION(gearman_verbose_name) {
	zend_long verbose;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l",
		&verbose) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Unable to parse parameters.");
		RETURN_NULL();
	}

	if (verbose < 0) {
		php_error_docref(NULL, E_WARNING, "Input must be an integer greater than 0.");
		RETURN_NULL();
	}

	RETURN_STRING((char *)gearman_verbose_name(verbose));
}
/* }}} */

/* Function list. */
zend_function_entry gearman_functions[] = {
	/* Functions from gearman.h */
	PHP_FE(gearman_version, arginfo_gearman_version)
	PHP_FE(gearman_bugreport, arginfo_gearman_bugreport)
	PHP_FE(gearman_verbose_name, arginfo_gearman_verbose_name)

	/* Functions from client.h */
	PHP_FE(gearman_client_create, arginfo_gearman_client_create)
	PHP_FE(gearman_client_return_code, arginfo_gearman_client_return_code)
	PHP_FE(gearman_client_error, arginfo_gearman_client_error)
	PHP_FE(gearman_client_get_errno, arginfo_gearman_client_get_errno)
	PHP_FE(gearman_client_options, arginfo_gearman_client_options)
	PHP_FE(gearman_client_set_options, arginfo_gearman_client_set_options)
	PHP_FE(gearman_client_add_options, arginfo_gearman_client_add_options)
	PHP_FE(gearman_client_remove_options, arginfo_gearman_client_remove_options)
	PHP_FE(gearman_client_timeout, arginfo_gearman_client_timeout)
	PHP_FE(gearman_client_set_timeout, arginfo_gearman_client_set_timeout)
	PHP_FE(gearman_client_context, arginfo_gearman_client_context)
	PHP_FE(gearman_client_set_context, arginfo_gearman_client_set_context)
	PHP_FE(gearman_client_add_server, arginfo_gearman_client_add_server)
	PHP_FE(gearman_client_add_servers, arginfo_gearman_client_add_servers)

	PHP_FE(gearman_client_wait, arginfo_gearman_client_wait)
	PHP_FE(gearman_client_do_normal, arginfo_gearman_client_do_normal)
	PHP_FE(gearman_client_do_high, arginfo_gearman_client_do_high)
	PHP_FE(gearman_client_do_low, arginfo_gearman_client_do_low)
	PHP_FE(gearman_client_do_job_handle, arginfo_gearman_client_do_job_handle)
	PHP_FE(gearman_client_do_status, arginfo_gearman_client_do_status)
	PHP_FE(gearman_client_do_background, arginfo_gearman_client_do_background)
	PHP_FE(gearman_client_do_high_background, arginfo_gearman_client_do_high_background)
	PHP_FE(gearman_client_do_low_background, arginfo_gearman_client_do_low_background)
	PHP_FE(gearman_client_job_status, arginfo_gearman_client_job_status)
	PHP_FE(gearman_client_job_status_by_unique_key, arginfo_gearman_client_job_status_by_unique_key)
	PHP_FE(gearman_client_ping, arginfo_gearman_client_ping)
	PHP_FE(gearman_client_add_task, arginfo_gearman_client_add_task)
	PHP_FE(gearman_client_add_task_high, arginfo_gearman_client_add_task_high)
	PHP_FE(gearman_client_add_task_low, arginfo_gearman_client_add_task_low)
	PHP_FE(gearman_client_add_task_background, arginfo_gearman_client_add_task_background)
	PHP_FE(gearman_client_add_task_high_background, arginfo_gearman_client_add_task_high_background)
	PHP_FE(gearman_client_add_task_low_background, arginfo_gearman_client_add_task_low_background)
	PHP_FE(gearman_client_add_task_status, arginfo_gearman_client_add_task_status)
	PHP_FE(gearman_client_set_workload_callback, arginfo_gearman_client_set_workload_callback)
	PHP_FE(gearman_client_set_created_callback, arginfo_gearman_client_set_created_callback)
	PHP_FE(gearman_client_set_data_callback, arginfo_gearman_client_set_data_callback)
	PHP_FE(gearman_client_set_warning_callback, arginfo_gearman_client_set_warning_callback)
	PHP_FE(gearman_client_set_status_callback, arginfo_gearman_client_set_status_callback)
	PHP_FE(gearman_client_set_complete_callback, arginfo_gearman_client_set_complete_callback)
	PHP_FE(gearman_client_set_exception_callback, arginfo_gearman_client_set_exception_callback)
	PHP_FE(gearman_client_set_fail_callback, arginfo_gearman_client_set_fail_callback)
	PHP_FE(gearman_client_clear_callbacks, arginfo_gearman_client_clear_callbacks)
	PHP_FE(gearman_client_run_tasks, arginfo_gearman_client_run_tasks)

	/* Functions from task.h */
	PHP_FE(gearman_task_return_code, arginfo_gearman_task_return_code)
	PHP_FE(gearman_task_function_name, arginfo_gearman_task_function_name)
	PHP_FE(gearman_task_unique, arginfo_gearman_task_unique)
	PHP_FE(gearman_task_job_handle, arginfo_gearman_task_job_handle)
	PHP_FE(gearman_task_is_known, arginfo_gearman_task_is_known)
	PHP_FE(gearman_task_is_running, arginfo_gearman_task_is_running)
	PHP_FE(gearman_task_numerator, arginfo_gearman_task_numerator)
	PHP_FE(gearman_task_denominator, arginfo_gearman_task_denominator)
	PHP_FE(gearman_task_data, arginfo_gearman_task_data)
	PHP_FE(gearman_task_data_size, arginfo_gearman_task_data_size)
	PHP_FE(gearman_task_send_workload, arginfo_gearman_task_send_workload)
	PHP_FE(gearman_task_recv_data, arginfo_gearman_task_recv_data)

	/* Functions from worker.h */
	PHP_FE(gearman_worker_return_code, arginfo_gearman_worker_return_code)
	PHP_FE(gearman_worker_create, arginfo_gearman_worker_create)
	PHP_FE(gearman_worker_error, arginfo_gearman_worker_error)
	PHP_FE(gearman_worker_errno, arginfo_gearman_worker_errno)
	PHP_FE(gearman_worker_options, arginfo_gearman_worker_options)
	PHP_FE(gearman_worker_set_options, arginfo_gearman_worker_set_options)
	PHP_FE(gearman_worker_add_options, arginfo_gearman_worker_add_options)
	PHP_FE(gearman_worker_remove_options, arginfo_gearman_worker_remove_options)
	PHP_FE(gearman_worker_timeout, arginfo_gearman_worker_timeout)
	PHP_FE(gearman_worker_set_timeout, arginfo_gearman_worker_set_timeout)
	PHP_FE(gearman_worker_set_id, arginfo_gearman_worker_set_id)
	PHP_FE(gearman_worker_add_server, arginfo_gearman_worker_add_server)
	PHP_FE(gearman_worker_add_servers, arginfo_gearman_worker_add_servers)
	PHP_FE(gearman_worker_wait, arginfo_gearman_worker_wait)
	PHP_FE(gearman_worker_register, arginfo_gearman_worker_register)
	PHP_FE(gearman_worker_unregister, arginfo_gearman_worker_unregister)
	PHP_FE(gearman_worker_unregister_all, arginfo_gearman_worker_unregister_all)
	PHP_FE(gearman_worker_grab_job, arginfo_gearman_worker_grab_job)
	PHP_FE(gearman_worker_add_function, arginfo_gearman_worker_add_function)
	PHP_FE(gearman_worker_work, arginfo_gearman_worker_work)
	PHP_FE(gearman_worker_ping, arginfo_gearman_worker_ping)

	/* Functions from job.h */
	PHP_FE(gearman_job_return_code, arginfo_gearman_job_return_code)
	PHP_FE(gearman_job_set_return, arginfo_gearman_job_set_return)
	PHP_FE(gearman_job_send_data, arginfo_gearman_job_send_data)
	PHP_FE(gearman_job_send_warning, arginfo_gearman_job_send_warning)
	PHP_FE(gearman_job_send_status, arginfo_gearman_job_send_status)
	PHP_FE(gearman_job_send_complete, arginfo_gearman_job_send_complete)
	PHP_FE(gearman_job_send_exception, arginfo_gearman_job_send_exception)
	PHP_FE(gearman_job_send_fail, arginfo_gearman_job_send_fail)
	PHP_FE(gearman_job_handle, arginfo_gearman_job_handle)
	PHP_FE(gearman_job_function_name, arginfo_gearman_job_function_name)
	PHP_FE(gearman_job_unique, arginfo_gearman_job_unique)
	PHP_FE(gearman_job_workload, arginfo_gearman_job_workload)
	PHP_FE(gearman_job_workload_size, arginfo_gearman_job_workload_size)

	ZEND_FE_END
};

zend_function_entry gearman_methods[]= {
	ZEND_FE_END
};

static zend_function_entry gearman_client_methods[]= {
	PHP_ME(GearmanClient, __construct, arginfo_gearman_client_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(GearmanClient, __destruct, arginfo_oo_gearman_client_destruct, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_client_return_code, arginfo_oo_gearman_client_return_code, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(error, gearman_client_error, arginfo_oo_gearman_client_error, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrno, gearman_client_get_errno, arginfo_oo_gearman_client_get_errno, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(options, gearman_client_options, arginfo_oo_gearman_client_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setOptions, gearman_client_set_options, arginfo_oo_gearman_client_set_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addOptions, gearman_client_add_options, arginfo_oo_gearman_client_add_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(removeOptions, gearman_client_remove_options, arginfo_oo_gearman_client_remove_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(timeout, gearman_client_timeout, arginfo_oo_gearman_client_timeout, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setTimeout, gearman_client_set_timeout, arginfo_oo_gearman_client_set_timeout, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(context, gearman_client_context, arginfo_oo_gearman_client_context, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setContext, gearman_client_set_context, arginfo_oo_gearman_client_set_context, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addServer, gearman_client_add_server, arginfo_oo_gearman_client_add_server, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addServers, gearman_client_add_servers, arginfo_oo_gearman_client_add_servers, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(wait, gearman_client_wait, arginfo_oo_gearman_client_wait, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doNormal, gearman_client_do_normal, arginfo_oo_gearman_client_do_normal, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doHigh, gearman_client_do_high, arginfo_oo_gearman_client_do_high, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doLow, gearman_client_do_low, arginfo_oo_gearman_client_do_low, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doBackground, gearman_client_do_background, arginfo_oo_gearman_client_do_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doHighBackground, gearman_client_do_high_background, arginfo_oo_gearman_client_do_high_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doLowBackground, gearman_client_do_low_background, arginfo_oo_gearman_client_do_low_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doJobHandle, gearman_client_do_job_handle, arginfo_oo_gearman_client_do_job_handle, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(doStatus, gearman_client_do_status, arginfo_oo_gearman_client_do_status, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(jobStatus, gearman_client_job_status, arginfo_oo_gearman_client_job_status, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(jobStatusByUniqueKey, gearman_client_job_status_by_unique_key, arginfo_oo_gearman_client_job_status_by_unique_key, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(ping, gearman_client_ping, arginfo_oo_gearman_client_ping, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTask, gearman_client_add_task, arginfo_oo_gearman_client_add_task, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskHigh, gearman_client_add_task_high, arginfo_oo_gearman_client_add_task_high, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskLow, gearman_client_add_task_low, arginfo_oo_gearman_client_add_task_low, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskBackground, gearman_client_add_task_background, arginfo_oo_gearman_client_add_task_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskHighBackground, gearman_client_add_task_high_background, arginfo_oo_gearman_client_add_task_high_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskLowBackground, gearman_client_add_task_low_background, arginfo_oo_gearman_client_add_task_low_background, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addTaskStatus, gearman_client_add_task_status, arginfo_oo_gearman_client_add_task_status, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setWorkloadCallback, gearman_client_set_workload_callback, arginfo_oo_gearman_client_set_workload_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setCreatedCallback, gearman_client_set_created_callback, arginfo_oo_gearman_client_set_created_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setDataCallback, gearman_client_set_data_callback, arginfo_oo_gearman_client_set_data_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setWarningCallback, gearman_client_set_warning_callback, arginfo_oo_gearman_client_set_warning_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setStatusCallback, gearman_client_set_status_callback, arginfo_oo_gearman_client_set_status_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setCompleteCallback, gearman_client_set_complete_callback, arginfo_oo_gearman_client_set_complete_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setExceptionCallback, gearman_client_set_exception_callback, arginfo_oo_gearman_client_set_exception_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setFailCallback, gearman_client_set_fail_callback, arginfo_oo_gearman_client_set_fail_callback, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(clearCallbacks, gearman_client_clear_callbacks, arginfo_oo_gearman_client_clear_callbacks, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(runTasks, gearman_client_run_tasks, arginfo_oo_gearman_client_run_tasks, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(enableExceptionHandler,
			gearman_client_enable_exception_handler,
			arginfo_oo_gearman_client_enable_exception_handler,
			ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_function_entry gearman_task_methods[]= {
	PHP_ME(GearmanTask, __construct, arginfo_oo_gearman_task_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(GearmanTask, __destruct, arginfo_oo_gearman_task_destruct, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_task_return_code, arginfo_oo_gearman_task_return_code, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(functionName, gearman_task_function_name, arginfo_oo_gearman_task_function_name, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(unique, gearman_task_unique, arginfo_oo_gearman_task_unique, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(jobHandle, gearman_task_job_handle, arginfo_oo_gearman_task_job_handle, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isKnown, gearman_task_is_known, arginfo_oo_gearman_task_is_known, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(isRunning, gearman_task_is_running, arginfo_oo_gearman_task_is_running, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(taskNumerator, gearman_task_numerator, arginfo_oo_gearman_task_numerator, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(taskDenominator, gearman_task_denominator, arginfo_oo_gearman_task_denominator, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendWorkload, gearman_task_send_workload, arginfo_oo_gearman_task_send_workload, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(data, gearman_task_data, arginfo_oo_gearman_task_data, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(dataSize, gearman_task_data_size, arginfo_oo_gearman_task_data_size, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(recvData, gearman_task_recv_data, arginfo_oo_gearman_task_recv_data, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_function_entry gearman_worker_methods[]= {
	PHP_ME(GearmanWorker, __construct, arginfo_oo_gearman_worker_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(GearmanWorker, __destruct, arginfo_oo_gearman_worker_destruct, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_worker_return_code, arginfo_oo_gearman_worker_return_code, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(error, gearman_worker_error, arginfo_oo_gearman_worker_error, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(getErrno, gearman_worker_errno, arginfo_oo_gearman_worker_errno, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(options, gearman_worker_options, arginfo_oo_gearman_worker_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setOptions, gearman_worker_set_options, arginfo_oo_gearman_worker_set_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addOptions, gearman_worker_add_options, arginfo_oo_gearman_worker_add_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(removeOptions, gearman_worker_remove_options, arginfo_oo_gearman_worker_remove_options, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(timeout, gearman_worker_timeout, arginfo_oo_gearman_worker_timeout, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setTimeout, gearman_worker_set_timeout, arginfo_oo_gearman_worker_set_timeout, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setId, gearman_worker_set_id, arginfo_oo_gearman_worker_set_id, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addServer, gearman_worker_add_server, arginfo_oo_gearman_worker_add_server, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addServers, gearman_worker_add_servers, arginfo_oo_gearman_worker_add_servers, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(wait, gearman_worker_wait, arginfo_oo_gearman_worker_wait, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(register, gearman_worker_register, arginfo_oo_gearman_worker_register, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(unregister, gearman_worker_unregister, arginfo_oo_gearman_worker_unregister, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(unregisterAll, gearman_worker_unregister_all, arginfo_oo_gearman_worker_unregister_all, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(grabJob, gearman_worker_grab_job, arginfo_oo_gearman_worker_grab_job, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(addFunction, gearman_worker_add_function, arginfo_oo_gearman_worker_add_function, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(work, gearman_worker_work, arginfo_oo_gearman_worker_work, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(echo, gearman_worker_ping, arginfo_oo_gearman_worker_ping, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_function_entry gearman_job_methods[]= {
	PHP_ME(GearmanJob, __destruct, arginfo_oo_gearman_job_destruct, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_job_return_code, arginfo_oo_gearman_job_return_code, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setReturn, gearman_job_set_return, arginfo_oo_gearman_job_set_return, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendData, gearman_job_send_data, arginfo_oo_gearman_job_send_data, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendWarning, gearman_job_send_warning, arginfo_oo_gearman_job_send_warning, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendStatus, gearman_job_send_status, arginfo_oo_gearman_job_send_status, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendComplete, gearman_job_send_complete, arginfo_oo_gearman_job_send_complete, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendException, gearman_job_send_exception, arginfo_oo_gearman_job_send_exception, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(sendFail, gearman_job_send_fail, arginfo_oo_gearman_job_send_fail, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(handle, gearman_job_handle, arginfo_oo_gearman_job_handle, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(functionName, gearman_job_function_name, arginfo_oo_gearman_job_function_name, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(unique, gearman_job_unique, arginfo_oo_gearman_job_unique, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(workload, gearman_job_workload, arginfo_oo_gearman_job_workload, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(workloadSize, gearman_job_workload_size, arginfo_oo_gearman_job_workload_size, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

zend_function_entry gearman_exception_methods[] = {
	ZEND_FE_END
};

PHP_MINIT_FUNCTION(gearman) {
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "GearmanClient", gearman_client_methods);
	gearman_client_ce = zend_register_internal_class(&ce);
	gearman_client_ce->create_object = gearman_client_obj_new;
	memcpy(&gearman_client_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_client_obj_handlers));
	gearman_client_obj_handlers.offset = XtOffsetOf(gearman_client_obj, std);

	INIT_CLASS_ENTRY(ce, "GearmanTask", gearman_task_methods);
	gearman_task_ce = zend_register_internal_class(&ce);
	gearman_task_ce->create_object = gearman_task_obj_new;
	memcpy(&gearman_task_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_task_obj_handlers));
	gearman_task_obj_handlers.offset = XtOffsetOf(gearman_task_obj, std);

	INIT_CLASS_ENTRY(ce, "GearmanWorker", gearman_worker_methods);
	gearman_worker_ce = zend_register_internal_class(&ce);
	gearman_worker_ce->create_object = gearman_worker_obj_new;
	memcpy(&gearman_worker_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_worker_obj_handlers));
	gearman_worker_obj_handlers.offset = XtOffsetOf(gearman_worker_obj, std);

	INIT_CLASS_ENTRY(ce, "GearmanJob", gearman_job_methods);
	gearman_job_ce = zend_register_internal_class(&ce);
	gearman_job_ce->create_object = gearman_job_obj_new;
	memcpy(&gearman_job_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_job_obj_handlers));
	gearman_job_obj_handlers.offset = XtOffsetOf(gearman_job_obj, std);

	/* XXX exception class */
	INIT_CLASS_ENTRY(ce, "GearmanException", gearman_exception_methods)
	gearman_exception_ce = zend_register_internal_class_ex(&ce, zend_exception_get_default());
	gearman_exception_ce->ce_flags |= ZEND_ACC_FINAL;
	zend_declare_property_long(gearman_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PUBLIC);

	/* These are automatically generated from gearman_constants.h using
	const_gen.sh. Do not remove the CONST_GEN_* comments, this is how the
	script locates the correct location to replace. */

	/* CONST_GEN_START */
	REGISTER_STRING_CONSTANT("GEARMAN_DEFAULT_TCP_HOST",
		GEARMAN_DEFAULT_TCP_HOST,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DEFAULT_TCP_PORT",
		GEARMAN_DEFAULT_TCP_PORT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DEFAULT_SOCKET_TIMEOUT",
		GEARMAN_DEFAULT_SOCKET_TIMEOUT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DEFAULT_SOCKET_SEND_SIZE",
		GEARMAN_DEFAULT_SOCKET_SEND_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DEFAULT_SOCKET_RECV_SIZE",
		GEARMAN_DEFAULT_SOCKET_RECV_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_MAX_ERROR_SIZE",
		GEARMAN_MAX_ERROR_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_PACKET_HEADER_SIZE",
		GEARMAN_PACKET_HEADER_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_HANDLE_SIZE",
		GEARMAN_JOB_HANDLE_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_OPTION_SIZE",
		GEARMAN_OPTION_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_UNIQUE_SIZE",
		GEARMAN_UNIQUE_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_MAX_COMMAND_ARGS",
		GEARMAN_MAX_COMMAND_ARGS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_ARGS_BUFFER_SIZE",
		GEARMAN_ARGS_BUFFER_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SEND_BUFFER_SIZE",
		GEARMAN_SEND_BUFFER_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_RECV_BUFFER_SIZE",
		GEARMAN_RECV_BUFFER_SIZE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_WAIT_TIMEOUT",
		GEARMAN_WORKER_WAIT_TIMEOUT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SUCCESS",
		GEARMAN_SUCCESS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_IO_WAIT",
		GEARMAN_IO_WAIT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SHUTDOWN",
		GEARMAN_SHUTDOWN,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SHUTDOWN_GRACEFUL",
		GEARMAN_SHUTDOWN_GRACEFUL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_ERRNO",
		GEARMAN_ERRNO,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_EVENT",
		GEARMAN_EVENT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TOO_MANY_ARGS",
		GEARMAN_TOO_MANY_ARGS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NO_ACTIVE_FDS",
		GEARMAN_NO_ACTIVE_FDS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_INVALID_MAGIC",
		GEARMAN_INVALID_MAGIC,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_INVALID_COMMAND",
		GEARMAN_INVALID_COMMAND,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_INVALID_PACKET",
		GEARMAN_INVALID_PACKET,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_UNEXPECTED_PACKET",
		GEARMAN_UNEXPECTED_PACKET,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_GETADDRINFO",
		GEARMAN_GETADDRINFO,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NO_SERVERS",
		GEARMAN_NO_SERVERS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_LOST_CONNECTION",
		GEARMAN_LOST_CONNECTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_MEMORY_ALLOCATION_FAILURE",
		GEARMAN_MEMORY_ALLOCATION_FAILURE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_EXISTS",
		GEARMAN_JOB_EXISTS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_QUEUE_FULL",
		GEARMAN_JOB_QUEUE_FULL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SERVER_ERROR",
		GEARMAN_SERVER_ERROR,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_ERROR",
		GEARMAN_WORK_ERROR,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_DATA",
		GEARMAN_WORK_DATA,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_WARNING",
		GEARMAN_WORK_WARNING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_STATUS",
		GEARMAN_WORK_STATUS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_EXCEPTION",
		GEARMAN_WORK_EXCEPTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORK_FAIL",
		GEARMAN_WORK_FAIL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NOT_CONNECTED",
		GEARMAN_NOT_CONNECTED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COULD_NOT_CONNECT",
		GEARMAN_COULD_NOT_CONNECT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SEND_IN_PROGRESS",
		GEARMAN_SEND_IN_PROGRESS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_RECV_IN_PROGRESS",
		GEARMAN_RECV_IN_PROGRESS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NOT_FLUSHING",
		GEARMAN_NOT_FLUSHING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DATA_TOO_LARGE",
		GEARMAN_DATA_TOO_LARGE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_INVALID_FUNCTION_NAME",
		GEARMAN_INVALID_FUNCTION_NAME,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_INVALID_WORKER_FUNCTION",
		GEARMAN_INVALID_WORKER_FUNCTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NO_REGISTERED_FUNCTIONS",
		GEARMAN_NO_REGISTERED_FUNCTIONS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NO_JOBS",
		GEARMAN_NO_JOBS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_ECHO_DATA_CORRUPTION",
		GEARMAN_ECHO_DATA_CORRUPTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NEED_WORKLOAD_FN",
		GEARMAN_NEED_WORKLOAD_FN,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_PAUSE",
		GEARMAN_PAUSE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_UNKNOWN_STATE",
		GEARMAN_UNKNOWN_STATE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_PTHREAD",
		GEARMAN_PTHREAD,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_PIPE_EOF",
		GEARMAN_PIPE_EOF,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_QUEUE_ERROR",
		GEARMAN_QUEUE_ERROR,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_FLUSH_DATA",
		GEARMAN_FLUSH_DATA,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_SEND_BUFFER_TOO_SMALL",
		GEARMAN_SEND_BUFFER_TOO_SMALL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_IGNORE_PACKET",
		GEARMAN_IGNORE_PACKET,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_UNKNOWN_OPTION",
		GEARMAN_UNKNOWN_OPTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TIMEOUT",
		GEARMAN_TIMEOUT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_MAX_RETURN",
		GEARMAN_MAX_RETURN,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_FATAL",
		GEARMAN_VERBOSE_FATAL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_ERROR",
		GEARMAN_VERBOSE_ERROR,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_INFO",
		GEARMAN_VERBOSE_INFO,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_DEBUG",
		GEARMAN_VERBOSE_DEBUG,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_CRAZY",
		GEARMAN_VERBOSE_CRAZY,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_MAX",
		GEARMAN_VERBOSE_MAX,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_NON_BLOCKING",
		GEARMAN_NON_BLOCKING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_DONT_TRACK_PACKETS",
		GEARMAN_DONT_TRACK_PACKETS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_READY",
		GEARMAN_CON_READY,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_PACKET_IN_USE",
		GEARMAN_CON_PACKET_IN_USE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_EXTERNAL_FD",
		GEARMAN_CON_EXTERNAL_FD,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_IGNORE_LOST_CONNECTION",
		GEARMAN_CON_IGNORE_LOST_CONNECTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_CLOSE_AFTER_FLUSH",
		GEARMAN_CON_CLOSE_AFTER_FLUSH,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_SEND_STATE_NONE",
		GEARMAN_CON_SEND_STATE_NONE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CON_RECV_STATE_READ_DATA",
		GEARMAN_CON_RECV_STATE_READ_DATA,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_TEXT",
		GEARMAN_COMMAND_TEXT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_CAN_DO",
		GEARMAN_COMMAND_CAN_DO,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_CANT_DO",
		GEARMAN_COMMAND_CANT_DO,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_RESET_ABILITIES",
		GEARMAN_COMMAND_RESET_ABILITIES,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_PRE_SLEEP",
		GEARMAN_COMMAND_PRE_SLEEP,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_UNUSED",
		GEARMAN_COMMAND_UNUSED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_NOOP",
		GEARMAN_COMMAND_NOOP,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB",
		GEARMAN_COMMAND_SUBMIT_JOB,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_JOB_CREATED",
		GEARMAN_COMMAND_JOB_CREATED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_GRAB_JOB",
		GEARMAN_COMMAND_GRAB_JOB,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_NO_JOB",
		GEARMAN_COMMAND_NO_JOB,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_JOB_ASSIGN",
		GEARMAN_COMMAND_JOB_ASSIGN,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_STATUS",
		GEARMAN_COMMAND_WORK_STATUS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_COMPLETE",
		GEARMAN_COMMAND_WORK_COMPLETE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_FAIL",
		GEARMAN_COMMAND_WORK_FAIL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_GET_STATUS",
		GEARMAN_COMMAND_GET_STATUS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_ECHO_REQ",
		GEARMAN_COMMAND_ECHO_REQ,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_ECHO_RES",
		GEARMAN_COMMAND_ECHO_RES,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_BG",
		GEARMAN_COMMAND_SUBMIT_JOB_BG,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_ERROR",
		GEARMAN_COMMAND_ERROR,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_STATUS_RES",
		GEARMAN_COMMAND_STATUS_RES,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_HIGH",
		GEARMAN_COMMAND_SUBMIT_JOB_HIGH,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SET_CLIENT_ID",
		GEARMAN_COMMAND_SET_CLIENT_ID,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_CAN_DO_TIMEOUT",
		GEARMAN_COMMAND_CAN_DO_TIMEOUT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_ALL_YOURS",
		GEARMAN_COMMAND_ALL_YOURS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_EXCEPTION",
		GEARMAN_COMMAND_WORK_EXCEPTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_OPTION_REQ",
		GEARMAN_COMMAND_OPTION_REQ,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_OPTION_RES",
		GEARMAN_COMMAND_OPTION_RES,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_DATA",
		GEARMAN_COMMAND_WORK_DATA,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_WORK_WARNING",
		GEARMAN_COMMAND_WORK_WARNING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_GRAB_JOB_UNIQ",
		GEARMAN_COMMAND_GRAB_JOB_UNIQ,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_JOB_ASSIGN_UNIQ",
		GEARMAN_COMMAND_JOB_ASSIGN_UNIQ,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_HIGH_BG",
		GEARMAN_COMMAND_SUBMIT_JOB_HIGH_BG,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_LOW",
		GEARMAN_COMMAND_SUBMIT_JOB_LOW,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_LOW_BG",
		GEARMAN_COMMAND_SUBMIT_JOB_LOW_BG,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_SCHED",
		GEARMAN_COMMAND_SUBMIT_JOB_SCHED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_SUBMIT_JOB_EPOCH",
		GEARMAN_COMMAND_SUBMIT_JOB_EPOCH,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_COMMAND_MAX",
		GEARMAN_COMMAND_MAX,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_NEW",
		GEARMAN_TASK_STATE_NEW,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_SUBMIT",
		GEARMAN_TASK_STATE_SUBMIT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_WORKLOAD",
		GEARMAN_TASK_STATE_WORKLOAD,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_WORK",
		GEARMAN_TASK_STATE_WORK,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_CREATED",
		GEARMAN_TASK_STATE_CREATED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_DATA",
		GEARMAN_TASK_STATE_DATA,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_WARNING",
		GEARMAN_TASK_STATE_WARNING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_STATUS",
		GEARMAN_TASK_STATE_STATUS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_COMPLETE",
		GEARMAN_TASK_STATE_COMPLETE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_EXCEPTION",
		GEARMAN_TASK_STATE_EXCEPTION,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_FAIL",
		GEARMAN_TASK_STATE_FAIL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_TASK_STATE_FINISHED",
		GEARMAN_TASK_STATE_FINISHED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_PRIORITY_HIGH",
		GEARMAN_JOB_PRIORITY_HIGH,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_PRIORITY_NORMAL",
		GEARMAN_JOB_PRIORITY_NORMAL,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_PRIORITY_LOW",
		GEARMAN_JOB_PRIORITY_LOW,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_JOB_PRIORITY_MAX",
		GEARMAN_JOB_PRIORITY_MAX,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_ALLOCATED",
		GEARMAN_CLIENT_ALLOCATED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_NON_BLOCKING",
		GEARMAN_CLIENT_NON_BLOCKING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_TASK_IN_USE",
		GEARMAN_CLIENT_TASK_IN_USE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_UNBUFFERED_RESULT",
		GEARMAN_CLIENT_UNBUFFERED_RESULT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_NO_NEW",
		GEARMAN_CLIENT_NO_NEW,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_FREE_TASKS",
		GEARMAN_CLIENT_FREE_TASKS,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_STATE_IDLE",
		GEARMAN_CLIENT_STATE_IDLE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_STATE_NEW",
		GEARMAN_CLIENT_STATE_NEW,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_STATE_SUBMIT",
		GEARMAN_CLIENT_STATE_SUBMIT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_CLIENT_STATE_PACKET",
		GEARMAN_CLIENT_STATE_PACKET,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_ALLOCATED",
		GEARMAN_WORKER_ALLOCATED,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_NON_BLOCKING",
		GEARMAN_WORKER_NON_BLOCKING,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_PACKET_INIT",
		GEARMAN_WORKER_PACKET_INIT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_GRAB_JOB_IN_USE",
		GEARMAN_WORKER_GRAB_JOB_IN_USE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_PRE_SLEEP_IN_USE",
		GEARMAN_WORKER_PRE_SLEEP_IN_USE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_WORK_JOB_IN_USE",
		GEARMAN_WORKER_WORK_JOB_IN_USE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_CHANGE",
		GEARMAN_WORKER_CHANGE,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_GRAB_UNIQ",
		GEARMAN_WORKER_GRAB_UNIQ,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_TIMEOUT_RETURN",
		GEARMAN_WORKER_TIMEOUT_RETURN,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_START",
		GEARMAN_WORKER_STATE_START,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_FUNCTION_SEND",
		GEARMAN_WORKER_STATE_FUNCTION_SEND,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_CONNECT",
		GEARMAN_WORKER_STATE_CONNECT,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_GRAB_JOB_SEND",
		GEARMAN_WORKER_STATE_GRAB_JOB_SEND,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_GRAB_JOB_RECV",
		GEARMAN_WORKER_STATE_GRAB_JOB_RECV,
		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GEARMAN_WORKER_STATE_PRE_SLEEP",
		GEARMAN_WORKER_STATE_PRE_SLEEP,
		CONST_CS | CONST_PERSISTENT);
	/* CONST_GEN_STOP */

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(gearman) {
	return SUCCESS;
}

PHP_MINFO_FUNCTION(gearman) {
	char port_str[6];

	php_info_print_table_start();
	php_info_print_table_header(2, "gearman support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_GEARMAN_VERSION);
	php_info_print_table_row(2, "libgearman version", gearman_version());
	php_info_print_table_row(2, "Default TCP Host", GEARMAN_DEFAULT_TCP_HOST);
	snprintf(port_str, 6, "%u", GEARMAN_DEFAULT_TCP_PORT);
	php_info_print_table_row(2, "Default TCP Port", port_str);
	php_info_print_table_end();
}

/* Module config struct. */
zend_module_entry gearman_module_entry = {
	STANDARD_MODULE_HEADER,
	"gearman",
	gearman_functions,
	PHP_MINIT(gearman),
	PHP_MSHUTDOWN(gearman),
	NULL,
	NULL,
	PHP_MINFO(gearman),
	PHP_GEARMAN_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GEARMAN
ZEND_GET_MODULE(gearman)
#endif
