//
//  DiskMultiMap.cpp
//  Project4
//
//  Created by hongshuhong on 3/5/16.
//  Copyright © 2016 hongshuhong. All rights reserved.
//

#include <stdio.h>
#include <functional>
#include <cstring>
#include <string.h>
#include "DiskMultiMap.h"
using namespace std;

/* ========================================================== HELPER FUNCTIONS ========================================================== */
size_t DiskMultiMap::hashKey(const std::string str){
    hash<std::string> keyHash;
    return keyHash(str);
}

int DiskMultiMap::hashToOffset(const std::size_t hashedTuple){
    Header head;
    if(!bf.read(head, 0)){
        cerr << "Could not read head"<<endl;
    }
    int index = (hashedTuple % head.numBuckets) * (sizeof(Bucket)) + sizeof(Header);
    return index;
}

bool DiskMultiMap::updateHeader(Header head){
    return bf.write(head, 0);
}

// updates the given bucket
bool DiskMultiMap::updateBucket(Bucket bucket, int offset){
    return bf.write(bucket, offset);
}

// updates the given node
bool DiskMultiMap::updateNode(Node node, int offset){
    return bf.write(node, offset);
}
/* =========================================== MAIN FUNCTIONS ========================================== */
/* ===== SPEC : DISKMULTIMAP :: DISKMULTIMAP() & ~DISKMULTIMAP() ===== */

DiskMultiMap::DiskMultiMap(){
}

DiskMultiMap::~DiskMultiMap(){
    bf.close();
}

/* ===== SPEC : DISKMULTIMAP :: CREATENEW() ===== */
bool DiskMultiMap::createNew(const string& filename, unsigned int numBuckets){
    /* already has been used to open or create a disk-based hash table,
     and the createNew() method is called,
     your method MUST first close the currently open data file*/
    bf.close();
    
    // create new file
    bool success = bf.createNew(filename);
    if(!success)
        return false;

    //save the file name
    fileName = filename;
    
    // count the offsets
    int curOffset = sizeof(Header);
    int bucketSize = sizeof(Bucket);
    bool flag = true;
    
    for(int i = 0; i < numBuckets; i++){
        flag = flag && bf.write(Bucket(), curOffset); // 1 number denoting # of buckets, and another bucketStruct * numbuckets for hashtable
        curOffset += bucketSize;
    }
    
    //now we add the head
    Header head;
    head.numBuckets = numBuckets;
    head.tail = curOffset;
    
    if(DEBUGGING){
        cerr << "Size of header : " << sizeof(Header) << endl;
        cerr << "Size of bucket : " << sizeof(Bucket) << endl;
        cerr << "Size of node : " << sizeof(Node) << endl;
        cerr << "Num buckets : " << head.numBuckets << endl;
        cerr << "Address at tail : " << curOffset << endl;
    }
    
    flag = flag && bf.write(head, 0);
    
    return flag;
}

/* ===== SPEC : DISKMULTIMAP :: OPENEXISTING() ===== */
bool DiskMultiMap::openExisting(const std::string& filename){
    /* already has been used to open or create a disk-based hash table,
     and the createNew() method is called,
     your method MUST first close the currently open data file*/
    bf.close();
    
    // create new file
    bool success = bf.openExisting(filename);
    fileName = filename;
    if(!success)
        return false;
    return true;
}

/* ===== SPEC : DISKMULTIMAP :: CLOSE() ===== */

void DiskMultiMap::close(){
    bf.close();
}

/* ===== SPEC : DISKMULTIMAP() :: INSERT() ===== */

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context){
    // Now let's create a Node struct that holds all this information:
    /* === CHECKING IF FILE IS OPEN FIRST! === */
    if(!bf.isOpen())
        return false;
    
    /* Upon successful insertion, this method returns true. Each key, value and context field MUST be NO MORE THAN 120 characters long, meaning that each of these fields can safely fit within a 121-byte traditional C string. Your insert() method MUST return false if the user tries to insert any key, value or context field with more than 120 or more characters. */
    if(key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    
    // let's first create a tuple with this
    MultiMapTuple temp;
    temp.key = key, temp.value = value, temp.context = context;

    Node add(temp);
    if(DEBUGGING){
        string k = add.key;
        string v = add.value;
        string c = add.context;
        cerr << "Key : " << k << " Value : " << v << " Context : " << c << endl;
    }
    
    
    // Let's try to insert it using the hash of the key:
    size_t hashedIndex = hashKey(key);
    
    // change position of hash into position of file and read the bucket
    int bucketIndex = hashToOffset(hashedIndex);
    Bucket curBucket;
    if(!bf.read(curBucket, bucketIndex)){
        cerr << "Cannot read current bucket" << endl;
    }
    
    Header head;
    if(!bf.read(head, 0)){
        cerr << "Can't read head" << endl;
    }
    
    int nodeIndex = NONE;
    if(head.deleted == NONE){ // we allocate new space
        if(DEBUGGING){
            cerr << "head.tail : " << head.tail << endl;
        }
        nodeIndex = head.tail; // rearrange bucket's linkedlist ordering
        head.tail += sizeof(Node);
        add.next = curBucket.start;

        curBucket.start = nodeIndex;
    }
    else{ // we recycle old nodes
        nodeIndex = head.deleted;
        if(DEBUGGING){
            cerr << "head.deleted : " << head.deleted << endl;
        }
        //now that we deleted, connect the new head.deleted to the next one
        bf.read(add, head.deleted);
        int nextDel = add.next;
        add.next = curBucket.start;
        curBucket.start = nodeIndex;
        head.deleted = nextDel;
    }
    
    // remember to update the header!
    if(!updateHeader(head))
        return false;
    if(!updateBucket(curBucket, bucketIndex))
        return false;
    if(DEBUGGING){
        cerr << "The start pointer of our linkedlist is at : " << curBucket.start << endl;
    }
    
    // We can finally add the node to the chosen index
    if(updateNode(add, nodeIndex))
        return true;
    else
        return false;
}

/* ===== SPEC : DISKMULTIMAP() :: SEARCH() ===== */

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key){
    /* === CHECKING IF FILE IS OPEN FIRST! === */
    if(!bf.isOpen())
        return Iterator();
    
    // get the bucketIndex from the key
    int bucketIndex = hashToOffset(hashKey(key));
    
    // Read in our bucket
    Bucket bucket;
    bf.read(bucket, bucketIndex);
    return Iterator(&bf, key, bucket.start);
}

/* ===== SPEC : DISKMULTIMAP() :: ERASE() ===== */

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context){
    /* === CHECKING IF FILE IS OPEN FIRST! === */
    if(!bf.isOpen())
        return 0;
    
    // get the header so we can modify the deleted index
    Header head;
    if(!bf.read(head, 0)){
        cerr << "Can't read head" << endl;
    }
    
    // get the bucketIndex from the key
    int bucketIndex = hashToOffset(hashKey(key));
    
    // Read in our bucket
    Bucket bucket;
    bf.read(bucket, bucketIndex);
    
    // Now that we have the index, let's keep going:
    // loop through to find all values that match:
    Node node, behindNode;
    if(bucket.start != NONE)
        bf.read(node, bucket.start);
    int prevAddress = NONE;
    int curAddress = bucket.start; // start from the 2nd node
    
    int counter = 0;
    
    while(curAddress != NONE){
        if(prevAddress != NONE)
            bf.read(behindNode, prevAddress);
        bf.read(node, curAddress);
        int origNext = node.next;
        
        //node.next may be replaced later, so we keep track of it right now
        // if the strcmp's for all 3 fields are the same
        string k = node.key;
        string v = node.value;
        string c = node.context;
        if(key == k && value == v && context == c){
            // we must soft delete this node
            counter++;
            
            // if nodes have been deleted, we must connect them like a stack
            int connect = head.deleted;
            
            // there is no previous address - first element in the bucket
            if(prevAddress == NONE){
                bucket.start = node.next;
                updateBucket(bucket, bucketIndex);
            }
            else{
                behindNode.next = node.next;
            }
            
            node.next = connect;
            // only if behindNode exists
            head.deleted = curAddress;
            if(prevAddress != NONE){
                updateNode(behindNode, prevAddress);
            }
            updateNode(node, curAddress);
            updateHeader(head);
            curAddress = origNext;
        }
        else{
            prevAddress = curAddress;
            curAddress = origNext;
        }
    }
    return counter;
}

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ITERATOR() ===== */

DiskMultiMap::Iterator::Iterator(){
    curAddress = NONE;
    key = "";
    bf = nullptr;
}

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ITERATOR() ===== */

DiskMultiMap::Iterator::Iterator(BinaryFile* binaryf, std::string k, int curAddr){
    bf = binaryf;
    key = k;
    Node node;
    curAddress = curAddr;
    
    //get the value of the first node that has the value of k
    while(curAddress != NONE){
        bf->read(node, curAddress);
        if(strcmp(node.key, key.c_str()) == 0){ // we found the key!
            if(DEBUGGING){
                cerr << "We found something at addr : " << curAddress << "!"<<endl;
                string str = node.key;
                cerr << str << endl;
            }
            break;
        }
        curAddress = node.next;
    }
    //now, the value of curAddress is either at the first node of key or at NONE, and we're done.
}

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: isValid() ===== */
bool DiskMultiMap::Iterator::isValid() const{ return (curAddress != NONE); }

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ++() ===== */
DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++(){
    if(!isValid())
        return *this;
    Node node;
    bf->read(node, curAddress); // we need to move at least once
    curAddress = node.next;
    while(curAddress != NONE){
        bf->read(node, curAddress);
        if(strcmp(node.key, key.c_str()) == 0){
            break;
        }
        int nextAddress = node.next;
        curAddress = nextAddress;
    }
    return *this;
}

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: *() ===== */
MultiMapTuple DiskMultiMap::Iterator::operator*(){
    if(!isValid())
        return MultiMapTuple();
    Node node;
    if(!bf->read(node, curAddress)){
        return MultiMapTuple();
    }
    
    MultiMapTuple tuple;
    tuple.key = node.key;
    tuple.value = node.value;
    tuple.context = node.context;
    return tuple;
}
