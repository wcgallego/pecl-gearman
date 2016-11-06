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
