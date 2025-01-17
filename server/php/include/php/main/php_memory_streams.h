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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id: php_memory_streams.h 306939 2011-01-01 02:19:59Z felipe $ */

#ifndef PHP_MEMORY_STREAM_H
#define PHP_MEMORY_STREAM_H

#include "php_streams.h"

#define PHP_STREAM_MAX_MEM	2 * 1024 * 1024

#define TEMP_STREAM_DEFAULT  0
#define TEMP_STREAM_READONLY 1
#define TEMP_STREAM_TAKE_BUFFER 2

#define php_stream_memory_create(mode) _php_stream_memory_create((mode) STREAMS_CC TSRMLS_CC)
#define php_stream_memory_create_rel(mode) _php_stream_memory_create((mode) STREAMS_REL_CC TSRMLS_CC)
#define php_stream_memory_open(mode, buf, length) _php_stream_memory_open((mode), (buf), (length) STREAMS_CC TSRMLS_CC)
#define php_stream_memory_get_buffer(stream, length) _php_stream_memory_get_buffer((stream), (length) STREAMS_CC TSRMLS_CC)

#define php_stream_temp_new() php_stream_temp_create(TEMP_STREAM_DEFAULT, PHP_STREAM_MAX_MEM)
#define php_stream_temp_create(mode, max_memory_usage) _php_stream_temp_create((mode), (max_memory_usage) STREAMS_CC TSRMLS_CC)
#define php_stream_temp_create_rel(mode, max_memory_usage) _php_stream_temp_create((mode), (max_memory_usage) STREAMS_REL_CC TSRMLS_CC)
#define php_stream_temp_open(mode, max_memory_usage, buf, length) _php_stream_temp_open((mode), (max_memory_usage), (buf), (length) STREAMS_CC TSRMLS_CC)

BEGIN_EXTERN_C()
PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC TSRMLS_DC);
PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC TSRMLS_DC);
PHPAPI char *_php_stream_memory_get_buffer(php_stream *stream, size_t *length STREAMS_DC TSRMLS_DC);

PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC TSRMLS_DC);
PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC TSRMLS_DC);
END_EXTERN_C()

extern PHPAPI php_stream_ops php_stream_memory_ops;
extern PHPAPI php_stream_ops php_stream_temp_ops;
extern PHPAPI php_stream_ops php_stream_rfc2397_ops;
extern PHPAPI php_stream_wrapper php_stream_rfc2397_wrapper;

#define PHP_STREAM_IS_MEMORY &php_stream_memory_ops
#define PHP_STREAM_IS_TEMP   &php_stream_temp_ops

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
