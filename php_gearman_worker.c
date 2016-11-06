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
