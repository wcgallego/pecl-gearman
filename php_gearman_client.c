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

#include "php_gearman_client.h"

inline gearman_client_obj *gearman_client_fetch_object(zend_object *obj) {
       return (gearman_client_obj *)((char*)(obj) - XtOffsetOf(gearman_client_obj, std));
}

inline zend_object *gearman_client_obj_new(zend_class_entry *ce) {
	gearman_client_obj *intern = ecalloc(1,
		sizeof(gearman_client_obj) +
		zend_object_properties_size(ce));

	zend_object_std_init(&(intern->std), ce);
	object_properties_init(&intern->std, ce);
	array_init(&intern->task_list);

	intern->std.handlers = &gearman_client_obj_handlers;
	return &intern->std;
}
