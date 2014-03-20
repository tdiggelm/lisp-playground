#include <assert.h>
#include <stdio.h>
#include <gc/gc.h>
 
// needs sudo apt-get install libgc-dev,
// see here: http://manpages.ubuntu.com/manpages/karmic/man3/gc.3.html, http://www.linuxjournal.com/article/6679.
 
int main(void)
{
    int i;
 
    GC_INIT();
    for (i = 0; i < 10000000; ++i)
    {
        int **p = (int**)GC_MALLOC(sizeof(int *));
        int *q = (int*)GC_MALLOC_ATOMIC(sizeof(int));
 
        assert(*p == 0);
        *p = (int*)GC_REALLOC(q, 2 * sizeof(int));
        if (i % 100000 == 0)
            printf("Heap size = %zu\n", GC_get_heap_size());
    }
 
    return 0;
}