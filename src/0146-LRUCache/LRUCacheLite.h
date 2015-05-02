
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
        HashItem * endPtr   = startPtr + 3;
        do {
            if (startPtr->key == key) {
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
        HashItem * endPtr   = startPtr + 3;
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
        HashItem * endPtr   = startPtr + 3;
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

        mHeadItem = NULL;
        mTailItem = NULL;
    }

    ~LRUCache() {
        if (mCacheList)
            delete[] mCacheList;
    }

    int getSize() { return mSize; }

    void appendItem(int key, int value) {
        LRUItem * newItem = mCacheListLast;
        mCacheListLast++;
        mSize++;

        newItem->key   = key;
        newItem->value = value;
        newItem->prev  = NULL;
        newItem->next  = mHeadItem;

        // Add a key
        mHashTable.add(key, newItem);

        if (mHeadItem != NULL) {
            // Record the recent used item.
            mHeadItem->prev = newItem;
            mHeadItem = newItem;
        }
        else {
            // Record the recent used item and the last used item.
            mHeadItem = mTailItem = newItem;
        }
    }

    void eliminateItem(int key, int value) {
        LRUItem * tailItem = mTailItem;
        if (key != tailItem->key) {
            // Remove a key
            mHashTable.remove(tailItem->key);
            // Add a key
            mHashTable.add(key, tailItem);

            tailItem->key = key;
        }
        tailItem->value = value;

        LRUItem * newTailItem = tailItem->prev;
        if (newTailItem) {
            newTailItem->next = NULL;
            // Record the last used item
            mTailItem = newTailItem;
        }
        tailItem->prev = NULL;

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
            if (cacheItem != mTailItem) {
                // It's not head and not tail.
                cacheItem->prev->next = cacheItem->next;
                cacheItem->next->prev = cacheItem->prev;
            }
            else {
                // Record the last used item.
                cacheItem->prev->next = NULL;
                mTailItem = cacheItem->prev;
            }

            // Link the old head item to new head item.
            cacheItem->prev = NULL;
            cacheItem->next = mHeadItem;

            // Modify the recent used item.
            mHeadItem->prev = cacheItem;
            // Record the recent used item.
            mHeadItem = cacheItem;
        }
    }

    LRUItem * find(int key) {
        return mHashTable.find(key);
    }

    int get(int key) {
        LRUItem * foundItem = find(key);
        if (foundItem) {
            pickupItem(foundItem, foundItem->value);
            return foundItem->value;
        }
        return -1;
    }
    
    void set(int key, int value) {
        LRUItem * foundItem = find(key);
        if (foundItem == NULL) {
            // It's a new key.
            if (mSize < mCapacity) {
                // The cache list capacity is not full, append a new item to head only.
                appendItem(key, value);
            }
            else {
                // The cache list capacity is full, must be eliminate a item.
                eliminateItem(key, value);
            }
        }
        else {
            // Pickup the item to head.
            pickupItem(foundItem, value);
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
