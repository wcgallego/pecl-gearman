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
