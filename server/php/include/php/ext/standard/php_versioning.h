/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig S\ufffdther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: php_versioning.h 306939 2011-01-01 02:19:59Z felipe $ */

#ifndef PHP_VERSIONING_H
#define PHP_VERSIONING_H

#include "ext/standard/basic_functions.h"

PHPAPI char *php_canonicalize_version(const char *);
PHPAPI int php_version_compare(const char *, const char *);
PHP_FUNCTION(version_compare);

#endif
