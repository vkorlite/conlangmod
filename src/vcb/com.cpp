#include <string>
#include <vector>
#include "vcb/com.h"
#include "st/st_string.h"
#include "vcb/condition.h"
#include "vcb/domain.h"
#include "vcb/st_Object.h"

Com::Com(string comstr, string language){
    this->comstr = st_remove(comstr, ' ');
    this->language = language;

    int posBr = st_find(this->comstr, '{', 0);
    string domstr = st_substr(this->comstr, posBr+1, st_compliment(this->comstr,posBr));

    this->domain = Domain(domstr, this->language);

    posBr = st_find(this->comstr, '{', 1);
    string optstr = st_substr(this->comstr, posBr+1, st_compliment(this->comstr,posBr));
    if(optstr.size() == 1){
        switch(tolower(optstr.at(0))){
            case 'a':
                this->option = FORALL_BOOL;
                break;
            case 'e':
                this->option = EXISTS_BOOL;
                break;
            case 's':
                this->option = STRICT_BOOL;
                break;
            case 'l':
                this->option = FORALL_LIST;
                break;
            case 'f':
                this->option = EXISTS_LIST;
                break;
            case 'n':
                this->option = NUMERIC;
                break;
            default:
                this->option = FORALL_LIST;
                break;
        }
    } else if(optstr.size() > 1){
        string decider = st_toLower(optstr);
        if(decider == "all" || decider == "forall" || decider == "allbool" || decider == "forallbool" || decider == "ab"){
            this->option = FORALL_BOOL;
        } else if(decider == "exists" || decider == "existsbool" || decider == "eb"){
            this->option = EXISTS_BOOL;
        } else if(decider == "strict" || decider == "strictbool" || decider == "strictlyexists" || decider == "strictlyexistsbool"){
            this->option = STRICT_BOOL;
        } else if(decider == "listall" || decider == "listforall" || decider == "alllist" || decider == "al" || decider == "foralllist"){
            this->option = FORALL_LIST;
        } else if(decider == "listexists" || decider == "existslist" || decider == "find" || decider == "el"){
            this->option = EXISTS_LIST;
        } else if(decider == "numeric" || decider == "num"){
            this->option = NUMERIC;
        } else {
            this->option = FORALL_LIST;
        }
    } else {
        this->option = FORALL_LIST;
    }
    posBr = st_find(this->comstr, '{', 2);
    string condstr = st_substr(this->comstr, posBr+1, st_compliment(this->comstr, posBr));
    this->condition = Condition(condstr, this->language, (this->option == NUMERIC));
}

void* Com::get(vector<st_Object> input){
    if(this->option == FORALL_BOOL || this->option == EXISTS_BOOL || this->option == STRICT_BOOL){
        bool* typeOut = new bool;
        *typeOut = this->boolGet(input);
        void* output = typeOut;
        return output;
    } else if(this->option == NUMERIC){
        int* typeOut = new int;
        *typeOut = this->intGet(input);
        void* output = typeOut;
        return output;
    } else {
        vector<vector<st_Object>>* typeOut = new vector<vector<st_Object>>;
        *typeOut = this->listGet(input);
        void* output = typeOut;
        return output;
    }
}

bool Com::boolGet(vector<st_Object> input){
    int holds = 0;
    vector<vector<st_Object>> domains = this->domain.get(input);
    for(vector<st_Object> eligibleDomain : domains){
        bool* eligible = static_cast<bool*>(this->condition.check(input));
        if(*eligible){
            if(this->option == EXISTS_BOOL){
                return true;
            }
            holds++;
        }
    }
    switch(this->option){
        case FORALL_BOOL:
            return (holds == domains.size());
            break;
        case STRICT_BOOL:
            return (holds == 1);
            break;
        default:
            return false;
            break;
    }
}

vector<vector<st_Object>> Com::listGet(vector<st_Object> input){
    vector<vector<st_Object>> output;
    vector<vector<st_Object>> domains = this->domain.get(input);
    for(vector<st_Object> eligibleDomain : domains){
        bool* eligible = static_cast<bool*>(this->condition.check(input));
        if(*eligible){
            output.push_back(eligibleDomain);
            if(this->option == EXISTS_LIST){
                return output;
            }
        }
    }
    return output;
}

int Com::intGet(vector<st_Object> input){
    int output = 0;
    vector<vector<st_Object>> domains = this->domain.get(input);
    for(vector<st_Object> eligibleDomain : domains){
        int* result = static_cast<int*>(this->condition.check(input));
        output += *result;
    }
    return output;
}
