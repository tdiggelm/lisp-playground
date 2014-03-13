#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* TODO
- create ndt_nil (pair(NULL, NULL)), ndt_true (atom("#t"))
- create environment (use ut_hash for symbol lut)
- error handling: what happens when wrong type is in argument?
- maybe implement NDT_NUMBER type containing union of int64_t + double that can be passes by-value from sum function
- implement simple parser to evaluate sexpr
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

NDT_OBJECT* ndt_dup(const NDT_OBJECT* obj)
{
    if (obj == NULL) {
        return NULL;
    }
    
    switch(obj->type) {
        case NDT_TYPE_INTEGER: {
            return ndt_make_integer(ndt_integer(obj));
        }
        case NDT_TYPE_DECIMAL: {
            return ndt_make_decimal(ndt_decimal(obj));
        }
        case NDT_TYPE_STRING: {
            return ndt_make_string(ndt_string(obj));
        }
        case NDT_TYPE_SYMBOL: {
            return ndt_make_symbol(ndt_symbol(obj));
            break;
        }
        case NDT_TYPE_PAIR: {
            return ndt_make_cons(ndt_dup(ndt_car(obj)), ndt_dup(ndt_cdr(obj)));
        }
        default: {
            assert(!"unkown type in ndt_dup\n");
        }
    }
}

void __ndt_print(const NDT_OBJECT* obj)
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
            __ndt_print(ndt_car(obj));
            printf(" . ");
            __ndt_print(ndt_cdr(obj));
            printf(")");
            break;
        }
        default: {
            assert(!"unkown type in __ndt_print\n");
        }
    }
}

void ndt_print(const NDT_OBJECT* obj)
{
    __ndt_print(obj);
    printf("\n");
}

NDT_OBJECT* ndt_eval(const NDT_OBJECT* obj);

NDT_OBJECT* ndt_list(const NDT_OBJECT* args)
{
    if (ndt_is_nil(args)) {
        return NULL;
    } else if (ndt_is_cons(args)) {
        if (ndt_is_symbol(ndt_car(args))) {
            return ndt_eval(args);
        } else {
            return ndt_make_cons(ndt_list(ndt_car(args)),
                ndt_list(ndt_cdr(args)));
        }
    } else {
        return ndt_dup(args); 
    }
}

NDT_OBJECT* ndt_sum(const NDT_OBJECT* args)
{
    if (ndt_is_nil(args)) {
        return ndt_make_integer(0);
    } else if (ndt_is_integer(args) || ndt_is_decimal(args)) {
        return ndt_dup(args);
    } else if (ndt_is_cons(args)) {
        if (ndt_is_symbol(ndt_car(args))) {
            return ndt_eval(args);
        } else {
            const NDT_OBJECT* a = ndt_car(args);
            NDT_OBJECT* b = ndt_sum(ndt_cdr(args));
            NDT_OBJECT* sum;
            if (ndt_is_decimal(a) && ndt_is_decimal(b)) {
                sum = ndt_make_decimal(ndt_decimal(a)+ndt_decimal(b));   
            } else if (ndt_is_integer(a) && ndt_is_decimal(b)) {
                sum = ndt_make_decimal(ndt_integer(a)+ndt_decimal(b));   
            } else if (ndt_is_decimal(a) && ndt_is_integer(b)) {
                sum = ndt_make_decimal(ndt_decimal(a)+ndt_integer(b));   
            } else if (ndt_is_integer(a) && ndt_is_integer(b)) {
                sum = ndt_make_integer(ndt_integer(a)+ndt_integer(b));   
            } else {
                assert(!"unhandled type in ndt_sum");
            }
            ndt_release(b);
            return sum;   
        }
    } else {
        assert(!"unhandled type in ndt_sum");
    }
}

// (link (add "hello" "world") (add "bye" "bye"))
// (link $1 $2)

NDT_OBJECT* ndt_eval(const NDT_OBJECT* obj)
{
    if (ndt_is_decimal(obj) || ndt_is_integer(obj) || ndt_is_string(obj)) {
        return ndt_dup(obj);
    } else if (ndt_is_cons(obj)) {
        const NDT_OBJECT* car = ndt_car(obj);
        assert(ndt_is_symbol(car)); // instead of assertion return error here
        if (strcmp(ndt_symbol(car), "+") == 0) { // use hashtable for lut
            return ndt_sum(ndt_cdr(obj));
        } else if (strcmp(ndt_symbol(car), "list") == 0) {
            return ndt_list(ndt_cdr(obj));
        } else {
            assert(!"unhandled symbol in ndt_eval");
        }
        
    } else {
        assert(!"unhandled type in ndt_eval");
    }
}

#define NDT_EVAL(stmt) do { \
    NDT_OBJECT* obj = (stmt); \
    printf("> "); \
    ndt_print(obj); \
    NDT_OBJECT* ret = ndt_eval(obj); \
    ndt_release(obj); \
    ndt_print(ret); \
    ndt_release(ret); \
} while(0)
    
#define NDT_PRINT(stmt) do { \
    NDT_OBJECT* obj = (stmt); \
    printf("> "); \
    ndt_print(obj); \
    ndt_print(obj); \
    ndt_release(obj); \
} while(0)

int main()
{
    // (list (+ 1 2))
    NDT_EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(1), ndt_make_cons(ndt_make_integer(2), NULL))), NULL)));
    
    // (list "Hello World!" 3.14159 100)
    NDT_EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_string("Hello World!"), ndt_make_cons(ndt_make_decimal(3.14159), ndt_make_cons(ndt_make_integer(100), NULL)))));
                
    // (+ 5 10 20)
    NDT_EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // (+ 5.0 10 20)
    NDT_EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_decimal(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // 123
    NDT_EVAL(ndt_make_integer(123));
    
    // "Hello World!"
    NDT_EVAL(ndt_make_string("Hello World!"));
}
