#!/bin/sh
#
# Gearman PHP Extension
#
# Copyright (C) 2008 James M. Luedke <contact@jamesluedke.com>,
#                    Eric Day <eday@oddments.org>
# All rights reserved.
#
# Use and distribution licensed under the PHP license.  See
# the LICENSE file in this directory for full text.

if [ "$1" = "" ]
then
  header=/usr/local/include/libgearman/constants.h
else
  header=$1
fi

if [ ! -e $header ]
then
  echo "$header does not exist"
  exit 1;
fi

if [ -e php_gearman.c.new ]
then
  echo "php_gearman.c.new already exists"
  exit 1;
fi

awk 'BEGIN { p= 1; } \
     /CONST_GEN_START/ { p= 0; print $0; } \
     { if (p == 1) { print $0; } }' php_gearman.c >> php_gearman.c.new

grep ' GEARMAN' $header | \
  sed 's/.*\(GEARMAN[A-Z0-9_]*\).*/\1/' | \
  sed 's/\(.*\)/	REGISTER_LONG_CONSTANT("\1",\
		\1,\
		CONST_CS | CONST_PERSISTENT);/' | \
  sed 's/LONG\(.*GEARMAN_DEFAULT_TCP_HOST\)/STRING\1/' | \
  sed 's/LONG\(.*GEARMAN_DEFAULT_UDS\)/STRING\1/' | \
  sed 's/LONG\(.*GEARMAN_DEFAULT_USER\)/STRING\1/' >> php_gearman.c.new

awk 'BEGIN { p= 0; } \
     /CONST_GEN_STOP/ { p= 1; } \
     { if (p == 1) { print $0; } }' php_gearman.c >> php_gearman.c.new

echo "New source file can be found in: php_gearman.c.new"

exit 0
