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

/* $Id: php_ext_syslog.h 306939 2011-01-01 02:19:59Z felipe $ */

#ifndef PHP_EXT_SYSLOG_H
#define PHP_EXT_SYSLOG_H

#ifdef HAVE_SYSLOG_H

#include "php_syslog.h"

PHP_MINIT_FUNCTION(syslog);
PHP_RINIT_FUNCTION(syslog);
#ifdef PHP_WIN32
PHP_RSHUTDOWN_FUNCTION(syslog);
#endif
PHP_MSHUTDOWN_FUNCTION(syslog);

PHP_FUNCTION(openlog);
PHP_FUNCTION(syslog);
PHP_FUNCTION(closelog);
PHP_FUNCTION(define_syslog_variables);

#endif

#endif /* PHP_EXT_SYSLOG_H */
