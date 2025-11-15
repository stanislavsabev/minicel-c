#ifndef FX_H
#define FX_H

#include <assert.h>
#include <stddef.h> /* for size_t, offsetof, wchar_t */
#include <stdint.h>
#include <stdlib.h>

/* ============================================================
   Alias declarations
   ============================================================ */

typedef uint64_t u64;
/* Unsigned integers aliases */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Signed integers aliases */
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* Signed bool aliases */
typedef int8_t b8;
typedef int16_t b16;
typedef int32_t b32;
typedef int64_t b64;

/* Floating point */
typedef float f32;
typedef double f64;

/* Other common aliases */
typedef size_t usize;
typedef wchar_t wchar;

/* Enum aliases */
#define enum8(type) u8
#define enum16(type) u16
#define enum32(type) u32
#define enum64(type) u64

/* ============================================================
   Struct declarations
   ============================================================ */

typedef struct Str {
  size_t len;
  char *data;
} Str;

/* ============================================================
    Alignment helper (portable)
    ============================================================ */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#include <stdalign.h>
#define ALIGNOF(t) alignof(t)
#elif defined(__GNUC__)
#define ALIGNOF(t) __alignof__(t)
#elif defined(_MSC_VER)
#define ALIGNOF(t) __alignof(t)
#else
/* Fallback using offsetof trick */
#define ALIGNOF(t)                                                             \
  offsetof(                                                                    \
      struct {                                                                 \
        char _c;                                                               \
        t _m;                                                                  \
      },                                                                       \
      _m)
#endif

/* ============================================================
   Arena Memory Allocator
   ============================================================ */

typedef struct {
  void *buffer;
  usize capacity;
  usize offset;
  usize alignment;
} Arena;

/* Create a new arena with given capacity */
Arena *arena_create(usize capacity, usize alignment);

/* Allocate memory from arena */
void *arena_alloc(Arena *arena, usize size);

/* Allocate aligned memory from arena */
void *arena_alloc_aligned(Arena *arena, usize size, usize align);

/* Reset arena (doesn't free, just resets offset) */
void arena_reset(Arena *arena);

/* Free arena and its buffer */
void arena_destroy(Arena *arena);

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define NULL ((void *)0)

/* ============================================================
   Function declarations
   ============================================================ */

/* Your function declarations here */
int fx_example(int x);

/* ============================================================
   Implementation
   ============================================================ */

#ifdef FX_IMPLEMENTATION

/* Helper: Align pointer to boundary */
static inline usize align_forward(usize value, usize align) {
  assert(align != 0); /* catch obvious misuse in debug builds */

  /* Fast path for power-of-two alignment using bit tricks */
  if ((align & (align - 1)) == 0) {
    return (value + (align - 1)) & ~(align - 1);
  }

  /* Fallback for non-power-of-two alignments (portable, slightly slower) */
  return ((value + (align - 1)) / align) * align;
}

Arena *arena_create(usize capacity, usize alignment) {
  Arena *arena = (Arena *)malloc(sizeof(Arena));
  if (!arena)
    return NULL;

  arena->buffer = (char *)malloc(capacity);
  if (!arena->buffer) {
    free(arena);
    return NULL;
  }

  arena->capacity = capacity;
  arena->offset = 0;
  arena->alignment = alignment > 0 ? alignment : 1;

  return arena;
}

void *arena_alloc(Arena *arena, usize size) {
  if (!arena || size == 0)
    return NULL;

  if (arena->offset + size > arena->capacity) {
    return NULL; /* Out of memory */
  }

  void *ptr = arena->buffer + arena->offset;
  arena->offset += size;

  return ptr;
}

void *arena_alloc_aligned(Arena *arena, usize size, usize align) {
  if (!arena || size == 0 || align == 0)
    return NULL;

  usize aligned_offset = align_forward(arena->offset, align);
  usize total_needed = (aligned_offset - arena->offset) + size;

  if (arena->offset + total_needed > arena->capacity) {
    return NULL; /* Out of memory */
  }

  void *ptr = arena->buffer + aligned_offset;
  arena->offset = aligned_offset + size;

  return ptr;
}

void arena_reset(Arena *arena) {
  if (arena) {
    arena->offset = 0;
  }
}

void arena_destroy(Arena *arena) {
  if (arena) {
    free(arena->buffer);
    free(arena);
  }
}

int fx_example(int x) { return x * 2; }

#endif // FX_IMPLEMENTATION

#endif // FX_H