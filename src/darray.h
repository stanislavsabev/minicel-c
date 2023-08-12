
#ifndef DARRAY_H_
#define DARRAY_H_

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef darr_clib_free
#define darr_clib_free free
#endif
#ifndef darr_clib_malloc
#define darr_clib_malloc malloc
#endif
#ifndef darr_clib_calloc
#define darr_clib_calloc calloc
#endif
#ifndef darr_clib_realloc
#define darr_clib_realloc realloc
#endif

#define DARR_INITIAL_CAPACITY_ 8

typedef struct darr_meta_t {
    size_t length;
    size_t capacity;
} darr_meta_t;

/**
 * @brief Declare the array type to be used.
 * @example darr_type(Point) points = NULL;
 */
#define darr_type(type) type *

/**
 * @brief For internal use, converts an array pointer to darr_meta_t
 * pointer. pointer
 * @param arr the array
 * @return darr_meta_t * - pointer to the array meta data
 */
#define _darr_arr_to_meta(arr) (&((darr_meta_t *)(arr))[-1])

/**
 * @brief For internal use, converts darr_meta_t pointer to array pointer.
 * @param ptr pointer to the darr_meta_t struct
 * @return pointer to the array
 */
#define _darr_meta_to_arr(ptr) ((void *)&((darr_meta_t *)(ptr))[1])

/**
 * @brief Gets current capacity of the array.
 * @param arr the array
 * @return size_t - the array capacity
 */
#define darr_capacity(arr) \
    ((arr) ? _darr_arr_to_meta(arr)->capacity : (size_t)0)

/**
 * @brief Gets the current length of the array.
 * @param arr the array
 * @return size_t - the array length
 */
#define darr_len(arr) ((arr) ? _darr_arr_to_meta(arr)->length : (size_t)0)

/**
 * @brief Returns non-zero if the array is empty.
 * @param arr the array
 * @return size_t - non-zero if empty, zero if non-empty
 */
#define darr_is_empty(arr) (darr_len(arr) == 0)

/**
 * @brief Grows the array to be at least <capacity> elements big.
 * If <capacity> is greater than the current capacity, the function
 * will reallocate array's storage and increase its capacity.
 * @param arr the array
 * @param capacity size_t - minimum capacity for the array.
 * @return void
 */
#define darr_reserve(arr, capacity)             \
    do {                                        \
        size_t darr_cap__ = darr_capacity(arr); \
        if (darr_cap__ < (capacity)) {          \
            _darr_grow((arr), (capacity));      \
        }                                       \
    } while (0)

/**
 * @brief Clear all of the elements from the array.
 * @param arr the array
 * @return void
 */
#define darr_clear(arr)            \
    do {                           \
        if (arr) {                 \
            _darr_set_len(arr, 0); \
        }                          \
    } while (0)

/**
 * @brief Frees the array memory.
 * NOTE: The array does not manage the memory its elements.
 * @param arr the array
 * @return void
 */
#define darr_free(arr)                           \
    do {                                         \
        if (arr) {                               \
            void *p1__ = _darr_arr_to_meta(arr); \
            darr_clib_free(p1__);                \
        }                                        \
    } while (0)

/**
 * @brief Returns an iterator to first element of the array.
 * @param arr the array
 * @return pointer to the first element or NULL
 */
#define darr_begin(arr) (arr)

/**
 * @brief Returns an iterator to one past the last element
 * of the array.
 * @param arr the array
 * @return pointer to one past the last element or NULL
 */
#define darr_end(arr) ((arr) ? &((arr)[darr_len(arr)]) : NULL)

/**
 * @brief Returns an iterator to the last element
 * of the array.
 * @param arr the array
 * @return pointer to the last element or NULL
 */
#define darr_back(arr) ((arr) ? &((arr)[darr_len(arr) - 1]) : NULL)

/**
 * @brief Computes the capacity of the next grow.
 * Capacity is increased by multiples of 2.
 * @param capacity current capacity
 * @return size_t - capacity after next grow
 */
#define darr_compute_next_grow(capacity) \
    ((capacity) ? ((capacity)*2) : DARR_INITIAL_CAPACITY_)

/**
 * @brief Adds an element to the end of the array.
 * @param arr the array
 * @param value the value to add
 * @return void
 */
#define darr_append(arr, value)                                    \
    do {                                                           \
        size_t darr_cap__ = darr_capacity(arr);                    \
        if (darr_cap__ <= darr_len(arr)) {                         \
            _darr_grow((arr), darr_compute_next_grow(darr_cap__)); \
        }                                                          \
        (arr)[darr_len(arr)] = (value);                            \
        _darr_set_len((arr), darr_len(arr) + 1);                   \
    } while (0)

/**
 * @brief Inserts element to the array at given <ix>.
 * @param arr the array
 * @param ix index where the new element is inserted.
 * @param val value to be copied (or moved) to the inserted elements.
 * @return void
 */
#define darr_insert(arr, ix, val)                                  \
    do {                                                           \
        size_t darr_cap__ = darr_capacity(arr);                    \
        if (darr_cap__ <= darr_len(arr)) {                         \
            _darr_grow((arr), darr_compute_next_grow(darr_cap__)); \
        }                                                          \
        if ((ix) < darr_len(arr)) {                                \
            memmove((arr) + (ix) + 1, (arr) + (ix),                \
                    sizeof(*(arr)) * ((darr_len(arr)) - (ix)));    \
        }                                                          \
        (arr)[(ix)] = (val);                                       \
        _darr_set_len((arr), darr_len(arr) + 1);                   \
    } while (0)

/**
 * @brief Removes the last element from the array, but copies it to
 * <elem_ptr> first.
 * @param arr the array
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define darr_pop_back(arr, elem_ptr)                        \
    do {                                                    \
        if ((elem_ptr)) {                                   \
            memcpy((elem_ptr), &((arr)[darr_len(arr) - 1]), \
                   sizeof(*(arr)));                         \
        }                                                   \
        _darr_set_len((arr), darr_len(arr) - 1);            \
    } while (0)

/**
 * @brief Removes the element at <ix> from the array.
 * @param arr the array
 * @param ix index of element to remove
 * @param elem_ptr pointer to receive the removed element
 * @return void
 */
#define darr_remove(arr, ix, elem_ptr)                              \
    do {                                                            \
        if (arr) {                                                  \
            const size_t ln__ = darr_len(arr);                      \
            if ((ix) == ln__ - 1) {                                 \
                darr_pop_back(arr, elem_ptr);                       \
            } else if ((ix) < ln__) {                               \
                if ((elem_ptr)) {                                   \
                    memcpy((elem_ptr), &((arr)[darr_len(arr) - 1]), \
                           sizeof(*(arr)));                         \
                }                                                   \
                _darr_set_len((arr), ln__ - 1);                     \
                memmove((arr) + (ix), (arr) + (ix) + 1,             \
                        sizeof(*(arr)) * (ln__ - 1 - (ix)));        \
            }                                                       \
        }                                                           \
    } while (0)

/**
 * @brief Copy an array.
 * @param src the source array
 * @param dest the destination to copy to
 * @return void
 */
#define darr_copy(src, dest)                                       \
    do {                                                           \
        if ((src)) {                                               \
            _darr_grow(dest, darr_len(src));                       \
            _darr_set_len(dest, darr_len(src));                    \
            memcpy((dest), (src), darr_len(src) * sizeof(*(src))); \
        }                                                          \
    } while (0)

/**
 * @brief For internal use, sets the capacity of the array.
 * @param arr the array
 * @param capacity_ the new capacity to set
 * @return void
 */
#define _darr_set_capacity(arr, capacity_)                  \
    do {                                                    \
        if (arr) {                                          \
            _darr_arr_to_meta(arr)->capacity = (capacity_); \
        }                                                   \
    } while (0)

/**
 * @brief For internal use, sets the length of the array.
 * @param arr the array
 * @param len the new length to be set
 * @return void
 */
#define _darr_set_len(arr, len)                     \
    do {                                            \
        if ((arr)) {                                \
            _darr_arr_to_meta(arr)->length = (len); \
        }                                           \
    } while (0)

/**
 * @brief For internal use, ensures that the array is at least
 * <capacity> elements big.
 * @param arr the array
 * @param capacity the new capacity to set
 * @return void
 */
#define _darr_grow(arr, capacity)                                      \
    do {                                                               \
        const size_t darr_ln__ =                                       \
            (capacity) * sizeof(*(arr)) + sizeof(darr_meta_t);         \
        if ((arr)) {                                                   \
            void *darr_p1__ = _darr_arr_to_meta(arr);                  \
            void *darr_p2__ = darr_clib_realloc(darr_p1__, darr_ln__); \
            assert(darr_p2__);                                         \
            (arr) = _darr_meta_to_arr(darr_p2__);                      \
        } else {                                                       \
            void *darr_p__ = darr_clib_malloc(darr_ln__);              \
            assert(darr_p__);                                          \
            (arr) = _darr_meta_to_arr(darr_p__);                       \
            _darr_set_len((arr), 0);                                   \
        }                                                              \
        _darr_set_capacity((arr), (capacity));                         \
    } while (0)

#endif /* DARRAY_H_ */
