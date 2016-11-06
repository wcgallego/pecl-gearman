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

inline gearman_task_obj *gearman_task_fetch_object(zend_object *obj) {
        return (gearman_task_obj *)((char*)(obj) - XtOffsetOf(gearman_task_obj, std));
}

inline zend_object *gearman_task_obj_new(zend_class_entry *ce) {
        gearman_task_obj *intern = ecalloc(1,
                sizeof(gearman_task_obj) +
                zend_object_properties_size(ce));

        zend_object_std_init(&(intern->std), ce); 
        object_properties_init(&intern->std, ce); 
        intern->task_id = 0;

        intern->std.handlers = &gearman_task_obj_handlers;
        return &intern->std;
}

/* this function will be used to call our user defined task callbacks */
gearman_return_t _php_task_cb_fn(gearman_task_obj *task, gearman_client_obj *client, zval zcall) {
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

void _php_task_free(gearman_task_st *task, void *context) {
	gearman_task_obj *task_obj= (gearman_task_obj *) context;
	gearman_client_obj *cli_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
	task_obj->flags &= ~GEARMAN_TASK_OBJ_CREATED;
	zend_hash_index_del(Z_ARRVAL(cli_obj->task_list), task_obj->task_id);
}

/* TODO: clean this up a bit, Macro? */
gearman_return_t _php_task_workload_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zworkload_fn);
}

gearman_return_t _php_task_created_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zcreated_fn);
}

gearman_return_t _php_task_data_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zdata_fn);
}

gearman_return_t _php_task_warning_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zwarning_fn);
}

gearman_return_t _php_task_status_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zstatus_fn);
}

gearman_return_t _php_task_complete_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zcomplete_fn);
}

gearman_return_t _php_task_exception_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zexception_fn);
}

gearman_return_t _php_task_fail_fn(gearman_task_st *task) {
        gearman_task_obj *task_obj = (gearman_task_obj *) gearman_task_context(task);
        gearman_client_obj *client_obj = Z_GEARMAN_CLIENT_P(&task_obj->zclient);
        return _php_task_cb_fn(task_obj, client_obj, client_obj->zfail_fn);
}

/* {{{ proto int gearman_task_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_task_return_code) {
        zval *zobj;
        gearman_task_obj *obj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_task_ce) == FAILURE) {
                RETURN_NULL();
        }    
        obj = Z_GEARMAN_TASK_P(zobj);

        RETURN_LONG(obj->ret);
}
/* }}} */

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
