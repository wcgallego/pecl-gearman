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

#ifndef __PHP_GEARMAN_JOB_H
#define __PHP_GEARMAN_JOB_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_gearman.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>

extern zend_class_entry *gearman_job_ce;
extern zend_object_handlers gearman_job_obj_handlers;

zend_object *gearman_job_obj_new(zend_class_entry *ce);

typedef enum {
        GEARMAN_JOB_OBJ_CREATED = (1 << 0)
} gearman_job_obj_flags_t;

typedef struct {
        gearman_return_t ret; 
        gearman_job_obj_flags_t flags;
        gearman_job_st *job;

        zend_object std; 
} gearman_job_obj;

gearman_job_obj *gearman_job_fetch_object(zend_object *obj);
#define Z_GEARMAN_JOB_P(zv) gearman_job_fetch_object(Z_OBJ_P((zv)))

PHP_METHOD(GearmanJob, __destruct);
PHP_FUNCTION(gearman_job_return_code);
PHP_FUNCTION(gearman_job_set_return);
PHP_FUNCTION(gearman_job_send_data);
PHP_FUNCTION(gearman_job_send_warning);
PHP_FUNCTION(gearman_job_send_status);
PHP_FUNCTION(gearman_job_send_complete);
PHP_FUNCTION(gearman_job_send_exception);
PHP_FUNCTION(gearman_job_send_fail);
PHP_FUNCTION(gearman_job_handle);
PHP_FUNCTION(gearman_job_function_name);
PHP_FUNCTION(gearman_job_unique);
PHP_FUNCTION(gearman_job_workload);
PHP_FUNCTION(gearman_job_workload_size);

#endif  /* __PHP_GEARMAN_JOB_H */
