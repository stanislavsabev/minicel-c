#include <stdint.h>

#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t
#define usize size_t

#define i64 int64_t
#define i32 int32_t
#define i16 int16_t
#define i8  int8_t

#define f64 double
#define f32 float


typedef struct {
    size_t len;
    char* data;
} Str;


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define NULL ((void*)0)