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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Ard Biesheuvel <ard@ard.nu>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_multiply.h 306939 2011-01-01 02:19:59Z felipe $ */

#if defined(__i386__) && defined(__GNUC__)

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, dval, usedval) do {	\
	long __tmpvar; 													\
	__asm__ ("imul %3,%0\n"											\
		"adc $0,%1" 												\
			: "=r"(__tmpvar),"=r"(usedval) 							\
			: "0"(a), "r"(b), "1"(0));								\
	if (usedval) (dval) = (double) (a) * (double) (b);				\
	else (lval) = __tmpvar;											\
} while (0)

#elif SIZEOF_LONG == 4 && defined(HAVE_ZEND_LONG64)

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, dval, usedval) do {	\
	zend_long64 __result = (zend_long64) (a) * (zend_long64) (b);	\
	if (__result > LONG_MAX || __result < LONG_MIN) {				\
		(dval) = (double) __result;									\
		(usedval) = 1;												\
	} else {														\
		(lval) = (long) __result;									\
		(usedval) = 0;												\
	}																\
} while (0)

#else

#define ZEND_SIGNED_MULTIPLY_LONG(a, b, lval, dval, usedval) do {		\
	long   __lres  = (a) * (b);											\
	long double __dres  = (long double)(a) * (long double)(b);							\
	long double __delta = (long double) __lres - __dres;							\
	if ( ((usedval) = (( __dres + __delta ) != __dres))) {				\
		(dval) = __dres;												\
	} else {															\
		(lval) = __lres;												\
	}																	\
} while (0)

#endif
