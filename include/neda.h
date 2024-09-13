/*
 * 1.3.0
 * neda - Dynamic Array library. By nenevictor (shyvikaisinlove)
 *
 * Peace! So, this is my realization of dynamic array library.
 * The only benefit of this library - it is simple.
 *
 * Little example of using:
 *   #include <stdio.h>
 *   #include <neda.h>
 *   NEDA_HEADER(float);
 *   NEDA_BODY_IMPLEMENTATION(float, 128);
 *   int main(void)
 *   {
 *     struct neda_float *da = 0;
 *     neda_float__init(&da);
 *     neda_float__set_default_compare_function();
 *     neda_float__set_default_swap_function();
 *     neda_float__push_back(da, 1.123);
 *     printf("Array size: %u;\n", (unsigned int)neda_float__size(da));
 *     printf("Stored value: %.3f;\n", neda_float__at(da, 0));
 *     neda_float__free(&da);
 *     return 0;
 *   }
 *
 * ------------------------------------------------------------------------------
 * This software is available under 2 licenses -- choose whichever you prefer.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE A - MIT License
 * Copyright (c) 2024 nenevictor
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE B - Public Domain (www.unlicense.org)
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ------------------------------------------------------------------------------
 *
 * Libary change dates:
 *   25.06.2024: basic functions added.
 *   29.07.2024: version 1.1.0 added new functions.
 *   29.07.2024: added "copy" function.
 *   02.08.2024: added new function. "swap" and "sort" now has the basic
 *               versions (sadly, they still too slow). Now library do
 *               not need "NEDA_IMPLEMENTATION". Library now works
 *               in c89 standard.
 *   05.08.2024: fixed "inline" and "reserve" problems.
 *   10.08.2024: library got "postfix" versions of header and body implementation.
 *   14.08.2024: little optimization. Now library works in "-Wpedantic" mode.
 *   14.09.2024: added new functions: "quick_sort", "set_default_functions",
 *               "push_back_empty" and "push_front_empty". Removed basic version
 *               of sort - it is pointless thing. Because of it also functions "swap"
 *               and "basic_swap" was removed. Code was a little refactored.
 * 
 * Optimized quick_sort was stolen from here: https://alienryderflex.com/quicksort/
 *                 
 */

#ifndef NEDA_H
#define NEDA_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef NEDA_FORCE_INLINE
#ifndef _MSC_VER
#define NEDA_INLINE __forceinline
#else
#define NEDA_INLINE inline
#endif
#else
#define NEDA_INLINE
#endif

#ifndef __cplusplus
#define NEDA_REGISTER register
#else
#define NEDA_REGISTER
#endif

#ifdef NEDA_STATIC
#define NEDA_DEF static
#else
#ifdef __cplusplus
#define NEDA_DEF extern "C"
#else
#define NEDA_DEF extern
#endif
#endif

#define NEDA_API NEDA_INLINE
#define NEDA_API_NO_INLINE

#if !defined(NEDA_MALLOC) || !defined(NEDA_REALLOC) || !defined(NEDA_FREE)
#include <malloc.h>
#endif

#ifndef NEDA_MALLOC
#define NEDA_MALLOC(_size) malloc(_size)
#endif

#ifndef NEDA_REALLOC
#define NEDA_REALLOC(_memory, _new_size) realloc(_memory, _new_size)
#endif

#ifndef NEDA_FREE
#define NEDA_FREE(_memory) free(_memory)
#endif

#if !defined(NEDA_ASSERT)

#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
#include <assert.h>
#define NEDA_ASSERT(_expresion) assert(_expresion)
#else
#define NEDA_ASSERT(_expresion)
#endif

#endif

#define NEDA_UNUSED(_value) (void)(_value)

#ifndef nedasize_t
#define nedasize_t unsigned int
#endif

#ifndef NEDA_QUICK_SORT_MAX_LEVELS
#define NEDA_QUICK_SORT_MAX_LEVELS 300
#endif

#define NEDA_CHUNK_RESERVE(_chunk_size, _buffer_size) (((nedasize_t)((float)(_buffer_size) / (float)(_chunk_size)) + 1) * _chunk_size)
/* \returns logical false, if array is valid. */
#define NEDA_VALIDATE(_da_ptr) (!_da_ptr || (_da_ptr->size > _da_ptr->capacity || ((_da_ptr->size > 1) && !_da_ptr->data)))

#define NEDA_HEADER_POSTFIX(_postfix, _type)                                                                            \
  typedef int (*neda_##_postfix##__compare_function_type)(const _type *_a, const _type *_b);                            \
  typedef void (*neda_##_postfix##__swap_function_type)(_type * _a, _type * _b);                                        \
  typedef void (*neda_##_postfix##__move_function_type)(_type * _destination, _type * _source);                         \
  typedef struct neda_##_postfix                                                                                        \
  {                                                                                                                     \
    _type *data;                                                                                                        \
    nedasize_t capacity, size;                                                                                          \
  } neda_##_postfix;                                                                                                    \
  NEDA_DEF void neda_##_postfix##__memset(_type *_data, const _type _value, const nedasize_t _elem_count);              \
  NEDA_API void neda_##_postfix##__byte_memset(_type *_data, const unsigned char _value, const nedasize_t _elem_count); \
  NEDA_DEF void neda_##_postfix##__init(struct neda_##_postfix **_da);                                                  \
  NEDA_DEF void neda_##_postfix##__reserve(struct neda_##_postfix *_da, const nedasize_t _size);                        \
  NEDA_DEF nedasize_t neda_##_postfix##__size(struct neda_##_postfix *_da);                                             \
  NEDA_DEF _type *neda_##_postfix##__data(struct neda_##_postfix *_da);                                                 \
  NEDA_DEF nedasize_t neda_##_postfix##__capacity(struct neda_##_postfix *_da);                                         \
  NEDA_DEF void neda_##_postfix##__push_back(struct neda_##_postfix *_da, const _type _value);                          \
  NEDA_DEF _type neda_##_postfix##__pop_back(struct neda_##_postfix *_da);                                              \
  NEDA_DEF void neda_##_postfix##__push_front(struct neda_##_postfix *_da, const _type _value);                         \
  NEDA_DEF _type neda_##_postfix##__pop_front(struct neda_##_postfix *_da);                                             \
  NEDA_DEF void neda_##_postfix##__push_back_empty(struct neda_##_postfix *_da);                                        \
  NEDA_DEF void neda_##_postfix##__push_front_empty(struct neda_##_postfix *_da);                                       \
  NEDA_DEF _type neda_##_postfix##__at(struct neda_##_postfix *_da, const nedasize_t _index);                           \
  NEDA_DEF _type *neda_##_postfix##__at_ptr(struct neda_##_postfix *_da, nedasize_t _index);                            \
  NEDA_DEF void neda_##_postfix##__shrink(struct neda_##_postfix *_da);                                                 \
  NEDA_DEF void neda_##_postfix##__shrink_to_fit(struct neda_##_postfix *_da);                                          \
  NEDA_DEF void neda_##_postfix##__insert(struct neda_##_postfix *_da, const nedasize_t _index, const _type _value);    \
  NEDA_DEF void neda_##_postfix##__erase(struct neda_##_postfix *_da, const nedasize_t _index);                         \
  NEDA_DEF void neda_##_postfix##__fill(struct neda_##_postfix *_da, const _type _value);                               \
  NEDA_DEF void neda_##_postfix##__fill_size(struct neda_##_postfix *_da, const nedasize_t _size, const _type _value);  \
  NEDA_DEF void neda_##_postfix##__copy(struct neda_##_postfix *_da1, struct neda_##_postfix *_da2);                    \
  NEDA_DEF void neda_##_postfix##__swap_function_default(_type *_a, _type *_b);                                         \
  NEDA_DEF void neda_##_postfix##__set_default_swap_function();                                                         \
  NEDA_DEF void neda_##_postfix##__set_swap_function(                                                                   \
      neda_##_postfix##__swap_function_type _swap_function);                                                            \
  NEDA_DEF int neda_##_postfix##__compare_function_default(const _type *_a, const _type *_b);                           \
  NEDA_DEF void neda_##_postfix##__set_default_compare_function();                                                      \
  NEDA_DEF void neda_##_postfix##__set_compare_function(                                                                \
      neda_##_postfix##__compare_function_type _compare_function);                                                      \
  NEDA_DEF void neda_##_postfix##__move_function_default(_type *_destination, _type *_source);                          \
  NEDA_DEF void neda_##_postfix##__set_default_move_function();                                                         \
  NEDA_DEF void neda_##_postfix##__set_move_function(                                                                   \
      neda_##_postfix##__move_function_type _move_function);                                                            \
  NEDA_DEF void neda_##_postfix##__set_default_functions();                                                             \
  NEDA_DEF void neda_##_postfix##__sort(struct neda_##_postfix *_da);                                                   \
  NEDA_DEF void neda_##_postfix##__clear(struct neda_##_postfix *_da);                                                  \
  NEDA_DEF void neda_##_postfix##__free(struct neda_##_postfix **_da);

#define NEDA_BODY_IMPLEMENTATION_POSTFIX(                           \
    _postfix,                                                       \
    _type,                                                          \
    _chunk_size)                                                    \
  const nedasize_t NEDA_CHUNK_SIZE_##_postfix = _chunk_size;        \
  neda_##_postfix##__swap_function_type                             \
      neda_##_postfix##__swap_function_callback = 0;                \
  neda_##_postfix##__compare_function_type                          \
      neda_##_postfix##__compare_function_callback = 0;             \
  neda_##_postfix##__move_function_type                             \
      neda_##_postfix##__move_function_callback = 0;                \
  NEDA_API void neda_##_postfix##__memset(                          \
      _type *_data,                                                 \
      const _type _value,                                           \
      const nedasize_t _elem_count)                                 \
  {                                                                 \
    nedasize_t i = 0;                                               \
    while (i < _elem_count)                                         \
    {                                                               \
      _data[i] = _value;                                            \
      i++;                                                          \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_postfix##__byte_memset(                     \
      _type *_data,                                                 \
      const unsigned char _value,                                   \
      const nedasize_t _elem_count)                                 \
  {                                                                 \
    nedasize_t i = 0;                                               \
    while (i < (_elem_count * sizeof(_type)))                       \
    {                                                               \
      ((unsigned char *)(_data))[i] = _value;                       \
      i++;                                                          \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_postfix##__init(                            \
      struct neda_##_postfix **_da)                                 \
  {                                                                 \
    NEDA_REGISTER nedasize_t i = 0;                                 \
    *_da = NEDA_MALLOC(sizeof(neda_##_postfix));                    \
    NEDA_ASSERT(*_da);                                              \
    while (i < sizeof(neda_##_postfix))                             \
    {                                                               \
      ((unsigned char *)*_da)[i] = 0;                               \
      i++;                                                          \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_postfix##__reserve(                         \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _size)                                       \
  {                                                                 \
    NEDA_REGISTER nedasize_t new_capacity;                          \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    new_capacity = NEDA_CHUNK_RESERVE(                              \
        NEDA_CHUNK_SIZE_##_postfix,                                 \
        _size);                                                     \
    if (new_capacity > _da->capacity)                               \
    {                                                               \
      _da->capacity = new_capacity;                                 \
      _da->data = (_type *)NEDA_REALLOC(                            \
          _da->data,                                                \
          sizeof(_type) * _da->capacity);                           \
      NEDA_ASSERT(_da->data);                                       \
    }                                                               \
  }                                                                 \
  NEDA_API nedasize_t neda_##_postfix##__size(                      \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_ASSERT(_da);                                               \
    return _da->size;                                               \
  }                                                                 \
  NEDA_API _type *neda_##_postfix##__data(                          \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_ASSERT(_da);                                               \
    return _da->data;                                               \
  }                                                                 \
  NEDA_API nedasize_t neda_##_postfix##__capacity(                  \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_ASSERT(_da);                                               \
    return _da->capacity;                                           \
  }                                                                 \
  NEDA_API void neda_##_postfix##__push_back(                       \
      struct neda_##_postfix *_da,                                  \
      const _type _value)                                           \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    neda_##_postfix##__reserve(                                     \
        _da,                                                        \
        _da->size + 1);                                             \
    _da->data[_da->size] = _value;                                  \
    _da->size++;                                                    \
  }                                                                 \
  NEDA_API _type neda_##_postfix##__pop_back(                       \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size);                                         \
    _da->size--;                                                    \
    return _da->data[_da->size];                                    \
  }                                                                 \
  NEDA_API void neda_##_postfix##__push_front(                      \
      struct neda_##_postfix *_da,                                  \
      const _type _value)                                           \
  {                                                                 \
    NEDA_REGISTER nedasize_t i;                                     \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    neda_##_postfix##__reserve(                                     \
        _da,                                                        \
        _da->size + 1);                                             \
    i = _da->size;                                                  \
    while (i > 0)                                                   \
    {                                                               \
      neda_##_postfix##__move_function_callback(                    \
          &_da->data[i],                                            \
          &_da->data[i - 1]);                                       \
      i--;                                                          \
    }                                                               \
    _da->data[0] = _value;                                          \
    _da->size++;                                                    \
  }                                                                 \
  NEDA_API _type neda_##_postfix##__pop_front(                      \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    _type result;                                                   \
    NEDA_REGISTER nedasize_t i = 0;                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size);                                         \
    result = _da->data[0];                                          \
    while (i < _da->size - 1)                                       \
    {                                                               \
      neda_##_postfix##__move_function_callback(                    \
          &_da->data[i],                                            \
          &_da->data[i + 1]);                                       \
      i++;                                                          \
    }                                                               \
    _da->size--;                                                    \
    return result;                                                  \
  }                                                                 \
  NEDA_DEF void neda_##_postfix##__push_back_empty(                 \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    _type value;                                                    \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    neda_##_postfix##__byte_memset(                                 \
        &value,                                                     \
        0,                                                          \
        1);                                                         \
    neda_##_postfix##__reserve(                                     \
        _da,                                                        \
        _da->size + 1);                                             \
    _da->data[_da->size] = value;                                   \
    _da->size++;                                                    \
  }                                                                 \
  NEDA_DEF void neda_##_postfix##__push_front_empty(                \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_REGISTER nedasize_t i;                                     \
    _type value;                                                    \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    neda_##_postfix##__byte_memset(                                 \
        &value,                                                     \
        0,                                                          \
        1);                                                         \
    neda_##_postfix##__reserve(                                     \
        _da,                                                        \
        _da->size + 1);                                             \
    i = _da->size;                                                  \
    while (i > 0)                                              \
    {                                                               \
      neda_##_postfix##__move_function_callback(                    \
          &_da->data[i],                                            \
          &_da->data[i - 1]);                                       \
      i--;                                                          \
    }                                                               \
    _da->data[0] = value;                                           \
    _da->size++;                                                    \
  }                                                                 \
  NEDA_API _type neda_##_postfix##__at(                             \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _index)                                      \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size > _index);                                \
    return _da->data[_index];                                       \
  }                                                                 \
  NEDA_API _type *neda_##_postfix##__at_ptr(                        \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _index)                                      \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size > _index);                                \
    return &_da->data[_index];                                      \
  }                                                                 \
  NEDA_API void neda_##_postfix##__shrink(                          \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_REGISTER nedasize_t new_capacity;                          \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    new_capacity = NEDA_CHUNK_RESERVE(                              \
        NEDA_CHUNK_SIZE_##_postfix,                                 \
        _da->size);                                                 \
    if (new_capacity < _da->capacity)                               \
    {                                                               \
      _da->data = (_type *)NEDA_REALLOC(                            \
          _da->data,                                                \
          sizeof(_type) * new_capacity);                            \
      _da->capacity = new_capacity;                                 \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_postfix##__shrink_to_fit(                   \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    if (_da->size == 0)                                             \
    {                                                               \
      NEDA_FREE(_da->data);                                         \
      _da->capacity = 0;                                            \
      return;                                                       \
    }                                                               \
    _da->data = (_type *)NEDA_REALLOC(                              \
        _da->data,                                                  \
        sizeof(_type) * _da->size);                                 \
    _da->capacity = _da->size;                                      \
  }                                                                 \
  NEDA_API void neda_##_postfix##__insert(                          \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _index,                                      \
      const _type _value)                                           \
  {                                                                 \
    NEDA_REGISTER nedasize_t i;                                     \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size + 1 > _index);                            \
    neda_##_postfix##__reserve(                                     \
        _da,                                                        \
        _da->size + 1);                                             \
    i = _da->size;                                                  \
    while (i > _index)                                              \
    {                                                               \
      neda_##_postfix##__move_function_callback(                    \
          &_da->data[i],                                            \
          &_da->data[i - 1]);                                       \
      i--;                                                          \
    }                                                               \
    _da->data[_index] = _value;                                     \
    _da->size++;                                                    \
  }                                                                 \
  NEDA_API void neda_##_postfix##__erase(                           \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _index)                                      \
  {                                                                 \
    NEDA_REGISTER nedasize_t i;                                     \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(_da->size > _index);                                \
    i = _index;                                                     \
    while (i < _da->size)                                           \
    {                                                               \
      neda_##_postfix##__move_function_callback(                    \
          &_da->data[i],                                            \
          &_da->data[i + 1]);                                       \
      i++;                                                          \
    }                                                               \
    _da->size--;                                                    \
  }                                                                 \
  NEDA_API void neda_##_postfix##__fill(                            \
      struct neda_##_postfix *_da,                                  \
      const _type _value)                                           \
  {                                                                 \
    NEDA_REGISTER nedasize_t i = 0;                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    while (i < _da->size)                                           \
    {                                                               \
      _da->data[i] = _value;                                        \
      i++;                                                          \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_postfix##__fill_size(                       \
      struct neda_##_postfix *_da,                                  \
      const nedasize_t _size,                                       \
      const _type _value)                                           \
  {                                                                 \
    NEDA_REGISTER nedasize_t i = 0;                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    neda_##_postfix##__reserve(_da, _size);                         \
    while (i < _size)                                               \
    {                                                               \
      _da->data[i] = _value;                                        \
      i++;                                                          \
    }                                                               \
    _da->size = _size;                                              \
  }                                                                 \
  NEDA_API void neda_##_postfix##__copy(                            \
      struct neda_##_postfix *_source,                              \
      struct neda_##_postfix *_destination)                         \
  {                                                                 \
    NEDA_REGISTER nedasize_t i = 0;                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_source));                           \
    NEDA_ASSERT(!NEDA_VALIDATE(_destination));                      \
    neda_##_postfix##__reserve(_destination, _source->size);        \
    while (i < _source->size)                                       \
    {                                                               \
      _destination->data[i] = _source->data[i];                     \
      i++;                                                          \
    }                                                               \
    _destination->size = _source->size;                             \
  }                                                                 \
  NEDA_API_NO_INLINE void neda_##_postfix##__swap_function_default( \
      _type *_a,                                                    \
      _type *_b)                                                    \
  {                                                                 \
    const _type temp = *_a;                                         \
    *_a = *_b;                                                      \
    *_b = temp;                                                     \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_default_swap_function()      \
  {                                                                 \
    neda_##_postfix##__swap_function_callback =                     \
        neda_##_postfix##__swap_function_default;                   \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_swap_function(               \
      neda_##_postfix##__swap_function_type _swap_function)         \
  {                                                                 \
    neda_##_postfix##__swap_function_callback = _swap_function;     \
  }                                                                 \
  NEDA_API_NO_INLINE int                                            \
      neda_##_postfix##__compare_function_default(                  \
          const _type *_a,                                          \
          const _type *_b)                                          \
  {                                                                 \
    NEDA_REGISTER int result = 0;                                   \
    /* If you want to compare variables more than 4 bite -          \
       write your own compare function */                           \
    NEDA_ASSERT(sizeof(_type) >= 4);                                \
    switch (sizeof(_type))                                          \
    {                                                               \
    case 1:                                                         \
      result = (*((char *)(_a))) < (*((char *)(_b)));               \
      break;                                                        \
    case 2:                                                         \
      result = (*((short *)(_a))) < (*((short *)(_b)));             \
      break;                                                        \
    case 4:                                                         \
      result = (*((int *)(_a))) < (*((int *)(_b)));                 \
      break;                                                        \
    default:                                                        \
      result = (*((nedasize_t *)(_a))) <                            \
               (*((nedasize_t *)(_b)));                             \
      break;                                                        \
    }                                                               \
    return result;                                                  \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_default_compare_function()   \
  {                                                                 \
    neda_##_postfix##__compare_function_callback =                  \
        neda_##_postfix##__compare_function_default;                \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_compare_function(            \
      neda_##_postfix##__compare_function_type _compare_function)   \
  {                                                                 \
    neda_##_postfix##__compare_function_callback =                  \
        _compare_function;                                          \
  }                                                                 \
  NEDA_API_NO_INLINE void neda_##_postfix##__move_function_default( \
      _type *_destination,                                          \
      _type *_source)                                               \
  {                                                                 \
    *_destination = *_source;                                       \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_default_move_function()      \
  {                                                                 \
    neda_##_postfix##__move_function_callback =                     \
        neda_##_postfix##__move_function_default;                   \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_move_function(               \
      neda_##_postfix##__move_function_type _move_function)         \
  {                                                                 \
    neda_##_postfix##__move_function_callback = _move_function;     \
  }                                                                 \
  NEDA_API void neda_##_postfix##__set_default_functions()          \
  {                                                                 \
    neda_##_postfix##__set_default_compare_function();              \
    neda_##_postfix##__set_default_swap_function();                 \
    neda_##_postfix##__set_default_move_function();                 \
  }                                                                 \
  NEDA_API void neda_##_postfix##__sort(                            \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    NEDA_REGISTER nedasize_t i_o = 0;                               \
    NEDA_REGISTER nedasize_t i_s = 0;                               \
    NEDA_REGISTER nedasize_t swap_index = 0;                        \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(neda_##_postfix##__compare_function_callback);      \
    NEDA_ASSERT(neda_##_postfix##__swap_function_callback);         \
    while (i_o < _da->size)                                         \
    {                                                               \
      swap_index = _da->size - 1;                                   \
      i_s = i_o;                                                    \
      while (i_s < _da->size)                                       \
      {                                                             \
        if (neda_##_postfix##__compare_function_callback(           \
                &_da->data[i_s],                                    \
                &_da->data[swap_index]))                            \
        {                                                           \
          swap_index = i_s;                                         \
        }                                                           \
        i_s++;                                                      \
      }                                                             \
      neda_##_postfix##__swap_function_callback(                    \
          &_da->data[i_o],                                          \
          &_da->data[swap_index]);                                  \
      i_o++;                                                        \
    }                                                               \
  }                                                                 \
  /* Original of this function was made by Darel Rex Finley. */     \
  void neda_##_postfix##__quick_sort(                               \
      struct neda_##_postfix *_da)                                  \
  {                                                                 \
    int beg[NEDA_QUICK_SORT_MAX_LEVELS];                            \
    int end[NEDA_QUICK_SORT_MAX_LEVELS];                            \
    NEDA_REGISTER int i = 0, left, right, swap;                     \
    _type piv;                                                      \
    NEDA_ASSERT(!NEDA_VALIDATE(_da));                               \
    NEDA_ASSERT(neda_##_postfix##__compare_function_callback);      \
    NEDA_ASSERT(neda_##_postfix##__swap_function_callback);         \
    NEDA_ASSERT(neda_##_postfix##__move_function_callback);         \
    beg[0] = 0;                                                     \
    end[0] = _da->size;                                             \
    while (i >= 0)                                                  \
    {                                                               \
      left = beg[i];                                                \
      right = end[i] - 1;                                           \
      if (left < right)                                             \
      {                                                             \
        neda_##_postfix##__move_function_callback(                  \
            &piv,                                                   \
            &_da->data[left]);                                      \
        while (left < right)                                        \
        {                                                           \
          while (neda_##_postfix##__compare_function_callback(      \
                     &_da->data[right],                             \
                     &piv) > 0 &&                                   \
                 left < right)                                      \
            right--;                                                \
          if (left < right)                                         \
            neda_##_postfix##__move_function_callback(              \
                &_da->data[left++],                                 \
                &_da->data[right]);                                 \
          while (neda_##_postfix##__compare_function_callback(      \
                     &piv,                                          \
                     &_da->data[left]) > 0 &&                       \
                 left < right)                                      \
            left++;                                                 \
          if (left < right)                                         \
            neda_##_postfix##__move_function_callback(              \
                &_da->data[right--],                                \
                &_da->data[left]);                                  \
        }                                                           \
        neda_##_postfix##__move_function_callback(                  \
            &_da->data[left],                                       \
            &piv);                                                  \
        beg[i + 1] = left + 1;                                      \
        end[i + 1] = end[i];                                        \
        end[i++] = left;                                            \
        if (end[i] - beg[i] > end[i - 1] - beg[i - 1])              \
        {                                                           \
          swap = beg[i];                                            \
          beg[i] = beg[i - 1];                                      \
          beg[i - 1] = swap;                                        \
          swap = end[i];                                            \
          end[i] = end[i - 1];                                      \
          end[i - 1] = swap;                                        \
        }                                                           \
      }                                                             \
      else                                                          \
      {                                                             \
        i--;                                                        \
      }                                                             \
    }                                                               \
  }                                                                 \
  NEDA_API void neda_##_type##__clear(                              \
      struct neda_##_type *_dynamic_array)                          \
  {                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                    \
    _dynamic_array->size = 0;                                       \
  }                                                                 \
  NEDA_API void neda_##_postfix##__free(                            \
      struct neda_##_postfix **_da)                                 \
  {                                                                 \
    NEDA_ASSERT((void *)(*_da));                                    \
    NEDA_FREE((*_da)->data);                                        \
    NEDA_FREE(*_da);                                                \
    *_da = 0;                                                       \
  }

#define NEDA_BODY_IMPLEMENTATION(_type, _chunk_size) NEDA_BODY_IMPLEMENTATION_POSTFIX(_type, _type, _chunk_size)
#define NEDA_HEADER(_type) NEDA_HEADER_POSTFIX(_type, _type)

#ifdef __cplusplus
}
#endif

#endif