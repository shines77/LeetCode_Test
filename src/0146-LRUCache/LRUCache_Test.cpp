

#include "0146-LRUCache/LRUCache_Test.h"

#define USE_LRUCACHELITE_VERSION    1

#if defined(USE_LRUCACHELITE_VERSION) && (USE_LRUCACHELITE_VERSION != 0)
#include "0146-LRUCache/LRUCacheLite.h"
using namespace Q0146_LRUCacheLite;
#else
using namespace Q0146_LRUCache;
#include "0146-LRUCache/LRUCache.h"
#endif

void LRUCache_Test1()
{
    int i, length;
    int szListKey[]   = { 8, 7, 3, 3, 2, 1, 3, 2, 9, 2, 1, 3, 4, 5, 0, 11, 12, 6, 8, 7, 8, 3, 1 };
    int szListValue[] = { 7, 3, 2, 2, 1, 3, 2, 9, 2, 1, 3, 4, 5, 0, 11, 12, 6, 8, 7, 8, 3, 1, 8 };

    LRUCache lruCache(10);

    length = _countof(szListKey);
    for (i = 0; i < length; ++i) {
        lruCache.set(szListKey[i], szListValue[i]);
        printf("[%2d]: set(%d,%d), size() = %2d,\n",
               i, szListKey[i], szListValue[i], lruCache.getSize());
        lruCache.displayList();
        printf("\n\n");
    }
    printf("\n");

    lruCache.display("The LRUCache is: ");
    printf("\n");
    printf("getSize() = %d\n", lruCache.getSize());
    printf("\n");

    for (i = 0; i < length; ++i) {
        int value = lruCache.get(szListKey[i]);
        printf("[%2d]: get(%2d) = %4d,  ", i, szListKey[i], value);
        lruCache.displayList();
        printf("\n");
    }
    printf("\n");
}

void LRUCache_CaseTest1()
{
    LRUCache lruCache(1);
    int value;

    lruCache.set(2, 1);
    printf("set(2, 1)\n");
    lruCache.display("List is: ");

    value = lruCache.get(2);
    printf("get(2) = %d\n", value);
    lruCache.display("List is: ");

    lruCache.set(3, 2);
    printf("set(3, 2)\n");
    lruCache.display("List is: ");

    value = lruCache.get(2);
    printf("get(2) = %d\n", value);
    lruCache.display("List is: ");

    value = lruCache.get(3);
    printf("get(3) = %d\n", value);
    lruCache.display("List is: ");

    printf("\n\n");
}

void LRUCache_CaseTest2()
{
    LRUCache lruCache(2);
    int value;

    lruCache.set(2, 1);
    printf("set(2, 1)\n");
    lruCache.display("List is: ");

    lruCache.set(1, 1);
    printf("set(1, 1)\n");
    lruCache.display("List is: ");

    value = lruCache.get(2);
    printf("get(2) = %d\n", value);
    lruCache.display("List is: ");

    lruCache.set(4, 1);
    printf("set(4, 1)\n");
    lruCache.display("List is: ");

    value = lruCache.get(1);
    printf("get(1) = %d\n", value);
    lruCache.display("List is: ");

    value = lruCache.get(2);
    printf("get(2) = %d\n", value);
    lruCache.display("List is: ");

    printf("\n\n");
}

int LRUCache_Test()
{
    LRUCache_Test1();
    LRUCache_CaseTest1();
    LRUCache_CaseTest2();

    system("pause");
    return 0;
}

