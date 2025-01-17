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
   | Author: Paul Panotzki - Bunyip Information Systems                   |
   +----------------------------------------------------------------------+
*/

/* $Id: microtime.h 306939 2011-01-01 02:19:59Z felipe $ */

#ifndef MICROTIME_H
#define MICROTIME_H

#ifdef HAVE_GETTIMEOFDAY
PHP_FUNCTION(microtime);
PHP_FUNCTION(gettimeofday);
#endif
#ifdef HAVE_GETRUSAGE
PHP_FUNCTION(getrusage);
#endif

#endif /* MICROTIME_H */
