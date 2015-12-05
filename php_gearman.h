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

/* module version */
#define PHP_GEARMAN_VERSION "2.0.0-dev"

extern zend_module_entry gearman_module_entry;
#define phpext_gearman_ptr &gearman_module_entry

#endif  /* __PHP_GEARMAN_H */
