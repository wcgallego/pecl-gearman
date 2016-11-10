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

#include "php_gearman_job.h"

gearman_job_obj *gearman_job_fetch_object(zend_object *obj) {
        return (gearman_job_obj *)((char*)(obj) - XtOffsetOf(gearman_job_obj, std));
}

/* {{{ proto object GearmanJob::__destruct()
   cleans up GearmanJob object */
PHP_METHOD(GearmanJob, __destruct) {
        gearman_job_obj *intern = Z_GEARMAN_JOB_P(getThis());
        if (!intern) {
                return;
        }    

        if (intern->flags & GEARMAN_JOB_OBJ_CREATED) {
                gearman_job_free(intern->job);
        }    

        zend_object_std_dtor(&intern->std);
}

zend_object *gearman_job_obj_new(zend_class_entry *ce) {
        gearman_job_obj *intern = ecalloc(1,
                sizeof(gearman_job_obj) +
                zend_object_properties_size(ce));

        zend_object_std_init(&(intern->std), ce); 
        object_properties_init(&intern->std, ce); 

        intern->std.handlers = &gearman_job_obj_handlers;
        return &intern->std;
}

/* {{{ proto int gearman_job_return_code()
   get last gearman_return_t */
PHP_FUNCTION(gearman_job_return_code) {
        gearman_job_obj *obj;
        zval *zobj;

        if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zobj, gearman_job_ce) == FAILURE) {
                RETURN_NULL();
        }
        obj = Z_GEARMAN_JOB_P(zobj);

        RETURN_LONG(obj->ret);
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
