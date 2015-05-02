
#ifndef _LEETCODE_Q0146_LRUCACHELITE_H_
#define _LEETCODE_Q0146_LRUCACHELITE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

namespace Q0146_LRUCacheLite {

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
    HashTable(int capacity) {
        mSize = calcSize(capacity);
        mMask = mSize - 1;
        mTableA = new HashItem[mSize];
        mTableB = new HashItem[mSize];
        ::memset(mTableA, -1, sizeof(HashItem) * mSize);
        ::memset(mTableB, -1, sizeof(HashItem) * mSize);
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
        else size = 32;
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
        int startIndex = indexB;
        do {
            if (mTableB[indexB].key >= 0 && mTableB[indexB].key != key) {
                indexB = (indexB + 1) & mMask;
                if (indexB != startIndex) continue;
            }
            else {
                mTableB[indexB].key = key;
                mTableB[indexB].value = item;;
                return;
            }
        } while (1);
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
        int startIndex = indexB;
        do {
            if (mTableB[indexB].key != key) {
                if (mTableB[indexB].key != -1) {
                    indexB = (indexB + 1) & mMask;
                    if (indexB != startIndex) continue;
                }
                else return NULL;
            }
            else return mTableB[indexB].value;
        } while (1);

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
        int startIndex = indexB;
        do {
            if (mTableB[indexB].key != key) {
                indexB = (indexB + 1) & mMask;
                if (indexB != startIndex) continue;
            }
            else {
                mTableB[indexB].key = -2;
                return;
            }
        } while (1);
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
    HashTable   mHashTable;

public:
    LRUCache(int capacity) : mSize(0), mCapacity(capacity), mHashTable(capacity)
    {
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
        LRUItem * newItem = mCacheListLast;
        newItem->key   = key;
        newItem->value = value;
        newItem->prev  = NULL;
        newItem->next  = mHeadItem;
        mCacheListLast++;
        mSize++;

        // Add a key
        mHashTable.add(key, newItem);

        // Record the recent used item.
        if (mHeadItem) {
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
        else mTailItem = newItem;
    }

    void eliminateItem(LRUItem * cacheItem, int key, int value) {
        LRUItem * tailItem = mTailItem;
        // Remove a key
        mHashTable.remove(tailItem->key);

        LRUItem * newTailItem = tailItem->prev;
        if (newTailItem) {
            newTailItem->next = NULL;
            // Record the last used item
            mTailItem = newTailItem;
        }

        tailItem->key   = key;
        tailItem->value = value;
        tailItem->prev  = NULL;

        // Add a key
        mHashTable.add(key, tailItem);

        // To avoid their own point to themselves.
        if (tailItem != mHeadItem) {
            tailItem->next = mHeadItem;

            // Record the recent used item
            mHeadItem->prev = tailItem;
            mHeadItem = tailItem;
        }
        else tailItem->next = NULL;
    }

    void pickupItem(LRUItem * cacheItem, int value) {
        // Modify the value directly.
        cacheItem->value = value;

        if (cacheItem != mHeadItem) {
            if (cacheItem->prev)
                cacheItem->prev->next = cacheItem->next;

            if (cacheItem->next)
                cacheItem->next->prev = cacheItem->prev;

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
            if (mHeadItem && cacheItem != mHeadItem->prev)
                mHeadItem->prev = cacheItem;
            // Record the recent used item.
            mHeadItem = cacheItem;
        }
    }

    LRUItem * find(int key, int &existed) {
        LRUItem * foundItem = mHashTable.find(key);
        existed = (foundItem != NULL);
        return existed ? foundItem : mTailItem;
    }

    int get(int key) {
        LRUItem * foundItem = mHashTable.find(key);
        if (foundItem) {
            pickupItem(foundItem, foundItem->value);
            return foundItem->value;
        }
        return -1;
    }
    
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

}  // namespace Q0146_LRUCacheLite

#endif  /* _LEETCODE_Q0146_LRUCACHELITE_H_ */
