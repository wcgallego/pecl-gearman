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

        intern->std.handlers = &gearman_task_obj_handlers;
        return &intern->std;
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
