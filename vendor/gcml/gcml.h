#ifndef GCML_H
#define GCML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------
// General Utility Macros
// -------------------------

/**
 * @brief Returns the minimum of two values.
 */
#define MIN(a, b) (( (a) < (b) ) ? (a) : (b))

/**
 * @brief Returns the maximum of two values.
 */
#define MAX(a, b) (( (a) > (b) ) ? (a) : (b))

/**
 * @brief Calculates the number of elements in an array.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Suppresses compiler warnings for unused variables.
 */
#define UNUSED(x) (void)(x)

/**
 * @brief Aligns a value `x` up to the nearest multiple of `align`.
 */
#define ALIGN_UP(x, align) (((x) + ((align)-1)) & ~((align)-1))

/**
 * @brief Aligns a value `x` down to the nearest multiple of `align`.
 */
#define ALIGN_DOWN(x, align) ((x) & ~((align)-1))

// -------------------------
// Debugging and Logging Macros
// -------------------------

#ifdef DEBUG
    /**
     * @brief Prints debug messages with file name, line number, and function name.
     */
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) // No operation in release builds
#endif

/**
 * @brief Logs informational messages.
 */
#define LOG_INFO(fmt, ...) \
    fprintf(stdout, "INFO: " fmt "\n", ##__VA_ARGS__)

/**
 * @brief Logs warning messages.
 */
#define LOG_WARN(fmt, ...) \
    fprintf(stderr, "WARNING: " fmt "\n", ##__VA_ARGS__)

/**
 * @brief Logs error messages.
 */
#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__)

/**
 * @brief Logs fatal error messages and exits the program.
 */
#define LOG_FATAL(fmt, ...) do { \
    fprintf(stderr, "FATAL: " fmt "\n", ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0)

// -------------------------
// Assertion Macros
// -------------------------

/**
 * @brief Asserts a condition and logs an error message if the condition is false.
 */
#ifndef NDEBUG
    #define ASSERT(cond, fmt, ...) do { \
        if (!(cond)) { \
            fprintf(stderr, "Assertion failed: (%s), function %s, file %s, line %d.\n", \
                    #cond, __func__, __FILE__, __LINE__); \
            fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
            abort(); \
        } \
    } while (0)
#else
    #define ASSERT(cond, fmt, ...) ((void)0)
#endif

// -------------------------
// Stringification Macros
// -------------------------

/**
 * @brief Converts a macro argument to a string.
 */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// -------------------------
// Token Pasting Macros
// -------------------------

/**
 * @brief Concatenates two tokens.
 */
#define CONCAT(a, b) a ## b

/**
 * @brief Concatenates three tokens.
 */
#define CONCAT3(a, b, c) a ## b ## c

// -------------------------
// Memory Management Macros
// -------------------------

/**
 * @brief Allocates memory and checks for allocation failure.
 * @param ptr The pointer to assign the allocated memory.
 * @param size The size in bytes to allocate.
 */
#define SAFE_MALLOC(ptr, size) do { \
    (ptr) = malloc(size); \
    if ((ptr) == NULL) { \
        LOG_FATAL("Memory allocation failed for size %zu", (size_t)(size)); \
    } \
} while (0)

/**
 * @brief Allocates zero-initialized memory and checks for allocation failure.
 * @param ptr The pointer to assign the allocated memory.
 * @param count The number of elements to allocate.
 * @param type The type of each element.
 */
#define SAFE_CALLOC(ptr, count, type) do { \
    (ptr) = calloc((count), sizeof(type)); \
    if ((ptr) == NULL) { \
        LOG_FATAL("Memory allocation (calloc) failed for count %zu of type %s", \
                  (size_t)(count), #type); \
    } \
} while (0)

/**
 * @brief Frees memory and sets the pointer to NULL.
 * @param ptr The pointer to free.
 */
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    ptr = NULL; \
} while(0)

// -------------------------
// Type Casting Macros
// -------------------------

/**
 * @brief Safely casts a pointer to a specific type.
 * @param ptr The pointer to cast.
 * @param type The target type.
 */
#define SAFE_CAST(ptr, type) ((type)(ptr))

// -------------------------
// Bit Manipulation Macros
// -------------------------

/**
 * @brief Sets a bit at a specific position.
 * @param x The variable.
 * @param pos The bit position.
 */
#define SET_BIT(x, pos) ((x) |= (1U << (pos)))

/**
 * @brief Clears a bit at a specific position.
 * @param x The variable.
 * @param pos The bit position.
 */
#define CLEAR_BIT(x, pos) ((x) &= ~(1U << (pos)))

/**
 * @brief Toggles a bit at a specific position.
 * @param x The variable.
 * @param pos The bit position.
 */
#define TOGGLE_BIT(x, pos) ((x) ^= (1U << (pos)))

/**
 * @brief Checks if a bit at a specific position is set.
 * @param x The variable.
 * @param pos The bit position.
 * @return Non-zero if the bit is set, zero otherwise.
 */
#define CHECK_BIT(x, pos) (((x) >> (pos)) & 1U)

// -------------------------
// Compile-Time Assertion Macro
// -------------------------

/**
 * @brief Performs a compile-time assertion.
 * @param expr The expression to evaluate.
 * @param msg The message to display if the assertion fails.
 */
#define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)

// -------------------------
// Deprecation Warning Macros
// -------------------------

/**
 * @brief Marks a function as deprecated with a custom message.
 */
#if defined(__GNUC__) || defined(__clang__)
    #define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define DEPRECATED(msg) __declspec(deprecated(msg))
#else
    #pragma message("WARNING: DEPRECATED macro is not supported for this compiler.")
    #define DEPRECATED(msg)
#endif

// -------------------------
// Loop Macros
// -------------------------

/**
 * @brief Iterates over each element in an array.
 * @param item The loop variable.
 * @param array The array to iterate over.
 */
#define FOREACH(item, array) \
    for (size_t keep = 1, \
                count = ARRAY_SIZE(array), \
                i = 0; \
         keep && i < count; \
         keep = !keep, i++) \
        for (item = (array) + i; keep; keep = !keep)

/**
 * @brief Repeats a block of code `n` times.
 * @param n The number of times to repeat.
 * @param block The block of code to execute.
 */
#define REPEAT(n, block) \
    for (size_t _i = 0; _i < (n); ++_i) { block; }

// -------------------------
// Swap Macro
// -------------------------

/**
 * @brief Swaps two variables of the same type.
 * @param a The first variable.
 * @param b The second variable.
 */
#define SWAP(a, b) do { \
    typeof(a) _swap_temp = (a); \
    (a) = (b); \
    (b) = _swap_temp; \
} while (0)

// -------------------------
// Execute Once Macro
// -------------------------

/**
 * @brief Executes a block of code only once.
 * @param block The block of code to execute.
 */
#define DO_ONCE(block) \
    do { \
        static int _do_once_flag = 0; \
        if (!_do_once_flag) { \
            _do_once_flag = 1; \
            block \
        } \
    } while (0)

// -------------------------
// Utility Macros
// -------------------------

/**
 * @brief Calculates the offset of a member within a struct.
 * @param type The struct type.
 * @param member The member within the struct.
 */
#define OFFSET_OF(type, member) ((size_t) &(((type *)0)->member))

/**
 * @brief Retrieves the containing struct from a member pointer.
 * @param ptr The pointer to the member.
 * @param type The type of the containing struct.
 * @param member The member within the struct.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - OFFSET_OF(type, member)))


// -------------------------
// Additional Utility Macros
// -------------------------

/**
 * @brief Safely reallocates memory and checks for allocation failure.
 * @param ptr The pointer to the previously allocated memory.
 * @param size The new size in bytes to allocate.
 */
#define SAFE_REALLOC(ptr, size) do { \
    void* _tmp = realloc((ptr), (size)); \
    if ((_tmp) == NULL) { \
        LOG_FATAL("Memory reallocation failed for size %zu", (size_t)(size)); \
    } else { \
        (ptr) = _tmp; \
    } \
} while (0)

/**
 * @brief Marks a function as unused to suppress compiler warnings.
 */
#define UNUSED_FUNCTION __attribute__((unused))

/**
 * @brief Converts a value to a string at compile time.
 * @param value The value to stringify.
 * @return The string representation of the value.
 */
#define TO_STRING(value) TOSTRING(value)

/**
 * @brief Generates a unique identifier by appending the line number.
 * @param prefix The prefix for the identifier.
 * @return A unique identifier.
 */
#define UNIQUE_ID(prefix) CONCAT(prefix, __LINE__)

/**
 * @brief Forces a value to evaluate to a specific type without altering its binary representation.
 * @param value The value to cast.
 * @param type The target type.
 * @return The value cast to the specified type.
 */
#define FORCE_CAST(value, type) (*(type*)&(value))

/**
 * @brief Creates a do-while loop that executes exactly once.
 * @param block The block of code to execute.
 */
#define EXECUTE_ONCE(block) do { block } while(0)

/**
 * @brief Checks if a pointer is aligned to a specified boundary.
 * @param ptr The pointer to check.
 * @param align The alignment boundary (must be a power of two).
 * @return Non-zero if aligned, zero otherwise.
 */
#define IS_ALIGNED(ptr, align) ((((uintptr_t)(const void*)(ptr)) & ((align) - 1)) == 0)

/**
 * @brief Calculates the number of bits set to 1 in a variable.
 * @param x The variable to count bits in.
 * @return The number of bits set to 1.
 */
#define COUNT_SET_BITS(x) (__builtin_popcount(x))

/**
 * @brief Calculates the ceiling of a division between two integers.
 * @param numerator The numerator.
 * @param denominator The denominator.
 * @return The ceiling of the division.
 */
#define CEIL_DIV(numerator, denominator) (((numerator) + (denominator) - 1) / (denominator))

/**
 * @brief Concatenates two tokens with an underscore.
 * @param a The first token.
 * @param b The second token.
 * @return The concatenated token separated by an underscore.
 */
#define CONCAT_WITH_UNDERSCORE(a, b) CONCAT(a, _##b)

/**
 * @brief Swaps two variables without using a temporary variable (only for integer types).
 * @param a The first variable.
 * @param b The second variable.
 */
#define SWAP_INPLACE(a, b) do { \
    (a) ^= (b); \
    (b) ^= (a); \
    (a) ^= (b); \
} while (0)

// -------------------------
// Safe Reallocation Macro
// -------------------------

/**
 * @brief Safely reallocates memory and checks for allocation failure.
 * @param ptr The pointer to the previously allocated memory.
 * @param size The new size in bytes to allocate.
 */
#define SAFE_REALLOC(ptr, size) do { \
    void* _tmp = realloc((ptr), (size)); \
    if ((_tmp) == NULL) { \
        LOG_FATAL("Memory reallocation failed for size %zu", (size_t)(size)); \
    } else { \
        (ptr) = _tmp; \
    } \
} while (0)





#define MAX_OF(...)  MAX_OF_IMPL(__VA_ARGS__, MAX_OF_RSEQ_N())
#define MAX_OF_IMPL(...)  MAX_OF_ARG_N(__VA_ARGS__)
#define MAX_OF_ARG_N(_1, _2, _3, _4, _5, N, ...) N
#define MAX_OF_RSEQ_N() 5,4,3,2,1,0

#define MIN_OF(...)  MIN_OF_IMPL(__VA_ARGS__, MIN_OF_RSEQ_N())
#define MIN_OF_IMPL(...)  MIN_OF_ARG_N(__VA_ARGS__)
#define MIN_OF_ARG_N(_1, _2, _3, _4, _5, N, ...) N
#define MIN_OF_RSEQ_N() 5,4,3,2,1,0


#define ZERO_STRUCT(s) memset(&(s), 0, sizeof(s))


#define PRINT_VAR(var) LOG_INFO(#var " = %d", var)







#endif // GCML_H



