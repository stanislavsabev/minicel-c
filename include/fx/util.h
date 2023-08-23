
#if !defined(FX_FXUTIL_H_)
#define FX_FXUTIL_H_

///@brief Gets the address pointing to the container which accommodates the respective member
///@param ptr a pointer to container (struct)
///@param type of the container
///@param the name of the member the pointer refers to
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })

#ifdef DEBUG
#define dprintf(__format, ...)                  \
    do {                                        \
        fprintf(stderr, __format, __VA_ARGS__); \
    } while (false)

#define dfprintf(__stream, __format, ...)         \
    do {                                          \
        fprintf(__stream, __format, __VA_ARGS__); \
    } while (false)
#else
#define dprintf(...) \
    do {             \
    } while (false)

#define dfprintf(...) \
    do {              \
    } while (false)
#endif

#endif   // FX_FXUTIL_H_