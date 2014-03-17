#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* TODO
- create ndt_nil (pair(NULL, NULL)), ndt_true (atom("#t"))
- create environment (use ut_hash for symbol lut)
- error handling: what happens when wrong type is in argument?
- implement simple parser to evaluate sexpr
- terminate list with empty list ():=nil instead of NULL
- implement union, difference, etc.
- implement sexp parser
- implement mexp parser
- implement lambda (+ shortcut: (\ [x y] x*y)
- implement let (with lambda)
- implement max recursion depth
- implement cons, car, cdr as commands
- implement + as (foldl + 0 (1 2 3))
- eval sexpr: compare:
    * status quo
    * cons arguments into temp (reversed) args list, then reverse (loop, cons again)
    * destructive eval: don't duplicate but remove uneeded stuff so that eval returns the same structure as stmt with replaced content (or something new)
- eval implementation of sum / prod etc. with foldl (-> foldl can be implemented with a loop rather then with recursion!!, then implement binary sum, prod, etc => ndt_sum(NDT_OBJECT* a, NDT_OBJECT* b)), see: http://codereview.stackexchange.com/questions/1803/scheme-sicp-ex-2-39-reverse-in-terms-of-fold-right-and-fold-left, http://codereview.stackexchange.com/questions/1700/scheme-sicp-ex-2-27-deep-reverse?rq=1, https://github.com/petermichaux/bootstrap-scheme/blob/v0.21/scheme.c,
http://michaux.ca/articles/scheme-from-scratch-bootstrap-v0_1-integers
- implement COND special form => and if, when
- handle symbols case invariant
- implement: pair? null? etc.
- implement true / false => use scheme style #t, #f => singleton (when typed #t return singleton true object)
- empty list / nil => singleton
- implement special forms / builtin symbols as singleton => QUOTE, #t, #f, etc. then use e.g. SEXP(QUOTE, STR("hello"), NIL) => remove LIST macro and ass SEXP macro, QUOTE macro returns singleton quote symbol
- add a VM struct that holds the environment and any other open pointers (e.g. nathan dataspace handle)
- TODO: see tailcall - trick: when evaluated, set in-place then goto begin of function and re-evaluate (no need for new stack frame => if, lambda, when, let, etc.): https://github.com/petermichaux/bootstrap-scheme/blob/v0.10/scheme.c#L715 for tail call optimization, https://www.gnu.org/software/guile/manual/html_node/Tail-Calls.html
- see here for special forms: http://sicp.ai.mit.edu/Fall-2003/manuals/scheme-7.5.5/doc/scheme_3.html
- when tail recursion correctly implemented map could be implemented in scheme: (define (map proc items)
    (if (null? items)
        '()
        (cons (proc (car items))
              (map proc (cdr items)))))
- support tuples and vectors?: (vector obj ...), (tuple obj ...) => tuples are immutable, vectors can only contain the same types? => use for returning from nathan, literals: _(...) => (tuple ...), $(...) => (vector ...)
- support lambda* for handling optional and keyword arguments: http://www.gnu.org/software/guile/manual/html_node/lambda_002a-and-define_002a.html#lambda_002a-and-define_002a
- for built-in procedures use static variable directly instead of making symbol look-up: e.g. EVAL(LIST(PROC_SUM, INT(1), INT(2))) instead of EVAL(LIST(SYM("+", INT(1), INT(2))))
*/

typedef enum {NDT_TYPE_PAIR, NDT_TYPE_DECIMAL, NDT_TYPE_INTEGER, NDT_TYPE_SYMBOL, NDT_TYPE_STRING, NDT_TYPE_FUNC} NDT_TYPE;

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

typedef struct ndt_func {
    NDT_TYPE type;
    const char* name;
    NDT_OBJECT* (*func)(const NDT_OBJECT*);
} NDT_FUNC;

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

NDT_OBJECT* ndt_call(const NDT_OBJECT* obj, const NDT_OBJECT* args)
{
    assert(obj->type == NDT_TYPE_FUNC);
    return ((NDT_FUNC*)obj)->func(args);
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

int ndt_is_func(const NDT_OBJECT* obj)
{
    return (obj != NULL && obj->type == NDT_TYPE_FUNC);
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
        }
        case NDT_TYPE_FUNC: {
            NDT_OBJECT* dup = malloc(sizeof(NDT_FUNC));
            return memcpy(dup, obj, sizeof(NDT_FUNC));
        }
        case NDT_TYPE_PAIR: {
            return ndt_make_cons(ndt_dup(ndt_car(obj)), ndt_dup(ndt_cdr(obj)));
        }
        default: {
            assert(!"unkown type in ndt_dup\n");
        }
    }
}

NDT_OBJECT* ndt_append(NDT_OBJECT* sexp1, NDT_OBJECT* sexp2)
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

void ndt_print(const NDT_OBJECT* obj);

void print_pair(const NDT_OBJECT* obj)
{
    ndt_print(ndt_car(obj));
    if (!ndt_is_nil(ndt_cdr(obj))) {
        if (ndt_is_cons(ndt_cdr(obj))) {
            printf(" ");
            print_pair(ndt_cdr(obj));   
        } else {
            printf(" . ");
            ndt_print(ndt_cdr(obj));   
        }
    }
}

void ndt_print(const NDT_OBJECT* obj)
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
        case NDT_TYPE_FUNC: {
            printf("<FUNC:%s>", ((const NDT_FUNC*)obj)->name);
            break;
        }
        case NDT_TYPE_PAIR: {
#ifdef PRINT_CONS
            printf("(");
            ndt_print(ndt_car(obj));
            printf(" . ");
            ndt_print(ndt_cdr(obj));
            printf(")");
            break;
#else
            printf("(");
            print_pair(obj);
            printf(")");
            break;
#endif
        }
        default: {
            assert(!"unkown type in __ndt_print\n");
        }
    }
}

void ndt_println(const NDT_OBJECT* obj)
{
    ndt_print(obj);
    printf("\n");
}

NDT_OBJECT* ndt_apply(const NDT_OBJECT* sexp)
{
    return ndt_call(ndt_car(sexp), ndt_car(ndt_cdr(sexp)));
}

NDT_OBJECT* ndt_map(const NDT_OBJECT* sexp)
{
    const NDT_OBJECT* curr = ndt_cdr(sexp);
    NDT_OBJECT* result = NULL;
    while(curr != NULL && ndt_is_cons(curr)) {      
        result = ndt_append(result, ndt_make_cons(
            ndt_call(ndt_car(sexp), ndt_car(curr)), NULL));
        curr = ndt_cdr(curr);
    }
    return result;
}

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

// TODO: implement as binary function then use foldl to compute sum
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

NDT_OBJECT* ndt_product(const NDT_OBJECT* sexp)
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
            sum = ndt_make_decimal(ndt_decimal(a)*ndt_decimal(b));   
        } else if (ndt_is_integer(a) && ndt_is_decimal(b)) {
            sum = ndt_make_decimal(ndt_integer(a)*ndt_decimal(b));   
        } else if (ndt_is_decimal(a) && ndt_is_integer(b)) {
            sum = ndt_make_decimal(ndt_decimal(a)*ndt_integer(b));   
        } else if (ndt_is_integer(a) && ndt_is_integer(b)) {
            sum = ndt_make_integer(ndt_integer(a)*ndt_integer(b));   
        } else {
            assert(!"unhandled type in ndt_sum (inner)");
        }
        ndt_release(b);
        return sum;   
    } else {
        assert(!"unhandled type in ndt_sum (outer)");
    }
}

const static NDT_FUNC sum_fn = {NDT_TYPE_FUNC, "+", &ndt_sum};
const static NDT_FUNC prod_fn = {NDT_TYPE_FUNC, "*", &ndt_product};
const static NDT_FUNC list_fn = {NDT_TYPE_FUNC, "list", &ndt_list};
const static NDT_FUNC apply_fn = {NDT_TYPE_FUNC, "apply", &ndt_apply};
const static NDT_FUNC map_fn = {NDT_TYPE_FUNC, "map", &ndt_map};

// TODO: add symbols to hash-table for quick look-up
const NDT_OBJECT* ndt_lookup(const char* symbol)
{
    if (strcmp(symbol, "+") == 0) { // use hashtable for lut
        return (const NDT_OBJECT*)&sum_fn;
    } else if (strcmp(symbol, "*") == 0) { // use hashtable for lut
        return (const NDT_OBJECT*)&prod_fn;
    } else if (strcmp(symbol, "list") == 0) {
        return (const NDT_OBJECT*)&list_fn;
    } else if (strcmp(symbol, "apply") == 0) {
        return (const NDT_OBJECT*)&apply_fn;
    } else if (strcmp(symbol, "map") == 0) {
        return (const NDT_OBJECT*)&map_fn;
    } else {
        assert(!"ndt_lookup: symbol not found");
        // TODO: return error here (or nil?)
    }
}

// eval: destructive eval => in-place eval of list cars
NDT_OBJECT* ndt_eval(const NDT_OBJECT* sexp)
{
    if (ndt_is_decimal(sexp) || ndt_is_integer(sexp) || ndt_is_string(sexp)) {
        return ndt_dup(sexp);
    } else if (ndt_is_symbol(sexp)) {
        return ndt_dup(ndt_lookup(ndt_symbol(sexp)));
        // TODO: decide whether to throw here or return nil when lookup failed
    } else if (ndt_is_cons(sexp) && ndt_is_symbol(ndt_car(sexp))) {
        
        // special procedure quote
        if (strcmp(ndt_symbol(ndt_car(sexp)), "quote") == 0) {
            return ndt_dup(ndt_car(ndt_cdr(sexp)));
        }
        
        const NDT_OBJECT* symbol = ndt_lookup(ndt_symbol(ndt_car(sexp)));
        // TODO: check if is func or lambda, throw otherwise => better: let ndt_call decide what to do when symbol != callable => e.g. return error there
        
        // evaluate arguments
        // TODO: compare with https://github.com/petermichaux/bootstrap-scheme/blob/v0.13/scheme.c#L1038
        const NDT_OBJECT* curr = ndt_cdr(sexp);
        NDT_OBJECT* args = NULL;
        int i = 0;
        while(curr != NULL && ndt_is_cons(curr)) {      
            args = ndt_append(args, ndt_make_cons(
                ndt_eval(ndt_car(curr)), NULL));
            curr = ndt_cdr(curr);
        }
        
        // evaluate function
        NDT_OBJECT* result;
        if (ndt_is_func(symbol)) {
            result = ndt_call(symbol, args); // also handle lambda eval in ndt_call()
        }
        
        ndt_release(args);
        return result;
    } else {
        assert(!"ndt_eval: unhandled type");
        // TODO: maybe simply return unevaluated here
    }
}

NDT_OBJECT* ndt_make_list(const NDT_OBJECT* arr[], size_t n)
{
    if (arr == NULL) return NULL;
    if (n > 1) {
        return ndt_make_cons(arr[0], ndt_make_list(&arr[1], n-1));
    } else {
        return ndt_make_cons(arr[0], NULL);
    }
}

#define EVAL(stmt) do { \
    NDT_OBJECT* obj = (stmt); \
    printf("> "); \
    ndt_println(obj); \
    NDT_OBJECT* ret = ndt_eval(obj); \
    ndt_release(obj); \
    ndt_println(ret); \
    ndt_release(ret); \
} while(0)
    
#define PRINT(stmt) do { \
    NDT_OBJECT* obj = (stmt); \
    printf("> "); \
    ndt_println(obj); \
    ndt_release(obj); \
} while(0)

#define APPEND(x, y) ndt_append(x, y)
#define CONS(x,y) ndt_make_cons(x, y)
#define SYM(x) ndt_make_symbol(x)
#define STR(x) ndt_make_string(x)
#define INT(x) ndt_make_integer(x)
#define DEC(x) ndt_make_decimal(x)
#define LIST(...) ({ \
    const NDT_OBJECT* arr[] = {__VA_ARGS__}; \
    ndt_make_list(arr, sizeof(arr)/sizeof(NDT_OBJECT*)); \
})
   
int main()
{    
    // print complex structure        
    PRINT(
        LIST(
            STR("Hello"), STR("World"), 
            CONS(
                SYM("foo"), CONS(
                    DEC(3.14159), CONS(
                        INT(5),
                        INT(6)
                    )
                )
            ), 
            LIST(
                INT(77),
                DEC(87.9)
            )
        )
    );
    
    // (list (+ 1 2))
    EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(1), ndt_make_cons(ndt_make_integer(2), NULL))), NULL)));
    
    // (list "Hello World!" 3.14159 100)
    EVAL(ndt_make_cons(ndt_make_symbol("list"), ndt_make_cons(ndt_make_string("Hello World!"), ndt_make_cons(ndt_make_decimal(3.14159), ndt_make_cons(ndt_make_integer(100), NULL)))));
                
    // (+ 5 10 20)
    EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_integer(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // (+ 5.0 10 20)
    EVAL(ndt_make_cons(ndt_make_symbol("+"), ndt_make_cons(ndt_make_decimal(5), ndt_make_cons(ndt_make_integer(10), ndt_make_cons(ndt_make_integer(20), NULL)))));
    
    // 123
    EVAL(ndt_make_integer(123));
    
    // 3.14159
    EVAL(ndt_make_decimal(3.14159));
    
    // "Hello World!"
    EVAL(ndt_make_string("Hello World!"));
    
    // (+ 3.14159 10)
    EVAL(LIST(SYM("+"), DEC(3.14159), INT(10)));
    
    // (list 3.141590 10 23.000000 24)
    EVAL(APPEND(LIST(SYM("list"), DEC(3.14159), INT(10)), 
        LIST(DEC(23), INT(24))));

    // (list (+ (+ 1 2) 3 4))
    EVAL(LIST(SYM("+"), LIST(SYM("+"), INT(1), INT(2)), INT(3), INT(4)));
    
    // (+ (* 2 3) (* 4 (+ 5 10)) 20)
    EVAL(LIST(SYM("+"), LIST(SYM("*"), INT(2), INT(3)), LIST(SYM("*"), INT(4), LIST(SYM("+"), INT(5), INT(10))), INT(20)));
    
    // (quote hello 22)
    EVAL(LIST(SYM("quote"), SYM("hello"), INT(22)));
   
    // (quote (hello "test" 123))
    EVAL(LIST(SYM("quote"), LIST(SYM("hello"), STR("test"), INT(123))));
    
    // (apply + (quote (1 2.000000 123)))
    EVAL(LIST(SYM("apply"), SYM("+"), LIST(SYM("quote"), LIST(INT(1), DEC(2), INT(123)))));
    
    // (map * (map + (list 1 2 3) (list 7.000000 8.000000 9.000000)))
    EVAL(LIST(SYM("map"), SYM("*"),
        LIST(SYM("map"), SYM("+"), 
            LIST(SYM("list"), INT(1), INT(2), INT(3)),
            LIST(SYM("quote"), LIST(DEC(7), DEC(8), DEC(9)))
        )
    ));
}
