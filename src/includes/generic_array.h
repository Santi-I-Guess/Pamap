#ifndef GENERIC_ARRAY_H
#define GENERIC_ARRAY_H 1

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// very simple struct creator: init, free, and append func, with doubling resize
// free, init, and append all take in the pointer to the generated array type

// usage macros
#define ARRAY_NAME(type) \
        type##_Array
#define FREE_FUNC(type,alpha) \
        free_array_##type(alpha)
#define INIT_FUNC(type,alpha,num_elements) \
        init_array_##type(alpha,num_elements)
#define APPEND_FUNC(type,alpha,beta) \
        append_array_##type(alpha,beta)

// header and source file macros
#define DECLARE_FREE_FUNC(type) \
        void free_array_##type(type##_Array *alpha)
#define DECLARE_INIT_FUNC(type) \
        bool init_array_##type(type##_Array *alpha, unsigned num_elements)
#define DECLARE_APPEND_FUNC(type) \
        bool append_array_##type(type##_Array *alpha, const type beta)

#define DEFINE_ARRAY_STRUCT(type) \
typedef struct { \
        size_t count; \
        size_t capacity; \
        type *data; \
} type##_Array;

#define DEFINE_FREE_FUNC(type) \
void free_array_##type(type##_Array *alpha) { \
        if (alpha->data != NULL) \
                free(alpha->data); \
        alpha->count = 0; \
        alpha->capacity = 0; \
        alpha->data = NULL; \
}

#define DEFINE_INIT_FUNC(type) \
bool init_array_##type(type##_Array *alpha, unsigned num_elements) { \
        alpha->data = (type*)calloc(num_elements, sizeof(type)); \
        if (alpha->data == NULL) \
                return false; \
        memset(alpha->data, 0, num_elements * sizeof(type)); \
        alpha->capacity = 128; \
        alpha->count = 0; \
        return true; \
}

#define DEFINE_APPEND_FUNC(type) \
bool append_array_##type(type##_Array *alpha, const type beta) { \
        if (alpha->count == alpha->capacity) { \
                size_t new_size = alpha->capacity * 2 * sizeof(type); \
                type *tmp = (type*)realloc(alpha->data, new_size); \
                if (tmp == NULL) \
                        return false; \
                alpha->data = tmp; \
                alpha->capacity *= 2; \
        } \
        alpha->data[alpha->count] = beta; \
        alpha->count++; \
        return true; \
}

#endif
