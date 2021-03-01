#ifndef _TEST_H
#define _TEST_H

#include <stdio.h>

#define LOG_LN() printf("\n")
#define LOG_MSG(msg) printf(msg)
#define LOG_FMT(fmt, ...) printf("\t" fmt "\n", __VA_ARGS__)

typedef enum {
	TEST_OK,
	TEST_FAIL,
	TEST_CONFIRM,
	TEST_NOTIMPLEMENTED,
} test_return_t;

typedef test_return_t (*test_t)(void);

#endif /* _TEST_H */
