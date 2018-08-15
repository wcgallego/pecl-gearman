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

#include "php_gearman_task.h"
#include "php_gearman_client.h"

inline gearman_client_obj *gearman_client_fetch_object(zend_object *obj) {
       return (gearman_client_obj *)((char*)(obj) - XtOffsetOf(gearman_client_obj, std));
}

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
        gearman_client_set_task_context_free_fn(&(client->client), _php_task_free);
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

inline zend_object *gearman_client_obj_new(zend_class_entry *ce) {
	gearman_client_obj *intern = ecalloc(1,
		sizeof(gearman_client_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);
	array_init(&intern->task_list);
	intern->created_tasks = 0;

	intern->std.handlers = &gearman_client_obj_handlers;
	return &intern->std;
}

/* {{{ proto object GearmanClient::__construct()
   Returns a GearmanClient object */
PHP_METHOD(GearmanClient, __construct)
{
        return_value = getThis();
        gearman_client_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);

}
/* }}} */

/* {{{ proto object GearmanClient::__destruct()
   cleans up GearmanClient object */
PHP_METHOD(GearmanClient, __destruct)
{
        char *context = NULL;
        gearman_client_obj *intern = Z_GEARMAN_CLIENT_P(getThis());
        if (!intern) {
                return;
        }

        context = gearman_client_context(&(intern->client));
        efree(context);

        if (intern->flags & GEARMAN_CLIENT_OBJ_CREATED) {
                gearman_client_free(&intern->client);
        }

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
   Set options for a client structure.
   NOTE: this is deprecated in gearmand */
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

/* {{{ proto bool gearman_client_add_server(object client [, string host [, int
 * port [, bool setupExceptionHandler ]]])
   Add a job server to a client. This goes into a list of servers than can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_client_add_server) {
        char *host = NULL;
        size_t host_len = 0;
        zend_long port = 0;
        zend_bool setupExceptionHandler = 1;

        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|slb",
					&zobj,
					gearman_client_ce,
					&host,
					&host_len,
					&port,
					&setupExceptionHandler) == FAILURE) {
                RETURN_FALSE;
        }            
        obj = Z_GEARMAN_CLIENT_P(zobj);

        obj->ret = gearman_client_add_server(&(obj->client), host, port);
        if (obj->ret != GEARMAN_SUCCESS) {
                php_error_docref(NULL, E_WARNING, "%s",
                                                 gearman_client_error(&(obj->client)));
                RETURN_FALSE;                         
        }

        if (setupExceptionHandler && !gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
                GEARMAN_EXCEPTION("Failed to set exception option", 0);
        }

        RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool gearman_client_add_servers(object client [, string servers [, bool setupExceptionHandler ]])
   Add a list of job servers to a client. This goes into a list of servers that can be used to run tasks. No socket I/O happens here, it is just added to a list. */
PHP_FUNCTION(gearman_client_add_servers) {
        char *servers = NULL;
        size_t servers_len = 0;
        zend_bool setupExceptionHandler = 1;

        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|sb",
							&zobj,
							gearman_client_ce,
							&servers,
							&servers_len,
							&setupExceptionHandler) == FAILURE) {
                RETURN_FALSE;
        }
        obj = Z_GEARMAN_CLIENT_P(zobj);

        obj->ret = gearman_client_add_servers(&(obj->client), servers);
        if (obj->ret != GEARMAN_SUCCESS) {
                php_error_docref(NULL, E_WARNING, "%s",
                                                 gearman_client_error(&(obj->client)));
                RETURN_FALSE;
        }

        if (setupExceptionHandler && !gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
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
                                                                const void *workload,
                                                                size_t workload_size,
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

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss|s!", &zobj, gearman_client_ce,
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

        if (Z_TYPE_P(zworkload) != IS_STRING) {
                convert_to_string(zworkload);
        }

        /* get a task object, and prepare it for return */
        if (object_init_ex(return_value, gearman_task_ce) != SUCCESS) {
                php_error_docref(NULL, E_WARNING, "GearmanTask Object creation failure.");
                RETURN_FALSE;
        }

        task = Z_GEARMAN_TASK_P(return_value);

        if (zdata) {
                ZVAL_COPY(&task->zdata, zdata);
        }

        ZVAL_COPY(&task->zworkload, zworkload);

        /* need to store a ref to the client for later access to cb's */
        ZVAL_COPY(&task->zclient, zobj);

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
        task->task_id = ++obj->created_tasks;

        // prepend task to list of tasks on client obj
        Z_ADDREF_P(return_value);
        add_index_zval(&obj->task_list, task->task_id, return_value);
}
/* }}} */

/* {{{ proto object GearmanClient::addTask(string function, zval workload [, string unique ])
   Add a task to be run in parallel. */
PHP_FUNCTION(gearman_client_add_task) {
        gearman_client_add_task_handler(gearman_client_add_task, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

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

/* {{{ proto object GearmanClient::addTaskBackground(string function, zval workload [, string unique ])
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

/* {{{ proto bool gearman_client_run_tasks(object client)
   Run tasks that have been added in parallel */
PHP_FUNCTION(gearman_client_run_tasks) {
        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
                RETURN_FALSE;
        }    
        obj = Z_GEARMAN_CLIENT_P(zobj);

        obj->ret = gearman_client_run_tasks(&(obj->client));

        if (! PHP_GEARMAN_CLIENT_RET_OK(obj->ret)) {
                php_error_docref(NULL, E_WARNING, "%s",
                                                 gearman_client_error(&(obj->client)));
                RETURN_FALSE;
        }    

        RETURN_TRUE;
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
                   ZVAL_COPY(&task->zdata, zdata);
        }    
        /* need to store a ref to the client for later access to cb's */
        ZVAL_COPY(&task->zclient, zobj);

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
	ZVAL_COPY(&obj->zworkload_fn, zworkload_fn);

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
	ZVAL_COPY(&obj->zcreated_fn, zcreated_fn);

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
	ZVAL_COPY(&obj->zdata_fn, zdata_fn);

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
	ZVAL_COPY(&obj->zwarning_fn, zwarning_fn);

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
	ZVAL_COPY(&obj->zstatus_fn, zstatus_fn);

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
	ZVAL_COPY(&obj->zcomplete_fn, zcomplete_fn);

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

        if (!gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
                GEARMAN_EXCEPTION("Failed to set exception option", 0);
        }

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
	ZVAL_COPY(&obj->zexception_fn, zexception_fn);

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

	/* store the cb in client object */
	ZVAL_COPY(&obj->zfail_fn, zfail_fn);

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
        const char *data;
        int length = 0;

        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
                RETURN_FALSE;
        }
        obj = Z_GEARMAN_CLIENT_P(zobj);

        data = gearman_client_context(&(obj->client));

        if (data) {
                length = strlen(data);
        }

        RETURN_STRINGL(data, length);
}
/* }}} */

/* {{{ proto bool GearmanClient::setContext(string data)
   Set the application data */
PHP_FUNCTION(gearman_client_set_context) {
        char *data, *old_context;
        size_t data_len = 0;

        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os", &zobj, gearman_client_ce, &data, &data_len) == FAILURE) {
                RETURN_FALSE;
        }
        obj = Z_GEARMAN_CLIENT_P(zobj);

        old_context = gearman_client_context(&(obj->client));
        efree(old_context);

        gearman_client_set_context(&(obj->client), (void*) estrndup(data, data_len));
        RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool GearmanClient::enableExceptionHandler()
   Enable exception handling to be used by exception callback function
   GearmanClient::setExceptionCallback */
PHP_FUNCTION(gearman_client_enable_exception_handler) {
        gearman_client_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_client_ce) == FAILURE) {
                RETURN_FALSE;
        }
        obj = Z_GEARMAN_CLIENT_P(zobj);

        if (!gearman_client_set_server_option(&(obj->client), "exceptions", (sizeof("exceptions") - 1))) {
                GEARMAN_EXCEPTION("Failed to set exception option", 0);
        }

        RETURN_TRUE;
}
/* }}} */
