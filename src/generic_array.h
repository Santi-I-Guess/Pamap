#ifndef GENERIC_ARRAY_H
#define GENERIC_ARRAY_H 1

// very simple struct creator: init, free, and append func, with doubling resize

#define ARRAY_NAME(type) Array_##type
#define FREE_FUNC(type,alpha) free_array_##type(alpha)
#define INIT_FUNC(type,alpha) init_array_##type(alpha)
#define APPEND_FUNC(type,alpha,beta) append_array_##type(alpha,beta)

#define DECLARE_FREE_FUNC(type) void free_array_##type(Array_##type *alpha);
#define DECLARE_INIT_FUNC(type) bool init_array_##type(Array_##type *alpha);
#define DECLARE_APPEND_FUNC(type) bool append_array_##type(Array_##type *alpha, type beta);

#define DEFINE_ARRAY_STRUCT(type) \
typedef struct { \
        size_t count; \
        size_t capacity; \
        type *data; \
} Array_##type;

#define DEFINE_FREE_FUNC(type) \
void free_array_##type(Array_##type *alpha) { \
        if (alpha->data) \
                free(alpha->data); \
        alpha->count = 0; \
        alpha->capacity = 0; \
        alpha->data = 0; \
}

#define DEFINE_INIT_FUNC(type) \
bool init_array_##type(Array_##type *alpha) { \
        alpha->data = (type*)calloc(128, sizeof(type)); \
        if (!alpha->data) \
                return false; \
        alpha->capacity = 128; \
        alpha->count = 0; \
        return true; \
}

#define DEFINE_APPEND_FUNC(type) \
bool append_array_##type(Array_##type *alpha, type beta) { \
        if (alpha->count == alpha->capacity) { \
                size_t new_size = alpha->capacity * 2 * sizeof(type); \
                type *tmp = (type*)realloc(alpha->data, new_size); \
                if (!tmp) \
                        return false; \
                alpha->data = tmp; \
                alpha->capacity *= 2; \
        } \
        type *curr_beta = &(alpha->data[alpha->count]); \
        *curr_beta = beta; \
        alpha->count++; \
        return true; \
}

#endif
