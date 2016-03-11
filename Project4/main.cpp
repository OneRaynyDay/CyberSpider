//
//  main.cpp
//  Project4
//
//  Created by hongshuhong on 3/5/16.
//  Copyright © 2016 hongshuhong. All rights reserved.
//

#include <iostream>
#include <cassert>
#include "DiskMultiMap.h"
#include "IntelWeb.h"

using namespace std;

//int main() {
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    DiskMultiMap::Iterator it = x.search("hmm.exe");
//    if (it.isValid())
//    {
//        cout << "I found at least 1 item with a key of hmm.exe\n";
//        do
//        {
//            MultiMapTuple m = *it; // get the association
//            cout << "The key is: " << m.key << endl;
//            cout << "The value is: " << m.value << endl;
//            cout << "The context is: " << m.context << endl; cout << endl;
//            ++it; // advance iterator to the next matching item
//        } while (it.isValid());
//    }
//}

//int main() {
//    DiskMultiMap x;
//    x.createNew("myhashtable.dat",100); // empty, with 100 buckets
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    DiskMultiMap::Iterator it = x.search("goober.exe");
//    if ( ! it.isValid())
//        cout << "I couldn't find goober.exe\n";
//}

//int main() {
//    DiskMultiMap x;
//    //x.createNew("myhashtable.dat", 100); // empty, with 100 buckets
//    x.openExisting("myhashtable.dat"); // empty, with 100 buckets
//    if(x.insert("hmm.exe", "pfft.exe", "m52902")){
//        cout << "inserting works" << endl;
//    }
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");
//    x.insert("blah.exe", "bletch.exe", "m0003");
//    // line 1
//    
//    cout << "Finished Inserting : " << endl;
//    if (x.erase("hmm.exe", "pfft.exe", "m52902") == 2)
//        cout << "Just erased 2 items from the table!\n";
//    // line 2
//    if (x.erase("hmm.exe", "pfft.exe", "m10001") > 0)
//        cout << "Just erased at least 1 item from the table!\n";
//    // line 3
//    if (x.erase("blah.exe", "bletch.exe", "m66666") == 0)
//        cout << "I didn't erase this item cause it wasn't there\n";
//    
//    /*x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m52902");
//    x.insert("hmm.exe", "pfft.exe", "m10001");*/
//}

//int main(int argc, const char * argv[]) {
//    DiskMultiMap dmm;
//    dmm.createNew("temp.dat", 100);
//    if(!dmm.insert("hmm.exe", "pfft.exe", "m52902")){
//        //cout << "wasn't able to insert it." << endl;
//    }
//    dmm.insert("hmm.exe", "pfft.exe", "m10001");
//    DiskMultiMap::Iterator iter = dmm.search("hmm.exe");
//    MultiMapTuple tuple1 = (*iter);
//    //cout << "Is iter valid? : " << iter.isValid() << endl;
//    cout << "Value of tuple : " << tuple1.key << " , " << tuple1.context << " , " << tuple1.value<<endl;
//    ++iter;
//    MultiMapTuple tuple2 = (*iter);
//    //cout << "Is iter valid? : " << iter.isValid() << endl;
//    cout << "Value of tuple : " << tuple2.key << " , " << tuple2.context << " , " << tuple2.value<<endl;
////    BinaryFile b, a;
////    if(b.openExisting("temp.dat") && b.write("David", 0, 5) && a.openExisting("temp.dat") && a.write("David", 0, 5)){
////        cout << "This is fine to do.";
////    }
//}

/*
void someFunc()
{
    IntelWeb x;
    // create a new set of data files with a prefix of mydata
    x.createNew("mydata", 10000);
    x.ingest("outputlog.dat");
    x.close();
}

int main(){
    DiskMultiMap d;
    d.createNew("hello.dat", 100);
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    assert(d.erase("a", "a", "a") == 4);
    d.close();
    assert(d.openExisting("hello.dat"));
    assert(d.erase("a", "a", "a") == 0);
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    d.insert("a", "a", "a");
    d.insert("c", "c", "c");
    
    someFunc();
    
    cout << "Worked" << endl;
}
*/
//
//  main.cpp
//  hi
//
//  Created by Bradley Zhu on 3/8/16.
//  Copyright © 2016 Bradley Zhu. All rights reserved.
//

/*int main(int argc, const char * argv[]) {
    DiskMultiMap d;
    if (d.createNew("asdf", 10)) {
        cout << "Created" << endl;
    }
    
    d.close();
    cout << "Closed" << endl;
    
    d.openExisting("asdf");
    cout << "Reopened" << endl;
    
    d.insert("hi", "bye", "m0");
    d.insert("hi", "diff", "m0");
    cout << "Inserted" << endl;
    
    d.close();
    d.openExisting("asdf");
    d.insert("I have a life", "asddadq", "m1");
    d.insert("Ray is bad at coding", "jk", "m2");
    d.insert("Tbh same", "can't even", "m3");
    
    
    DiskMultiMap::Iterator i = d.search("hi");
    MultiMapTuple tuple = *i;
    assert(tuple.key == "hi");
    assert(tuple.value == "diff");
    assert(tuple.context == "m0");
    
    ++i;
    tuple = *i;
    assert(tuple.key == "hi");
    assert(tuple.value == "bye");
    assert(tuple.context == "m0");
    
    i = d.search("bye");
    tuple = *i;
    assert(tuple.key == "");
    assert(tuple.value == "");
    assert(tuple.context == "");
    
    i = d.search("I have a life");
    tuple = *i;
    assert(tuple.key == "I have a life");
    assert(tuple.value == "asddadq");
    assert(tuple.context == "m1");
    
    ++i;
    tuple = *i;
    assert(tuple.key == "");
    assert(tuple.value == "");
    assert(tuple.context == "");
    
    i = d.search("Ray is bad at coding");
    tuple = *i;
    assert(tuple.key == "Ray is bad at coding");
    assert(tuple.value == "jk");
    assert(tuple.context == "m2");
    
    i = d.search("Tbh same");
    tuple = *i;
    assert(tuple.key == "Tbh same");
    assert(tuple.value == "can't even");
    assert(tuple.context == "m3");
}*/
