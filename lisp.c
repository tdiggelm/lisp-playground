#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*typedef enum {NDT_TYPE_INTEGER, NDT_TYPE_DECIMAL, NDT_TYPE_STRING, NDT_TYPE_NIL} type_e;

typedef struct obj {
    type_e type;
    union {
        long long ndt_integer;
        double ndt_decimal;
        const char* ndt_string;
    } val;
} obj;*/

typedef enum {NDT_TYPE_PAIR, NDT_TYPE_DECIMAL, NDT_TYPE_INTEGER, NDT_TYPE_SYMBOL, NDT_TYPE_STRING} NDT_TYPE;

typedef struct object {
    NDT_TYPE type;
} OBJECT;

typedef struct ndt_decimal {
    NDT_TYPE type;
    double ndt_decimal;
} DECIMAL;

typedef struct ndt_integer {
    NDT_TYPE type;
    long long ndt_integer;
} INTEGER;

typedef struct ndt_string {
    NDT_TYPE type;
    const char* ndt_string;
} STRING;

typedef struct ndt_symbol {
    NDT_TYPE type;
    const char* ndt_symbol;
} SYMBOL;

typedef struct pair {
    NDT_TYPE type;
    const OBJECT* ndt_car;
    const OBJECT* ndt_cdr;
} PAIR;

OBJECT* ndt_make_integer(long long ndt_integer)
{
    INTEGER* obj = malloc(sizeof(INTEGER));
    obj->type = NDT_TYPE_INTEGER;
    obj->ndt_integer = ndt_integer;
    return (OBJECT*)obj;
}

OBJECT* ndt_make_symbol(const char* ndt_symbol)
{
    SYMBOL* obj = malloc(sizeof(SYMBOL));
    obj->type = NDT_TYPE_SYMBOL;
    obj->ndt_symbol = strdup(ndt_symbol);
    return (OBJECT*)obj;
}

OBJECT* ndt_make_string(const char* ndt_string)
{
    STRING* obj = malloc(sizeof(STRING));
    obj->type = NDT_TYPE_STRING;
    obj->ndt_string = strdup(ndt_string);
    return (OBJECT*)obj;
}

OBJECT* ndt_make_decimal(double ndt_decimal)
{
    DECIMAL* obj = malloc(sizeof(DECIMAL));
    obj->type = NDT_TYPE_DECIMAL;
    obj->ndt_decimal = ndt_decimal;
    return (OBJECT*)obj;
}

OBJECT* ndt_make_cons(const OBJECT* ndt_car, const OBJECT* ndt_cdr)
{
    PAIR* obj = malloc(sizeof(PAIR));
    obj->type = NDT_TYPE_PAIR;
    obj->ndt_car = ndt_car;
    obj->ndt_cdr = ndt_cdr;
    return (OBJECT*)obj;
}

long long ndt_integer(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_INTEGER);
    return ((INTEGER*)obj)->ndt_integer;
}

double ndt_decimal(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_DECIMAL);
    return ((DECIMAL*)obj)->ndt_decimal;
}

const char* ndt_symbol(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_SYMBOL);
    return ((SYMBOL*)obj)->ndt_symbol;
}

const char* ndt_string(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_STRING);
    return ((STRING*)obj)->ndt_string;
}

const OBJECT* ndt_car(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_PAIR);
    return ((PAIR*)obj)->ndt_car;
}

const OBJECT* ndt_cdr(const OBJECT* obj)
{
    assert(obj->type == NDT_TYPE_PAIR);
    return ((PAIR*)obj)->ndt_cdr;
}

void ndt_release(OBJECT* obj)
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

void print(const OBJECT* obj)
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

/*void print_arr(obj* arr, size_t n)
{
    int i;
    for (i = 0; i < n; i++) {
        switch (arr[i].type) {
            case NDT_TYPE_INTEGER:
                printf("(ndt_integer) %lld\n", (arr[i].val.ndt_integer));
                break;
            case NDT_TYPE_DECIMAL:
                printf("(ndt_decimal) %lf\n", (arr[i].val.ndt_decimal));
                break;
            case NDT_TYPE_STRING:
                printf("(ndt_string) %s\n", (arr[i].val.ndt_string));
                break;
            case NDT_TYPE_NIL:
                printf("(nil)\n");
                break;
            default:
                printf("unkown type\n");
        }
    }
}*/

int main()
{
    size_t i;
    //for (i = 0; i < 10000; i++)
    {
        OBJECT* obj = ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_string("Hello World!"), ndt_make_cons(ndt_make_decimal(3.14159), ndt_make_cons(ndt_make_integer(100), NULL))));
        print(obj);
        printf("\n");
        ndt_release(obj);
    }
    
    /*obj arr[] = {
        {.type = NDT_TYPE_NIL},
        {.type = NDT_TYPE_INTEGER, .val.ndt_integer = 1},
        {.type = NDT_TYPE_INTEGER, .val.ndt_integer = 2},
        {.type = NDT_TYPE_INTEGER, .val.ndt_integer = 3},
        {.type = NDT_TYPE_DECIMAL, .val.ndt_decimal = 3.14159},
        {.type = NDT_TYPE_STRING, .val.ndt_string = "Hello World!"},
        {.type = NDT_TYPE_STRING, .val.ndt_string = "Bla bla bla!"}
    };
    print_arr(arr, 7);
    return 0;*/
}
