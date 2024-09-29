#ifndef CDC_DEF_H
#define CDC_DEF_H

#include <stdbool.h>

// Typedefs
typedef int dk_err_t;  // Used for error handling

// Macros
#define DK_IMPL_UNUSED0()
#define DK_IMPL_UNUSED1(a) (void) (a)
#define DK_IMPL_UNUSED2(a, b) (void) (a), DK_IMPL_UNUSED1(b)
#define DK_IMPL_UNUSED3(a, b, c) (void) (a), DK_IMPL_UNUSED2(b, c)
#define DK_IMPL_UNUSED4(a, b, c, d) (void) (a), DK_IMPL_UNUSED3(b, c, d)
#define DK_IMPL_UNUSED5(a, b, c, d, e) (void) (a), DK_IMPL_UNUSED4(b, c, d, e)

#define DK_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define DK_VA_NUM_ARGS(...) \
	DK_VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define DK_UNUSED_IMPL_(nargs) DK_IMPL_UNUSED##nargs
#define DK_UNUSED_IMPL(nargs) DK_UNUSED_IMPL_(nargs)
#define DK_UNUSED(...) DK_UNUSED_IMPL(DK_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif
