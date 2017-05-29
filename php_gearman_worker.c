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

#include "php_gearman_worker.h"

gearman_worker_obj *gearman_worker_fetch_object(zend_object *obj) {
        return (gearman_worker_obj *)((char*)(obj) - XtOffsetOf(gearman_worker_obj, std));
}

/* {{{ proto object gearman_worker_ctor()
   Initialize a worker object.	*/
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
PHP_METHOD(GearmanWorker, __construct) {
	return_value = getThis();
	gearman_worker_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object GearmanWorker::__destruct()
   Destroys a worker object */
PHP_METHOD(GearmanWorker, __destruct) {
	gearman_worker_obj *intern = Z_GEARMAN_WORKER_P(getThis());

	if (!intern)  {
		return;
	}

	if (intern->flags & GEARMAN_WORKER_OBJ_CREATED) {
		gearman_worker_free(&(intern->worker));
	}

	zval_dtor(&intern->cb_list);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

static inline void cb_list_dtor(zval *zv) {
	gearman_worker_cb_obj *worker_cb = Z_PTR_P(zv);
	zval_dtor(&worker_cb->zname);
	zval_dtor(&worker_cb->zdata);
	zval_dtor(&worker_cb->zcall);
	efree(worker_cb);
}

zend_object *gearman_worker_obj_new(zend_class_entry *ce) {
	gearman_worker_obj *intern = ecalloc(1,
		sizeof(gearman_worker_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);

	ZVAL_NEW_ARR(&intern->cb_list);
	zend_hash_init(Z_ARRVAL(intern->cb_list), 0, NULL, cb_list_dtor, 0);

	intern->std.handlers = &gearman_worker_obj_handlers;
	return &intern->std;
}

/* {{{ proto int gearman_worker_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_worker_return_code) {
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
   Note: EXPERIMENTAL - This is undocumented on php.net and needs a test*/
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
        gearman_worker_cb_obj *worker_cb = (gearman_worker_cb_obj *)context;
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
                ZVAL_COPY(&argv[1], &worker_cb->zdata);
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

        if (!Z_ISUNDEF(argv[0])) {
                zval_dtor(&argv[0]);
        }

        if (!Z_ISUNDEF(argv[1])) {
                zval_dtor(&argv[1]);
        }


        return result;
}
/* }}} */

/* {{{ proto bool gearman_worker_add_function(object worker, zval function_name, zval callback [, zval data [, int timeout]])
   Register and add callback function for worker. */
PHP_FUNCTION(gearman_worker_add_function) {
        zval *zobj = NULL;
        gearman_worker_obj *obj;
        gearman_worker_cb_obj *worker_cb;

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
                php_error_docref(NULL, E_WARNING, "Function name must be a string");
                RETURN_FALSE;
        }

        /* check that the function can be called */
        if (!zend_is_callable(zcall, 0, &callable)) {
                php_error_docref(NULL, E_WARNING, "Function '%s' is not a valid callback", ZSTR_VAL(callable));
                zend_string_release(callable);
                RETURN_FALSE;
        }

        zend_string_release(callable);

        /* create a new worker cb */
        worker_cb = emalloc(sizeof(gearman_worker_cb_obj));

        // Name of the callback function
        ZVAL_COPY(&worker_cb->zname, zname);

        // Reference to the callback function
        ZVAL_COPY(&worker_cb->zcall, zcall);

        // Additional data passed along to the callback function
        if (zdata) {
                ZVAL_COPY(&worker_cb->zdata,zdata);
        } else {
                ZVAL_NULL(&worker_cb->zdata);
        }

        // Add the worker_cb to the list
        zend_hash_next_index_insert_ptr(Z_ARRVAL(obj->cb_list), worker_cb);

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
