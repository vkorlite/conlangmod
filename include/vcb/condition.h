#pragma once
#include <string>
#include <vector>
#include <utility>
#include "vcb/st_Object.h"
using namespace std;

class Condition{
    public:
        string language;
        string condstr;
        pair<vector<Condition>, vector<char>> subconditions;
        int needObjects;
        char operand;
        bool arithmetic;
        Condition(string condstr, string language, bool arithmetic);
        Condition();
        // vector<st_Object> getObjects(){
        void* check(vector<st_Object> input);
    private:

        bool boolCheck(vector<st_Object> input); //0: the object in question, 1-len: objects prev defined
        int arCheck(vector<st_Object> input);
        int getObjectsNum();
        pair<vector<Condition>, vector<char>> getSubconditions();
        char getOperand();
        char getArithmeticOperand();
        string getValue(vector<st_Object> input);
};
