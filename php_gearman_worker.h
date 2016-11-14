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

#ifndef __PHP_GEARMAN_WORKER_H
#define __PHP_GEARMAN_WORKER_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_gearman.h"
#include "php_gearman_job.h"
#include "php_gearman_client.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>

extern zend_class_entry *gearman_worker_ce;
extern zend_object_handlers gearman_worker_obj_handlers;

zend_object *gearman_worker_obj_new(zend_class_entry *ce);

typedef struct {
        zval zname; /* name associated with callback */
        zval zcall; /* name of callback */
        zval zdata; /* data passed to callback via worker */
} gearman_worker_cb_obj;

typedef enum {
        GEARMAN_WORKER_OBJ_CREATED = (1 << 0)
} gearman_worker_obj_flags_t;

typedef struct {
        gearman_return_t ret; 
        gearman_worker_obj_flags_t flags;
        gearman_worker_st worker;
        zval cb_list;

        zend_object std; 
} gearman_worker_obj;

gearman_worker_obj *gearman_worker_fetch_object(zend_object *obj);
#define Z_GEARMAN_WORKER_P(zv) gearman_worker_fetch_object(Z_OBJ_P((zv)))

PHP_FUNCTION(gearman_worker_create);
PHP_METHOD(GearmanWorker, __construct);
PHP_METHOD(GearmanWorker, __destruct);
PHP_FUNCTION(gearman_worker_return_code);
PHP_FUNCTION(gearman_worker_error);
PHP_FUNCTION(gearman_worker_errno);
PHP_FUNCTION(gearman_worker_options);
PHP_FUNCTION(gearman_worker_set_options);
PHP_FUNCTION(gearman_worker_add_options);
PHP_FUNCTION(gearman_worker_remove_options);
PHP_FUNCTION(gearman_worker_timeout);
PHP_FUNCTION(gearman_worker_set_timeout);
PHP_FUNCTION(gearman_worker_set_id);
PHP_FUNCTION(gearman_worker_add_server);
PHP_FUNCTION(gearman_worker_add_servers);
PHP_FUNCTION(gearman_worker_wait);
PHP_FUNCTION(gearman_worker_register);
PHP_FUNCTION(gearman_worker_unregister);
PHP_FUNCTION(gearman_worker_unregister_all);
PHP_FUNCTION(gearman_worker_grab_job);
PHP_FUNCTION(gearman_worker_add_function);
PHP_FUNCTION(gearman_worker_work);
PHP_FUNCTION(gearman_worker_ping);

#endif  /* __PHP_GEARMAN_WORKER_H */
