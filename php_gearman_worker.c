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

