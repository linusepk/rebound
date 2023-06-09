#ifndef REBOUND_H
#define REBOUND_H

/*=========================*/
// Includes
/*=========================*/

#define _GNU_SOURCE

#include <string.h> // memset
#include <stdio.h>
#include <stdarg.h>

//  ____                   _
// | __ )  __ _ ___  ___  | |    __ _ _   _  ___ _ __
// |  _ \ / _` / __|/ _ \ | |   / _` | | | |/ _ \ '__|
// | |_) | (_| \__ \  __/ | |__| (_| | |_| |  __/ |
// |____/ \__,_|___/\___| |_____\__,_|\__, |\___|_|
//                                    |___/
// Base layer

/*=========================*/
// Context cracking
/*=========================*/

#ifdef _WIN32
#define RE_OS_WINDOWS
#error "Windows is currently not supported"
#endif // _WIN32

#ifdef __linux__
#define RE_OS_LINUX
#endif // linux

/*=========================*/
// API macros
/*=========================*/

#if defined(RE_DYNAMIC) && defined(RE_OS_WINDOWS)
    // Using compiled windows DLL.
    #define RE_API __declspec(dllimport)
#elif defined(RE_DYNAMIC) && defined(RE_COMPILE) && defined(RE_OS_WINDOWS)
    // Compiling windows DLL.
    #define RE_API __declspec(dllexport)
#elif defined(RE_DYNAMIC) && defined(RE_COMPILE) && defined(RE_OS_LINUX)
    // Compiling linux shared object.
    #define RE_API __attribute__((visibility("default")))
#else
    #define RE_API extern
#endif

#ifdef RE_OS_LINUX
#define RE_FORMAT_FUNCTION(FORMAT_INDEX, VA_INDEX) __attribute__((format(printf, FORMAT_INDEX, VA_INDEX)))
#else
#define RE_FORMAT_FUNCTION(FORMAT_INDEX, VA_INDEX)
#endif

/*=========================*/
// Basic types
/*=========================*/

typedef unsigned char      u8_t;
typedef unsigned short     u16_t;
typedef unsigned int       u32_t;
typedef unsigned long long u64_t;
typedef unsigned long      usize_t;

typedef signed char      i8_t;
typedef signed short     i16_t;
typedef signed int       i32_t;
typedef signed long long i64_t;
typedef signed long      isize_t;

typedef float  f32_t;
typedef double f64_t;

typedef u8_t  b8_t;
typedef u32_t b32_t;

#ifndef true
#define true 1
#endif // true
#ifndef false
#define false 0
#endif // false

typedef u8_t *ptr_t;
#ifndef NULL
#define NULL ((void *) 0)
#endif // NULL

/*=========================*/
// Allocators
/*=========================*/

#if defined(RE_MALLOC) && defined(RE_REALLOC) && defined(RE_FREE)
#elif !defined(RE_MALLOC) && !defined(RE_REALLOC) && !defined(RE_FREE)
#else
#error "Must define all or none of RE_MALLOC, RE_REALLOC and RE_FREE."
#endif

#ifndef RE_MALLOC
#include <stdlib.h>
#define RE_MALLOC malloc
#define RE_REALLOC realloc
#define RE_FREE free
#endif

#define OUT_OF_MEMORY "Out of memory"

RE_API void *re_malloc(usize_t size);
RE_API void *re_realloc(void *ptr, usize_t size);
RE_API void  re_free(void *ptr);

/*=========================*/
// Utils
/*=========================*/

// Aborts the program and logs the reason.
// Both format string and format arguments should be supplied in the variatic arguments.
#define RE_ABORT(...) do { \
    char buffer[1024]; \
    re_format_string(buffer, __VA_ARGS__); \
    re_log_fatal("ABORT: %s", buffer); \
    abort(); \
} while (0)

// Ensures a condition is met. If it isn't met, program will abort.
#define RE_ENSURE(cond, ...) do { \
    if (!(cond)) { \
        re_log_error("Condition '%s' not met.", #cond); \
        RE_ABORT(__VA_ARGS__); \
    } \
} while (0)

// Ensures a condition is met while debugging. Does nothing if not in debug mode.
#ifdef RE_DEBUG
#define RE_ASSERT(cond, ...) RE_ENSURE(cond, __VA_ARGS__)
#else
#define RE_ASSERT(cond, ...)
#endif

typedef usize_t (*re_hash_func_t)(const void *data, usize_t size);

// Concatinates A and B into an identifier.
#define re_concat(A, B) _re_concat(A, B)
// Creates a unique variable name within a macro to avoid name collisions.
#define re_macro_var(NAME) re_concat(re_concat(UNIQUE_MACRO_ID, __LINE__), NAME)
// Second concatination function needed for macro expansion.
#define _re_concat(A, B) A##B

// Clamps V between MIN and MAX.
#define re_clamp(V, MIN, MAX) (V) > (MAX) ? (MAX) : (V) < (MIN) ? (MIN) : (V)
// Clamps V maximum value to MAX.
#define re_clamp_max(V, MAX) (V) > (MAX) ? (MAX) : (V)
// Clamps V minimum value to MIN.
#define re_clamp_min(V, MIN) (V) < (MIN) ? (MIN) : (V)
// Returns the biggest value of A and B.
#define re_max(A, B) (A) > (B) ? (A) : (B)
// Returns the smallest value of A and B.
#define re_min(A, B) (A) < (B) ? (A) : (B)

// Converts a pointer to an integer.
#define re_ptr_to_usize(PTR) ((usize_t) ((u8_t *) (PTR) - (u8_t) 0))
// Converts an integer to a pointer.
#define re_usize_to_ptr(N) ((void *) ((u8_t *) + N))
// Calculates the byte offset of the member M in struct S.
#define re_offsetof(S, M) re_ptr_to_usize(&((S *) 0)->M)
// Calculates the length of ARR.
#define re_arr_len(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define U8_MAX    ((u8_t) ~0)
#define U16_MAX   ((u16_t) ~0)
#define U32_MAX   ((u32_t) ~0)
#define U64_MAX   ((u64_t) ~0)
#define USIZE_MAX ((usize_t) ~0)

#define I8_MIN    ((i8_t)    (0 | (i8_t) (1 << 7)))
#define I16_MIN   ((i16_t)   (0 | (i16_t) (1 << 15)))
#define I32_MIN   ((i32_t)   (0 | (i32_t) (1 << 31)))
#define I64_MIN   ((i64_t)   (0 | (i64_t) (1ll << 63)))
#define ISIZE_MIN ((isize_t) (0 | (1l << (sizeof(isize_t) * 8  -1))))

#define I8_MAX    ((i8_t)    ~I8_MIN)
#define I16_MAX   ((i16_t)   ~I16_MIN)
#define I32_MAX   ((i32_t)   ~I32_MIN)
#define I64_MAX   ((i64_t)   ~I64_MIN)
#define ISIZE_MAX ((isize_t) ~ISIZE_MIN)

#define re_bit(N) (1 << (N))
#define re_bit_set(M, N, V) \
    ((V) == 1 ? \
        ((M) |= re_bit(N)) : \
        ((M) &= ~re_bit(N)) \
    )
#define re_bit_get(M, N) \
    (M) >> (N) & 1

// Hashes data using the fvn1a algorithm.
RE_API usize_t re_fvn1a_hash(const char *key, usize_t len);
// Formats the fmt string into the provided buffer.
RE_API void re_format_string(char buffer[1024], const char *fmt, ...) RE_FORMAT_FUNCTION(2, 3);

/*=========================*/
// Hash table
/*=========================*/

#ifndef RE_HT_INIT_CAP
#define RE_HT_INIT_CAP  8
#endif

#ifndef RE_HT_MAX_FILL
#define RE_HT_MAX_FILL  0.5f
#endif

#ifndef RE_HT_GROW_RATE
#define RE_HT_GROW_RATE 2
#endif

// Declares a new hash table.
#define re_ht_t(K, V) struct { \
    struct { \
        usize_t hash; \
        K key; \
        V value; \
        b8_t alive; \
    } *entries; \
    usize_t count; \
    usize_t capacity; \
    usize_t key_size; \
    K temp_key; \
    re_hash_func_t hash_func; \
} *

// Initializes a hash table.
#define re_ht_create(HT, HASH_FUNC) do { \
    (HT) = re_malloc(sizeof(__typeof__(*(HT)))); \
    (HT)->entries = re_malloc(RE_HT_INIT_CAP * sizeof(__typeof__(*(HT)->entries))); \
    memset((HT)->entries, 0, RE_HT_INIT_CAP * sizeof(__typeof__(*(HT)->entries))); \
    (HT)->count = 0; \
    (HT)->capacity = RE_HT_INIT_CAP; \
    (HT)->key_size = sizeof((HT)->temp_key); \
    (HT)->hash_func = (HASH_FUNC); \
} while (0)

// Frees all data used by hash table.
#define re_ht_destroy(HT) do { \
    re_free((HT)->entries); \
    re_free((HT)); \
    (HT) = NULL; \
} while (0)

// If key isn't present within the hash table it will be added.
// If it is present it will instead be updated.
#define re_ht_set(HT, KEY, VALUE) do { \
    (HT)->temp_key = KEY; \
    if ((HT)->count + 1 > (HT)->capacity * RE_HT_MAX_FILL) { \
        _re_ht_resize(HT); \
    } \
    __typeof__(*(HT)->entries) *re_macro_var(entry) = NULL; \
    usize_t re_macro_var(hash) = (HT)->hash_func(&(HT)->temp_key, (HT)->key_size); \
    _re_ht_get_entry((HT)->entries, (HT)->capacity, re_macro_var(hash), re_macro_var(entry)); \
    if (!re_macro_var(entry)->alive) { \
        (HT)->count++; \
    } \
    re_macro_var(entry)->hash = re_macro_var(hash); \
    re_macro_var(entry)->key = (HT)->temp_key; \
    re_macro_var(entry)->value = (VALUE); \
    re_macro_var(entry)->alive = true; \
} while (0)

// If key is present within the hash table OUT will be set to the retrieved value, if not nothing will happen.
#define re_ht_get(HT, KEY, OUT) do { \
    (HT)->temp_key = KEY; \
    __typeof__(*(HT)->entries) *re_macro_var(entry) = NULL; \
    usize_t re_macro_var(hash) = (HT)->hash_func(&(HT)->temp_key, (HT)->key_size); \
    _re_ht_get_entry((HT)->entries, (HT)->capacity, re_macro_var(hash), re_macro_var(entry)); \
    if (re_macro_var(entry)->alive) {                                                         \
        (OUT) = re_macro_var(entry)->value; \
    } \
} while (0)

// Removes an entry from the hash table. If it doesn't exist nothing happens.
#define re_ht_remove(HT, KEY) do { \
    __typeof__(KEY) re_macro_var(temp_key) = (KEY); \
    __typeof__(*(HT)->entries) *re_macro_var(entry) = NULL; \
    usize_t re_macro_var(hash) = (HT)->hash_func(&re_macro_var(temp_key), (HT)->key_size); \
    _re_ht_get_entry((HT)->entries, (HT)->capacity, re_macro_var(hash), re_macro_var(entry)); \
    if (re_macro_var(entry)->alive) { \
        (HT)->count--; \
    } \
    re_macro_var(entry)->alive = false; \
} while (0)

// Clears all the entries from the hash table.
#define re_ht_clear(HT) do { \
    __typeof__((HT)->entries) re_macro_var(new_entries) = re_malloc(RE_HT_INIT_CAP * sizeof(__typeof__(*(HT)->entries))); \
    memset(re_macro_var(new_entries), 0, RE_HT_INIT_CAP * sizeof(__typeof__(*(HT)->entries))); \
    re_free((HT)->entries); \
    (HT)->entries = re_macro_var(new_entries); \
    (HT)->capacity = RE_HT_INIT_CAP; \
    (HT)->count = 0; \
} while (0)

// Retrieves the current active entry count.
#define re_ht_count(HT) (HT)->count

typedef usize_t re_ht_iter_t;

// Retrieves the first valid iterator.
#define re_ht_iter_new(HT) _re_ht_iter_next((HT), 0)
// Checks if the iterator is still valid and usable.
#define re_ht_iter_valid(HT, ITER) (ITER) < (HT)->capacity
// Advances iterator to next valid iteration.
#define re_ht_iter_advance(HT, ITER) (ITER) = _re_ht_iter_next((HT), (ITER) + 1)

// Gets the key and value at the given iteration.
#define re_ht_get_iter(HT, ITER, KEY, VALUE) do { \
    (KEY) = (HT)->entries[(ITER)].key; \
    (VALUE) = (HT)->entries[(ITER)].value; \
} while (0)

// Private API
#define _re_ht_get_entry(ENTRIES, CAP, HASH, OUT_ENTRY) do { \
    usize_t re_macro_var(index) = (HASH) % (CAP); \
    for (;;) { \
        if (!(ENTRIES)[re_macro_var(index)].alive || (ENTRIES)[re_macro_var(index)].hash == (HASH)) { \
            (OUT_ENTRY) = &(ENTRIES)[re_macro_var(index)]; \
            break; \
        } \
        re_macro_var(index)= (re_macro_var(index) + 1) % (CAP); \
    } \
} while (0)

#define _re_ht_resize(HT) do { \
    usize_t re_macro_var(new_cap) = (HT)->capacity * RE_HT_GROW_RATE; \
    usize_t re_macro_var(size) = re_macro_var(new_cap) * sizeof(__typeof__(*(HT)->entries)); \
    __typeof__((HT)->entries) re_macro_var(new_entries) = re_malloc(re_macro_var(size)); \
    memset(re_macro_var(new_entries), 0, re_macro_var(size)); \
    for (usize_t i = 0; i < (HT)->capacity; i++) { \
        __typeof__(*(HT)->entries) re_macro_var(old) = (HT)->entries[i]; \
        if (!re_macro_var(old).alive) { \
            continue; \
        } \
        __typeof__(*(HT)->entries) *re_macro_var(entry) = NULL; \
        _re_ht_get_entry(re_macro_var(new_entries), re_macro_var(new_cap), re_macro_var(old).hash, re_macro_var(entry)); \
        *re_macro_var(entry) = re_macro_var(old); \
    } \
    re_free((HT)->entries); \
    (HT)->entries = re_macro_var(new_entries); \
    (HT)->capacity = re_macro_var(new_cap); \
} while (0)

RE_API re_ht_iter_t __re_ht_iter_next(usize_t start, void *entries, usize_t entry_count, usize_t alive_offset, usize_t stride);
#define _re_ht_iter_next(HT, START) \
    __re_ht_iter_next( \
        (START), \
        (HT)->entries, \
        (HT)->capacity, \
        re_offsetof(__typeof__(*(HT)->entries), alive), \
        sizeof(__typeof__(*(HT)->entries)) \
    )

/*=========================*/
// Strings
/*=========================*/

typedef struct re_str_t re_str_t;
struct re_str_t {
    usize_t len;
    const char *str;
};

#define re_str_null { 0, NULL }
#define re_str_lit(str) re_str(str, sizeof(str) - 1)
#define re_str_cstr(str) re_str(str, strlen(str))
RE_API re_str_t re_str(const char *cstr, usize_t len);
RE_API re_str_t re_str_sub(re_str_t string, usize_t start, usize_t end);
RE_API re_str_t re_str_prefix(re_str_t string, usize_t len);
RE_API re_str_t re_str_suffix(re_str_t string, usize_t len);
RE_API re_str_t re_str_chop(re_str_t string, usize_t len);
RE_API re_str_t re_str_skip(re_str_t string, usize_t len);
RE_API i32_t    re_str_cmp(re_str_t a, re_str_t b);

/*=========================*/
// Linked lists
/*=========================*/

// Singly linked list stack (slls)
#define re_slls_push_n(STACK, NODE, NEXT) ( \
    (NODE)->NEXT = NULL, \
    (STACK) == NULL ? \
        ((STACK) = NODE) : \
        ((NODE)->NEXT = (STACK), (STACK) = (NODE)) \
)
#define re_slls_pop_n(STACK, NODE) ( \
    (STACK) != NULL ? \
        (STACK) = (STACK)->NODE : \
        0 \
)
#define re_slls_push(STACK, NODE) re_slls_push_n(STACK, NODE, next)
#define re_slls_pop(STACK) re_slls_pop_n(STACK, next)

// Singly linked list queue (sllq)
#define re_sllq_push_front_n(FIRST, LAST, NODE, NEXT) ( \
    (FIRST) == NULL ? \
        ((FIRST) = (LAST) = (NODE), (NODE)->NEXT = NULL) : \
        ((NODE)->NEXT = (FIRST), (FIRST) = (NODE)) \
)
#define re_sllq_push_back_n(FIRST, LAST, NODE, NEXT) ( \
    (NODE)->NEXT = NULL, \
    (FIRST) == NULL ? \
        (FIRST) = (LAST) = (NODE) : \
        ((LAST)->NEXT = (NODE), (LAST) = (NODE)) \
)
#define re_sllq_pop_n(FIRST, LAST, NEXT) ( \
    (FIRST) == (LAST) ? \
        (FIRST) = (LAST) = NULL : \
        ((FIRST) = (FIRST)->NEXT) \
)
#define re_sllq_push_front(FIRST, LAST, NODE) \
    re_sllq_push_front_n(FIRST, LAST, NODE, next)
#define re_sllq_push_back(FIRST, LAST, NODE) \
    re_sllq_push_back_n(FIRST, LAST, NODE, next)
#define re_sllq_pop(FIRST, LAST) \
    re_sllq_pop_n(FIRST, LAST, next)

// Doubly linked list (dll)
#define re_dll_push_back_np(FIRST, LAST, NODE, NEXT, PREV) ( \
    (NODE)->NEXT = NULL, \
    (FIRST) == NULL ? \
        ((FIRST) = (LAST) = (NODE), (NODE)->PREV = NULL) : \
        ((LAST)->NEXT = (NODE), ((NODE)->PREV = (LAST), (LAST) = (NODE))) \
)
#define re_dll_push_front_np(FIRST, LAST, NODE, NEXT, PREV) ( \
    (NODE)->PREV = NULL, \
    (FIRST) == NULL ? \
        ((FIRST) = (LAST) = (NODE), (NODE)->NEXT = NULL) : \
        ((FIRST)->PREV = (NODE), ((NODE)->NEXT = (FIRST), (FIRST) = (NODE))) \
)
#define re_dll_remove_np(FIRST, LAST, NODE, NEXT, PREV) ( \
    (NODE) == (FIRST) ? \
        ((FIRST) = (FIRST)->NEXT, (FIRST)->PREV = NULL) : \
        ((NODE) == (LAST) ? \
            ((LAST) = (LAST)->PREV, (LAST)->NEXT = NULL) : \
            ((NODE)->PREV->NEXT = (NODE)->NEXT, (NODE)->NEXT->PREV = (NODE)->PREV)) \
)
#define re_dll_push_back(FIRST, LAST, NODE) \
    re_dll_push_back_np(FIRST, LAST, NODE, next, prev)
#define re_dll_push_front(FIRST, LAST, NODE) \
    re_dll_push_front_np(FIRST, LAST, NODE, next, prev)
#define re_dll_remove(FIRST, LAST, NODE) \
    re_dll_remove_np(FIRST, LAST, NODE, next, prev)

/*=========================*/
// Dynamic array
/*=========================*/

// Declares a new dynamic array.
// Use this instead of 'i32_t *dyn_arr;' to make it easier to identify dynamic arrays.
#define re_da_t(T) T *

// Initializes the dynamic array.
#define re_da_create(DA) _re_da_create((void **) &(DA), sizeof(*(DA)))
// Frees all memory used by dynamic array and sets DA to NULL.
#define re_da_destroy(DA) _re_da_destroy((void **) &(DA))

// Insert value into dynamic array at a certain index preserving the order of items.
#define re_da_insert(DA, VALUE, INDEX) do { \
    __typeof__(*DA) re_macro_var(temp_value) = (VALUE); \
    _re_da_insert_arr((void **) &(DA), &re_macro_var(temp_value), 1, (INDEX)); \
} while (0)
// Removes value from dynamic array at a certain index preserving the order of items.
// If OUT is not NULL the removed values will be copied to it.
#define re_da_remove(DA, INDEX, OUT) _re_da_remove_arr((void **) &(DA), 1, (INDEX), OUT)

// Inserts value into dynamic array by swapping the value at INDEX to the last position and replacing it with VALUE.
#define re_da_insert_fast(DA, VALUE, INDEX) do { \
    __typeof__(*DA) re_macro_var(temp_value) = (VALUE); \
    _re_da_insert_fast((void **) &(DA), &re_macro_var(temp_value), (INDEX)); \
} while (0)
// Removes value at INDEX by replace the value with the last value. 
// If OUT is not NULL the removed values will be copied to it.
#define re_da_remove_fast(DA, INDEX, OUT) _re_da_remove_fast((void **) &(DA), (INDEX), OUT)

// Pushes VALUE to the back of the dynamic array.
#define re_da_push(DA, VALUE) re_da_insert_fast(DA, VALUE, re_da_count(DA));
// Removes the last value from the dynamic array.
// If OUT is not NULL the removed values will be copied to it.
#define re_da_pop(DA, OUT) re_da_remove_fast(DA, re_da_count(DA) - 1, OUT)

// Inserts an entire array into the dynamic array.
#define re_da_insert_arr(DA, ARR, COUNT, INDEX) _re_da_insert_arr((void **) &(DA), (ARR), (COUNT), (INDEX))
// Removes COUNT elements from dyanmic array.
// If OUT is not NULL the removed values will be copied to it.
#define re_da_remove_arr(DA, COUNT, INDEX, OUT) _re_da_remove_arr((void **) &(DA), (COUNT), (INDEX), (OUT))

// Pushes an entire array onto the back of the dynamic array.
#define re_da_push_arr(DA, ARR, COUNT) _re_da_insert_arr((void **) &(DA), (ARR), (COUNT), re_da_count(DA))
// Removes COUNT elements from the back of the dyanmic array.
// If OUT is not NULL the removed values will be copied to it.
#define re_da_pop_arr(DA, COUNT, OUT) _re_da_remove_arr((void **) &(DA), (COUNT), re_da_count(DA) - (COUNT), (OUT));

// Retrieves the number of elements stored in the dynamic array.
#define re_da_count(DA) _re_da_count(DA)
// Retrieves the last value in the dynamic array.
#define re_da_last(DA) ((DA)[re_da_count(DA) - 1])
// Makes an iterator to iterate over dyanmic array.
#define re_da_iter(DA, I) for (usize_t I = 0; I < re_da_count(DA); I++)

// Private API
RE_API void _re_da_resize(void **da, usize_t count);
RE_API void _re_da_create(void **da, usize_t size);
RE_API void _re_da_destroy(void **da);
RE_API void _re_da_insert_fast(void **da, const void *value, usize_t index);
RE_API void _re_da_remove_fast(void **da, usize_t index, void *output);
RE_API void _re_da_insert_arr(void **da, const void *arr, usize_t count, usize_t index);
RE_API void _re_da_remove_arr(void **da, usize_t count, usize_t index, void *output);
RE_API usize_t _re_da_count(void *da);

/*=========================*/
// Logger
/*=========================*/

#ifndef RE_LOG_MESSAGE_MAX_LENGTH
#define RE_LOG_MESSAGE_MAX_LENGTH 1024
#endif

#ifndef RE_LOGGER_CALLBACK_MAX
#define RE_LOGGER_CALLBACK_MAX 32
#endif

typedef enum {
    RE_LOG_LEVEL_FATAL,
    RE_LOG_LEVEL_ERROR,
    RE_LOG_LEVEL_WARN,
    RE_LOG_LEVEL_INFO,
    RE_LOG_LEVEL_DEBUG,
    RE_LOG_LEVEL_TRACE,

    RE_LOG_LEVEL_COUNT
} re_log_level_t;

typedef struct re_log_event_t re_log_event_t;
struct re_log_event_t {
    char message[RE_LOG_MESSAGE_MAX_LENGTH];
    u32_t message_length;
    const char *file;
    i32_t line;
    re_log_level_t level;
    void *user_data;
    struct {
        u8_t hour;
        u8_t min;
        u8_t sec;
    } time;
};

typedef void (*re_log_callback_t)(re_log_event_t *const event);

// Adds a callback to be called every time a logging function happens.
// Callback will only be called if log level is equal or more urgent than 'level'.
// 'user_data' will be passed to the callback via the log event.
RE_API void re_logger_add_callback(
        re_log_callback_t callback,
        re_log_level_t level,
        void *user_data);
// Adds an output file to write logs to.
// Only logs with 'level' or higher urgency will be written.
RE_API void re_logger_add_fp(FILE *fp, re_log_level_t level);
// Silences the logger from outputting to standard output and standard error.
// Callbacks will still be called.
RE_API void re_logger_set_silent(b8_t silent);
// Set lowest level of logging urgency for standard output and standard error.
// Silencing will override this option.
// Default level is RE_LOG_LEVEL_TRACE.
RE_API void re_logger_set_level(re_log_level_t level);

// Log a fatal error.
#define re_log_fatal(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_FATAL, __VA_ARGS__)
// Log an error.
#define re_log_error(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_ERROR, __VA_ARGS__)
// Log a warning.
#define re_log_warn(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_WARN, __VA_ARGS__)
// Log some information.
#define re_log_info(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_INFO, __VA_ARGS__)
// Log some debug information.
#define re_log_debug(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_DEBUG, __VA_ARGS__)
// Log some trace information.
#define re_log_trace(...) \
    _re_log(__FILE__, __LINE__, RE_LOG_LEVEL_TRACE, __VA_ARGS__)

// Private API
RE_API void _re_log(
        const char *file,
        i32_t line,
        re_log_level_t level,
        const char *fmt,
        ...) RE_FORMAT_FUNCTION(4, 5);

/*=========================*/
// Math
/*=========================*/

#define RAD(DEGS) (DEGS * 0.0174532925)
#define DEG(RADS) (RADS * 57.2957795)

// 2D vector.
typedef struct re_vec2_t re_vec2_t;
struct re_vec2_t {
    f32_t x, y;
};

RE_API re_vec2_t re_vec2(f32_t x, f32_t y);
RE_API re_vec2_t re_vec2s(f32_t scaler);

RE_API re_vec2_t re_vec2_mul(re_vec2_t a, re_vec2_t b);
RE_API re_vec2_t re_vec2_div(re_vec2_t a, re_vec2_t b);
RE_API re_vec2_t re_vec2_add(re_vec2_t a, re_vec2_t b);
RE_API re_vec2_t re_vec2_sub(re_vec2_t a, re_vec2_t b);

RE_API re_vec2_t re_vec2_muls(re_vec2_t vec, f32_t scaler);
RE_API re_vec2_t re_vec2_divs(re_vec2_t vec, f32_t scaler);
RE_API re_vec2_t re_vec2_adds(re_vec2_t vec, f32_t scaler);
RE_API re_vec2_t re_vec2_subs(re_vec2_t vec, f32_t scaler);

RE_API re_vec2_t re_vec2_rotate(re_vec2_t vec, f32_t degrees);

RE_API re_vec2_t re_vec2_normalize(re_vec2_t vec);
RE_API f32_t re_vec2_magnitude(re_vec2_t vec);
RE_API f32_t re_vec2_cross(re_vec2_t a, re_vec2_t b);
RE_API f32_t re_vec2_dot(re_vec2_t a, re_vec2_t b);

// 2D integer vector.
typedef struct re_ivec2_t re_ivec2_t;
struct re_ivec2_t {
    i32_t x, y;
};

RE_API re_ivec2_t re_ivec2(i32_t x, i32_t y);
RE_API re_ivec2_t re_ivec2s(i32_t scaler);

RE_API re_ivec2_t re_ivec2_mul(re_ivec2_t a, re_ivec2_t b);
RE_API re_ivec2_t re_ivec2_div(re_ivec2_t a, re_ivec2_t b);
RE_API re_ivec2_t re_ivec2_add(re_ivec2_t a, re_ivec2_t b);
RE_API re_ivec2_t re_ivec2_sub(re_ivec2_t a, re_ivec2_t b);

RE_API re_ivec2_t re_ivec2_muls(re_ivec2_t vec, i32_t scaler);
RE_API re_ivec2_t re_ivec2_divs(re_ivec2_t vec, i32_t scaler);
RE_API re_ivec2_t re_ivec2_adds(re_ivec2_t vec, i32_t scaler);
RE_API re_ivec2_t re_ivec2_subs(re_ivec2_t vec, i32_t scaler);

RE_API re_ivec2_t re_ivec2_rotate(re_ivec2_t vec, f32_t degrees);

RE_API re_ivec2_t re_ivec2_normalize(re_ivec2_t vec);
RE_API f32_t re_ivec2_magnitude(re_ivec2_t vec);
RE_API i32_t re_ivec2_cross(re_ivec2_t a, re_ivec2_t b);
RE_API i32_t re_ivec2_dot(re_ivec2_t a, re_ivec2_t b);

// Conversion.
RE_API re_ivec2_t re_vec2_to_ivec2(re_vec2_t vec);
RE_API re_vec2_t re_ivec2_to_vec2(re_ivec2_t vec);

// 3D vector.
typedef struct re_vec3_t re_vec3_t;
struct re_vec3_t {
    f32_t x, y, z;
};

RE_API re_vec3_t re_vec3(f32_t x, f32_t y, f32_t z);
RE_API re_vec3_t re_vec3s(f32_t scaler);

RE_API re_vec3_t re_vec3_mul(re_vec3_t a, re_vec3_t b);
RE_API re_vec3_t re_vec3_div(re_vec3_t a, re_vec3_t b);
RE_API re_vec3_t re_vec3_add(re_vec3_t a, re_vec3_t b);
RE_API re_vec3_t re_vec3_sub(re_vec3_t a, re_vec3_t b);

RE_API re_vec3_t re_vec3_muls(re_vec3_t vec, f32_t scaler);
RE_API re_vec3_t re_vec3_divs(re_vec3_t vec, f32_t scaler);
RE_API re_vec3_t re_vec3_adds(re_vec3_t vec, f32_t scaler);
RE_API re_vec3_t re_vec3_subs(re_vec3_t vec, f32_t scaler);

RE_API re_vec3_t re_vec3_normalize(re_vec3_t vec);
RE_API f32_t re_vec3_magnitude(re_vec3_t vec);
RE_API re_vec3_t re_vec3_cross(re_vec3_t a, re_vec3_t b);
RE_API f32_t re_vec3_dot(re_vec3_t a, re_vec3_t b);

// 3D integer vector.
typedef struct re_ivec3_t re_ivec3_t;
struct re_ivec3_t {
    i32_t x, y, z;
};

RE_API re_ivec3_t re_ivec3(i32_t x, i32_t y, i32_t z);
RE_API re_ivec3_t re_ivec3s(i32_t scaler);

// 4D vector.
typedef struct re_vec4_t re_vec4_t;
struct re_vec4_t {
    f32_t x, y, z, w;
};

RE_API re_vec4_t re_vec4(f32_t x, f32_t y, f32_t z, f32_t w);
RE_API re_vec4_t re_vec4s(f32_t scaler);

// 4x4 matrix.
typedef struct re_mat4_t re_mat4_t;
struct re_mat4_t {
    re_vec4_t i, j, k, l;
};

// Creates a 4x4 identity matrix.
RE_API re_mat4_t re_mat4_identity(void);
// Calculates a 4x4 ortographic projection matrix.
RE_API re_mat4_t re_mat4_orthographic_projection(f32_t left, f32_t right, f32_t top, f32_t bottom, f32_t near, f32_t far);

//  ____  _       _    __                        _
// |  _ \| | __ _| |_ / _| ___  _ __ _ __ ___   | |    __ _ _   _  ___ _ __
// | |_) | |/ _` | __| |_ / _ \| '__| '_ ` _ \  | |   / _` | | | |/ _ \ '__|
// |  __/| | (_| | |_|  _| (_) | |  | | | | | | | |__| (_| | |_| |  __/ |
// |_|   |_|\__,_|\__|_|  \___/|_|  |_| |_| |_| |_____\__,_|\__, |\___|_|
//                                                          |___/
// Platform layer

/*=========================*/
// Dynamic library loading
/*=========================*/

typedef struct re_lib_t re_lib_t;

typedef void (*re_func_ptr_t)(void);

// Loads a dynamic library.
// Returns NULL if it fails.
RE_API re_lib_t *re_lib_load(const char *filepath);
// Unloads a dynamic library.
RE_API void re_lib_unload(re_lib_t *lib);
// Retrieves a function using 'name' from dynamic library.
RE_API re_func_ptr_t re_lib_func(const re_lib_t *lib, const char *name);

/*=========================*/
// Multithreading
/*=========================*/

// Threads
typedef struct re_thread_t re_thread_t;
struct re_thread_t {
    usize_t handle;
};

typedef void (*re_thread_func_t)(void *arg);

// Creates a new thread and executes 'func' passing 'arg' to it.
RE_API re_thread_t re_thread_create(re_thread_func_t func, void *arg);
// Frees all memory and handles to thread.
RE_API void re_thread_destroy(re_thread_t thread);
// Pauses current thread until 'thread' is finished.
RE_API void re_thread_wait(re_thread_t thread);

// Mutexes
typedef struct re_mutex_t re_mutex_t;

// Creates a mutex.
RE_API re_mutex_t *re_mutex_create(void);
// Frees all memory and handles to mutex.
RE_API void re_mutex_destroy(re_mutex_t *mutex);
// Locks mutex.
RE_API void re_mutex_lock(re_mutex_t *mutex);
// Unlocks mutex.
RE_API void re_mutex_unlock(re_mutex_t *mutex);

/*=========================*/
// System info
/*=========================*/

// Gets time since last re_os_get_time call.
RE_API f32_t re_os_get_time(void);
// Gets number of usable cores.
RE_API u32_t re_os_get_processor_count(void);
// Gets size of a memory page.
RE_API u32_t re_os_get_page_size(void);

#endif // REBOUND_H
