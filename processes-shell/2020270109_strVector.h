#ifndef __STRVECTOR_H
#define __STRVECTOR_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct _strVector strVector;

strVector* strVector_new();
size_t strVector_size(strVector* _this);
bool strVector_empty(strVector* _this);
bool strVector_full(strVector* _this);
size_t strVector_capacity(strVector* _this);
void strVector_resize(strVector* _this, size_t _newSize);
void strVector_reserve(strVector* _this, size_t _newCapacity);
char* strVector_get(strVector* _this, size_t _idx);
char** strVector_at(strVector* _this);
void strVector_push_back(strVector* _this, char* _str);
char* strVector_pop_back(strVector* _this);
void strVector_clear(strVector* _this);
void strVector_delete(strVector* _this);
void strVector_error(const char const *_error_message);


struct _strVector
{
    size_t (*size)(strVector* _this);
    size_t (*capacity)(strVector* _this);
    bool (*empty)(strVector* _this);
    bool (*full)(strVector* _this);
    void (*resize)(strVector* _this, size_t _newSize);
    void (*reserve)(strVector* _this, size_t _newCapacity);
    char* (*get)(strVector* _this, size_t _idx);
    char** (*at)(strVector* _this);
    void (*push_back)(strVector* _this, char *_str);
    char* (*pop_back)(strVector* _this); // char*을 다루므로 pop 이여도 반환
    void (*clear)(strVector* _this);

    size_t _size;
    size_t _capacity;
    char** _vec;
};

#endif