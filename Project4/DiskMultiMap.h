#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

/** =================== SPECIFICATIONS ===================
 You MUST write a class named DiskMultiMap that lets the user:
 1. Efficiently associate a given string key with one or more pairs of string values, where each pair contains a value string and a context string. That means that each association is between a single key, e.g. “foo.exe” and one or more pairs of data, where each pair holds two strings, e.g. {“bar.exe”, “m1234”}, or {“www.yahoo.com”, “m5678”}.
 2. Efficiently look up items by a key string, and receive an iterator to enumerate all matching associations.
 3. Efficiently delete existing associations with a given key.
 All strings that will be added to the DiskMultiMap should be no more than 120 characters long (and thus representable as a C string in storage 121 bytes long). You should check for this in your code, and return an error if this is not the case.
 Your implementation MUST use an open hash table, and the hash table data structure MUST be stored a binary disk file rather than in RAM.
 Here’s the interface that you MUST implement in your DiskMultiMap class:
 */

class DiskMultiMap
{
public:
    class Iterator
    {
    public:
        Iterator();
        // You may add additional constructors
        Iterator(BinaryFile * f, std::string k, int curAddress);
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        BinaryFile* bf;
        std::string key;
        int curAddress;
        bool foo(){ return true; }
        // Your private member declarations will go here
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    
private:
    // Your private member declarations will go here
    /* ------------- MY PRIVATE CONSTANTS --------------- */
    static const bool DEBUGGING = false;
    static const int NONE = -1; // equivalent of null in disk
    static const int SIZE_STRING = 121;
    /* ------------- MY PRIVATE STRUCTS ------------- */
    struct Header{
        Header(){
            numBuckets = 0;
            tail = NONE;
            deleted = NONE;
        }
        int numBuckets;
        int tail;
        int deleted;
    };
    struct Bucket{
        Bucket(){
            start = NONE;
        }
        int start;
    };
    struct Node{
        Node(){
            key[0] = 0;
            value[0] = 0;
            context[0] = 0;
            next = NONE;
        }
        Node(MultiMapTuple tuple){
            strcpy(key, tuple.key.c_str());
            strcpy(value, tuple.value.c_str());
            strcpy(context, tuple.context.c_str());
        }
        char key[SIZE_STRING];
        char value[SIZE_STRING];
        char context[SIZE_STRING];
        int next;
    };

    /* ------------- MY PRIVATE FUNCTIONS --------------- */
    size_t hashKey(const std::string str);
    
    // changes the hash into the correct bucket place.
    int hashToOffset(const std::size_t hashedTuple);
    
    // updates the header
    bool updateHeader(Header head);
    
    // updates the given bucket
    bool updateBucket(Bucket bucket, int offset);
    
    // updates the given node
    bool updateNode(Node node, int offset);
    /* ------------- MY PRIVATE MEMBERS -------------- */
    std::string fileName;
    BinaryFile bf;
};
#endif // DISKMULTIMAP_H_