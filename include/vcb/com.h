#pragma once
#include <string>
#include <vector>
#include "vcb/domain.h"
#include "vcb/condition.h"
#include "vcb/st_Object.h"

enum opt{
    FORALL_BOOL,
    EXISTS_BOOL,
    STRICT_BOOL,
    FORALL_LIST,
    EXISTS_LIST,
    NUMERIC
};

class Com{
    public:
        Com(string comstr, string language);
        string comstr;
        string language;
        Domain domain;
        Condition condition;
        enum opt option;
        void* get(vector<st_Object> input);
    private:
        bool boolGet(vector<st_Object> input);
        vector<vector<st_Object>> listGet(vector<st_Object> input);
        int intGet(vector<st_Object> input);
};
