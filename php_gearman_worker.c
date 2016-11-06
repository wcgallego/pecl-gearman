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
