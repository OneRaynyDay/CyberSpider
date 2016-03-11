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
/* ===== SPEC : DISKMULTIMAP :: DISKMULTIMAP() & ~DISKMULTIMAP() =====
 You MUST implement a constructor and destructor for your DiskMultiMap. The constructor MUST initialize your object. When the destructor returns, the disk file associated with the hash table must have been closed (either previously by a client calling DiskMultiMap::close() or by the destructor itself).
*/

DiskMultiMap::DiskMultiMap(){
}

DiskMultiMap::~DiskMultiMap(){
    bf.close();
}

/* ===== SPEC : DISKMULTIMAP :: CREATENEW() =====
 You MUST implement the createNew() method. This method MUST create an empty, open hash table in a binary disk file with the specified filename, with the specified number of empty buckets. If there is already an existing disk file with the same filename at the time that the createNew() method is called, then your createNew() method MUST overwrite the original file with a brand-new file (our BinaryFile::createNew() method will automatically perform this overwriting for you). Once the hash table has been created, you may then proceed to add new associations, search through it, or delete associations with the object’s other methods.
 
 If your DiskMultiMap object already has been used to open or create a disk-based hash table, and the createNew() method is called, your method MUST first close the currently- open data file, and then proceed with creating the newly specified data file.
 
 If your method fails for any reason, it MUST return false. Otherwise, if it succeeds it MUST return true.
 This method MUST run in O(B) time where B is the number of buckets in the hash table.
*/
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

/* ===== SPEC : DISKMULTIMAP :: OPENEXISTING() =====
 You MUST implement the openExisting() method. This method MUST open a previously-created disk-based hash table with the specified filename. If the specified disk file does not exist on the hard drive, then your method MUST return false. If your method succeeds in opening the specified file, then it MUST return true. Once the hash table has been opened, you may then proceed to search through it, add new associations, or delete associations.
 If your DiskMultiMap object already has been used to open or create a disk-based hash table, and the openExisting() method is called, your method MUST first close the currently-open data file, and then proceed with opening the newly specified data file.
 This method MUST run in O(1) time.
*/
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

/* ===== SPEC : DISKMULTIMAP :: CLOSE() =====
 You MUST implement the close() method. This method is used to ensure that all data written to your hash table is safely saved to the binary disk file, and the binary disk file is closed. If a client doesn't call this method, then your DiskMultiMap destructor MUST close the binary disk file. Obviously, the client should only call the close() method if they first called the createNew() or openExisting() methods to create/open a hash table data file. Otherwise, the close() method should do nothing, as it should is called on an already- closed file.
 This method MUST run in O(1) time.
*/

void DiskMultiMap::close(){
    bf.close();
}

/* ===== SPEC : DISKMULTIMAP() :: INSERT() =====
 You MUST implement the insert() method. This method adds a new association to the disk-based hash table associating the specified key, e.g. “foo.exe”, with the associated value and context strings, e.g. “www.google.com” and “m12345”. Since you’re implementing a multimap, your insert function MUST be able to properly store duplicate associations. Consider this code:
 
 int main() {
     DiskMultiMap x;
     bool insert(const std::string& key, const std::string& value,
     const std::string& context)
 }
 
 x.createNew("myhashtable.dat",100); // empty, with 100 buckets
 x.insert("hmm.exe", "pfft.exe", "m52902"); 
 x.insert("hmm.exe", "pfft.exe", "m52902");
 x.insert("hmm.exe", "pfft.exe", "m10001"); 
 x.insert("blah.exe", "bletch.exe", "m0003");
 
 The above would result in your hash table holding the following associations (although not necessarily in the ordering shown below. We place no restrictions on what order your associations are stored in your hash table):
 “hmm.exe” {“pfft.exe”, “m52902”}, {“pfft.exe”, “m52902”}, {“pfft.exe”, “m10001”}
 “blah.exe” {“bletch.exe”, “m0003”}

 Remember, this method MUST insert the new association directly into your hash table’s disk file, NOT to an in-memory hash table! This means that every valid insert call will result in one or more modifications to your hash table’s on-disk data structures!
 
 This method MUST run in O(N/B) time assuming your hash table holds a generally diverse set of N items and has B buckets. If you are inserting an association with a particular key X, and the hash table already holds K associations with key X, then this method may run in O(K) time.
*/

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

/* ===== SPEC : DISKMULTIMAP() :: SEARCH() =====
 You MUST implement the search() method. This method is used to find all associations in the hash table that match the specified key string. Your method MUST return a DiskMultiMap::Iterator object, which is analogous to a C++ iterator. The user can then use this iterator to enumerate all associations that matched the specified key string. If no associations matched the specified key string, then the Iterator returned must be invalid (i.e., one for which DiskMultiMap::Iterator::isValid() returns false). Here’s how you might use the search() method – look for the bold parts:
 
 int main() {
     DiskMultiMap x;
     x.createNew("myhashtable.dat",100); // empty, with 100 buckets
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m10001"); 
     x.insert("blah.exe", "bletch.exe", "m0003");
     DiskMultiMap::Iterator it = x.search("hmm.exe"); 
     if (it.isValid())
     {
         cout << "I found at least 1 item with a key of hmm.exe\n"; 
         do
         {
             MultiMapTuple m = *it; // get the association cout << "The key is: " << m.key << endl;
             cout << "The value is: " << m.value << endl; cout << "The context is: " << m.context << endl; cout << endl;
             ++it; // advance iterator to the next matching item 
         } while (it.isValid());
     }
 }
 
 The above code might print:
 
 I found at least 1 item with a key of hmm.exe 
 
 The key was: hmm.exe
 The value was: pfft.exe
 The context was: m52902
 
 The key was: hmm.exe
 The value was: pfft.exe
 The context was: m52902
 
 The key was: hmm.exe
 The value was: pfft.exe 
 The context was: m00001
 
 Here’s another example:
 int main() {
     DiskMultiMap x;
     x.createNew("myhashtable.dat",100); // empty, with 100 buckets
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m10001"); 
     x.insert("blah.exe", "bletch.exe", "m0003");
     DiskMultiMap::Iterator it = x.search("goober.exe"); 
     if ( ! it.isValid())
        cout << "I couldn't find goober.exe\n";
 }
 
 The above code would print:
 I couldn’t find goober.exe
 
 As you can see, your DiskMultiMap’s search() method (and the returned Iterator) works similarly to the C++ find() methods provided by the STL associative container classes!
 This method MUST run in O(N/B) time assuming your hash table holds a generally diverse set of N items and has B buckets. If you are searching for an item with a particular key X, and the hash table holds K associations with key X, then this method may run in O(K) time
*/

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

/* ===== SPEC : DISKMULTIMAP() :: ERASE() =====
 You MUST implement the erase() method. This method MUST remove all associations in the hash table that exactly match the specified key, value, and context strings passed in and return the number of associations removed. Your hash table MUST somehow track the location of all deleted nodes, so these nodes can be reused should one or more new associations later be added to the hash table after deletion. Here’s an example how your erase() method might be used:
 

 int main() {
     DiskMultiMap x;
     x.createNew("myhashtable.dat",100); // empty, with 100 buckets
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m52902"); 
     x.insert("hmm.exe", "pfft.exe", "m10001"); 
     x.insert("blah.exe", "bletch.exe", "m0003");
     // line 1
     if (x.erase("hmm.exe", "pfft.exe", "m52902") == 2) 
        cout << "Just erased 2 items from the table!\n";
     // line 2
     if (x.erase("hmm.exe", "pfft.exe", "m10001") > 0)
        cout << "Just erased at least 1 item from the table!\n";
     // line 3
     if (x.erase("blah.exe", "bletch.exe", "m66666") == 0)
        cout << "I didn't erase this item cause it wasn't there\n";
 }
*/

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

/* =========================================================================================================================== */
/* ========================================== DISKMULTIMAP :: ITERATOR IMPLEMENTATION ======================================== */
/* =========================================================================================================================== */

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ITERATOR() =====
The default constructor must create an Iterator in an invalid state. This constructor must run in O(1) time.
*/

DiskMultiMap::Iterator::Iterator(){
    curAddress = NONE;
    key = "";
    bf = nullptr;
}

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ITERATOR() =====
 You may write other Iterator constructors with whatever parameters you like. It is your
 choice whether the Iterator created by any such constructor is in a valid or invalid state.
 
 If the Iterator created is in a valid state, then it must Any such constructor MUST run in
 O(N/B) time or better, assuming that is being initialized to point into a hash table holding 
 a generally diverse set of N items, with B buckets.
 */

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

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: isValid() =====
 This method MUST return true if the iterator is in a valid state, and false otherwise. This
 method MUST always run in O(1) time.
 */
bool DiskMultiMap::Iterator::isValid() const{ return (curAddress != NONE); }

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: ++() =====
 You MUST implement a prefix increment operator (but not a decrement operator or a postfix increment operator) for the Iterator class. This operator MUST do nothing if the Iterator it’s called on is invalid. Otherwise, the ++ operator MUST advance the Iterator to the next association in the DiskMultiMap with the same key as the association the Iterator currently points to, if there is one; if there is no next association with the same key, then the ++ operator MUST change the Iterator’s state to invalid. The method returns a reference to the Iterator it's called on.
 This method MUST run in O(N/B) time or less, assuming that is called on an Iterator pointing into a hash table holding a generally diverse set of N items, with B total buckets.
 */
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

/* ===== SPEC : DISKMULTIMAP() :: ITERATOR() :: *() =====
 You MUST implement the unary * operator for your Iterator class – this is the dereference operator, and it allows you to examine an association pointed to by a valid Iterator, using syntax akin to that of dereferencing a C++ pointer or an STL iterator. The * operator MUST return an object of type MultiMapTuple:
 struct MultiMapTuple {
     std::string key;
     std::string value;
     std::string context;
 };
 
 If the Iterator is in an invalid state, each string in the MultiMapTuple returned is the empty string. Otherwise, the key, value and context strings of the MultiMapTuple returned have values equal to the key, value, and context components of the association the Iterator points to.
 This method MUST run in O(1) time.
 Note: Reading data from a disk file is literally thousands or millions of times slower than reading data from your computer’s RAM, so consider the performance consequences of the following code:
 */
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















































