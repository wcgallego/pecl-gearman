/*
 * Gearman PHP Extension
 *
 * Copyright (C) 2008 James M. Luedke <contact@jamesluedke.com>,
 *                    Eric Day <eday@oddments.org>
 * All rights reserved.
 *
 * Use and distribution licensed under the PHP license.  See
 * the LICENSE file in this directory for full text.
 */

#ifndef __PHP_GEARMAN_H
#define __PHP_GEARMAN_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include <libgearman-1.0/gearman.h>
#include <libgearman-1.0/interface/status.h>
#include <libgearman-1.0/status.h>

/* module version */
#define PHP_GEARMAN_VERSION "2.0.6"

extern zend_module_entry gearman_module_entry;
#define phpext_gearman_ptr &gearman_module_entry

typedef enum {
        GEARMAN_OBJ_CREATED = (1 << 0)
} gearman_obj_flags_t;

extern zend_class_entry *gearman_exception_ce;

#define GEARMAN_EXCEPTION(__error, __error_code) { \
        zend_throw_exception(gearman_exception_ce, __error, __error_code); \
        return; \
}

void *_php_malloc(size_t size, void *arg);
void _php_free(void *ptr, void *arg);

#endif  /* __PHP_GEARMAN_H */
