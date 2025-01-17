/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2011 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_stack.h 306939 2011-01-01 02:19:59Z felipe $ */

#ifndef ZEND_STACK_H
#define ZEND_STACK_H

typedef struct _zend_stack {
	int top, max;
	void **elements;
} zend_stack;


#define STACK_BLOCK_SIZE 64

BEGIN_EXTERN_C()
ZEND_API int zend_stack_init(zend_stack *stack);
ZEND_API int zend_stack_push(zend_stack *stack, const void *element, int size);
ZEND_API int zend_stack_top(const zend_stack *stack, void **element);
ZEND_API int zend_stack_del_top(zend_stack *stack);
ZEND_API int zend_stack_int_top(const zend_stack *stack);
ZEND_API int zend_stack_is_empty(const zend_stack *stack);
ZEND_API int zend_stack_destroy(zend_stack *stack);
ZEND_API void **zend_stack_base(const zend_stack *stack);
ZEND_API int zend_stack_count(const zend_stack *stack);
ZEND_API void zend_stack_apply(zend_stack *stack, int type, int (*apply_function)(void *element));
ZEND_API void zend_stack_apply_with_argument(zend_stack *stack, int type, int (*apply_function)(void *element, void *arg), void *arg);
END_EXTERN_C()

#define ZEND_STACK_APPLY_TOPDOWN	1
#define ZEND_STACK_APPLY_BOTTOMUP	2

#endif /* ZEND_STACK_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
