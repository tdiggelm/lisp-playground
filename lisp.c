#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* TODO
- create ndt_nil (pair(NULL, NULL)), ndt_true (atom("#t"))
- create environment (use ut_hash for symbol lut)
- error handling: what happens when wrong type is in argument?
- maybe implement NDT_NUMBER type containing union of int64_t + double that can be passes by-value from sum function
*/


typedef enum {NDT_TYPE_PAIR, NDT_TYPE_DECIMAL, NDT_TYPE_INTEGER, NDT_TYPE_SYMBOL, NDT_TYPE_STRING} NDT_TYPE;

typedef struct object {
    NDT_TYPE type;
} NDT_OBJECT;

typedef struct ndt_decimal {
    NDT_TYPE type;
    double ndt_decimal;
} NDT_DECIMAL;

typedef struct ndt_integer {
    NDT_TYPE type;
    long long ndt_integer;
} NDT_INTEGER;

typedef struct ndt_string {
    NDT_TYPE type;
    const char* ndt_string;
} NDT_STRING;

typedef struct ndt_symbol {
    NDT_TYPE type;
    const char* ndt_symbol;
} NDT_SYMBOL;

typedef struct pair {
    NDT_TYPE type;
    const NDT_OBJECT* ndt_car;
    const NDT_OBJECT* ndt_cdr;
} NDT_PAIR;

NDT_OBJECT* ndt_make_integer(long long ndt_integer)
{
    NDT_INTEGER* obj = malloc(sizeof(NDT_INTEGER));
    obj->type = NDT_TYPE_INTEGER;
    obj->ndt_integer = ndt_integer;
    return (NDT_OBJECT*)obj;
}

NDT_OBJECT* ndt_make_symbol(const char* ndt_symbol)
{
    NDT_SYMBOL* obj = malloc(sizeof(NDT_SYMBOL));
    obj->type = NDT_TYPE_SYMBOL;
    obj->ndt_symbol = strdup(ndt_symbol);
    return (NDT_OBJECT*)obj;
}

NDT_OBJECT* ndt_make_string(const char* ndt_string)
{
    NDT_STRING* obj = malloc(sizeof(NDT_STRING));
    obj->type = NDT_TYPE_STRING;
    obj->ndt_string = strdup(ndt_string);
    return (NDT_OBJECT*)obj;
}

NDT_OBJECT* ndt_make_decimal(double ndt_decimal)
{
    NDT_DECIMAL* obj = malloc(sizeof(NDT_DECIMAL));
    obj->type = NDT_TYPE_DECIMAL;
    obj->ndt_decimal = ndt_decimal;
    return (NDT_OBJECT*)obj;
}

NDT_OBJECT* ndt_make_cons(const NDT_OBJECT* ndt_car, const NDT_OBJECT* ndt_cdr)
{
    NDT_PAIR* obj = malloc(sizeof(NDT_PAIR));
    obj->type = NDT_TYPE_PAIR;
    obj->ndt_car = ndt_car;
    obj->ndt_cdr = ndt_cdr;
    return (NDT_OBJECT*)obj;
}

long long ndt_integer(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_INTEGER);
    return ((NDT_INTEGER*)obj)->ndt_integer;
}

double ndt_decimal(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_DECIMAL);
    return ((NDT_DECIMAL*)obj)->ndt_decimal;
}

const char* ndt_symbol(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_SYMBOL);
    return ((NDT_SYMBOL*)obj)->ndt_symbol;
}

const char* ndt_string(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_STRING);
    return ((NDT_STRING*)obj)->ndt_string;
}

const NDT_OBJECT* ndt_car(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_PAIR);
    return ((NDT_PAIR*)obj)->ndt_car;
}

const NDT_OBJECT* ndt_cdr(const NDT_OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_PAIR);
    return ((NDT_PAIR*)obj)->ndt_cdr;
}

int ndt_is_nil(const NDT_OBJECT* obj)
{
    return (obj == NULL 
        || (obj->type == NDT_TYPE_PAIR 
            && ndt_car(obj) == NULL && ndt_cdr(obj) == NULL));
}

int ndt_is_integer(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_INTEGER);
}

int ndt_is_decimal(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_DECIMAL);
}

int ndt_is_string(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_STRING);
}

int ndt_is_symbol(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_SYMBOL);
}

int ndt_is_cons(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_PAIR);
}

void ndt_release(NDT_OBJECT* obj)
{
    if (obj == NULL) {
        return;
    }
    
    switch(obj->type) {
        case NDT_TYPE_STRING: {
            free((void*)ndt_string(obj));
            break;
        }
        case NDT_TYPE_SYMBOL: {
            free((void*)ndt_symbol(obj));
            break;
        }
        case NDT_TYPE_PAIR: {
            ndt_release((void*)ndt_car(obj));
            ndt_release((void*)ndt_cdr(obj));
            break;
        }
        default: {
            // do nothing
        }
    }
    
    free((void*)obj);
}

void print(const NDT_OBJECT* obj)
{
    if (obj == NULL) {
        printf("nil");
        return;
    }
    
    switch(obj->type) {
        case NDT_TYPE_INTEGER: {
            printf("%lld", ndt_integer(obj));
            break;
        }
        case NDT_TYPE_DECIMAL: {
            printf("%lf", ndt_decimal(obj));
            break;
        }
        case NDT_TYPE_STRING: {
            printf("\"%s\"", ndt_string(obj));
            break;
        }
        case NDT_TYPE_SYMBOL: {
            printf("%s", ndt_symbol(obj));
            break;
        }
        case NDT_TYPE_PAIR: {
            printf("(");
            print(ndt_car(obj));
            printf(" . ");
            print(ndt_cdr(obj));
            printf(")");
            break;
        }
        default: {
            printf("unkown type\n");
        }
    }
}

long long ndt_sum_integer(const NDT_OBJECT* args)
{
    assert(ndt_is_cons(args) 
        || ndt_is_nil(args) || ndt_is_integer(args) || ndt_is_decimal(args));
    
    if (args == NULL) { // create functions is_nil, is_integer, is_pair, etc.
        return 0;
    } else if (args->type == NDT_TYPE_INTEGER) {
        return ndt_integer(args);
    } else if (args->type == NDT_TYPE_DECIMAL) {
        return ndt_decimal(args);
    } else {
        const NDT_OBJECT* car = ndt_car(args);
        long long num;
        if (ndt_is_integer(car)) num = ndt_integer(car);
        else if (ndt_is_decimal(car)) num = ndt_decimal(car);
        return num + ndt_sum_integer(ndt_cdr(args));  
    }
}

double ndt_sum_decimal(const NDT_OBJECT* args)
{
    assert(ndt_is_cons(args) 
        || ndt_is_nil(args) || ndt_is_integer(args) || ndt_is_decimal(args));
    
    if (args == NULL) { // create functions is_nil, is_integer, is_pair, etc.
        return 0;
    } else if (args->type == NDT_TYPE_INTEGER) {
        return ndt_integer(args);
    } else if (args->type == NDT_TYPE_DECIMAL) {
        return ndt_decimal(args);
    } else {
        const NDT_OBJECT* car = ndt_car(args);
        double num;
        if (ndt_is_integer(car)) num = ndt_integer(car);
        else if (ndt_is_decimal(car)) num = ndt_decimal(car);
        return num + ndt_sum_decimal(ndt_cdr(args));  
    }
}

// infer type from first argument
NDT_OBJECT* ndt_sum(const NDT_OBJECT* args)
{
    if (ndt_is_cons(args) && ndt_is_decimal(ndt_car(args))) {
        return ndt_make_decimal(ndt_sum_decimal(args));
    } else {
        return ndt_make_integer(ndt_sum_integer(args));
    }
}

int main()
{
    size_t i;
    //for (i = 0; i < 10000; i++)
    {
        NDT_OBJECT* obj = ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_string("Hello World!"), ndt_make_cons(ndt_make_decimal(3.14159), ndt_make_cons(ndt_make_integer(100), NULL))));
        print(obj);
        printf("\n");
        ndt_release(obj);
    }
    
    long long sum = 0;
    for (i = 0; i < 10000; i++)
    {
        NDT_OBJECT* arg = ndt_make_cons(ndt_make_integer(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)));
        //print(arg);
        //printf("\n");
        NDT_OBJECT* ret = ndt_sum(arg);
        sum += ndt_integer(ret);
        //print(ret);
        //printf("\n");
        ndt_release(ret);
        ndt_release(arg);
    }
    
    printf("%lld\n", sum);
}
