#pragma once
#include <string>
#include <vector>
#include "vcb/st_Object.h"
#include "vcb/condition.h"
using namespace std;

class Domain{
    public:
        string domstr;
        vector<Condition> subconditions;
        string language;
        Domain(string domstr, string language);
        Domain();
        vector<vector<st_Object>> get(vector<st_Object> input);
    private:
        vector<Condition> getSubconditions();
};
