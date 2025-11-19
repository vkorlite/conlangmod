#include <string>
#include <vector>
#include "st/st_string.h"
#include "vcb/st_Object.h"
#include "vcb/condition.h"
#include "vcb/domain.h"

Domain::Domain(){
    this->domstr = "";
    this->language = "";
    vector<Condition> subconds;
    Condition empty;
    subconds.push_back(empty);
    this->subconditions = subconds;
}

Domain::Domain(string domstr, string language){
    this->domstr = st_remove(domstr, ' ');
    this->language = language;
    this->subconditions = this->getSubconditions();
}

vector<Condition> Domain::getSubconditions(){
    vector<Condition> output;
    vector<string> condstrSplit = st_split(this->domstr, ',');
    for(string condstrSplitter : condstrSplit){
        output.push_back(Condition(condstrSplitter, this->language, false));
    }
    return output;
}

//todo
vector<vector<st_Object>> Domain::get(vector<st_Object> input){
    vector<vector<st_Object>> output;
    for(int i = 0; i < input.size(); i++){
        vector<st_Object> temp_input;
        temp_input.push_back(input.at(i));
        bool* eligible = static_cast<bool*>(this->subconditions.at(0).check(temp_input));
        if(*eligible){
            output.push_back(temp_input);
        }
    }
    for(int i = 1; i < subconditions.size(); i++){
        vector<vector<st_Object>> temp_output;
        for(int j = 0; j < output.size(); j++){
            for(int k = 0; k < input.size(); k++){
                vector<st_Object> temp_input = output.at(j);
                bool add = true;
                for(int l = 0; (l < temp_input.size() && add); l++){
                    add = !(temp_input.at(l).equals(input.at(k)));
                }
                if(add){
                    temp_input.push_back(input.at(k));
                    bool* eligible = static_cast<bool*>(this->subconditions.at(i).check(temp_input));
                    if(*eligible){
                        temp_output.push_back(temp_input);
                    }
                }
            }
        }
        output = temp_output;
    }
    return output;
}
