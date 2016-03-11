//
//  IntelWeb.cpp
//  Project4
//
//  Created by hongshuhong on 3/7/16.
//  Copyright © 2016 hongshuhong. All rights reserved.
//

#include <stdio.h>
#include <cstring>
#include <algorithm>
#include "IntelWeb.h"

/* ===== INITIALIZING STATIC MEMBERS ===== */
 const std::string IntelWeb::POSTFIX_FROMMAP("_from_map.dat");
 const std::string IntelWeb::POSTFIX_TOMAP("_to_map.dat");
/* ===== HELPER FUNCTIONS ===== */
bool IntelWeb::isPrevalent(std::string key, unsigned int minPrevalenceToBeGood){
    //we have to find the key in both maps!
    int counter = 0;
    DiskMultiMap::Iterator iter1 = fromMap.search(key);
    DiskMultiMap::Iterator iter2 = toMap.search(key);
    
    while(iter1.isValid()){
        counter++;
        if(counter >= minPrevalenceToBeGood)
            return true;
        ++iter1;
    }
    while(iter2.isValid()){
        counter++;
        if(counter >= minPrevalenceToBeGood)
            return true;
        ++iter2;
    }
    return false;
}

/* ===== SPECS : INTELWEB :: INTELWEB() =====
 The IntelWeb constructor MUST initialize each new IntelWeb object.It probably won’t
 need to do too much work
 */
IntelWeb::IntelWeb()
{
    
}
/* ===== SPECS : INTELWEB :: ~INTELWEB() =====
 The IntelWeb destructor MUST cause any open disk files to be closed and must free any
 dynamically allocated memory that hasn’t already been freed.
 */
IntelWeb::~IntelWeb()
{
    close();
}
/* ===== SPECS : INTELWEB :: CREATENEW() =====
 The createNew() method MUST create a set of empty disk-based data structures to hold
 information about all of the telemetry log data that you’ll be loading from one or more
 telemetry log data files. Your disk-based data structures MUST consist of one or more
 DiskMultiMaps, and you MUST not use any other disk files.
 
 -- make searching through the data structures to discover attacks (in your crawl() method) as efficient as possible
 
 -- The first parameter is a filename prefix
 
 -- no restrictions on how many DiskMultiMaps your createNew() method creates, or what data the hash tables hold
 
 -- The second parameter to createNew() specifies the expected number of telemetry data items that your data structures will have to hold (e.g., 2,000,000)
 
 -- well-performing hash tables have a load factor of at most .75, so your hash tables MUST also have a planned load factor less than or equal to .75 given the value of the second parameter
 
 -- The createNew() method MUST close any DiskMultiMaps this IntelWeb object might have created or opened before creating any new ones. If successful, createNew() MUST
 have overwritten any existing DiskMultiMap disk files with new, empty versions. This method MUST run in O(maxDataItems) time
 */
bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems)
{
    //The createNew() method MUST close any DiskMultiMaps this IntelWeb object might have created or opened before creating any new ones.
    fromMap.close();
    toMap.close();
    
    //hash tables MUST also have a planned load factor less than or equal to .75 given the value of the second parameter
    int numBuckets = ((double)maxDataItems)/getLoadFactor();
    
    if(fromMap.createNew(filePrefix + POSTFIX_FROMMAP, numBuckets) &&
       toMap.createNew(filePrefix + POSTFIX_TOMAP, numBuckets)){
        curFile = filePrefix;
        return true;
    }
    return false;
}
/* ===== SPECS : INTELWEB :: OPENEXISTING() =====
 open an existing set of DiskMultiMap(s) that were previously created and populated using calls to IntelWeb's createNew() with the given file
 prefix and ingest().
 
 Any data previously ingested into these DiskMultiMaps will be
 available for access after the openExisting() method has been called.
 
 If this method is able to successfully open its data structures, then it MUST return true.
 
 On the other hand, if there is any problem (e.g., one or more of your data files cannot be
 opened), this method MUST close any successfully-opened data files (so that all data
 files end up closed) and return false.
 
 Once this method has been called successfully, the user may use the ingest() method to
 ingest one or more additional telemetry log data files and store their information within
 your data structures, the crawl() method to search for attacks, and the purge() method to
 purge specific telemetry data items from the data structures.
 
 This method MUST close any DiskMultiMaps this IntelWeb object might have created or
 opened before opening any new ones.
 
 This method MUST run in O(1) time.
 */
bool IntelWeb::openExisting(const std::string& filePrefix)
{
    fromMap.close();
    toMap.close();
    
    if(toMap.openExisting(filePrefix + POSTFIX_TOMAP) && fromMap.openExisting(filePrefix + POSTFIX_FROMMAP)){
        curFile = filePrefix;
        return true;
    }
    
    return false;
}
/* ===== SPECS : INTELWEB :: CLOSE() =====
 The close() method is used to close any DiskMultiMaps created or opened by this
 IntelWeb object.
 This method MUST run in O(1) time.
 */
void IntelWeb::close()
{
    toMap.close();
    fromMap.close();
}

/* ===== SPECS : INTELWEB :: INGEST() =====
 The ingest() method is used to insert all the data from a telemetry log file of the specified
 name into your disk-based data structures. While you will ingest data from each
 telemetry file into your data structures once, you’ll be searching through these structures
 (using crawl()) over and over and over, so the speed of ingestion is much less important
 than the speed at which you can search your data structures. So you MUST design your
 data structures to facilitate fast searching rather than trying to make insertion fast if it will
 slow down searching.
 
 ---------------------------
 File Creation Events
 
 A File Creation Event indicates that a program (e.g., foo.exe) created another program (e.g., bar.exe) on a particular machine (e.g., m491). Its format is:
 
 machineID <space> creatorFile <space> createdFile <newline>
 
 m491 foo.exe bar.exe
 
 ---------------------------
 File Download Events
 
 A File Download Event indicates that a given URL (e.g., http://www.viruses.com/downloads) sent a program (e.g., foo.exe) down to a particular machine (e.g., m7711).
 
 machineID <space> URL <space> downloadedFile <newline>
 
 m7711 http://www.viruses.com/downloads foo.exe
 
 ---------------------------
 File Contact Events
 A File Contact Event indicates that a given program (e.g., foo.exe) contacted a specific URL.
 The components of a line can be machine names, program names, or URLs, each of
 which is a string of non-whitespace characters. The lines are in one of three different
 formats:

 machineID <space> programFile <space> URL <newline>
 
 m15006 foo.exe http://www.viruses.com/downloads
 */
bool IntelWeb::ingest(const std::string& telemetryFile){
    //refresh our prevMap
    prevMap.clear();
    
    ifstream inf(telemetryFile);
    if ( ! inf)
    {
        cerr << "Cannot open " << telemetryFile << " file!" << endl;
        return false;
    }
    string line;
    //Adding unorderedMap iters
    unordered_map<string,int>::iterator iter1, iter2;
    while (getline(inf, line))
    {
        istringstream iss(line);
        string key, context, value;
        // The return value of operator>> acts like false
        // if we can't extract a word followed by a number
        if ( ! (iss >> context >> key >> value) )
        {
            cerr << "Ignoring badly-formatted input line: " << line << endl;
            continue;
        }
        toMap.insert(key, value, context);
        fromMap.insert(value, key, context);
    }
    /*unordered_map<string,int>::iterator iter = prevMap.begin();
    while(iter != prevMap.end()){
        cout << iter->first << " " << iter->second << endl;
        ++iter;
    }*/
    return true;
}

/* ===== SPECS : INTELWEB :: CRAWL() =====
 responsible for (a) discovering and outputting an ordered
 vector of all malicious entities found in the previously-ingested telemetry, and (b)
 outputting an ordered vector of every interaction5 discovered that includes at least
 one malicious entity, by “crawling” through the ingested telemetry!
 
 Your crawl() method MUST discover ALL malicious entities within the provided telemetry, 
 and it MUST place these discovered malicious entities in lexicographical order 
 (i.e., sorted as strings are sorted in increasing order) within the badEntitiesFound parameter.
 
 If the same interaction occurs on two different machines, e.g.:
 {from=“www.yahoo.com”, to=“r.exe”, context=“m10101”} 
 {from=“www.yahoo.com”, to=“r.exe”, context=“m09090”}
 
 
 */

int IntelWeb::processPairs(DiskMultiMap::Iterator& iter,
                           unordered_set<string>& badSet,
                           set<InteractionTuple, intTupleComp>& badInteractionSet,
                           unordered_map<string, bool>& prevalents,
                           unsigned int minPrevalenceToBeGood, queue<string>& q,
                           bool forwardOrBack){
    int numBad = 0;

    while(iter.isValid()){
        MultiMapTuple temp = *iter;
        if(forwardOrBack)
            badInteractionSet.insert(InteractionTuple(temp.key, temp.value, temp.context));
        else
            badInteractionSet.insert(InteractionTuple(temp.value, temp.key, temp.context));
        
        unordered_set<string>::iterator it = badSet.find(temp.value);
        
        if(it != badSet.end()){
            ++iter;
            continue;
        }
        
        unordered_map<string, bool>::iterator prevIter = prevalents.find(temp.value);
        bool prev;
        //not added into the map yet
        if(prevIter == prevalents.end()){
            prev = isPrevalent(temp.value, minPrevalenceToBeGood);
            prevalents.insert({temp.value, prev});
        }
        else{
            prev = prevIter->second;
        }

        if(!prev){
            //if it's NOT above Pgood and it is not in the set yet
            q.push(temp.value);
            badSet.insert(temp.value);
            numBad++;
        }
        ++iter;
    }

    return numBad;
}
unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
                   unsigned int minPrevalenceToBeGood,
                   std::vector<std::string>& badEntitiesFound,
                   std::vector<InteractionTuple>& badInteractions){
    /*The badEntitiesFound parameter MUST NOT contain ANY values other than those malware entities discovered within the telemetry, even if the badEntitiesFound vector was not empty upon the call to crawl()
     */
    badEntitiesFound.clear();
    /* ===== INPUTS =====
     1. An aggregated set of millions of lines of telemetry - you’ve already digested this telemetry and stored it in the disk-based data structures with the ingest() method!
     2. A vector of one or more initially-known malicious entities (e.g., a vector of one or more strings with the names of bad files and/or websites) – these are passed to crawl() in the indicators parameter.
     3. A prevalence threshold value Pgood (e.g., Pgood = 12) that can be used to determine whether an entity is too popular to be considered malicious: If the number of telemetry log lines that refer to entity E is greater than or equal to Pgood, then E is considered legitimate (aka not malicious) because it’s too popular to be an advanced attack – this prevalence threshold is the minPrevalenceToBeGood parameter. 
     
     While the number of ingested telemetry interactions can be so huge that we are requiring that you store them in disk files, you may assume that the number of malicious entities and bad interactions, while possibly large, is still small enough for them to fit in RAM
     
     You may also assume that the number of entities whose prevalence is at least minPrevalenceToBeGood is small enough for them to fit in RAM.
     */
    unordered_set<string> badSet;
    unordered_map<string, bool> prevalents;
    
    set<InteractionTuple, intTupleComp> badInteractionSet;
    
    queue<string> q;
    for(int i = 0; i < indicators.size(); i++){
        DiskMultiMap::Iterator iter1 = toMap.search(indicators[i]);
        DiskMultiMap::Iterator iter2 = fromMap.search(indicators[i]);

        if(iter1.isValid() || iter2.isValid()){
            q.push(indicators[i]);
            badSet.insert(indicators[i]);
        }
    }
    while(!q.empty()){
        string key = q.front();
        q.pop();
        DiskMultiMap::Iterator iter1 = toMap.search(key);
        DiskMultiMap::Iterator iter2 = fromMap.search(key);
        
        processPairs(iter1, badSet, badInteractionSet, prevalents, minPrevalenceToBeGood, q, true);
        processPairs(iter2, badSet, badInteractionSet, prevalents, minPrevalenceToBeGood, q, false);
    }
    
    /* ===== GOTCHAS =====
     Gotchas with the crawl() Method
     Be careful when writing the crawl() method; it’s very easy to crawl in circles if you don’t
     take precautions not to do so! For example, consider the following three telemetry items:
     
     m1 www.virus.com b.exe
     m2 b.exe c.exe
     m3 c.exe www.virus.com
     */
    
    
    /* ===== RETURN =====
     Note: You should not return the number of interactions discovered, but the number of malicious entities!
     */
    
    
    /* Sort in Lexicographical ordering! */
    badEntitiesFound = vector<string>(badSet.begin(), badSet.end());
    badInteractions = vector<InteractionTuple>(badInteractionSet.begin(), badInteractionSet.end());
    sort(badEntitiesFound.begin(), badEntitiesFound.end());
    badInteractions.erase(unique(badInteractions.begin(), badInteractions.end(), (intTupleEquals())), badInteractions.end());
    return badEntitiesFound.size();
}

/* ===== SPEC : INTELWEB :: PURGE() =====
 The purge command is used to remove ALL references to a specified entity (e.g., a
 filename or website) from your IntelWeb disk-based data structures. Subsequent crawls of
 your telemetry using the crawl() method MUST NOT find any references to the specified
 entry after it’s been purged.
 
 For example, let’s assume that you previously ingested the following telemetry items into
 your IntelWeb log files:
 
 Machine Initiator Entity Target Entity
 m1 a.exe b.exe 
 m1 a.exe b.exe
 m2 b.exe c.exe
 m3 a.exe d.exe
 m4 www.bad.com c.exe
 
 Let’s further assume, for the purposes of this example, that your IntelWeb class uses a
 disk-based map data structure that associates each Initiator Entity to all related telemetry
 items that hold that Initiator Entity9:
 
 a.exe {m1,a.exe,b.exe}, {m1,a.exe,b.exe}, {m3, a.exe, d.exe}
 b.exe {m2, b.exe, c.exe}
 www.bad.com {m4, www.bad.com, c.exe}
 
 AFTER CALLING THIS:
 purge(b.exe);
 
 Your resulting disk-based data structure should look like this after the purge() method
 finishes (struck-out items have been completely removed from the file):
 
 a.exe {m3, a.exe, d.exe}
 b.exe
 www.bad.com {m4, www.bad.com, c.exe}
 
 */
bool IntelWeb::purge(const std::string& entity){
    // erase requires parameters:
    // const std::string& key, const std::string& value, const std::string& context
    DiskMultiMap::Iterator iter1 = fromMap.search(entity);
    int count;
    vector<MultiMapTuple> tempVec;
    while(iter1.isValid()){
        MultiMapTuple tuple = *iter1;
        ++iter1;
        tempVec.push_back(tuple);
    }
    DiskMultiMap::Iterator iter2 = toMap.search(entity);
    while(iter2.isValid()){
        MultiMapTuple tuple = *iter2;
        MultiMapTuple tupleReversed;
        tupleReversed.key = tuple.value;
        tupleReversed.value = tuple.key;
        tupleReversed.context = tuple.context;
        ++iter2;
        tempVec.push_back(tupleReversed);
    }
    int i;
    for(i = 0; i < tempVec.size(); i++){
        MultiMapTuple tuple = tempVec[i];
        fromMap.erase(tuple.key, tuple.value, tuple.context);
        toMap.erase(tuple.value, tuple.key, tuple.context);
    }
    return i > 0;
}



