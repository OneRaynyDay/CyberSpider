#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include <string>
#include <vector>
#include "DiskMultiMap.h"
#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <queue>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
                       );
    bool purge(const std::string& entity);
    
private:
    double getLoadFactor(){ return 0.5; }
    bool isPrevalent(std::string key, unsigned int minPrevalenceToBeGood);
    // overloaded < operator
    struct intTupleComp{
        bool operator()(const InteractionTuple& a, const InteractionTuple& b)
        {
            if(a.context == b.context){
                if(a.from == b.from)
                    return a.to < b.to;
                else
                    return a.from < b.from;
            }
            else
                return a.context < b.context;
        }
    };
    // a binary predicate implemented as a class:
    struct intTupleEquals{
        bool operator()(const InteractionTuple& first, const InteractionTuple& second)
        {
            return (first.context == second.context) && (first.from == second.from) && (first.to == second.to);
        }
    };
    // a binary predicate implemented as a class:
    struct multTupleEquals{
        bool operator()(const MultiMapTuple& first, const MultiMapTuple& second)
        {
            return (first.context == second.context) && (first.value == second.value) && (first.key == second.key);
        }
    };
    // returns how many bads were found
    int processPairs(DiskMultiMap::Iterator& iter,
                      unordered_set<string>& badSet,
                      set<InteractionTuple, intTupleComp>& badInteractionSet,
                      unordered_map<string, bool>& prevalents,
                      unsigned int minPrevalenceToBeGood, queue<string>& q,
                      bool fowardOrBack);
    
    static const std::string POSTFIX_FROMMAP;
    static const std::string POSTFIX_TOMAP;
    std::string curFile;
    DiskMultiMap fromMap, toMap;
    unordered_map<string, int> prevMap;
    
    // Your private member declarations will go here
};

#endif // INTELWEB_H_
