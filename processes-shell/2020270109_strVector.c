#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include "2020270109_strVector.h"

strVector* strVector_new()
{
    strVector* _this = malloc(sizeof(strVector));
    
    (*_this)._size = 0;
    (*_this)._capacity = 16;
    (*_this)._vec = calloc((*_this)._capacity, sizeof(char *));

    (*_this).size = strVector_size;
    (*_this).capacity = strVector_capacity;
    (*_this).empty = strVector_empty;
    (*_this).full = strVector_full;
    (*_this).resize = strVector_resize;
    (*_this).reserve = strVector_reserve;
    (*_this).get = strVector_get;
    (*_this).at = strVector_at;
    (*_this).push_back = strVector_push_back;
    (*_this).pop_back = strVector_pop_back;
    (*_this).clear = strVector_clear;

    return _this;
}

size_t strVector_size(strVector* _this) { return (*_this)._size; }
size_t strVector_capacity(strVector* _this) { return (*_this)._capacity; }
bool strVector_empty(strVector* _this) { return (*_this)._size == 0; }
bool strVector_full(strVector* _this) { return (*_this)._size == (*_this)._capacity; }

void strVector_resize(strVector* _this, size_t _newSize)
{
    if(_newSize < (*_this)._size) strVector_error("can't make smaller");
    if(_newSize > (*_this)._capacity) (*_this).reserve(_this, _newSize);

    memset((*_this)._vec + (*_this)._size, 0x00, (_newSize - (*_this)._size) * sizeof(char *));

    (*_this)._size = _newSize;
}

void strVector_reserve(strVector* _this, size_t _newCapacity)
{
    char** tmp = (char **)NULL;
    if(!(tmp = (char **)realloc((*_this)._vec, _newCapacity * sizeof(char *)))) strVector_error("malloc failed\n");
    
    (*_this)._vec = tmp;
    (*_this)._capacity = _newCapacity;
    memset((*_this)._vec + (*_this)._size, 0x00, ((*_this)._capacity - (*_this)._size) * sizeof(char *));
}

char* strVector_get(strVector* _this, size_t _idx) { return (*_this)._vec[_idx]; }

char** strVector_at(strVector* _this) { return (*_this)._vec; }

void strVector_push_back(strVector* _this, char* _str)
{
    if((*_this).full(_this)) (*_this).reserve(_this, (*_this)._capacity * 2);

    (*_this)._vec[(*_this)._size] = _str;
    (*_this)._size++;
}

char* strVector_pop_back(strVector* _this)
{
    if((*_this).empty(_this)) strVector_error("strVector is empty\n");
    (*_this)._size--;

    return (*_this)._vec[(*_this)._size];
}

void strVector_clear(strVector* _this) { (*_this)._size = 0; }

void strVector_delete(strVector* _this)
{
    free((*_this)._vec);
    free(_this);
}

void strVector_error(const char const *_error_message)
{
    const char* _error_name = "strVector_error : ";
    write(STDERR_FILENO, _error_name, strlen(_error_name));
    write(STDERR_FILENO, _error_message, strlen(_error_message));
    exit(1);
}