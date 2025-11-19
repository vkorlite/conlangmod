#pragma once

#include <string>
#include <vector>
#include "vcb/var.h"
using namespace std;

class st_Object{
    public:
        string language;
        vector<Var> properties;
        string value;
        //constructors
        st_Object(string language, vector<Var> properties);
        st_Object(string language);
        //accessors
        string getPropertyValue(string varName);
        vector<Var> treeTraversal();
        bool equals(st_Object object);
        //mutator
        void addProperties(vector<Var> newProperties);
        void get();
};
