#ifndef __FX_LIB__
#define __FX_LIB__
#include <stdbool.h>
#include <stddef.h> // for NULL
#include <stdint.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memset

// TYPES
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

// BOOL values
typedef int8_t b8;
typedef int32_t b32;

typedef wchar_t wchar;
typedef uintptr_t up64;
typedef intptr_t p64;
typedef size_t usize;
typedef ptrdiff_t pdiff;

#define enum8(type) u8
#define enum16(type) u16
#define enum32(type) u32
#define enum64(type) u64

#ifndef MEM_DEFAULT_ALIGNMENT
#    define MEM_DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

// MACROS
// clang-format off
#define alignof(type) __alignof__(type)
#define mem_zero(ptr, size) memset((ptr), 0, (size))

#ifndef FX_DEBUG
    #ifdef NDEBUG
        #define FX_DEBUG 0
    #else
        #define FX_DEBUG 1
    #endif
#endif

#ifndef FX_ASSERT

    #ifdef FX_DEBUG

        #include <stdio.h>
        #include <stdlib.h>

        #if defined(_MSC_VER)
            #define FX_DEBUG_BREAK() __debugbreak()
        #elif defined(__GNUC__) || defined(__clang__)
            #define FX_DEBUG_BREAK() __builtin_trap()
        #else
            #define FX_DEBUG_BREAK() abort()
        #endif

        #define FX_ASSERT(expr)                                              \
            do {                                                             \
                if (!(expr)) {                                               \
                    fprintf(stderr,                                          \
                        "FX_ASSERT FAILED\n"                                 \
                        "  Expr: %s\n"                                       \
                        "  File: %s:%d\n",                                   \
                        #expr, __FILE__, __LINE__);                           \
                    FX_DEBUG_BREAK();                                                 \
                }                                                            \
            } while (0)
            
            #define FX_ASSERT_MSG(expr, msg)                                          \
                do {                                                                 \
                    if (!(expr)) {                                                   \
                        fprintf(stderr,                                              \
                            "FX_ASSERT FAILED\n"                                     \
                            "  Expr: %s\n"                                           \
                            "  Msg : %s\n"                                           \
                            "  File: %s:%d\n",                                       \
                            #expr, msg, __FILE__, __LINE__);                          \
                        FX_DEBUG_BREAK();                                                     \
                    }                                                                \
                } while (0)
    #else
        #define FX_ASSERT(expr) ((void)0)
        #define FX_ASSERT_MSG(expr, msg) ((void)0)
    #endif
#endif

// clang-format on
// END MACROS

// ARENA structs
typedef struct Arena {
    u8 *buffer;
    u64 buffer_size;
    u64 offset;
} Arena;

typedef struct ArenaSnapshot {
    Arena *arena;
    u64 offset; // The offset at the time of the snapshot
} ArenaSnapshot;

// STRING

// Immutable String View (Slice)
typedef struct StrView {
    const char *data;
    usize len;
} StrView;

// Mutable String with fixed length
typedef struct String {
    char *data;
    usize len;
} String;

// Mutable, growable String array with memory owning Arena
typedef struct StringBuilder {
    char *data;
    usize len;
    usize capacity;
    Arena *arena;
} StringBuilder;

// ARRAY

// Generic, growable Array
// TODO: Add macros to create, resize and free the array
//  Add bounce checking access macros/functions, etc.
typedef struct Array {
    void *data;
    usize len;
    usize capacity;
} Array;

// fn DECLARATION
#define fn

// Check if a value is a power of two
fn b8 mem_is_power_of_two(u64 value);

// Align a pointer forward to the nearest alignment boundary
fn u64 mem_align_forward(u64 ptr, u64 alignment);

// ARENA DECLARATIONS

// Initialize an arena with a buffer and capacity
fn void arena_init(Arena *arena, void *buffer, u64 capacity);

// Allocate memory from arena with specific alignment
// set_zero: if true, memory is zeroed; if false, uninitialized
fn void *arena_alloc_align(Arena *arena, u64 size, u64 alignment, b32 set_zero);

// No-op for arena. Arenas don't manage individual allocations.
fn void arena_free(Arena *arena);

// Reset arena to zero offset, freeing all allocations
fn void arena_reset(Arena *arena);

// Get remaining free space in arena (in bytes)
fn size_t arena_remaining(Arena *arena);

// Save current arena state for later restoration
fn ArenaSnapshot arena_snapshot_save(Arena *arena);

// Restore arena to a previously saved state
fn void arena_snapshot_restore(ArenaSnapshot snapshot);

// Allocate memory from arena with default alignment, zeroed
fn void *arena_alloc(Arena *arena, u64 size);

// STRING DECLARATIONS

// Create a StrView from a null-terminated C string
// Returns STRV_NULL if cstr is NULL
fn StrView strv_from_cstr(const char *cstr);

// Create an immutable view of a String
fn StrView str_to_strv(const String *str);

// Allocate a String with given capacity from arena
// String data buffer is allocated contiguously with the struct
fn String *str_create(Arena *arena, usize len);

// STRVIEW OPERATIONS (immutable, zero-copy)
// Check if StrView is null (data pointer is NULL)
fn b8 strv_is_null(StrView strv);

// Check if StrView is empty (len == 0)
fn b8 strv_is_empty(StrView strv);

// Check if StrView is null or empty
fn b8 strv_is_null_or_empty(StrView strv);

// Compare two StrViews for equality
fn b8 strv_eq(StrView left, StrView right);

// Compare StrView with null-terminated C string for equality
fn b8 strv_eq_cstr(StrView left, const char *right);

// Check if StrView starts with a given prefix
fn b8 strv_starts_with(StrView strv, StrView prefix);

// Check if StrView ends with a given suffix
fn b8 strv_ends_with(StrView strv, StrView suffix);

// Trim leading and trailing whitespace from StrView
// Returns new StrView with same underlying data
fn StrView strv_trim(StrView strv);

// Trim leading whitespace from StrView
// Returns new StrView with same underlying data
fn StrView strv_ltrim(StrView strv);

// Trim trailing whitespace from StrView
// Returns new StrView with same underlying data
fn StrView strv_rtrim(StrView strv);

// Trim sub-string from a string
// Returns new StrView with same underlying data
fn StrView strv_trim_str(StrView strv, StrView trim);

// Trim left sub-string from a string
// Returns new StrView with same underlying data
fn StrView strv_ltrim_str(StrView strv, StrView trim);

// Trim right sub-string from a string
// Returns new StrView with same underlying data
fn StrView strv_rtrim_str(StrView strv, StrView trim);

// Find first occurrence of needle in hay
// Returns hay.len if not found
fn usize strv_find(StrView hay, StrView needle);

// Find first occurrence of char needle in hay
// Returns hay.len if not found
fn usize strv_find_char(StrView hay, char needle);

// Find first occurrence of needle in hay, searching from right to left
// Returns hay.len if not found
fn usize strv_rfind(StrView hay, StrView needle);

// Find first occurrence of char needle in hay, searching from right to left
// Returns hay.len if not found
fn usize strv_rfind_char(StrView hay, char needle);

// Check if needle contains hay
// Returns bool
fn b8 strv_contains(StrView hay, StrView needle);

// Split StrView on delimiter and advance remaining pointer
// Returns the token before delimiter,
//  STRV_NULL if remaining is NULL or STRV_NULL
fn StrView strv_split_next(StrView *remaining, char delimiter);

// Split StrView on sub-string delimiter and advance remaining pointer
// Returns: the token before delimiter
fn StrView strv_split_next_str(StrView *remaining, StrView delimiter);

// STRING OPERATIONS (mutable)
// Append a StrView to a String if space available
// Returns true on success, false if capacity exceeded
fn b8 strb_append(StringBuilder *strb, StrView strv);

// Append a single character to String if space available
// Returns true on success, false if capacity exceeded
fn b8 strb_append_char(StringBuilder *strb, char c);

// Append a null-terminated C string to String if space available
// Returns true on success, false if capacity exceeded
fn b8 strb_append_cstr(StringBuilder *strb, const char *cstr);

// Clear String contents (sets len to 0, preserves capacity)
// Returns true on success
fn b8 strb_reset(StringBuilder *strb);

// Copy StrView contents into String, replacing existing data
// Returns true on success, false if capacity exceeded
fn b8 str_copy(String *dest, StrView src);

// Initialize a String with existing buffer and data
fn void str_init(String *str, char *data, usize len);

// Create a String from null-terminated C string, allocating from arena
// Returns NULL if cstr is NULL or allocation fails
fn String *str_from_cstr(Arena *arena, const char *cstr);

// Create a String from StrView, allocating from arena
// Returns NULL if sv is null or allocation fails
fn String *str_from_strv(Arena *arena, StrView strv);
// END fn DECLARATION

// Arena helpers
#define arena_alloc_struct(arena, Type) (Type *)arena_alloc((arena), sizeof(Type))

// String helpers
#define STRV_NULL                                                                                  \
    (StrView) { 0 }
#define STRV_EMPTY                                                                                 \
    (StrView) { .data = "", .len = 0 }

#define STRV_LIT(cstr) strv_from_cstr((cstr))

// HASH TABLE
// - TBD

// IMPLEMENTATION
#ifdef FX_IMPLEMENTATION

fn b8 mem_is_power_of_two(u64 value) { return (value & (value - 1)) == 0; }

// Arena Implementation
#    include <assert.h>

fn u64 mem_align_forward(u64 ptr, u64 alignment) {
    assert(mem_is_power_of_two(alignment) && "Alignment must be a power of two");

    u64 p, a, modulo;
    p = ptr;
    a = alignment;
    modulo = p & (a - 1);
    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

fn void arena_init(Arena *arena, void *buffer, u64 capacity) {
    arena->buffer = (u8 *)buffer;
    arena->buffer_size = capacity;
    arena->offset = 0;
}

fn void *arena_alloc_align(Arena *arena, u64 size, u64 alignment, b32 set_zero) {
    up64 curr_ptr = (up64)(arena->buffer + arena->offset);
    up64 offset = mem_align_forward(curr_ptr, alignment);
    offset -= (up64)arena->buffer; // Adjust offset to be relative to arena->data

    if (offset + size > arena->buffer_size) {
        return NULL; // Not enough space in the arena
    }

    void *ptr = &arena->buffer[offset];
    arena->offset = offset + size;

    if (set_zero) {
        mem_zero(ptr, size);
    }
    return ptr;
}

fn void *arena_alloc(Arena *arena, u64 size) {
    return arena_alloc_align(arena, size, MEM_DEFAULT_ALIGNMENT, true);
}

fn void arena_free(Arena *arena) {
    // No-op, as we don't manage heap memory in the arena
    (void)arena;
}

// Get remaining space in arena
fn size_t arena_remaining(Arena *arena) {
    FX_ASSERT_MSG(arena->offset <= arena->buffer_size, "arena overflow");
    return arena->buffer_size - arena->offset;
}

fn void arena_reset(Arena *arena) { arena->offset = 0; }

fn ArenaSnapshot arena_snapshot_save(Arena *arena) {
    ArenaSnapshot snapshot;
    snapshot.arena = arena;
    snapshot.offset = arena->offset;
    return snapshot;
}

fn void arena_snapshot_restore(ArenaSnapshot snapshot) {
    Arena *arena = snapshot.arena;
    arena->offset = snapshot.offset;
}

// String implementation
fn b8 strv_is_null(StrView strv) { return strv.data == NULL; }
fn b8 strv_is_empty(StrView strv) { return strv.len == 0; }
fn b8 strv_is_null_or_empty(StrView strv) { return (strv_is_null(strv) || strv_is_empty(strv)); }

fn String *str_create(Arena *arena, usize len) {
    u8 *mem = (u8 *)arena_alloc((arena), sizeof(String) + len);
    if (!mem) {
        return NULL;
    }
    String *str = (String *)mem;
    str_init(str, (char *)(mem + sizeof(String)), len);
    return str;
}

// TODO: not implemented
fn b8 str_copy(String *dest, StrView src) {
    if (strv_is_null(src)) {
        return false;
    }
    if (dest->len < src.len) {
        return false;
    }
    memcpy(dest->data, src.data, src.len);
    dest->len = src.len;
    return true;
}

fn void str_init(String *str, char *data, usize len) {
    str->data = data;
    str->len = len;
}

fn String *str_from_cstr(Arena *arena, const char *cstr) {
    if (cstr == NULL)
        return NULL;
    String *str = str_create(arena, strlen(cstr));
    if (!str)
        return NULL;
    memcpy(str->data, cstr, str->len);
    return str;
}

fn String *str_from_strv(Arena *arena, StrView strv) {
    if (strv_is_null(strv))
        return NULL;
    String *str = str_create(arena, strv.len);
    if (!str)
        return NULL;
    memcpy(str->data, strv.data, strv.len);
    return str;
}

fn StrView strv_from_cstr(const char *cstr) {
    if (cstr == NULL)
        return STRV_NULL;
    return (StrView){.data = cstr, .len = strlen(cstr)};
}

fn StrView str_to_strv(const String *str) { return (StrView){.data = str->data, .len = str->len}; }

fn b8 strv_eq(StrView left, StrView right) {
    if (left.len != right.len)
        return false;
    if (left.data == right.data) // same data pointer
        return true;
    if (!left.data || !right.data)
        return false;
    return memcmp(left.data, right.data, left.len) == 0;
}

fn b8 strv_eq_cstr(StrView left, const char *right) {
    StrView r = strv_from_cstr(right);
    return strv_eq(left, r);
}

fn b8 strv_starts_with(StrView strv, StrView prefix) {
    if (strv_is_null_or_empty(strv) || strv_is_null_or_empty(prefix)) {
        return false;
    }
    if (strv.len < prefix.len) {
        return false;
    }
    if (strv.data == prefix.data) {
        // same data pointer
        return true;
    }
    return memcmp(strv.data, prefix.data, prefix.len) == 0;
}

fn b8 strv_ends_with(StrView strv, StrView suffix) {
    if (strv_is_null_or_empty(strv) || strv_is_null_or_empty(suffix)) {
        return false;
    }
    if (strv.len < suffix.len) {
        return false;
    }
    if (strv.data == suffix.data) {
        // same data pointer
        return true;
    }
    return memcmp(strv.data + (strv.len - suffix.len), suffix.data, suffix.len) == 0;
}

// TODO: not implemented
fn usize strv_find(StrView hay, StrView needle) {
    assert(0 && "not implemented");
    return hay.len;
}

// TODO: not implemented
fn usize strv_find_char(StrView hay, char needle) {
    assert(0 && " not implemented");
    return 0;
}

// TODO: not implemented
fn usize strv_rfind(StrView hay, StrView needle) {
    assert(0 && " not implemented");
    return 0;
}

// TODO: not implemented
fn usize strv_rfind_char(StrView hay, char needle) {
    assert(0 && " not implemented");
    return 0;
}

// TODO: not implemented
fn b8 strv_contains(StrView hay, StrView needle) {
    assert(0 && " not implemented");
    return false;
}

// TODO: not implemented
fn StrView strv_trim(StrView strv) {
    assert(0 && "not implemented");
    return STRV_NULL;
}

// TODO: not implemented
fn StrView strv_ltrim(StrView strv) {
    assert(0 && "not implemented");
    return STRV_NULL;
}

// TODO: not implemented
fn StrView strv_rtrim(StrView strv) {
    assert(0 && "not implemented");
    return STRV_NULL;
}

// TODO: not implemented
fn StrView strv_trim_str(StrView strv, StrView trim) {
    assert(0 && "not implemented");
    return STRV_NULL;
}

// TODO: not implemented
fn StrView strv_ltrim_str(StrView strv, StrView trim) {
    assert(0 && "not implemented");
    return (StrView){0};
}

// TODO: not implemented
fn StrView strv_rtrim_str(StrView strv, StrView trim) {
    assert(0 && "not implemented");
    return (StrView){0};
}

fn StrView strv_split_next(StrView *remaining, char delimiter) {
    if (!remaining || strv_is_null(*remaining)) {
        return STRV_NULL;
    }

    for (usize i = 0; i < remaining->len; i++) {
        if (remaining->data[i] == delimiter) {
            StrView token = {.data = remaining->data, .len = i};
            token.len = i;
            remaining->data += i + 1;
            remaining->len -= i + 1;
            return token;
        }
    }
    // No delimiter found - return the rest and mark remaining as done
    StrView last = *remaining;
    *remaining = STRV_NULL;
    return last;
}

// TODO: not implemented
fn StrView strv_split_next_str(StrView *remaining, StrView delimiter) {
    assert(0 && " not implemented");
    return STRV_NULL;
}

// TODO: not implemented
fn b8 strb_append(StringBuilder *strb, StrView strv) {
    assert(0 && "not implemented");
    return false;
}

// TODO: not implemented
fn b8 strb_append_char(StringBuilder *strb, char c) {
    assert(0 && "not implemented");
    return false;
}

// TODO: not implemented
fn b8 strb_append_cstr(StringBuilder *strb, const char *cstr) {
    assert(0 && "not implemented");
    return false;
}

// TODO: not implemented
fn b8 strb_reset(StringBuilder *strb) {
    assert(0 && "not implemented");
    return false;
}

#endif // FX_IMPLEMENTATION
#endif // __FX_LIB__
