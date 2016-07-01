/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke <contact@jamesluedke.com>,
 *                    Eric Day <eday@oddments.org>
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

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>


// TODO - find a better place for this
static inline zend_object *gearman_client_obj_new(zend_class_entry *ce);
static inline zend_object *gearman_task_obj_new(zend_class_entry *ce);
static inline zend_object *gearman_worker_obj_new(zend_class_entry *ce);
static inline zend_object *gearman_job_obj_new(zend_class_entry *ce);

// TODO - move this somewhere...
static void gearman_task_obj_free(zend_object *object);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_version, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_bugreport, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_verbose_name, 0, 0, 1)
	ZEND_ARG_INFO(0, verbose)
ZEND_END_ARG_INFO()
/*

#if jluedke_0
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_create)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_error)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_errno)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_set_options)
ZEND_END_ARG_INFO()
#endif
*/

/*
 * Gearman Task arginfo
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_task_construct, 0, 0, 0)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_server, 0, 0, 3)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_server, 0, 0, 2)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_add_servers, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, servers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_add_servers, 0, 0, 1)
	ZEND_ARG_INFO(0, servers)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_gearman_client_run_tasks, 0, 0, 2)
	ZEND_ARG_INFO(0, client_object)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_oo_gearman_client_run_tasks, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
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

/*
 * Object types and structures.
 */

typedef enum {
	GEARMAN_OBJ_CREATED = (1 << 0)
} gearman_obj_flags_t;

typedef enum {
	GEARMAN_CLIENT_OBJ_CREATED = (1 << 0)
} gearman_client_obj_flags_t;

// Defined below
typedef struct _gearman_task_obj gearman_task_obj;

typedef struct {
	gearman_return_t ret;
	gearman_client_obj_flags_t flags;
	gearman_client_st client;
	zval zclient;
	/* used for keeping track of task interface callbacks */
	zval zworkload_fn;
	zval zcreated_fn;
	zval zdata_fn;
	zval zwarning_fn;
	zval zstatus_fn;
	zval zcomplete_fn;
	zval zexception_fn;
	zval zfail_fn;

	zval task_list;

	// has to be last member
	zend_object std;
} gearman_client_obj;

typedef struct _gearman_worker_cb gearman_worker_cb;
struct _gearman_worker_cb {
	zval zname; /* name associated with callback */
	zval zcall; /* name of callback */
	zval zdata; /* data passed to callback via worker */
	gearman_worker_cb *next;
};

typedef enum {
	GEARMAN_WORKER_OBJ_CREATED = (1 << 0)
} gearman_worker_obj_flags_t;

typedef struct {
	gearman_return_t ret;
	gearman_worker_obj_flags_t flags;
	gearman_worker_st worker;
	gearman_worker_cb *cb_list;

	// Has to be last member
	zend_object std;
} gearman_worker_obj;

typedef enum {
	GEARMAN_JOB_OBJ_CREATED = (1 << 0)
} gearman_job_obj_flags_t;

typedef struct {
	gearman_return_t ret;
	gearman_job_obj_flags_t flags;
	gearman_job_st *job;
	zval *worker;
	zval *zworkload;

	// Has to be last member
	zend_object std;
} gearman_job_obj;

typedef enum {
	GEARMAN_TASK_OBJ_CREATED = (1 << 0),
} gearman_task_obj_flags_t;

struct _gearman_task_obj {
	gearman_return_t ret;
	gearman_task_obj_flags_t flags;
	gearman_task_st *task;
	zval zclient;
	zval zdata;
	zval zworkload;

	// Has to be last member
	zend_object std;
};


// TODO - probably find a better place for this to live
// TODO - move to top above gearman_client_new constructor
static inline gearman_client_obj *gearman_client_fetch_object(zend_object *obj) {
    return (gearman_client_obj *)((char*)(obj) - XtOffsetOf(gearman_client_obj, std));
}

#define Z_GEARMAN_CLIENT_P(zv) gearman_client_fetch_object(Z_OBJ_P((zv)))

static inline gearman_task_obj *gearman_task_fetch_object(zend_object *obj) {
    return (gearman_task_obj *)((char*)(obj) - XtOffsetOf(gearman_task_obj, std));
}

#define Z_GEARMAN_TASK_P(zv) gearman_task_fetch_object(Z_OBJ_P((zv)))

static inline gearman_worker_obj *gearman_worker_fetch_object(zend_object *obj) {
    return (gearman_worker_obj *)((char*)(obj) - XtOffsetOf(gearman_worker_obj, std));
}

#define Z_GEARMAN_WORKER_P(zv) gearman_worker_fetch_object(Z_OBJ_P((zv)))

static inline gearman_job_obj *gearman_job_fetch_object(zend_object *obj) {
    return (gearman_job_obj *)((char*)(obj) - XtOffsetOf(gearman_job_obj, std));
}

#define Z_GEARMAN_JOB_P(zv) gearman_job_fetch_object(Z_OBJ_P((zv)))



/*
 * Object variables
 */

#if jluedke_0
zend_class_entry *gearman_ce;
static zend_object_handlers gearman_obj_handlers;

zend_class_entry *gearman_con_ce;
static zend_object_handlers gearman_con_obj_handlers;

zend_class_entry *gearman_packet_ce;
static zend_object_handlers gearman_packet_obj_handlers;
#endif

zend_class_entry *gearman_client_ce;
static zend_object_handlers gearman_client_obj_handlers;

zend_class_entry *gearman_worker_ce;
static zend_object_handlers gearman_worker_obj_handlers;

zend_class_entry *gearman_job_ce;
static zend_object_handlers gearman_job_obj_handlers;

zend_class_entry *gearman_task_ce;
static zend_object_handlers gearman_task_obj_handlers;

zend_class_entry *gearman_exception_ce;

/*
 * Helper macros.
 */

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

#define GEARMAN_EXCEPTION(__error, __error_code) { \
	zend_throw_exception(gearman_exception_ce, __error, __error_code); \
    return; \
}

/* Custom malloc and free calls to avoid excessive buffer copies. */
static void *_php_malloc(size_t size, void *arg) {
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

#if jluedke_0
PHP_FUNCTION(gearman_create) {
	/* TODO
	gearman= gearman_create(NULL);
	if (gearman == NULL)
	{
	  php_error_docref(NULL, E_WARNING, "Memory allocation failure.");
	  RETURN_NULL();
	}

	ZEND_REGISTER_RESOURCE(return_value, gearman, le_gearman_st);
	*/

}
/* }}} */

PHP_FUNCTION(gearman_error) {
	/* TODO
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r",
	                          &zgearman) == FAILURE)
	{
	  RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE(gearman, gearman_st *, &zgearman, -1, "gearman_st",
	                    le_gearman_st);

	RETURN_STRING((char *)gearman_error(gearman), 1);
	*/
}
/* }}} */

PHP_FUNCTION(gearman_errno) {
	/* TODO
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r",
	                          &zgearman) == FAILURE)
	{
	  RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE(gearman, gearman_st *, &zgearman, -1, "gearman_st",
	                    le_gearman_st);

	RETURN_LONG(gearman_errno(gearman));
	*/
}
/* }}} */

PHP_FUNCTION(gearman_set_options) {
	/* TODO
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &zgearman,
	                          &options, &data) == FAILURE)
	{
	  RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(gearman, gearman_st *, &zgearman, -1, "gearman_st",
	                    le_gearman_st);

	gearman_set_options(gearman, options, data);

	RETURN_TRUE;
	*/
}
/* }}} */
#endif

/*
 * Functions from con.h
 */

/*
 * Functions from packet.h
 */

/*
 * Functions from task.h
 */

/* {{{ proto int gearman_task_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_task_return_code)
{
	zval *zobj;
	gearman_task_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	RETURN_LONG(obj->ret);
}
/* }}} */


#if jluedke_0
/* {{{ proto string gearman_task_context(object task)
   Set callback function argument for a task. */
PHP_FUNCTION(gearman_task_context) {
	zval *zobj;
	gearman_task_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	RETURN_STRINGL(Z_STRVAL_P(obj->zdata), Z_STRLEN_P(obj->zdata));
}
/* }}} */
#endif

/* {{{ proto string gearman_task_function_name(object task)
   Returns function name associated with a task. */
PHP_FUNCTION(gearman_task_function_name) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_STRING((char *)gearman_task_function_name(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string gearman_task_unique(object task)
   Returns unique identifier for a task. */
PHP_FUNCTION(gearman_task_unique) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_STRING((char *)gearman_task_unique(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string gearman_task_job_handle(object task)
   Returns job handle for a task. */
PHP_FUNCTION(gearman_task_job_handle) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_STRING((char *)gearman_task_job_handle(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool gearman_task_is_known(object task)
   Get status on whether a task is known or not */
PHP_FUNCTION(gearman_task_is_known) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_BOOL(gearman_task_is_known(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto bool gearman_task_is_running(object task)
   Get status on whether a task is running or not */
PHP_FUNCTION(gearman_task_is_running) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_BOOL(gearman_task_is_running(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto int gearman_task_numerator(object task)
   Returns the numerator of percentage complete for a task. */
PHP_FUNCTION(gearman_task_numerator) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_LONG(gearman_task_numerator(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto int gearman_task_denominator(object task)
   Returns the denominator of percentage complete for a task. */
PHP_FUNCTION(gearman_task_denominator) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_LONG(gearman_task_denominator(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto string gearman_task_data(object task)
   Get data being returned for a task. */
PHP_FUNCTION(gearman_task_data) {
	zval *zobj;
	gearman_task_obj *obj;
	const uint8_t *data;
	size_t data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED &&
		!gearman_client_has_option(&Z_GEARMAN_CLIENT_P(&obj->zclient)->client, GEARMAN_CLIENT_UNBUFFERED_RESULT)) {
		data = gearman_task_data(obj->task);
		data_len = gearman_task_data_size(obj->task);

		RETURN_STRINGL((char *)data, (long) data_len);
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto int gearman_task_data_size(object task)
   Get data size being returned for a task. */
PHP_FUNCTION(gearman_task_data_size) {
	zval *zobj;
	gearman_task_obj *obj;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (obj->flags & GEARMAN_TASK_OBJ_CREATED) {
		RETURN_LONG(gearman_task_data_size(obj->task));
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto int gearman_task_send_workload(object task, string data)
   NOT-TESTED Send packet data for a task. */
PHP_FUNCTION(gearman_task_send_workload) {
	zval *zobj;
	gearman_task_obj *obj;
	char *data;
	size_t data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_task_ce,
								&data, &data_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_TASK_P(zobj);


	if (!(obj->flags & GEARMAN_TASK_OBJ_CREATED)) {
		RETURN_FALSE;
	}

	/* XXX verify that i am doing this correctly */
	data_len = gearman_task_send_workload(obj->task, data, data_len, &obj->ret);
	if (obj->ret != GEARMAN_SUCCESS)
	{
		php_error_docref(NULL, E_WARNING,  "%s",
						 gearman_client_error(&Z_GEARMAN_CLIENT_P(&obj->zclient)->client));
		RETURN_FALSE;
	}

	RETURN_LONG(data_len);
}
/* }}} */


/* {{{ proto array gearman_task_recv_data(object task, long buffer_size)
   NOT-TESTED Read work or result data into a buffer for a task. */
PHP_FUNCTION(gearman_task_recv_data) {
	zval *zobj;
	gearman_task_obj *obj;
	char *data_buffer;
	zend_long data_buffer_size;
	size_t data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_task_ce,
								&data_buffer_size) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_TASK_P(zobj);

	if (!(obj->flags & GEARMAN_TASK_OBJ_CREATED)) {
		RETURN_FALSE;
	}

	data_buffer= (char *) emalloc(data_buffer_size);

	data_len= gearman_task_recv_data(obj->task, data_buffer, data_buffer_size,
									 &obj->ret);
	if (obj->ret != GEARMAN_SUCCESS &&
		!gearman_client_has_option(&Z_GEARMAN_CLIENT_P(&obj->zclient)->client, GEARMAN_CLIENT_UNBUFFERED_RESULT)) {
		php_error_docref(NULL, E_WARNING,  "%s",
						 gearman_client_error(&Z_GEARMAN_CLIENT_P(&obj->zclient)->client));
		RETURN_FALSE;
	}

	array_init(return_value);
	add_next_index_long(return_value, (long)data_len);
	add_next_index_stringl(return_value, (char *)data_buffer,
						  (long)data_len);
}
/* }}} */

/*
 * Functions from job.h
 */

/* {{{ proto int gearman_job_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_job_return_code)
{
	gearman_job_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	RETURN_LONG(obj->ret);
}
/* }}} */

/* {{{ proto bool gearman_job_send_data(object job, string data)
   Send data for a running job. */
PHP_FUNCTION(gearman_job_send_data) {
	zval *zobj;
	gearman_job_obj *obj;
	char *data;
	size_t data_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_job_ce,
								&data, &data_len) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

        /* make sure worker initialized a job */
        if (obj->job == NULL) {
                RETURN_FALSE;
        }

	obj->ret = gearman_job_send_data(obj->job, data, data_len);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_job_send_warning(object job, string warning)
   Send warning for a running job. */
PHP_FUNCTION(gearman_job_send_warning) {
	zval *zobj;
	gearman_job_obj *obj;
	char *warning = NULL;
	size_t  warning_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_job_ce,
								&warning, &warning_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_JOB_P(zobj);

        /* make sure worker initialized a job */
        if (obj->job == NULL) {
                RETURN_FALSE;
        }

	obj->ret = gearman_job_send_warning(obj->job, (void *) warning,
								 (size_t) warning_len);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_job_send_status(object job, int numerator, int denominator)
   Send status information for a running job. */
PHP_FUNCTION(gearman_job_send_status) {
	zval *zobj;
	gearman_job_obj *obj;
	zend_long numerator, denominator;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oll", &zobj, gearman_job_ce,
								&numerator, &denominator) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	obj->ret = gearman_job_send_status(obj->job, (uint32_t)numerator,
								(uint32_t)denominator);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_job_send_complete(object job, string result)
   Send result and complete status for a job. */
PHP_FUNCTION(gearman_job_send_complete) {
	zval *zobj;
	gearman_job_obj *obj;
	char *result;
	size_t result_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_job_ce,
								&result, &result_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	obj->ret = gearman_job_send_complete(obj->job, result, result_len);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_job_send_exception(object job, string exception)
   Send exception for a running job. */
PHP_FUNCTION(gearman_job_send_exception) {
	zval *zobj;
	gearman_job_obj *obj;
	char *exception;
	size_t exception_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_job_ce,
								&exception, &exception_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	obj->ret= gearman_job_send_exception(obj->job, exception, exception_len);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_job_send_fail(object job)
   Send fail status for a job. */
PHP_FUNCTION(gearman_job_send_fail) {
	zval *zobj;
	gearman_job_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	obj->ret = gearman_job_send_fail(obj->job);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING,  "%s",
			gearman_job_error(obj->job));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string gearman_job_handle(object job)
   Return job handle. */
PHP_FUNCTION(gearman_job_handle) {
	zval *zobj;
	gearman_job_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

        /* make sure worker initialized a job */
        if (obj->job == NULL) {
                RETURN_FALSE;
        }

	RETURN_STRING((char *)gearman_job_handle(obj->job))
}
/* }}} */

/* {{{ proto string gearman_job_function_name(object job)
   Return the function name associated with a job. */
PHP_FUNCTION(gearman_job_function_name) {
	zval *zobj;
	gearman_job_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

        /* make sure worker initialized a job */
        if (obj->job == NULL) {
                RETURN_FALSE;
        }

	RETURN_STRING((char *)gearman_job_function_name(obj->job))
}
/* }}} */

/* {{{ proto string gearman_job_unique(object job)
   Get the unique ID associated with a job. */
PHP_FUNCTION(gearman_job_unique) {
	zval *zobj;
	gearman_job_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

        /* make sure worker initialized a job */
        if (obj->job == NULL) {
                RETURN_FALSE;
        }

	RETURN_STRING((char *)gearman_job_unique(obj->job))
}
/* }}} */

/* {{{ proto string gearman_job_workload(object job)
   Returns the workload for a job. */
PHP_FUNCTION(gearman_job_workload) {
	zval *zobj;
	gearman_job_obj *obj;
	const uint8_t *workload;
	size_t workload_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	workload = gearman_job_workload(obj->job);
	workload_len = gearman_job_workload_size(obj->job);

	RETURN_STRINGL((char *)workload, (long) workload_len);
}
/* }}} */

/* {{{ proto int gearman_job_workload_size(object job)
   Returns size of the workload for a job. */
PHP_FUNCTION(gearman_job_workload_size) {
	zval *zobj;
	gearman_job_obj *obj;
	size_t workload_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

	workload_len = gearman_job_workload_size(obj->job);

	RETURN_LONG((long) workload_len);
}
/* }}} */

/* {{{ proto bool gearman_job_set_return(int gearman_return_t)
   This function will set a return value of a job */
PHP_FUNCTION(gearman_job_set_return) {
	zval *zobj;
	gearman_job_obj *obj;
	gearman_return_t ret;
	zend_long ret_val;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_job_ce, &ret_val) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_JOB_P(zobj);

       ret = ret_val;
	/* make sure its a valid gearman_return_t */
	if (ret < GEARMAN_SUCCESS || ret > GEARMAN_MAX_RETURN) {
		php_error_docref(NULL, E_WARNING,
						 "Invalid gearman_return_t: %d", ret);
		RETURN_FALSE;
	}

	obj->ret = ret;
	RETURN_TRUE;
}
/* }}} */

/*
 * Functions from client.h
 */

static void gearman_client_ctor(INTERNAL_FUNCTION_PARAMETERS) {
	gearman_client_obj *client;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	client = Z_GEARMAN_CLIENT_P(return_value);

	if (gearman_client_create(&(client->client)) == NULL) {
		GEARMAN_EXCEPTION("Memory allocation failure", 0);
	}

	client->flags |= GEARMAN_CLIENT_OBJ_CREATED;
	gearman_client_add_options(&(client->client), GEARMAN_CLIENT_FREE_TASKS);
	gearman_client_set_workload_malloc_fn(&(client->client), _php_malloc, NULL);
	gearman_client_set_workload_free_fn(&(client->client), _php_free, NULL);
}

/* {{{ proto object gearman_client_create()
   Returns a GearmanClient object */
PHP_FUNCTION(gearman_client_create) {
	if (object_init_ex(return_value, gearman_client_ce) != SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Object creation failure.");
		RETURN_FALSE;
	}

	gearman_client_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* {{{ proto object GearmanClient::__construct()
   Returns a GearmanClient object */
PHP_METHOD(GearmanClient, __construct)
{
	return_value = getThis();
	gearman_client_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);

}
/* }}} */

/* {{{ proto int gearman_client_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_client_return_code)
{
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	RETURN_LONG(obj->ret);
}
/* }}} */

/* {{{ proto string gearman_client_error()
   Return an error string for the last error encountered. */
PHP_FUNCTION(gearman_client_error) {
	char *error = NULL;
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	error = (char *)gearman_client_error(&(obj->client));
	if (error) {
		RETURN_STRING(error)
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int gearman_client_get_errno()
   Value of errno in the case of a GEARMAN_ERRNO return value. */
PHP_FUNCTION(gearman_client_get_errno) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	RETURN_LONG(gearman_client_errno(&(obj->client)))
}
/* }}} */

/* {{{ proto int gearman_client_options()
   Get options for a client structure. */
PHP_FUNCTION(gearman_client_options) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	RETURN_LONG(gearman_client_options(&(obj->client)))
}
/* }}} */

/* {{{ proto void gearman_client_set_options(constant option)
   Set options for a client structure. */
PHP_FUNCTION(gearman_client_set_options) {
	zend_long options;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_client_ce, &options) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_set_options(&(obj->client), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void GearmanClient::addOptions(constant option)
   Set options for a client structure. */
PHP_FUNCTION(gearman_client_add_options) {
	zend_long options;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_client_ce, &options) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_add_options(&(obj->client), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void GearmanClient::removeOptions(constant option)
   Set options for a client structure. */
PHP_FUNCTION(gearman_client_remove_options) {
	zend_long options;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_client_ce, &options) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_remove_options(&(obj->client), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gearman_client_timeout(object)
   Get timeout for a client structure. */
PHP_FUNCTION(gearman_client_timeout) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	RETURN_LONG(gearman_client_timeout(&(obj->client)))
}
/* }}} */

/* {{{ proto void gearman_client_set_timeout(object, constant timeout)
   Set timeout for a client structure. */
PHP_FUNCTION(gearman_client_set_timeout) {
	zend_long timeout;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_client_ce, &timeout) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_set_timeout(&(obj->client), timeout);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_client_add_server(object client [, string host [, int port]])
   Add a job server to a client. This goes into a list of servers than can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_client_add_server) {
	char *host = NULL;
	size_t host_len = 0;
	zend_long port = 0;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|sl", &zobj, gearman_client_ce, &host, &host_len, &port) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->ret = gearman_client_add_server(&(obj->client), host, port);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	if (!gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
	    GEARMAN_EXCEPTION("Failed to set exception option", 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_client_add_servers(object client [, string servers])
   Add a list of job servers to a client. This goes into a list of servers that can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_client_add_servers) {
	char *servers = NULL;
	size_t servers_len = 0;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|s", &zobj, gearman_client_ce, &servers, &servers_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->ret = gearman_client_add_servers(&(obj->client), servers);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	if (!gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
	    GEARMAN_EXCEPTION("Failed to set exception option", 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::wait()
   Wait for I/O activity on all connections in a client. */
PHP_FUNCTION(gearman_client_wait) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->ret = gearman_client_wait(&(obj->client));

	if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
		if (obj->ret != GEARMAN_TIMEOUT) {
			php_error_docref(NULL, E_WARNING, "%s",
				gearman_client_error(&(obj->client)));
		}
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object gearman_client_do_work_handler(void *add_task_func, object client, string function, zval workload [, string unique ])
   Run a task, high/normal/low dependent upon do_work_func */
static void gearman_client_do_work_handler(void* (*do_work_func)(
								gearman_client_st *client,
								const char *function_name,
								const char *unique,
								const void *workload, size_t workload_size,
								size_t *result_size,
								gearman_return_t *ret_ptr
					),
					INTERNAL_FUNCTION_PARAMETERS) {
	char *function_name;
	size_t function_name_len;
	char *workload;
	size_t workload_len;
	char *unique = NULL;
	size_t unique_len = 0;
	void *result;
	size_t result_size = 0;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|s", &zobj, gearman_client_ce,
							&function_name, &function_name_len,
							&workload, &workload_len,
							&unique, &unique_len) == FAILURE) {
		RETURN_EMPTY_STRING();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	result = (char *)(*do_work_func)(
						&(obj->client),
						function_name,
						unique,
						workload,
						(size_t)workload_len,
						&result_size,
						&(obj)->ret
					);

	if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
		php_error_docref(NULL, E_WARNING, "%s", gearman_client_error(&(obj->client)));
		RETURN_EMPTY_STRING();
	}

	/* NULL results are valid */
	if (! result) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STRINGL((char *)result, (long) result_size);
}
/* }}} */

/* {{{ proto string GearmanClient::doNormal(string function, string workload [, string unique ])
   Run a single task and return an allocated result. */
PHP_FUNCTION(gearman_client_do_normal) {
	gearman_client_do_work_handler(gearman_client_do, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string GearmanClient::doHigh(object client, string function, string workload [, string unique ])
   Run a high priority task and return an allocated result. */
PHP_FUNCTION(gearman_client_do_high) {
	gearman_client_do_work_handler(gearman_client_do_high, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto array GearmanClient::doLow(object client, string function, string workload [, string unique ])
   Run a low priority task and return an allocated result. */
PHP_FUNCTION(gearman_client_do_low) {
	gearman_client_do_work_handler(gearman_client_do_low, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object gearman_client_do_background_work_handler(void *add_task_func, object client, string function, zval workload [, string unique ])
   Run a task in the background, high/normal/low dependent upon do_work_func */
static void gearman_client_do_background_work_handler(gearman_return_t (*do_background_work_func)(
								gearman_client_st *client,
								const char *function_name,
								const char *unique,
								const void *workload,
								size_t workload_size,
                                              			gearman_job_handle_t job_handle
					),
					INTERNAL_FUNCTION_PARAMETERS) {
	char *function_name;
	size_t function_name_len;
	char *workload;
	size_t workload_len;
	char *unique = NULL;
	size_t unique_len = 0;
	zend_string *job_handle;
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|s", &zobj, gearman_client_ce,
							&function_name, &function_name_len,
							&workload, &workload_len,
							&unique, &unique_len) == FAILURE) {
		RETURN_EMPTY_STRING();
	}

	obj = Z_GEARMAN_CLIENT_P(zobj);

	job_handle = zend_string_alloc(GEARMAN_JOB_HANDLE_SIZE-1, 0);

	obj->ret = (*do_background_work_func)(
						&(obj->client),
						(char *)function_name,
						(char *)unique,
						(void *)workload,
						(size_t)workload_len,
						job_handle->val
					);

	ZSTR_LEN(job_handle) = strnlen(ZSTR_VAL(job_handle), GEARMAN_JOB_HANDLE_SIZE-1);

	if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		zend_string_release(job_handle);
		RETURN_EMPTY_STRING();
	}

	if (! job_handle) {
		zend_string_release(job_handle);
		RETURN_EMPTY_STRING();
	}

	RETURN_STR(job_handle);
}
/* }}} */

/* {{{ proto string GearmanClient::doBackground(string function, string workload [, string unique ])
   Run a task in the background. */
PHP_FUNCTION(gearman_client_do_background) {
	gearman_client_do_background_work_handler(gearman_client_do_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string GearmanClient::doHighBackground(string function, string workload [, string unique ])
   Run a high priority task in the background. */
PHP_FUNCTION(gearman_client_do_high_background) {
	gearman_client_do_background_work_handler(gearman_client_do_high_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string GearmanClient::doLowBackground(string function, string workload [, string unique ])
   Run a low priority task in the background. */
PHP_FUNCTION(gearman_client_do_low_background) {
	gearman_client_do_background_work_handler(gearman_client_do_low_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto string GearmanClient::doJobHandle()
   Get the job handle for the running task. This should be used between repeated gearman_client_do_normal() and gearman_client_do_high() calls to get information. */
PHP_FUNCTION(gearman_client_do_job_handle) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_EMPTY_STRING();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);


	RETURN_STRING((char *)gearman_client_do_job_handle(&(obj->client)))
}
/* }}} */

/* {{{ proto array GearmanClient::doStatus()
   Get the status for the running task. This should be used between repeated gearman_client_do() and gearman_client_do_high() calls to get information. */
PHP_FUNCTION(gearman_client_do_status) {
	uint32_t numerator;
	uint32_t denominator;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_EMPTY_STRING();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_do_status(&(obj->client), &numerator, &denominator);

	array_init(return_value);
	add_next_index_long(return_value, (long) numerator);
	add_next_index_long(return_value, (long) denominator);
}
/* }}} */

/* {{{ proto array GearmanClient::jobStatus(string job_handle)
   Get the status for a backgound job. */
PHP_FUNCTION(gearman_client_job_status) {
	char *job_handle;
	size_t job_handle_len;
	bool is_known;
	bool is_running;
	uint32_t numerator;
	uint32_t denominator;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_client_ce,
								&job_handle, &job_handle_len) == FAILURE) {
		RETURN_EMPTY_STRING();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->ret = gearman_client_job_status(&(obj->client), job_handle,
										&is_known, &is_running,
										&numerator, &denominator);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
	}

	array_init(return_value);
	add_next_index_bool(return_value, is_known);
	add_next_index_bool(return_value, is_running);
	add_next_index_long(return_value, (long) numerator);
	add_next_index_long(return_value, (long) denominator);
}
/* }}} */

/* {{{ proto array GearmanClient::jobStatusByUniqueKey(string unique_key)
   Get the status for a backgound job using the unique key passed in during job submission, rather than job handle. */
PHP_FUNCTION(gearman_client_job_status_by_unique_key) {
	char *unique_key;
	size_t unique_key_len;
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_client_ce,
								&unique_key, &unique_key_len) == FAILURE) {
		RETURN_EMPTY_STRING();
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_status_t status = gearman_client_unique_status(&(obj->client), unique_key, unique_key_len);
	gearman_return_t rc = gearman_status_return(status);

	if (rc != GEARMAN_SUCCESS && rc != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
	}

	array_init(return_value);
	add_next_index_bool(return_value, gearman_status_is_known(status));
	add_next_index_bool(return_value, gearman_status_is_running(status));
	add_next_index_long(return_value, (long) gearman_status_numerator(status));
	add_next_index_long(return_value, (long) gearman_status_denominator(status));
}
/* }}} */

/* {{{ proto bool GearmanClient::ping(string workload)
   Send data to all job servers to see if they send it back. */
PHP_FUNCTION(gearman_client_ping) {
	char *workload;
	size_t workload_len;
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_client_ce, &workload, &workload_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->ret = gearman_client_echo(&(obj->client), workload, (size_t)workload_len);

	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object gearman_client_add_task_handler(void *add_task_func, object client, string function, zval workload [, string unique ])
   Add a task to be run in parallel, background or not, high/normal/low dependent upon add_task_func. */
static void gearman_client_add_task_handler(gearman_task_st* (*add_task_func)(
								gearman_client_st *client,
								gearman_task_st *task,
								void *context,
								const char *function_name,
								const char *unique,
								const void *workload,
								size_t workload_size,
								gearman_return_t *ret_ptr),
					INTERNAL_FUNCTION_PARAMETERS) {
	zval *zworkload;
	zval *zdata = NULL;
	gearman_task_obj *task;

	char *unique;
	char *function_name;
	size_t unique_len = 0;
	size_t function_name_len = 0;

	gearman_client_obj *obj;
	zval *zobj;

	// TODO - the documentation on php.net differs from this
	// As found, this doesn't allow for user to pass in context.
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Osz|zs", &zobj, gearman_client_ce,
								&function_name, &function_name_len,
								&zworkload,
								&zdata,
								&unique, &unique_len
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	if (unique_len == 0) {
	  unique = NULL;
	}

	/* get a task object, and prepare it for return */
	if (object_init_ex(return_value, gearman_task_ce) != SUCCESS) {
		php_error_docref(NULL, E_WARNING, "GearmanTask Object creation failure.");
		RETURN_FALSE;
	}

	task = Z_GEARMAN_TASK_P(return_value);

	if (zdata) {
		if (Z_REFCOUNTED_P(zdata)) {
			Z_ADDREF_P(zdata);
		}
		ZVAL_COPY_VALUE(&task->zdata, zdata);
	}

	/* store our workload and add ref so it wont go away on us */
		if (Z_REFCOUNTED_P(zworkload)) {
			Z_ADDREF_P(zworkload);
		}
	ZVAL_COPY_VALUE(&task->zworkload, zworkload);

	/* need to store a ref to the client for later access to cb's */
	ZVAL_COPY_VALUE(&task->zclient, zobj);

	/* add the task */
	task->task = (*add_task_func)(
					&(obj->client),
					task->task,
					(void *)task,
					function_name,
					unique,
					Z_STRVAL_P(zworkload),
					(size_t) Z_STRLEN_P(zworkload),
					&obj->ret
				);

	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	task->flags |= GEARMAN_TASK_OBJ_CREATED;

	// prepend task to list of tasks on client obj
	Z_ADDREF_P(return_value);
	add_next_index_zval(&obj->task_list, return_value);
}
/* }}} */

/* {{{ proto object GearmanClient::addTask(string function, zval workload [, string unique ])
   Add a task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task) {
	gearman_client_add_task_handler(gearman_client_add_task, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* {{{ proto object GearmanClient::addTaskHigh(string function, zval workload [, string unique ])
   Add a high priority task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task_high) {
	gearman_client_add_task_handler(gearman_client_add_task_high, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanClient::addTaskLow(string function, zval workload [, string unique ])
   Add a low priority task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task_low) {
	gearman_client_add_task_handler(gearman_client_add_task_low, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanClient(string function, zval workload [, string unique ])
   Add a background task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task_background) {
	gearman_client_add_task_handler(gearman_client_add_task_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanClient::addTaskHighBackground(string function, zval workload [, string unique ])
   Add a high priority background task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task_high_background) {
	gearman_client_add_task_handler(gearman_client_add_task_high_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanClient::addTaskLowBackground(string function, zval workload [, string unique ])
   Add a low priority background task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task_low_background) {
	gearman_client_add_task_handler(gearman_client_add_task_low_background, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* this function is used to request status information from the gearmand
 * server. it will then call your predefined status callback, passing
 * zdata/context to it */
/* {{{ proto object gearman_client_add_task_status(object client, string job_handle [, zval data])
   Add task to get the status for a backgound task in parallel. */
PHP_FUNCTION(gearman_client_add_task_status) {
	zval *zdata = NULL;

	char *job_handle;
	size_t job_handle_len = 0;

	gearman_client_obj *obj;
	zval *zobj;
	gearman_task_obj *task;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|z", &zobj, gearman_client_ce,
								&job_handle, &job_handle_len,
								&zdata
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* get a task object, and prepare it for return */
	if (object_init_ex(return_value, gearman_task_ce) != SUCCESS) {
		php_error_docref(NULL, E_WARNING, "GearmanTask Object creation failure.");
		RETURN_FALSE;
	}

       task = Z_GEARMAN_TASK_P(return_value);

	if (zdata) {
               if (Z_REFCOUNTED_P(zdata)) {
                       Z_ADDREF_P(zdata);
               }
               ZVAL_COPY_VALUE(&task->zdata, zdata);
	}
	/* need to store a ref to the client for later access to cb's */
	ZVAL_COPY_VALUE(&task->zclient, zobj);

	/* add the task */
	task->task = gearman_client_add_task_status(&(obj->client),
							task->task,
							(void *)task,
							job_handle,
							&obj->ret
							);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	task->flags |= GEARMAN_TASK_OBJ_CREATED;

       // prepend task to list of tasks on client obj
       Z_ADDREF_P(return_value);
       add_next_index_zval(&obj->task_list, return_value);
}
/* }}} */

/* this function will be used to call our user defined task callbacks */
static gearman_return_t _php_task_cb_fn(gearman_task_obj *task, gearman_client_obj *client, zval zcall) {
	gearman_return_t ret;

	zval ztask, argv[2], retval;
	uint32_t param_count;

	ZVAL_OBJ(&ztask, &task->std);
        ZVAL_COPY_VALUE(&argv[0], &ztask);

	if (Z_ISUNDEF(task->zdata)) {
		param_count = 1;
	} else {
		ZVAL_COPY_VALUE(&argv[1], &task->zdata);
		param_count = 2;
	}

	if (call_user_function_ex(EG(function_table), NULL, &zcall, &retval, param_count, argv, 0, NULL) != SUCCESS) {
		php_error_docref(NULL,
				E_WARNING,
				"Could not call the function %s",
				( Z_ISUNDEF(zcall) || Z_TYPE(zcall) != IS_STRING)  ? "[undefined]" : Z_STRVAL(zcall)
				);
		ret = 0;
	} else {
		if (Z_ISUNDEF(retval)) {
			ret = 0;
		} else {
			if (Z_TYPE(retval) != IS_LONG) {
				convert_to_long(&retval);
			}
			ret = Z_LVAL(retval);
		}
	}

	return ret;
}

/* TODO: clean this up a bit, Macro? */
static gearman_return_t _php_task_workload_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zworkload_fn);
}

static gearman_return_t _php_task_created_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zcreated_fn);
}

static gearman_return_t _php_task_data_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zdata_fn);
}

static gearman_return_t _php_task_warning_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zwarning_fn);
}

static gearman_return_t _php_task_status_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zstatus_fn);
}

static gearman_return_t _php_task_complete_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zcomplete_fn);
}

static gearman_return_t _php_task_exception_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zexception_fn);
}

static gearman_return_t _php_task_fail_fn(gearman_task_st *task) {
	gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
	gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	return _php_task_cb_fn(task_obj, client_obj, client_obj->zfail_fn);
}

/* {{{ proto bool GearmanClient::setWorkloadCallback(callback function)
   Callback function when workload data needs to be sent for a task. */
PHP_FUNCTION(gearman_client_set_workload_callback) {
	zval *zworkload_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zworkload_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zworkload_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zworkload_fn)) {
		zval_dtor(&obj->zworkload_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zworkload_fn, zworkload_fn);

	/* set the callback for php */
	gearman_client_set_workload_fn(&(obj->client), _php_task_workload_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setCreatedCallback(callback function)
   Callback function when workload data needs to be sent for a task. */
PHP_FUNCTION(gearman_client_set_created_callback) {
	zval *zcreated_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zcreated_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zcreated_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zcreated_fn)) {
		zval_dtor(&obj->zcreated_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zcreated_fn, zcreated_fn);

	/* set the callback for php */
	gearman_client_set_created_fn(&(obj->client), _php_task_created_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setDataCallback(callback function)
   Callback function when there is a data packet for a task. */
PHP_FUNCTION(gearman_client_set_data_callback) {
	zval *zdata_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zdata_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zdata_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zdata_fn)) {
		zval_dtor(&obj->zdata_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zdata_fn, zdata_fn);

	/* set the callback for php */
	gearman_client_set_data_fn(&(obj->client), _php_task_data_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setWarningCallback(callback function)
   Callback function when there is a warning packet for a task. */
PHP_FUNCTION(gearman_client_set_warning_callback) {
	zval *zwarning_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zwarning_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zwarning_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zwarning_fn)) {
		zval_dtor(&obj->zwarning_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zwarning_fn, zwarning_fn);

	/* set the callback for php */
	gearman_client_set_warning_fn(&(obj->client), _php_task_warning_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setStatusCallback(callback function)
   Callback function when there is a status packet for a task. */
PHP_FUNCTION(gearman_client_set_status_callback) {
	zval *zstatus_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zstatus_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (!zend_is_callable(zstatus_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}

	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zstatus_fn)) {
		zval_dtor(&obj->zstatus_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zstatus_fn, zstatus_fn);

	/* set the callback for php */
	gearman_client_set_status_fn(&(obj->client), _php_task_status_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setCompleteCallback(callback function)
   Callback function when there is a status packet for a task. */
PHP_FUNCTION(gearman_client_set_complete_callback) {
	zval *zcomplete_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zcomplete_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zcomplete_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zcomplete_fn)) {
		zval_dtor(&obj->zcomplete_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zcomplete_fn, zcomplete_fn);

	/* set the callback for php */
	gearman_client_set_complete_fn(&(obj->client), _php_task_complete_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setExceptionCallback(callback function)
   Callback function when there is a exception packet for a task. */
PHP_FUNCTION(gearman_client_set_exception_callback) {
	zval *zexception_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zexception_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */
	if (! zend_is_callable(zexception_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zexception_fn)) {
		zval_dtor(&obj->zexception_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zexception_fn, zexception_fn);

	/* set the callback for php */
	gearman_client_set_exception_fn(&(obj->client), _php_task_exception_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::setFailCallback(callback function)
   Callback function when there is a fail packet for a task. */
PHP_FUNCTION(gearman_client_set_fail_callback) {
	zval *zfail_fn;
	zend_string *callable = NULL;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz", &zobj, gearman_client_ce,
								&zfail_fn
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	/* check that the function is callable */

	if (! zend_is_callable(zfail_fn, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}
	zend_string_release(callable);

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zfail_fn)) {
		zval_dtor(&obj->zfail_fn);
	}

	/* Defining callback again? Clean up old one first */
        if (!Z_ISUNDEF(obj->zfail_fn)) {
		zval_dtor(&obj->zfail_fn);
	}

	/* store the cb in client object */
	ZVAL_DUP(&obj->zfail_fn, zfail_fn);

	/* set the callback for php */
	gearman_client_set_fail_fn(&(obj->client), _php_task_fail_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void GearmanClient::clearCallbacks()
   Clear all task callback functions. */
PHP_FUNCTION(gearman_client_clear_callbacks) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_clear_fn(&obj->client);

	zval_dtor(&obj->zworkload_fn);
	zval_dtor(&obj->zcreated_fn);
	zval_dtor(&obj->zdata_fn);
	zval_dtor(&obj->zwarning_fn);
	zval_dtor(&obj->zstatus_fn);
	zval_dtor(&obj->zcomplete_fn);
	zval_dtor(&obj->zexception_fn);
	zval_dtor(&obj->zfail_fn);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string GearmanClient::context()
   Get the application data */
PHP_FUNCTION(gearman_client_context) {
	const uint8_t *data;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	data = gearman_client_context(&(obj->client));

	RETURN_STRINGL((char *)data, (long) sizeof(data));
}
/* }}} */

/* {{{ proto bool GearmanClient::setContext(string data)
   Set the application data */
PHP_FUNCTION(gearman_client_set_context) {
	char *data;
	size_t data_len = 0;

	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_client_ce, &data, &data_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	gearman_client_set_context(&(obj->client), (void *)data);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_client_run_tasks(object client)
   Run tasks that have been added in parallel */
PHP_FUNCTION(gearman_client_run_tasks) {
	gearman_client_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_CLIENT_P(zobj);

	obj->zclient = *zobj;
	obj->ret = gearman_client_run_tasks(&(obj->client));

	if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_client_error(&(obj->client)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/*
 * Functions from worker.h
 */

/* {{{ proto int gearman_worker_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_worker_return_code)
{
	gearman_worker_obj *obj;
	zval *zobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);
	RETURN_LONG(obj->ret);
}
/* }}} */

/* {{{ proto string gearman_worker_error(object worker)
   Return an error string for the last error encountered. */
PHP_FUNCTION(gearman_worker_error) {
        char *error;
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

        error = (char *)gearman_worker_error(&(obj->worker));
        if (error) {
            RETURN_STRING(error);
        }

        RETURN_FALSE;
}
/* }}} */

/* {{{ proto int gearman_worker_errno(object worker)
   Value of errno in the case of a GEARMAN_ERRNO return value. */
PHP_FUNCTION(gearman_worker_errno) {
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	RETURN_LONG(gearman_worker_errno(&(obj->worker)))
}
/* }}} */

/* {{{ proto int gearman_worker_options(object worker)
   Get options for a worker structure. */
PHP_FUNCTION(gearman_worker_options) {
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	RETURN_LONG(gearman_worker_options(&(obj->worker)))
}
/* }}} */

/* {{{ proto void gearman_worker_set_options(object worker, constant option)
   Set options for a worker structure. */
PHP_FUNCTION(gearman_worker_set_options) {
	zval *zobj;
	gearman_worker_obj *obj;
	zend_long options;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_worker_ce, &options) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	gearman_worker_set_options(&(obj->worker), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void gearman_worker_add_options(object worker, constant option)
   Set options for a worker structure. */
PHP_FUNCTION(gearman_worker_add_options) {
	zval *zobj;
	gearman_worker_obj *obj;
	zend_long options;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_worker_ce, &options) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	gearman_worker_add_options(&(obj->worker), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void gearman_worker_remove_options(object worker, constant option)
   Set options for a worker structure. */
PHP_FUNCTION(gearman_worker_remove_options) {
	zval *zobj;
	gearman_worker_obj *obj;
	zend_long options;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_worker_ce, &options) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	gearman_worker_remove_options(&(obj->worker), options);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gearman_worker_timeout(object worker)
   Get timeout for a worker structure. */
PHP_FUNCTION(gearman_worker_timeout) {
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_NULL();
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	RETURN_LONG(gearman_worker_timeout(&(obj->worker)))
}
/* }}} */

/* {{{ proto void gearman_worker_set_timeout(object worker, constant timeout)
   Set timeout for a worker structure. */
PHP_FUNCTION(gearman_worker_set_timeout) {
	zval *zobj;
	gearman_worker_obj *obj;
	zend_long timeout;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zobj, gearman_worker_ce, &timeout) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	gearman_worker_set_timeout(&(obj->worker), timeout);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void gearman_worker_set_id(object worker, string id)
   Set id for a worker structure. */
PHP_FUNCTION(gearman_worker_set_id) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *id;
	size_t id_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_worker_ce,
							&id, &id_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	if(gearman_failed(gearman_worker_set_identifier(&(obj->worker), id, id_len))) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_add_server(object worker [, string host [, int port ]])
   Add a job server to a worker. This goes into a list of servers than can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_worker_add_server) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *host = NULL;
	size_t host_len = 0;
	zend_long port = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|sl", &zobj,
								gearman_worker_ce,
								&host, &host_len,
								&port
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_add_server(&obj->worker, host, port);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&obj->worker));
		RETURN_FALSE;
	}

	if (! gearman_worker_set_server_option(&(obj->worker), "exceptions", (sizeof("exceptions") - 1))) {
		GEARMAN_EXCEPTION("Failed to set exception option", 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_add_servers(object worker [, string servers])
   Add a list of job servers to a worker. This goes into a list of servers that can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_worker_add_servers) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *servers = NULL;
	size_t servers_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj,
								gearman_worker_ce,
								&servers, &servers_len
								) == FAILURE) {
		RETURN_FALSE;
	}

	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_add_servers(&obj->worker, servers);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&obj->worker));
		RETURN_FALSE;
	}

	if (! gearman_worker_set_server_option(&(obj->worker), "exceptions", (sizeof("exceptions") - 1))) {
		GEARMAN_EXCEPTION("Failed to set exception option", 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_wait(object worker)
   Wait for I/O activity on all connections in a worker. */
PHP_FUNCTION(gearman_worker_wait) {
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_wait(&(obj->worker));

	if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
		if (obj->ret != GEARMAN_TIMEOUT) {
			php_error_docref(NULL, E_WARNING, "%s",
				gearman_worker_error(&(obj->worker)));
		}

		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_register(object worker, string function [, int timeout ])
   Register function with job servers with an optional timeout. The timeout specifies how many seconds the server will wait before marking a job as failed. If timeout is zero, there is no timeout. */
PHP_FUNCTION(gearman_worker_register) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *function_name;
	size_t function_name_len;
	zend_long timeout = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|l", &zobj, gearman_worker_ce,
							&function_name, &function_name_len,
							&timeout
							) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_register(&(obj->worker), function_name, timeout);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&(obj->worker)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_unregister(object worker, string function)
    Unregister function with job servers. */
PHP_FUNCTION(gearman_worker_unregister) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *function_name;
	size_t function_name_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_worker_ce,
							&function_name, &function_name_len
							) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_unregister(&(obj->worker), function_name);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&(obj->worker)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_unregister_all(object worker)
   Unregister all functions with job servers. */
PHP_FUNCTION(gearman_worker_unregister_all) {
	zval *zobj;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret= gearman_worker_unregister_all(&(obj->worker));
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&(obj->worker)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto object gearman_worker_grab_job(obect worker)
   Get a job from one of the job servers.
   Note: This is undocumented on php.net and needs a test*/
PHP_FUNCTION(gearman_worker_grab_job) {
	zval *zobj;
	gearman_worker_obj *obj;
	gearman_job_obj *job;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	object_init_ex(return_value, gearman_job_ce);
	job = Z_GEARMAN_JOB_P(return_value);

	job->worker = zobj;

	job->job = gearman_worker_grab_job(&(obj->worker), NULL, &obj->ret);
	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&(obj->worker)));
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	job->flags |= GEARMAN_JOB_OBJ_CREATED;
}

/* *job is passed in via gearman, need to convert that into a zval that
 * is accessable in the user_defined php callback function */
static void *_php_worker_function_callback(gearman_job_st *job,
						void *context,
						size_t *result_size,
						gearman_return_t *ret_ptr) {
	zval zjob, message;
	gearman_job_obj *jobj;
	gearman_worker_cb *worker_cb = (gearman_worker_cb *)context;
	char *result = NULL;
	uint32_t param_count;

	/* cb vars */
	zval argv[2], retval;

	/* first create our job object that will be passed to the callback */
        if (object_init_ex(&zjob, gearman_job_ce) != SUCCESS) {
                php_error_docref(NULL, E_WARNING, "Failed to create gearman_job_ce object.");
                return result;
        }
	jobj = Z_GEARMAN_JOB_P(&zjob);
	jobj->job = job;

        ZVAL_COPY_VALUE(&argv[0], &zjob);

        if (Z_ISUNDEF(worker_cb->zdata)) {
                param_count = 1;
		ZVAL_NULL(&argv[1]);
        } else {
                ZVAL_COPY_VALUE(&argv[1], &worker_cb->zdata);
                param_count = 2;
        }

	jobj->ret = GEARMAN_SUCCESS;

	if (call_user_function_ex(EG(function_table), NULL, &worker_cb->zcall, &retval, param_count, argv, 0, NULL) != SUCCESS) {
		php_error_docref(NULL,
				E_WARNING,
				"Could not call the function %s",
				( Z_ISUNDEF(worker_cb->zcall) || Z_TYPE(worker_cb->zcall) != IS_STRING)  ? "[undefined]" : Z_STRVAL(worker_cb->zcall)
				);
		jobj->ret = GEARMAN_WORK_FAIL;
	}

	*ret_ptr = jobj->ret;

	if (EG(exception)) {
		*ret_ptr = GEARMAN_WORK_EXCEPTION;

		ZVAL_STRING(&message, "Unable to add worker function");

		jobj->ret = gearman_job_send_exception(jobj->job, Z_STRVAL(message), Z_STRLEN(message));

		if (jobj->ret != GEARMAN_SUCCESS && jobj->ret != GEARMAN_IO_WAIT) {
			php_error_docref(NULL, E_WARNING,  "Unable to add worker function: %s",
					gearman_job_error(jobj->job));
		}
	}

	if (Z_ISUNDEF(retval)) {
		result = NULL;
		*result_size = 0;
	} else {
		if (Z_TYPE(retval) != IS_STRING) {
			convert_to_string(&retval);
		}
		result = estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
		*result_size = Z_STRLEN(retval);
		zval_dtor(&retval);
	}

	zval_dtor(&argv[0]);
	zval_dtor(&argv[1]);

	return result;
}
/* }}} */

/* {{{ proto bool gearman_worker_add_function(object worker, zval function_name, zval callback [, zval data [, int timeout]])
   Register and add callback function for worker. */
PHP_FUNCTION(gearman_worker_add_function) {
	zval *zobj = NULL;
	gearman_worker_obj *obj;
	gearman_worker_cb *worker_cb;

	zval *zname, *zcall, *zdata = NULL;
	zend_long timeout = 0;

	zend_string *callable = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ozz|zl", &zobj, gearman_worker_ce,
								&zname,
								&zcall,
								&zdata,
								&timeout
								) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

        /* check that the function name is a string */
	if (Z_TYPE_P(zname) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "function name must be a string");
		RETURN_FALSE;
	}

	/* check that the function can be called */
	if (!zend_is_callable(zcall, 0, &callable)) {
		php_error_docref(NULL, E_WARNING, "function %s is not callable", callable->val);
		zend_string_release(callable);
		RETURN_FALSE;
	}

	zend_string_release(callable);

	/* create a new worker cb */
// TODO - this feels like we shouldn't be emalloc-ing here
	worker_cb = emalloc(sizeof(gearman_worker_cb));
	memset(worker_cb, 0, sizeof(gearman_worker_cb));

	/* copy over zname, zcall and zdata */
	ZVAL_DUP(&worker_cb->zname, zname);
	ZVAL_DUP(&worker_cb->zcall, zcall);

// TODO - Z_ISUNDEF_P instead?
	if (zdata != NULL) {
		worker_cb->zdata = *zdata;
	}

	worker_cb->next = obj->cb_list;
	obj->cb_list = worker_cb;

	/* add the function */
	/* NOTE: _php_worker_function_callback is a wrapper that calls
	 * the function defined by gearman_worker_add_function */
	obj->ret = gearman_worker_add_function(&(obj->worker),
						Z_STRVAL(worker_cb->zname),
						(uint32_t)timeout,
						_php_worker_function_callback,
						(void *)worker_cb
						);
	if (obj->ret != GEARMAN_SUCCESS) {
		php_error_docref(NULL, E_WARNING, "Unable to add function to Gearman Worker: %s %s",
						 gearman_worker_error(&(obj->worker)), gearman_strerror(obj->ret));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int gearman_worker_work(object worker)
    Wait for a job and call the appropriate callback function when it gets one. */
PHP_FUNCTION(gearman_worker_work) {
	zval *zobj = NULL;
	gearman_worker_obj *obj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_worker_ce) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_work(&(obj->worker));

	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT &&
			obj->ret != GEARMAN_WORK_FAIL && obj->ret != GEARMAN_TIMEOUT &&
			obj->ret != GEARMAN_WORK_EXCEPTION && obj->ret != GEARMAN_NO_JOBS) {
		php_error_docref(NULL, E_WARNING, "%s",
				gearman_worker_error(&(obj->worker)));
		RETURN_FALSE;
	}

	if (obj->ret != GEARMAN_SUCCESS) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_worker_ping(object worker, string data)
   Send data to all job servers to see if they echo it back. */
PHP_FUNCTION(gearman_worker_ping) {
	zval *zobj;
	gearman_worker_obj *obj;
	char *workload;
	size_t workload_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_worker_ce,
							&workload, &workload_len) == FAILURE) {
		RETURN_FALSE;
	}
	obj = Z_GEARMAN_WORKER_P(zobj);

	obj->ret = gearman_worker_echo(&(obj->worker), workload, (size_t)workload_len);

	if (obj->ret != GEARMAN_SUCCESS && obj->ret != GEARMAN_IO_WAIT) {
		php_error_docref(NULL, E_WARNING, "%s",
						 gearman_worker_error(&(obj->worker)));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/*
 * Methods for gearman_client
 */

static void gearman_client_obj_free(zend_object *object) {
	gearman_client_obj *intern = gearman_client_fetch_object(object);
	if (!intern) {
		return;
	}

	if (intern->flags & GEARMAN_CLIENT_OBJ_CREATED) {
		gearman_client_free(&intern->client);
	}

	zval_dtor(&intern->zclient);

	// Clear Callbacks
	zval_dtor(&intern->zworkload_fn);
	zval_dtor(&intern->zcreated_fn);
	zval_dtor(&intern->zdata_fn);
	zval_dtor(&intern->zwarning_fn);
	zval_dtor(&intern->zstatus_fn);
	zval_dtor(&intern->zcomplete_fn);
	zval_dtor(&intern->zexception_fn);
	zval_dtor(&intern->zfail_fn);

	zval_dtor(&intern->task_list);

	zend_object_std_dtor(&intern->std);
}

static inline zend_object *gearman_client_obj_new(zend_class_entry *ce) {
	gearman_client_obj *intern = ecalloc(1,
		sizeof(gearman_client_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);
	array_init(&intern->task_list);

	intern->std.handlers = &gearman_client_obj_handlers;
	return &intern->std;
}


/*
 * Methods for gearman_worker
 */

/* {{{ proto object gearman_worker_ctor()
   Initialize a worker object.  */
static void gearman_worker_ctor(INTERNAL_FUNCTION_PARAMETERS) {
	gearman_worker_obj *worker;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	worker = Z_GEARMAN_WORKER_P(return_value);

	if (gearman_worker_create(&(worker->worker)) == NULL) {
		zval_dtor(return_value);
		GEARMAN_EXCEPTION("Memory allocation failure", 0);
	}

	worker->flags |= GEARMAN_WORKER_OBJ_CREATED;
	gearman_worker_set_workload_malloc_fn(&(worker->worker), _php_malloc, NULL);
	gearman_worker_set_workload_free_fn(&(worker->worker), _php_free, NULL);
}
/* }}} */

/* {{{ proto object gearman_worker_create()
   Returns a worker object */
PHP_FUNCTION(gearman_worker_create) {
	if (object_init_ex(return_value, gearman_worker_ce) != SUCCESS) {
		php_error_docref(NULL, E_WARNING,
						 "GearmanWorker Object creation failure.");
		RETURN_FALSE;
	}

	gearman_worker_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanWorker::__construct()
   Returns a worker object */
PHP_METHOD(GearmanWorker, __construct)
{
	return_value = getThis();
	gearman_worker_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void gearman_worker_obj_free(zend_object *object) {
	gearman_worker_cb *next_cb = NULL;
	gearman_worker_obj *intern = gearman_worker_fetch_object(object);

	if (!intern)  {
		return;
	}

	if (intern->flags & GEARMAN_WORKER_OBJ_CREATED) {
		gearman_worker_free(&(intern->worker));
	}

	while (intern->cb_list) {
		next_cb = intern->cb_list->next;

		zval_dtor(&intern->cb_list->zname);
		zval_dtor(&intern->cb_list->zcall);
		zval_dtor(&intern->cb_list->zdata);

		efree(intern->cb_list);
		intern->cb_list = next_cb;
	}

	zend_object_std_dtor(&(intern->std));
}

static inline zend_object *gearman_worker_obj_new(zend_class_entry *ce) {
	gearman_worker_obj *intern = ecalloc(1,
		sizeof(gearman_worker_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);

	intern->std.handlers = &gearman_worker_obj_handlers;
	return &intern->std;
}

/*
 * Methods Job object
 */
static void gearman_job_obj_free(zend_object *object) {
	gearman_job_obj *intern = gearman_job_fetch_object(object);
	if (!intern) {
		return;
	}

	if (intern->flags & GEARMAN_JOB_OBJ_CREATED) {
		gearman_job_free(intern->job);
	}

	zend_object_std_dtor(&intern->std);
}

static inline zend_object *gearman_job_obj_new(zend_class_entry *ce) {
	gearman_job_obj *intern = ecalloc(1,
		sizeof(gearman_job_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);

	intern->std.handlers = &gearman_job_obj_handlers;
	return &intern->std;
}

/*
 * Methods Task object
 */

static void gearman_task_obj_free(zend_object *object) {
	gearman_task_obj *intern = gearman_task_fetch_object(object);
	if (!intern) {
		return;
	}

	zval_dtor(&intern->zworkload);
	zval_dtor(&intern->zdata);
	zval_dtor(&intern->zclient);

	zend_object_std_dtor(&intern->std);
}

static PHP_METHOD(GearmanTask, __construct) {
}

static inline zend_object *gearman_task_obj_new(zend_class_entry *ce) {
	gearman_task_obj *intern = ecalloc(1,
		sizeof(gearman_task_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);

	intern->std.handlers = &gearman_task_obj_handlers;
	return &intern->std;
}

/* Function list. */
zend_function_entry gearman_functions[] = {
	/* Functions from gearman.h */
#if jluedke_0
        PHP_FE(gearman_return_code, arginfo_gearman_return_code)
#endif
	PHP_FE(gearman_version, arginfo_gearman_version)
	PHP_FE(gearman_bugreport, arginfo_gearman_bugreport)
	PHP_FE(gearman_verbose_name, arginfo_gearman_verbose_name)

#if jluedke_0
	PHP_FE(gearman_create, arginfo_gearman_create)
	PHP_FE(gearman_error, arginfo_gearman_error)
	PHP_FE(gearman_errno, arginfo_gearman_errno)
	PHP_FE(gearman_options, arginfo_gearman_options)
	PHP_FE(gearman_set_options, arginfo_gearman_set_options)
	PHP_FE(gearman_add_options, arginfo_gearman_add_options)
	PHP_FE(gearman_remove_options, arginfo_gearman_remove_options)
	PHP_FE(gearman_timeout, arginfo_gearman_timeout)
	PHP_FE(gearman_set_timeout, arginfo_gearman_set_timeout)
	PHP_FE(gearman_set_log_fn, arginfo_gearman_set_log_fn)
	PHP_FE(gearman_set_event_watch_fn, arginfo_gearman_set_event_watch_fn)

	PHP_FE(gearman_con_create, arginfo_gearman_con_create)
	PHP_FE(gearman_con_add, arginfo_gearman_con_add)
	PHP_FE(gearman_con_free_all, arginfo_gearman_con_free_all)
	PHP_FE(gearman_con_flush_all, arginfo_gearman_con_flush_all)
	PHP_FE(gearman_con_send_all, arginfo_gearman_con_send_all)
	PHP_FE(gearman_con_wait, arginfo_gearman_con_wait)
	PHP_FE(gearman_con_ready, arginfo_gearman_con_ready)
	PHP_FE(gearman_con_echo, arginfo_gearman_con_echo)

	PHP_FE(gearman_packet_create, arginfo_gearman_packet_create)
	PHP_FE(gearman_packet_add, arginfo_gearman_packet_add)
	PHP_FE(gearman_packet_free_all, arginfo_gearman_packet_free_all)
#endif

	/* Functions from conn.h */
#if jluedke_0
	PHP_FE(gearman_con_set_host, arginfo_gearman_con_set_host)
	PHP_FE(gearman_con_set_port, arginfo_gearman_con_set_port)
	PHP_FE(gearman_con_options, arginfo_gearman_con_options)
	PHP_FE(gearman_con_add_options, arginfo_gearman_con_add_options)
	PHP_FE(gearman_con_remove_options, arginfo_gearman_con_remove_options)
	PHP_FE(gearman_con_set_fd, arginfo_gearman_con_set_fd)
	PHP_FE(gearman_con_context, arginfo_gearman_con_context)
	PHP_FE(gearman_con_set_context, arginfo_gearman_con_set_context)
	PHP_FE(gearman_con_connect, arginfo_gearman_con_connect)
	PHP_FE(gearman_con_close, arginfo_gearman_con_close)
	PHP_FE(gearman_con_reset_addrinfo, arginfo_gearman_con_reset_addrinfo)
	PHP_FE(gearman_con_send, arginfo_gearman_con_send)
	PHP_FE(gearman_con_send_data, arginfo_gearman_con_send_data)
	PHP_FE(gearman_con_flush, arginfo_gearman_con_flush)
	PHP_FE(gearman_con_recv, arginfo_gearman_con_recv)
	PHP_FE(gearman_con_recv_data, arginfo_gearman_con_recv_data)
	PHP_FE(gearman_con_read, arginfo_gearman_con_read)
	PHP_FE(gearman_con_set_events, arginfo_gearman_con_set_events)
	PHP_FE(gearman_con_set_revents, arginfo_gearman_con_set_revents)
#endif

	/* Functions from packet.h */
#if jluedke_0
	PHP_FE(gearman_packet_add_arg, arginfo_gearman_packet_add_arg)
	PHP_FE(gearman_packet_pack_header, arginfo_gearman_packet_pack_header)
	PHP_FE(gearman_packet_unpack_header, arginfo_gearman_packet_unpack_header)
	PHP_FE(gearman_packet_pack, arginfo_gearman_packet_pack)
	PHP_FE(gearman_packet_unpack, arginfo_gearman_packet_unpack)
#endif

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
/*
#if jluedke_0
	PHP_FE(gearman_client_set_log_fn, arginfo_gearman_client_set_log_fn)
	PHP_FE(gearman_client_set_event_watch_fn, arginfo_gearman_client_set_event_watch_fn)
#endif
*/

	PHP_FE(gearman_client_add_server, arginfo_gearman_client_add_server)
	PHP_FE(gearman_client_add_servers, arginfo_gearman_client_add_servers)

/*
#if jluedke_0
	PHP_FE(gearman_client_remove_servers, arginfo_gearman_client_remove_servers)
#endif
*/

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
/*
#if jluedke_0
	PHP_FE(gearman_client_task_free_all, arginfo_gearman_client_task_free_all)
	PHP_FE(gearman_client_set_task_context_free_fn, arginfo_gearman_client_set_task_context_free_fn)
#endif
*/
	PHP_FE(gearman_client_add_task, arginfo_gearman_client_add_task)
	PHP_FE(gearman_client_add_task_high, arginfo_gearman_client_add_task_high)
	PHP_FE(gearman_client_add_task_low, arginfo_gearman_client_add_task_low)
	PHP_FE(gearman_client_add_task_background, arginfo_gearman_client_add_task_background)
	PHP_FE(gearman_client_add_task_high_background, arginfo_gearman_client_add_task_high_background)
	PHP_FE(gearman_client_add_task_low_background, arginfo_gearman_client_add_task_low_background)
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
/*
#if jluedke_0
	PHP_FE(gearman_task_context, arginfo_gearman_task_context)
	PHP_FE(gearman_task_set_context, arginfo_gearman_task_set_context)
#endif
*/
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
/*
#if jluedke_0
	PHP_FE(gearman_worker_context, arginfo_gearman_worker_context)
	PHP_FE(gearman_worker_set_context, arginfo_gearman_worker_set_context)
	PHP_FE(gearman_worker_set_log_fn, arginfo_gearman_worker_set_log_fn)
	PHP_FE(gearman_worker_set_event_watch_fn, arginfo_gearman_worker_set_event_watch_fn)
	PHP_FE(gearman_worker_remove_servers, arginfo_gearman_worker_remove_servers)
	PHP_FE(gearman_worker_job_free_all, arginfo_gearman_worker_job_free_all)
#endif
*/

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

	{NULL, NULL, NULL} /* Must be the last line in gearman_functions[] */
};

zend_function_entry gearman_methods[]= {
#if jluedke_0
	PHP_ME(gearman, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_return_code, arginfo_oo_gearman_return_code, 0)
	PHP_ME_MAPPING(error, gearman_error, arginfo_oo_gearman_error, 0)
	PHP_ME_MAPPING(getErrno, gearman_errno, arginfo_oo_gearman_errno, 0)
	PHP_ME_MAPPING(options, gearman_options, arginfo_oo_gearman_options, 0)
	PHP_ME_MAPPING(setOptions, gearman_set_options, arginfo_oo_gearman_set_options, 0)
	PHP_ME_MAPPING(addOptions, gearman_add_options, arginfo_oo_gearman_add_options, 0)
	PHP_ME_MAPPING(removeOptions, gearman_remove_options, arginfo_oo_gearman_remove_options, 0)
	PHP_ME_MAPPING(timeout, gearman_timeout, arginfo_oo_gearman_timeout, 0)
	PHP_ME_MAPPING(setTimeout, gearman_set_timeout, arginfo_oo_gearman_set_timeout, 0)
	PHP_ME_MAPPING(setLogCallback, gearman_set_log_callback, arginfo_oo_gearman_set_log_callback, 0)
	PHP_ME_MAPPING(setEventWatchCallback, gearman_set_event_watch_callback, arginfo_oo_gearman_set_event_watch_callback, 0)

	PHP_ME_MAPPING(conCreate, gearman_con_create, arginfo_oo_gearman_con_create, 0)
	PHP_ME_MAPPING(conAdd, gearman_con_add, arginfo_oo_gearman_con_add, 0)
	PHP_ME_MAPPING(conFreeAll, gearman_con_free_all, arginfo_oo_gearman_con_free_all, 0)
	PHP_ME_MAPPING(conFlushAll, gearman_con_flush_all, arginfo_oo_gearman_con_flush_all, 0)
	PHP_ME_MAPPING(conSendAll, gearman_con_send_all, arginfo_oo_gearman_con_send_all, 0)
	PHP_ME_MAPPING(conWait, gearman_con_wait, arginfo_oo_gearman_con_wait, 0)
	PHP_ME_MAPPING(conReady, gearman_con_ready, arginfo_oo_gearman_con_ready, 0)
	PHP_ME_MAPPING(conEcho, gearman_con_echo, arginfo_oo_gearman_con_echo, 0)

	PHP_ME_MAPPING(packetCreate, gearman_packet_create, arginfo_oo_gearman_packet_create, 0)
	PHP_ME_MAPPING(packetAdd, gearman_packet_add, arginfo_oo_gearman_packet_add, 0)
	PHP_ME_MAPPING(packetFreeAll, gearman_packet_free_all, arginfo_oo_gearman_packet_free_all, 0)
#endif

	{NULL, NULL, NULL}
};

#if jluedke_0
zend_function_entry gearman_con_methods[];
zend_function_entry gearman_packet_methods[];
#endif

static zend_function_entry gearman_client_methods[]= {
	ZEND_ME(GearmanClient, __construct, arginfo_gearman_client_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(returnCode, gearman_client_return_code, arginfo_oo_gearman_client_return_code, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(error, gearman_client_error, arginfo_oo_gearman_client_error, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(getErrno, gearman_client_get_errno, arginfo_oo_gearman_client_get_errno, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(options, gearman_client_options, arginfo_gearman_client_options, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setOptions, gearman_client_set_options, arginfo_gearman_client_set_options, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addOptions, gearman_client_add_options, arginfo_gearman_client_add_options, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(removeOptions, gearman_client_remove_options, arginfo_gearman_client_remove_options, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(timeout, gearman_client_timeout, arginfo_oo_gearman_client_timeout, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setTimeout, gearman_client_set_timeout, arginfo_oo_gearman_client_set_timeout, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(context, gearman_client_context, arginfo_oo_gearman_client_context, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setContext, gearman_client_set_context, arginfo_oo_gearman_client_set_context, ZEND_ACC_PUBLIC)
/*
#if jluedke_0
	PHP_ME_MAPPING(setLogCallback, gearman_client_set_log_callback, arginfo_oo_gearman_client_set_log_callback, 0)
	PHP_ME_MAPPING(setEventWatchCallback, gearman_client_set_event_watch_callback, arginfo_oo_gearman_client_set_event_watch_callback, 0)
#endif
*/
	ZEND_ME_MAPPING(addServer, gearman_client_add_server, arginfo_oo_gearman_client_add_server, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addServers, gearman_client_add_servers, arginfo_oo_gearman_client_add_servers, ZEND_ACC_PUBLIC)
/*
#if jluedke_0
	PHP_ME_MAPPING(removeServers, gearman_client_remove_servers, arginfo_oo_gearman_client_remove_servers, 0)
#endif
*/
	ZEND_ME_MAPPING(wait, gearman_client_wait, arginfo_oo_gearman_client_wait, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doNormal, gearman_client_do_normal, arginfo_oo_gearman_client_do_normal, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doHigh, gearman_client_do_high, arginfo_oo_gearman_client_do_high, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doLow, gearman_client_do_low, arginfo_oo_gearman_client_do_low, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doBackground, gearman_client_do_background, arginfo_oo_gearman_client_do_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doHighBackground, gearman_client_do_high_background, arginfo_oo_gearman_client_do_high_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doLowBackground, gearman_client_do_low_background, arginfo_oo_gearman_client_do_low_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doJobHandle, gearman_client_do_job_handle, arginfo_oo_gearman_client_do_job_handle, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(doStatus, gearman_client_do_status, arginfo_oo_gearman_client_do_status, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(jobStatus, gearman_client_job_status, arginfo_oo_gearman_client_job_status, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(jobStatusByUniqueKey, gearman_client_job_status_by_unique_key, arginfo_oo_gearman_client_job_status_by_unique_key, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(ping, gearman_client_ping, arginfo_oo_gearman_client_ping, ZEND_ACC_PUBLIC)
/*
#if jluedke_0
	PHP_ME_MAPPING(taskFreeAll, gearman_client_task_free_all, arginfo_oo_gearman_client_task_free_all, 0)
	PHP_ME_MAPPING(setTaskContextFreeCallback, gearman_client_set_context_free_fn, arginfo_oo_gearman_client_set_context_free_fn, 0)
#endif
*/
	ZEND_ME_MAPPING(addTask, gearman_client_add_task, arginfo_oo_gearman_client_add_task, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskHigh, gearman_client_add_task_high, arginfo_oo_gearman_client_add_task_high, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskLow, gearman_client_add_task_low, arginfo_gearman_client_add_task_low, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskBackground, gearman_client_add_task_background, arginfo_gearman_client_add_task_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskHighBackground, gearman_client_add_task_high_background, arginfo_gearman_client_add_task_high_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskLowBackground, gearman_client_add_task_low_background, arginfo_gearman_client_add_task_low_background, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(addTaskStatus, gearman_client_add_task_status, arginfo_oo_gearman_client_add_task_status, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setWorkloadCallback, gearman_client_set_workload_callback, arginfo_oo_gearman_client_set_workload_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setCreatedCallback, gearman_client_set_created_callback, arginfo_oo_gearman_client_set_created_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setDataCallback, gearman_client_set_data_callback, arginfo_oo_gearman_client_set_data_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setWarningCallback, gearman_client_set_warning_callback, arginfo_oo_gearman_client_set_warning_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setStatusCallback, gearman_client_set_status_callback, arginfo_oo_gearman_client_set_status_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setCompleteCallback, gearman_client_set_complete_callback, arginfo_oo_gearman_client_set_complete_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setExceptionCallback, gearman_client_set_exception_callback, arginfo_oo_gearman_client_set_exception_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(setFailCallback, gearman_client_set_fail_callback, arginfo_oo_gearman_client_set_fail_callback, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(clearCallbacks, gearman_client_clear_callbacks, arginfo_oo_gearman_client_clear_callbacks, ZEND_ACC_PUBLIC)
	ZEND_ME_MAPPING(runTasks, gearman_client_run_tasks, arginfo_oo_gearman_client_run_tasks, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

zend_function_entry gearman_task_methods[]= {
	PHP_ME(GearmanTask, __construct, arginfo_oo_gearman_task_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(returnCode, gearman_task_return_code, arginfo_oo_gearman_task_return_code, ZEND_ACC_PUBLIC)
#if jluedke_0
	PHP_ME_MAPPING(context, gearman_task_context, arginfo_oo_gearman_task_context, ZEND_ACC_PUBLIC)
	PHP_ME_MAPPING(setContext, gearman_task_set_context, arginfo_oo_gearman_task_set_context, ZEND_ACC_PUBLIC)
#endif
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

	{NULL, NULL, NULL}
};

zend_function_entry gearman_worker_methods[]= {
	PHP_ME(GearmanWorker, __construct, arginfo_oo_gearman_worker_construct, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
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
/*
#if jluedke_0
	PHP_ME_MAPPING(context, gearman_worker_context, arginfo_oo_gearman_worker_context, 0)
	PHP_ME_MAPPING(setContext, gearman_worker_set_context, arginfo_oo_gearman_worker_set_context, 0)
	PHP_ME_MAPPING(setLogCallback, gearman_worker_set_log_callback, arginfo_oo_gearman_worker_set_log_callback, 0)
	PHP_ME_MAPPING(setEventWatchCallback, gearman_worker_set_event_watch_callback, arginfo_oo_gearman_worker_set_event_watch_callback, 0)
	PHP_ME_MAPPING(removeServers, gearman_worker_remove_servers, arginfo_oo_gearman_worker_remove_servers, 0)
	PHP_ME_MAPPING(jobFreeAll, gearman_worker_job_free_all, arginfo_oo_gearman_worker_job_free_all, 0)
#endif
*/

	{NULL, NULL, NULL}
};

zend_function_entry gearman_job_methods[]= {
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

	{NULL, NULL, NULL}
};

zend_function_entry gearman_exception_methods[] = {
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(gearman) {
	zend_class_entry ce;

#if jluedke_0
	INIT_CLASS_ENTRY(ce, "Gearman", gearman_methods);
	ce.create_object= gearman_obj_new;
	gearman_ce= zend_register_internal_class_ex(&ce, NULL, NULL);
	memcpy(&gearman_obj_handlers, zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));
	gearman_obj_handlers.clone_obj= NULL; /* use our clone method */

	INIT_CLASS_ENTRY(ce, "GearmanCon", gearman_con_methods);
	ce.create_object= gearman_con_obj_new;
	gearman_con_ce= zend_register_internal_class_ex(&ce, NULL,
		NULL);
	memcpy(&gearman_con_obj_handlers, zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));
	gearman_con_obj_handlers.clone_obj= NULL; /* use our clone method */

	INIT_CLASS_ENTRY(ce, "GearmanPacket", gearman_packet_methods);
	ce.create_object= gearman_packet_obj_new;
	gearman_packet_ce= zend_register_internal_class_ex(&ce, NULL,
		NULL);
	memcpy(&gearman_packet_obj_handlers, zend_get_std_object_handlers(),
		sizeof(zend_object_handlers));
	gearman_packet_obj_handlers.clone_obj= NULL; /* use our clone method */
#endif

	INIT_CLASS_ENTRY(ce, "GearmanClient", gearman_client_methods);
	gearman_client_ce = zend_register_internal_class(&ce);
	gearman_client_ce->create_object = gearman_client_obj_new;
	memcpy(&gearman_client_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_client_obj_handlers));
	gearman_client_obj_handlers.offset = XtOffsetOf(gearman_client_obj, std);
	gearman_client_obj_handlers.free_obj = gearman_client_obj_free;

	INIT_CLASS_ENTRY(ce, "GearmanTask", gearman_task_methods);
	gearman_task_ce = zend_register_internal_class(&ce);
	gearman_task_ce->create_object = gearman_task_obj_new;
	memcpy(&gearman_task_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_task_obj_handlers));
	gearman_task_obj_handlers.offset = XtOffsetOf(gearman_task_obj, std);
	gearman_task_obj_handlers.free_obj = gearman_task_obj_free;

	INIT_CLASS_ENTRY(ce, "GearmanWorker", gearman_worker_methods);
	gearman_worker_ce = zend_register_internal_class(&ce);
	gearman_worker_ce->create_object = gearman_worker_obj_new;
	memcpy(&gearman_worker_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_worker_obj_handlers));
	gearman_worker_obj_handlers.offset = XtOffsetOf(gearman_worker_obj, std);
	gearman_worker_obj_handlers.free_obj = gearman_worker_obj_free;

	INIT_CLASS_ENTRY(ce, "GearmanJob", gearman_job_methods);
	gearman_job_ce = zend_register_internal_class(&ce);
	gearman_job_ce->create_object = gearman_job_obj_new;
	memcpy(&gearman_job_obj_handlers, zend_get_std_object_handlers(), sizeof(gearman_job_obj_handlers));
	gearman_job_obj_handlers.offset = XtOffsetOf(gearman_job_obj, std);
	gearman_job_obj_handlers.free_obj = gearman_job_obj_free;

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
#ifdef GEARMAN_VERBOSE_NEVER
	REGISTER_LONG_CONSTANT("GEARMAN_VERBOSE_NEVER",
		GEARMAN_VERBOSE_NEVER,
		CONST_CS | CONST_PERSISTENT);
#endif
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
