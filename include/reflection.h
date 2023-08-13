
#if !defined(FX_FXREFLECTION_H_)
#define FX_FXREFLECTION_H_

///@brief Gets the address pointing to the container which accommodates the respective member
///@param ptr a pointer to container (struct)
///@param type of the container
///@param the name of the member the pointer refers to
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })

#endif   // FX_FXREFLECTION_H_
