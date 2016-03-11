//
//  IntelWeb.cpp
//  Project4
//
//  Created by Ray Zhang on 3/7/16.
//  Copyright © 2016 Ray Zhang. All rights reserved.
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
/* ===== SPECS : INTELWEB :: CREATENEW() ===== */
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
/* ===== SPECS : INTELWEB :: OPENEXISTING() ===== */
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

/* ===== SPECS : INTELWEB :: INGEST() ===== */
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

/* ===== SPECS : INTELWEB :: CRAWL() ===== */

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

    badEntitiesFound.clear();

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
    
    /* Sort in Lexicographical ordering! */
    badEntitiesFound = vector<string>(badSet.begin(), badSet.end());
    badInteractions = vector<InteractionTuple>(badInteractionSet.begin(), badInteractionSet.end());
    sort(badEntitiesFound.begin(), badEntitiesFound.end());
    badInteractions.erase(unique(badInteractions.begin(), badInteractions.end(), (intTupleEquals())), badInteractions.end());
    return badEntitiesFound.size();
}

/* ===== SPEC : INTELWEB :: PURGE() ===== */
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