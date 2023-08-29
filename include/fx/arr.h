
#ifndef FX_FXARR_H_
#define FX_FXARR_H_

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef fxarr_stdlib_free
#define fxarr_stdlib_free free
#endif
#ifndef fxarr_stdlib_malloc
#define fxarr_stdlib_malloc malloc
#endif
#ifndef fxarr_stdlib_calloc
#define fxarr_stdlib_calloc calloc
#endif
#ifndef fxarr_stdlib_realloc
#define fxarr_stdlib_realloc realloc
#endif

#define _FXARR_INITIAL_CAPACITY 8
#define _FXARR_GROW_MULTIPLIER  2

typedef struct fxarr_meta_t {
    size_t length;
    size_t capacity;
} fxarr_meta_t;

#ifndef FX_NO_SHORT_NAMES

/**
 * @brief Declare the array type to be used.
 * @example fxarr_type(Point) points = NULL;
 */
#define arr_type(type) fxarr_type(type)
/**
 * @brief Define array of type <type> and name <name>.
 * The array is created with default initial capacity.
 * @param type the array type
 * @param name variable name for the array.
 * @example `fxarr_init(int, arr);`
 * @return void
 */
#define arr_init(type, name) fxarr_init(type, name)
/**
 * @brief Gets current capacity of the array.
 * @param arr the array
 * @return size_t - the array capacity
 */
#define arr_capacity(arr) fxarr_capacity(arr)
/**
 * @brief Gets the current length of the array.
 * @param arr the array
 * @return size_t - the array length
 */
#define arr_len(arr) fxarr_len(arr)
/**
 * @brief Returns non-zero if the array is empty.
 * @param arr the array
 * @return size_t - non-zero if empty, zero if non-empty
 */
#define arr_is_empty(arr) fxarr_is_empty(arr)
/**
 * @brief Grows the array to be at least <capacity> elements big.
 * If <capacity> is greater than the current capacity, the function
 * will reallocate array's storage and increase its capacity.
 * @param arr the array
 * @param capacity size_t - minimum capacity for the array.
 * @return void
 */
#define arr_reserve(arr, capacity) fxarr_reserve(arr, capacity)
/**
 * @brief Clear all of the elements from the array.
 * @param arr the array
 * @return void
 */
#define arr_clear(arr) fxarr_clear(arr)
/**
 * @brief Frees the array memory.
 * NOTE: The array does not manage the memory its elements.
 * @param arr the array
 * @return void
 */
#define arr_free(arr) fxarr_free(arr)
/**
 * @brief Returns an iterator to first element of the array.
 * @param arr the array
 * @return pointer to the first element or NULL
 */
#define arr_begin(arr) fxarr_begin(arr)
/**
 * @brief Returns an iterator to one past the last element
 * of the array.
 * @param arr the array
 * @return pointer to one past the last element or NULL
 */
#define arr_end(arr) fxarr_end(arr)
/**
 * @brief Returns an iterator to the last element
 * of the array.
 * @param arr the array
 * @return pointer to the last element or NULL
 */
#define arr_back(arr) fxarr_back(arr)
/**
 * @brief Computes the capacity of the next grow.
 * Capacity is increased by multiples of 2.
 * @param capacity current capacity
 * @return size_t - capacity after next grow
 */
#define arr_compute_next_grow(capacity) fxarr_compute_next_grow(capacity)
/**
 * @brief Adds an element to the end of the array.
 * @param arr the array
 * @param value the value to add
 * @return void
 */
#define arr_append(arr, value) fxarr_append(arr, value)
/**
 * @brief Inserts element to the array at given <ix>.
 * @param arr the array
 * @param ix index where the new element is inserted.
 * @param val value to be copied (or moved) to the inserted elements.
 * @return void
 */
#define arr_insert(arr, ix, val) fxarr_insert(arr, ix, val)
/**
 * @brief Removes the last element from the array, but copies it to
 * <elem_ptr> first.
 * @param arr the array
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define arr_pop(arr, elem_ptr) fxarr_pop(arr, elem_ptr)
/**
 * @brief Removes the element at <ix> from the array.
 * @param arr the array
 * @param ix index of element to remove
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define arr_remove(arr, ix, elem_ptr) fxarr_remove(arr, ix, elem_ptr)
/**
 * @brief Copy an array.
 * @param src the source array
 * @param dest the destination to copy to
 * @return void
 */
#define arr_copy(src, dest) fxarr_copy(src, dest)

#endif   // FX_NO_SHORT_NAMES

/**
 * @brief Declare the array type to be used.
 * @example fxarr_type(Point) points = NULL;
 */
#define fxarr_type(type) type*

/**
 * @brief Define array of type <type> and name <name>.
 * The array is created with default initial capacity.
 * @param type the array type
 * @param name variable name for the array.
 * @example `fxarr_init(int, arr);`
 * @return void
 */
#define fxarr_init(type, name) type* name = NULL

/**
 * @brief For internal use, converts an array pointer to fxarr_meta_t pointer.
 * @param arr the array
 * @return fxarr_meta_t * - pointer to the array meta data
 */
#define _fxarr_arr_to_meta(arr) (&((fxarr_meta_t*)(arr))[-1])

/**
 * @brief For internal use, converts fxarr_meta_t pointer to array pointer.
 * @param ptr pointer to the fxarr_meta_t struct
 * @return pointer to the array
 */
#define _fxarr_meta_to_arr(ptr) ((void*)&((fxarr_meta_t*)(ptr))[1])

/**
 * @brief Gets current capacity of the array.
 * @param arr the array
 * @return size_t - the array capacity
 */
#define fxarr_capacity(arr) ((arr) ? _fxarr_arr_to_meta(arr)->capacity : (size_t)0)

/**
 * @brief Gets the current length of the array.
 * @param arr the array
 * @return size_t - the array length
 */
#define fxarr_len(arr) ((arr) ? _fxarr_arr_to_meta(arr)->length : (size_t)0)

/**
 * @brief Returns non-zero if the array is empty.
 * @param arr the array
 * @return size_t - non-zero if empty, zero if non-empty
 */
#define fxarr_is_empty(arr) (fxarr_len(arr) == 0)

/**
 * @brief Grows the array to be at least <capacity> elements big.
 * If <capacity> is greater than the current capacity, the function
 * will reallocate array's storage and increase its capacity.
 * @param arr the array
 * @param capacity size_t - minimum capacity for the array.
 * @return void
 */
#define fxarr_reserve(arr, capacity)              \
    do {                                          \
        size_t fxarr_cap__ = fxarr_capacity(arr); \
        if (fxarr_cap__ < (capacity)) {           \
            _fxarr_grow((arr), (capacity));       \
        }                                         \
    } while (0)

/**
 * @brief Clear all of the elements from the array.
 * @param arr the array
 * @return void
 */
#define fxarr_clear(arr)            \
    do {                            \
        if (arr) {                  \
            _fxarr_set_len(arr, 0); \
        }                           \
    } while (0)

/**
 * @brief Frees the array memory.
 * NOTE: The array does not manage the memory its elements.
 * @param arr the array
 * @return void
 */
#define fxarr_free(arr)                           \
    do {                                          \
        if (arr) {                                \
            void* p1__ = _fxarr_arr_to_meta(arr); \
            fxarr_stdlib_free(p1__);              \
        }                                         \
    } while (0)

/**
 * @brief Returns an iterator to first element of the array.
 * @param arr the array
 * @return pointer to the first element or NULL
 */
#define fxarr_begin(arr) (arr)

/**
 * @brief Returns an iterator to one past the last element
 * of the array.
 * @param arr the array
 * @return pointer to one past the last element or NULL
 */
#define fxarr_end(arr) ((arr) ? &((arr)[fxarr_len(arr)]) : NULL)

/**
 * @brief Returns an iterator to the last element
 * of the array.
 * @param arr the array
 * @return pointer to the last element or NULL
 */
#define fxarr_back(arr) ((arr) ? &((arr)[fxarr_len(arr) - 1]) : NULL)

/**
 * @brief Computes the capacity of the next grow.
 * Capacity is increased by multiples of 2.
 * @param capacity current capacity
 * @return size_t - capacity after next grow
 */
#define fxarr_compute_next_grow(capacity) \
    ((capacity) ? ((capacity)*_FXARR_GROW_MULTIPLIER) : _FXARR_INITIAL_CAPACITY)

/**
 * @brief Adds an element to the end of the array.
 * @param arr the array
 * @param value the value to add
 * @return void
 */
#define fxarr_append(arr, value)                                      \
    do {                                                              \
        size_t fxarr_cap__ = fxarr_capacity(arr);                     \
        if (fxarr_cap__ <= fxarr_len(arr)) {                          \
            _fxarr_grow((arr), fxarr_compute_next_grow(fxarr_cap__)); \
        }                                                             \
        (arr)[fxarr_len(arr)] = (value);                              \
        _fxarr_set_len((arr), fxarr_len(arr) + 1);                    \
    } while (0)

/**
 * @brief Inserts element to the array at given <ix>.
 * @param arr the array
 * @param ix index where the new element is inserted.
 * @param val value to be copied (or moved) to the inserted elements.
 * @return void
 */
#define fxarr_insert(arr, ix, val)                                                               \
    do {                                                                                         \
        size_t fxarr_cap__ = fxarr_capacity(arr);                                                \
        if (fxarr_cap__ <= fxarr_len(arr)) {                                                     \
            _fxarr_grow((arr), fxarr_compute_next_grow(fxarr_cap__));                            \
        }                                                                                        \
        if ((ix) < fxarr_len(arr)) {                                                             \
            memmove((arr) + (ix) + 1, (arr) + (ix), sizeof(*(arr)) * ((fxarr_len(arr)) - (ix))); \
        }                                                                                        \
        (arr)[(ix)] = (val);                                                                     \
        _fxarr_set_len((arr), fxarr_len(arr) + 1);                                               \
    } while (0)

/**
 * @brief Removes the last element from the array, but copies it to
 * <elem_ptr> first.
 * @param arr the array
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define fxarr_pop(arr, elem_ptr)                                              \
    do {                                                                      \
        if ((elem_ptr)) {                                                     \
            memcpy((elem_ptr), &((arr)[fxarr_len(arr) - 1]), sizeof(*(arr))); \
        }                                                                     \
        _fxarr_set_len((arr), fxarr_len(arr) - 1);                            \
    } while (0)

/**
 * @brief Removes the element at <ix> from the array.
 * @param arr the array
 * @param ix index of element to remove
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define fxarr_remove(arr, ix, elem_ptr)                                                      \
    do {                                                                                     \
        if (arr) {                                                                           \
            const size_t ln__ = fxarr_len(arr);                                              \
            if ((ix) == ln__ - 1) {                                                          \
                fxarr_pop(arr, elem_ptr);                                                    \
            } else if ((ix) < ln__) {                                                        \
                if ((elem_ptr)) {                                                            \
                    memcpy((elem_ptr), &((arr)[fxarr_len(arr) - 1]), sizeof(*(arr)));        \
                }                                                                            \
                _fxarr_set_len((arr), ln__ - 1);                                             \
                memmove((arr) + (ix), (arr) + (ix) + 1, sizeof(*(arr)) * (ln__ - 1 - (ix))); \
            }                                                                                \
        }                                                                                    \
    } while (0)

/**
 * @brief Copy an array.
 * @param src the source array
 * @param dest the destination to copy to
 * @return void
 */
#define fxarr_copy(src, dest)                                       \
    do {                                                            \
        if ((src)) {                                                \
            _fxarr_grow(dest, fxarr_len(src));                      \
            _fxarr_set_len(dest, fxarr_len(src));                   \
            memcpy((dest), (src), fxarr_len(src) * sizeof(*(src))); \
        }                                                           \
    } while (0)

/**
 * @brief For internal use, sets the capacity of the array.
 * @param arr the array
 * @param capacity_ the new capacity to set
 * @return void
 */
#define _fxarr_set_capacity(arr, capacity_)                  \
    do {                                                     \
        if (arr) {                                           \
            _fxarr_arr_to_meta(arr)->capacity = (capacity_); \
        }                                                    \
    } while (0)

/**
 * @brief For internal use, sets the length of the array.
 * @param arr the array
 * @param len the new length to be set
 * @return void
 */
#define _fxarr_set_len(arr, len)                     \
    do {                                             \
        if ((arr)) {                                 \
            _fxarr_arr_to_meta(arr)->length = (len); \
        }                                            \
    } while (0)

/**
 * @brief For internal use, ensures that the array is at least
 * <capacity> elements big.
 * @param arr the array
 * @param capacity the new capacity to set
 * @return void
 */
#define _fxarr_grow(arr, capacity)                                                    \
    do {                                                                              \
        const size_t fxarr_ln__ = (capacity) * sizeof(*(arr)) + sizeof(fxarr_meta_t); \
        if ((arr)) {                                                                  \
            void* fxarr_p1__ = _fxarr_arr_to_meta(arr);                               \
            void* fxarr_p2__ = fxarr_stdlib_realloc(fxarr_p1__, fxarr_ln__);          \
            assert(fxarr_p2__);                                                       \
            (arr) = _fxarr_meta_to_arr(fxarr_p2__);                                   \
        } else {                                                                      \
            void* fxarr_p__ = fxarr_stdlib_malloc(fxarr_ln__);                        \
            assert(fxarr_p__);                                                        \
            (arr) = _fxarr_meta_to_arr(fxarr_p__);                                    \
            _fxarr_set_len((arr), 0);                                                 \
        }                                                                             \
        _fxarr_set_capacity((arr), (capacity));                                       \
    } while (0)

#endif /* FX_FXARR_H_ */
