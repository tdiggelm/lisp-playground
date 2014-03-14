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
- terminate list with empty list ():=nil instead of NULL
- implement map, apply, union, difference, etc.
- implement sexp parser
- implement mexp parser
- implement lambda (+ shortcut: (\ [x y] x*y)
- implement let (with lambda)
- fix const madness
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

const NDT_OBJECT* ndt_append(const NDT_OBJECT* sexp1, const NDT_OBJECT* sexp2)
{
    if (ndt_is_nil(sexp1)) return sexp2;
    if (ndt_is_nil(sexp2)) return sexp1;
    const NDT_OBJECT* obj = sexp1;
    while(!ndt_is_nil(ndt_cdr(obj))) {
        obj = ndt_cdr(obj);
    }
    ((NDT_PAIR*)obj)->ndt_cdr = sexp2;
    return sexp1;
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

NDT_OBJECT* ndt_list(const NDT_OBJECT* sexp)
{
    if (ndt_is_nil(sexp)) {
        return NULL;
    } else if (ndt_is_cons(sexp)) {
        return ndt_make_cons(ndt_list(ndt_car(sexp)),
            ndt_list(ndt_cdr(sexp)));
    } else {
        return ndt_dup(sexp); 
    }
}

NDT_OBJECT* ndt_sum(const NDT_OBJECT* sexp)
{
    if (ndt_is_nil(sexp)) {
        return ndt_make_integer(0);
    } else if (ndt_is_integer(sexp) || ndt_is_decimal(sexp)) {
        return ndt_dup(sexp);
    } else if (ndt_is_cons(sexp)) {
        const NDT_OBJECT* a = ndt_car(sexp);
        NDT_OBJECT* b = ndt_sum(ndt_cdr(sexp));
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
            assert(!"unhandled type in ndt_sum (inner)");
        }
        ndt_release(b);
        return sum;   
    } else {
        assert(!"unhandled type in ndt_sum (outer)");
    }
}

NDT_OBJECT* ndt_eval(const NDT_OBJECT* sexp)
{
    if (ndt_is_decimal(sexp) || ndt_is_integer(sexp) || ndt_is_string(sexp)) {
        return ndt_dup(sexp);
    } else if (ndt_is_cons(sexp)) {
        const NDT_OBJECT* car = ndt_car(sexp);
        if (ndt_is_symbol(car)) {
            if (strcmp(ndt_symbol(car), "+") == 0) { // use hashtable for lut
                return ndt_sum(ndt_cdr(sexp));
            } else if (strcmp(ndt_symbol(car), "list") == 0) {
                return ndt_list(ndt_cdr(sexp));
            } else {
                assert(!"unhandled symbol in ndt_eval");
            }
        }
    } else {
        assert(!"unhandled type in ndt_eval");
    }
}

NDT_OBJECT* ndt_make_sexp(const NDT_OBJECT* arr[], size_t n)
{
    if (arr == NULL) return NULL;
    if (n > 1) {
        return ndt_make_cons(arr[0], ndt_make_sexp(&arr[1], n-1));
    } else {
        return ndt_make_cons(arr[0], NULL);
    }
}

#define EVAL(stmt) do { \
    NDT_OBJECT* obj = (stmt); \
    printf("> "); \
    ndt_print(obj); \
    NDT_OBJECT* ret = ndt_eval(obj); \
    ndt_release(obj); \
    ndt_print(ret); \
    ndt_release(ret); \
} while(0)

#define APPEND(x, y) ndt_append(x, y)
#define SYM(x) ndt_make_symbol(x)
#define STR(x) ndt_make_string(x)
#define INT(x) ndt_make_integer(x)
#define DEC(x) ndt_make_decimal(x)
#define SEXPR(...) ({ \
    const NDT_OBJECT* arr[] = {__VA_ARGS__}; \
    ndt_make_sexp(arr, sizeof(arr)/sizeof(NDT_OBJECT*)); \
})
   
int main()
{    
    // (list (+ 1 2))
    EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(1), ndt_make_cons(ndt_make_integer(2), NULL))), NULL)));
    
    // (list "Hello World!" 3.14159 100)
    EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_string("Hello World!"), ndt_make_cons(ndt_make_decimal(3.14159), ndt_make_cons(ndt_make_integer(100), NULL)))));
                
    // (+ 5 10 20)
    EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // (+ 5.0 10 20)
    EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_decimal(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // foo => implement quote
    // EVAL(ndt_make_symbol("foo"));
    
    // 123
    EVAL(ndt_make_integer(123));
    
    // 3.14159
    EVAL(ndt_make_decimal(3.14159));
    
    // "Hello World!"
    EVAL(ndt_make_string("Hello World!"));
    
    // (+ 3.14159 10)
    EVAL(SEXPR(SYM("+"), DEC(3.14159), INT(10)));
    
    EVAL(SEXPR(SYM("+"), DEC(3.14159), INT(10)));
    
    EVAL(APPEND(SEXPR(SYM("list"), DEC(3.14159), INT(10)), 
        SEXPR(DEC(23), INT(24))));

    // (list (+ (+ 1 2) 3 4))
    //EVAL(LIST(SYM("+"), LIST(SYM("+"), INT(1), INT(2)), INT(3), INT(4)));
}
