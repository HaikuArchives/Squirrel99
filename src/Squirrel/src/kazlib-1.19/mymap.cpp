#include "hash.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <string>

typedef struct t_data {
    int a;
    int b;
};


class MyMap {

    public:
    
        class Iterator {
            public:
                Iterator(hash_t *h);
                ~Iterator();
                char *Key();
                t_data *Data();
                bool Next(); // use ++
            private:
                hscan_t hs;
                hnode_t *thenode;
        };

        MyMap();
        ~MyMap();
        // main methods
        bool Set(char *key,t_data *data);
        t_data *Get(char *data); 
        bool Delete(char *key);
        bool Exists(char *key);
        // iterator
        Iterator *Iterate(); 
        // other methods
        long Length();
        void Clear();
    private:
        hash_t  *h;
};

MyMap::Iterator::Iterator(hash_t *h)
{
    hash_scan_begin(&hs, h);
    thenode = NULL;
}

MyMap::Iterator::~Iterator()
{
}


char *MyMap::Iterator::Key()
{
    if(thenode)
        return (char*) hnode_getkey(thenode);  
    else
        return NULL;
}

t_data*MyMap::Iterator::Data()
{
    if(thenode)
        return (t_data*) hnode_get(thenode);
    else
        return NULL;
}

bool MyMap::Iterator::Next()
{
    thenode = hash_scan_next(&hs);    
}

MyMap::MyMap()
{
    h = hash_create(HASHCOUNT_T_MAX, NULL, NULL);    
    //h = hash_create(10000, NULL, NULL);
}

MyMap::~MyMap()
{
    // no need to delete the key for the function/primitive map
    hscan_t hs;
    hnode_t *hn;
    hash_scan_begin(&hs, h);
	while ((hn = hash_scan_next(&hs)))
        free(hnode_getkey(hn));
			   
    hash_free(h);    
}

bool MyMap::Set(char *key,t_data *data)
{
    hnode_t *hn;
    
    // if exist, we replace
    hn = hash_lookup(h, key);
    if(hn)
    {
        hnode_put(hn,data);
        return true;
    }
    else // else we create
        return hash_alloc_insert(h,strdup(key),data);
}

t_data *MyMap::Get(char *key)
{
    hnode_t *hn;

    hn = hash_lookup(h, key);
    if(hn)
        return (t_data*)hnode_get(hn);
    else
        return NULL;
}

bool MyMap::Exists(char *key)
{
    hnode_t *hn;
    hn = hash_lookup(h, key);
    if(hn)
        return true;
    else
        return false;
}

bool MyMap::Delete(char *key)
{
    hnode_t *hn;
    hn = hash_lookup(h, key);
    if(hn)
    {
        // need to delete the key and data
        hash_scan_delfree(h, hn);
        return true;
    }
    else
        return false;
}


MyMap::Iterator *MyMap::Iterate()
{
    return new Iterator(h);
}

long MyMap::Length()
{
    return hash_count(h);
}

void MyMap::Clear()
{
    hscan_t hs;
    hnode_t *hn;
    hash_scan_begin(&hs, h);
	while ((hn = hash_scan_next(&hs)))
        free(hnode_getkey(hn));

    hash_free_nodes(h);   
}

char *table[16] = {
    "plane",
    "car",
    "bike",
    "submarine",
    "rocket",
    "moon",
    "mars",
    "yellow",
    "saturn",
    "space",
    "bus",
    "truck",
    "train",
    "subway",
    "computer",
    "mars"
};

long tmy=0,tstl=0;
long t;

#define TIMES 1000 

mymap()
{
    t_data *data;
    MyMap *map = new MyMap;
    MyMap::Iterator *i;

    for(int i=0;i<16;i++)
    {
        data = new t_data;
        data->a = i;
        data->b = i;

        map->Set(table[i],data);    
    }
    
    //cout << map->Length() << "\n";

    i = map->Iterate();
    while(i->Next());
        //cout << i->Key() << "\n";
    delete i;

    t = clock();
    for(int k=0;k<TIMES;k++)
        data = map->Get("bus");
    tmy += clock() - t;    
    //if(data)
        //cout << data->a << "\n";

    map->Delete("moon");    

    delete map;
}

typedef map<string,t_data *,less<string> > STLMap;

stlmap()
{
    t_data *data;
    STLMap::const_iterator i;
    STLMap *map = new STLMap;

    for(int i=0;i<16;i++)
    {
        data = new t_data;
        data->a = i;
        data->b = i;

        (*map)[string(table[i])] = data;    
    }

    //cout << map->size() << "\n";

    for(i=map->begin();i!=map->end();i++);
        //cout << i->first << "\n";
      
    t = clock();  
    for(int k=0;k<TIMES;k++)
        data = (*map)[string("bus")];
    tstl += clock() - t;        
    //if(data)
    //    cout << data->a << "\n";   

    map->erase("moon");

    delete map;
}

main()
{
    long i;
    clock_t t0,t1;
    long t_my,t_stl;

    t0 = clock();
    for(i=1;i<TIMES;i++)
        mymap();
    t1 = clock();
    t_my = t1 - t0;
    cout << t_my << "\n";

    t0 = clock();
    for(i=1;i<TIMES;i++)
        stlmap();
    t1 = clock();
    t_stl = t1 - t0;
    cout << t_stl << "\n"; 

    if(t_my==0)
        t_my = 1;
    if(tmy==0)
        tmy = 1;       

    cout << (float)t_stl / (float)t_my << "\n";
    cout << tmy / TIMES << "\n";
    cout << tstl / TIMES << "\n";
    cout << (float)tstl / (float)tmy << "\n";


}

