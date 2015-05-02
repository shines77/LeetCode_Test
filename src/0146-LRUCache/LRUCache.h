
#ifndef _LEETCODE_Q0146_LRUCACHE_H_
#define _LEETCODE_Q0146_LRUCACHE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

namespace Q0146_LRUCache {

#define LRUCACHE_USE_HASH_TABLE     1

struct LRUItem {
    int         key;
    int         value;
    LRUItem *   prev;
    LRUItem *   next;
};

struct HashItem {
    int         key;
    LRUItem *   value;
};

class HashTable {
private:
    int         mSize;
    int         mMask;
    HashItem *  mTableA;
    HashItem *  mTableB;

public:
    HashTable(int capacity) : mSize(0), mMask(0xFFFFFFFFU), mTableA(NULL), mTableB(NULL)
    {
        mSize = calcSize(capacity);
        mMask = mSize - 1;
        mTableA = new HashItem[mSize];
        mTableB = new HashItem[mSize];
#if 1
        ::memset(mTableA, -1, sizeof(HashItem) * mSize);
        ::memset(mTableB, -1, sizeof(HashItem) * mSize);
#else
        HashItem * tablePtrA = mTableA;
        HashItem * tablePtrB = mTableB;
        for (int i = 0; i < mSize; ++i) {
            tablePtrA->key = -1;
            tablePtrB->key = -1;
            tablePtrA++;
            tablePtrB++;
        }
#endif
    }

    ~HashTable() {
        if (mTableA)
            delete[] mTableA;
        if (mTableB)
            delete[] mTableB;
    }

protected:
    int getHashA(int key) { return key & mMask; }
    int getHashB(int key) { return (key ^ 4491738) & mMask; }

public:
    int calcSize(int capacity) {
        int size = 16;
        if (capacity > 16) {
            while (size < capacity)
                size <<= 1;
            if ((size - capacity) < size / 4)
                size = size << 1;
        }
        else { size = 32; }
        return size;
    }

    void add(int key, LRUItem * item) {
        int indexA = getHashA(key);

        HashItem * startPtr = &mTableA[indexA];
        HashItem * endPtr   = startPtr + 4;
        do {
            if (startPtr->key == key) {
                startPtr->key   = key;
                startPtr->value = item;
                return;
            }
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        startPtr = &mTableB[indexB];
        endPtr   = &mTableB[mSize];
        while (startPtr != endPtr) {
            if (startPtr->key >= 0 && startPtr->key != key) {
                startPtr++;
                continue;
            }
            else {
                startPtr->key   = key;
                startPtr->value = item;;
                return;
            }
        }

        startPtr = mTableB;
        endPtr   = &mTableB[indexB];
        while (startPtr != endPtr) {
            if (startPtr->key >= 0 && startPtr->key != key) {
                startPtr++;
                continue;
            }
            else {
                startPtr->key   = key;
                startPtr->value = item;;
                return;
            }
        }
    }

    LRUItem * find(int key) {
        int indexA = getHashA(key);

        HashItem * startPtr = &mTableA[indexA];
        HashItem * endPtr   = startPtr + 4;
        do {
            if (startPtr->key == key)
                return startPtr->value;
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        startPtr = &mTableB[indexB];
        endPtr   = &mTableB[mSize];
        while (startPtr != endPtr) {
            if (startPtr->key != key) {
                if (startPtr->key != -1) {
                    startPtr++;
                    continue;
                }
                else return NULL;
            }
            else return startPtr->value;
        }

        startPtr = mTableB;
        endPtr   = &mTableB[indexB];
        while (startPtr != endPtr) {
            if (startPtr->key != key) {
                if (startPtr->key != -1) {
                    startPtr++;
                    continue;
                }
                else return NULL;
            }
            else return startPtr->value;
        }
        return NULL;
    }

    void remove(int key) {
        int indexA = getHashA(key);

        HashItem * startPtr = &mTableA[indexA];
        HashItem * endPtr   = startPtr + 4;
        do {
            if (startPtr->key == key) {
                startPtr->key = -2;
                return;
            }
            startPtr++;
        } while (startPtr != endPtr);

        int indexB = getHashB(key);
        startPtr = &mTableB[indexB];
        endPtr   = &mTableB[mSize];
        while (startPtr != endPtr) {
            if (startPtr->key != key) {
                startPtr++;
                continue;
            }
            else {
                startPtr->key = -2;
                return;
            }
        }

        startPtr = mTableB;
        endPtr   = &mTableB[indexB];
        while (startPtr != endPtr) {
            if (startPtr->key != key) {
                startPtr++;
                continue;
            }
            else {
                startPtr->key = -2;
                return;
            }
        }
    }
};

class LRUCache {
private:
    int         mSize;
    int         mCapacity;

    LRUItem *   mCacheList;
    LRUItem *   mCacheListLast;
    LRUItem *   mHeadItem;
    LRUItem *   mTailItem;

#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)
    HashTable   mHashTable;
#endif

public:
    LRUCache(int capacity)
        : mSize(0), mCapacity(capacity), mCacheList(NULL), mCacheListLast(NULL)
#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)
          , mHashTable(capacity)
#endif
    {
        assert(capacity > 0);

        mCacheList = new LRUItem[capacity];
        mCacheListLast = mCacheList;

        mCacheList[0].key   = -1;
        mCacheList[0].value = -1;
        mCacheList[0].prev  = NULL;
        mCacheList[0].next  = NULL;

        mHeadItem = NULL;
        mTailItem = NULL;
    }

    ~LRUCache() {
        if (mCacheList)
            delete[] mCacheList;
    }

    int getSize() { return mSize; }

    void appendNewItem(LRUItem * cacheItem, int key, int value) {
        assert(mSize < mCapacity);
        assert(mCacheListLast < (mCacheList + mCapacity));

        LRUItem * newItem = mCacheListLast;
        newItem->key   = key;
        newItem->value = value;
        newItem->prev  = NULL;
        newItem->next  = mHeadItem;
        mCacheListLast++;
        mSize++;

#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)
        // Add a key
        mHashTable.add(key, newItem);
#endif
        assert(mCacheListLast <= (mCacheList + mCapacity));
        assert(mSize <= mCapacity);

        // Record the recent used item.
        if (mHeadItem) {
            assert(mHeadItem->prev == NULL);
            if (mHeadItem->prev != newItem)
                mHeadItem->prev = newItem;
        }
        mHeadItem = newItem;

        // Record the last used item.
        if (cacheItem) {
            if (cacheItem != mTailItem) {
                cacheItem->next = NULL;
                mTailItem = cacheItem;
            }
        }
        else {
            mTailItem = newItem;
        }
    }

    void eliminateItem(LRUItem * cacheItem, int key, int value) {
        LRUItem * tailItem = mTailItem;
        assert(tailItem != NULL);
        if (tailItem) {
#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)
            // Remove a key
            mHashTable.remove(tailItem->key);
#endif
            LRUItem * newTailItem = tailItem->prev;
            if (newTailItem) {
                newTailItem->next = NULL;
                // Record the last used item
                mTailItem = newTailItem;
            }

            tailItem->key   = key;
            tailItem->value = value;
            tailItem->prev  = NULL;

#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)
            // Add a key
            mHashTable.add(key, tailItem);
#endif
            // To avoid their own point to themselves.
            if (tailItem != mHeadItem) {
                tailItem->next = mHeadItem;

                // Record the recent used item
                mHeadItem->prev = tailItem;
                mHeadItem = tailItem;
            }
            else {
                tailItem->next = NULL;
            }
        }
    }

    void pickupItem(LRUItem * cacheItem, int value) {
        assert(cacheItem != NULL);

        // Modify the value directly.
        cacheItem->value = value;

        if (cacheItem != mHeadItem) {
            if (cacheItem->prev) {
                cacheItem->prev->next = cacheItem->next;
            }
            if (cacheItem->next) {
                cacheItem->next->prev = cacheItem->prev;
            }

            // Record the last used item.
            if (cacheItem == mTailItem) {
                if (cacheItem->prev) {
                    cacheItem->prev->next = NULL;
                    mTailItem = cacheItem->prev;
                }
            }

            // Link the old head item to new head item.
            cacheItem->prev = NULL;
            cacheItem->next = mHeadItem;

            // Modify the recent used item.
            if (mHeadItem) {
                if (cacheItem != mHeadItem->prev)
                    mHeadItem->prev = cacheItem;
            }
            // Record the recent used item.
            mHeadItem = cacheItem;
        }
    }

#if defined(LRUCACHE_USE_HASH_TABLE) && (LRUCACHE_USE_HASH_TABLE != 0)

    LRUItem * find(int key, int &existed) {
        LRUItem * foundItem = mHashTable.find(key);
        existed = (foundItem != NULL);
        return (foundItem != NULL) ? foundItem : mTailItem;
    }

    int get(int key) {
        LRUItem * foundItem = mHashTable.find(key);
        if (foundItem) {
            pickupItem(foundItem, foundItem->value);
            return foundItem->value;
        }
        return -1;
    }

#else

    LRUItem * find(int key, int &existed) {
        LRUItem * lastItem = NULL;
        LRUItem * cacheItem = mHeadItem;
        while (cacheItem) {
            if (cacheItem->key == key) {
                existed = 1;
                return cacheItem;
            }
            if (cacheItem->next == NULL) {
                lastItem = cacheItem;
                break;
            }
            cacheItem = cacheItem->next;
        }
        existed = 0;
        return lastItem;
    }

    int get(int key) {
        LRUItem * cacheItem = mHeadItem;
        while (cacheItem) {
            if (cacheItem->key == key) {
                pickupItem(cacheItem, cacheItem->value);
                return cacheItem->value;
            }
            cacheItem = cacheItem->next;
        }
        return -1;
    }

#endif  /* LRUCACHE_USE_HASH_TABLE */
    
    void set(int key, int value) {
        int existed;
        LRUItem * cacheItem = find(key, existed);
        if (existed == 0) {
            // It's a new key.
            if (mSize < mCapacity) {
                // The cache list capacity is not full, append new item to head only.
                appendNewItem(cacheItem, key, value);
            }
            else {
                // The cache list capacity is full, must be eliminate a item.
                eliminateItem(cacheItem, key, value);
            }
        }
        else {
            // Pickup the item to head.
            pickupItem(cacheItem, value);
        }
    }

    void displayList(char * title = NULL) {
        if (title)
            printf("%s", title);
        LRUItem * cacheItem = mHeadItem;
        while (cacheItem) {
            if (cacheItem->next)
                printf("(%d,%d), ", cacheItem->key, cacheItem->value);
            else
                printf("(%d,%d)",   cacheItem->key, cacheItem->value);
            cacheItem = cacheItem->next;
        }
    }

    void display(char * title = NULL) {
        if (title)
            printf("%s", title);
        else
            printf("The LRUCache is: ");
        LRUItem * cacheItem = mHeadItem;
        while (cacheItem) {
            if (cacheItem->next)
                printf("(%d,%d), ", cacheItem->key, cacheItem->value);
            else
                printf("(%d,%d)",   cacheItem->key, cacheItem->value);
            cacheItem = cacheItem->next;
        }
        printf("\n");
    }
};

}  // namespace Q0146_LRUCache

#endif  /* _LEETCODE_Q0146_LRUCACHE_H_ */
