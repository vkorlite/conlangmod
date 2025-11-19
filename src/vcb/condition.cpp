#include <string>
#include <cmath>
#include <vector>
#include <utility>
#include "st/st_Exception.h"
#include "st/st_string.h"
#include "vcb/st_Object.h"
#include "vcb/condition.h"
using namespace std;

Condition::Condition(){
    this->language = "";
    this->condstr = "";
    this->arithmetic = false;
    this->needObjects = 0;
    this->operand = '\0';
}

Condition::Condition(string condstr, string language, bool arithmetic){
    this->language = language;
    this->condstr = st_remove(condstr, ' ');
    this->arithmetic = arithmetic;

    //get subconditions
    if(this->condstr.find("(") != string::npos){
        this->subconditions = this->getSubconditions();
    }
    //Identify objects needed
    this->needObjects = this->getObjectsNum();
    //Identify operand 
    if(arithmetic){
        this->operand = this->getArithmeticOperand();
    } else {
        this->operand = this->getOperand();
    }
}

void* Condition::check(vector<st_Object> input){
    if(arithmetic){
        int* typeOut = new int;
        *typeOut = this->arCheck(input);
        void* output = typeOut;
        return output;
    } else {
        bool* typeOut = new bool;
        *typeOut = this->boolCheck(input);
        void* output = typeOut;
        return output;
    }
}

bool Condition::boolCheck(vector<st_Object> input){ 
    if(input.size() < this->needObjects){
        throw(st_Exception("NOT ENOUGH OBJECTS"));
    }
    if(this->subconditions.first.empty()){
        string temp = this->getValue(input);
        vector<string> comparisons = st_split(temp, this->operand);
        string comparator = st_toLower(comparisons.at(0));
        switch(this->operand){
            case '=':
                return (comparisons.at(0) == comparisons.at(1));
            case '<':
                return (stoi(comparisons.at(0)) < stoi(comparisons.at(1)));
            case '>':
                return (stoi(comparisons.at(0)) > stoi(comparisons.at(1)));
            case '!':
                if(comparator == "false"){
                    return true;
                } else {
                    return false;
                }
            case '\0':
                if(comparator == "true"){
                    return true;
                } else {
                    return false;
                }
            default:
                return false;
        }
    } else {
        bool anyArithmetic = false;
        for(int i =0 ; i < this->subconditions.first.size(); i++){
            if(this->subconditions.first.at(i).arithmetic){
                anyArithmetic = true;
                break;
            }
        }
        if(this->operand == '<' || this->operand == '>' || anyArithmetic){ //if one subcondition is arithmetic so are they all
            if(this->subconditions.first.size() > 1){
                int num1 = this->subconditions.first.at(0).arCheck(input);
                int num2 = this->subconditions.first.at(1).arCheck(input);
                switch(this->operand){
                    case '=':
                        return (num1==num2);
                    case '>':
                        return (num1>num2);
                    case '<':
                        return (num1<num2);
                    default:
                        return (num1!=0);
                }
            } else { return (this->subconditions.first.at(0).arCheck(input) != 0); }
        } else {
           vector<bool> subAns;
           for(Condition subcondition : this->subconditions.first){
               subAns.push_back(subcondition.boolCheck(input));
           }
           bool ans = true;
           int off = 2;
           switch(this->subconditions.second.at(0)){
               case '|':
                   ans = (subAns.at(0) || subAns.at(1));
                   break;
               case '&':
                   ans = (subAns.at(0) && subAns.at(1));
                   break;
               case '!':
                   ans = (!subAns.at(0));
                   off = 1;
                   break;
               default:
                   ans = subAns.at(0);
                   off = 1;
                   break;
           }
           for(int i = off; i < subAns.size(); i++){
               if(i < this->subconditions.second.size()){
                   switch(this->subconditions.second.at(i)){
                       case '|':
                           ans = (ans || subAns.at(i));
                           break;
                        case '&':
                           ans = (ans && subAns.at(i));
                           break;
                        case '!':
                           ans = !subAns.at(i);
                           break;
                   }
               }
           }
           return ans;
        }
    }
}
int Condition::arCheck(vector<st_Object> input){
    if(input.size() < this->needObjects){
        throw(st_Exception("NOT ENOUGH OBJECTS"));
    }

    if(!arithmetic){
        return (this->boolCheck(input))? 1 : 0;
    }
    if(this->subconditions.first.empty()){
        string temp = this->getValue(input);
        vector<string> comparisons = st_split(temp, this->operand);
        switch(this->operand){
            case '+':
                return (stoi(comparisons.at(0)) + stoi(comparisons.at(1)));
            case '-':
                return (stoi(comparisons.at(0)) - stoi(comparisons.at(1)));
            case '*':
                return (stoi(comparisons.at(0)) * stoi(comparisons.at(1)));
            case '\\':
                return (stoi(comparisons.at(0)) / stoi(comparisons.at(1)));
            case '\0':
                return stoi(comparisons.at(0));
            default:
                return 0;
        }
    } else {
        switch(this->operand){
            case '+':
                return this->subconditions.first.at(0).arCheck(input) + this->subconditions.first.at(1).arCheck(input);
            case '-':
                return this->subconditions.first.at(0).arCheck(input) - this->subconditions.first.at(1).arCheck(input);
            case '*':
                return this->subconditions.first.at(0).arCheck(input) * this->subconditions.first.at(1).arCheck(input);
            case '\\':
                return this->subconditions.first.at(0).arCheck(input) / this->subconditions.first.at(1).arCheck(input);
            default:
                return this->subconditions.first.at(0).arCheck(input);
        }
    }
}
    
int Condition::getObjectsNum(){
    vector<int> allIndices;
    for(int i = 0; i < st_match(this->condstr, '#'); i++){
        int len = 1;
        int find = st_find(this->condstr, '#', i)+1;
        while(len < this->condstr.length()){
            if(this->condstr.at(find+len) == '.'){
                break;
            }
            len++;
        }
        allIndices.push_back(stoi(this->condstr.substr(find, len)));
    }
    int out = 0;
    for(int index : allIndices){
        if(index > out){
            out = index;
        }
    }
    return out;
}
pair<vector<Condition>, vector<char>> Condition::getSubconditions(){
    pair<vector<Condition>,vector<char>> output;
    vector<Condition> outputSplit;
    vector<char> outputOp;
    string temp = this->condstr;
    while(temp.find("(") != string::npos){
        int posOfBr = temp.find("(");
        int posOfCom = st_compliment(temp,posOfBr);
        string subcondstr = st_substr(temp, posOfBr+1, posOfCom);
        if(subcondstr.find("=") == string::npos && subcondstr.find("<") == string::npos && subcondstr.find(">") == string::npos){
            Condition out = Condition(subcondstr, this->language, true);
            outputSplit.push_back(out);
        } else {
            outputSplit.push_back(Condition(subcondstr, this->language, false));
            if(posOfCom < temp.length()-1){
                if(temp.at(posOfCom+1) == '|' || temp.at(posOfCom+1) == '&'){
                    outputOp.push_back(temp.at(posOfCom+1));
                } else if(posOfBr > 0){
                    if(temp.at(posOfBr-1) == '!'){
                        outputOp.push_back('!');
                    }
                }
            }
        }
        temp = st_substr(temp, 0, posOfBr)+st_substr(temp, posOfCom+1, temp.length());
    }
    outputOp.push_back('\0');
    output.first = outputSplit;
    output.second = outputOp;
    return output;
}
char Condition::getOperand(){
    char output = '\0';
    string temp = this->condstr;
    while(temp.find("(") != string::npos){
        temp = st_substr(temp, 0, temp.find("("))+st_substr(temp, st_compliment(temp, temp.find("("))+1, temp.length());
    }
    if(temp.find("=") != string::npos){
        output = '=';
    } else if(temp.find(">") != string::npos){
        output = '>';
    } else if(temp.find("<") != string::npos){
        output = '<';
    }
    return output;
}
char Condition::getArithmeticOperand(){
    char output = '\0';
    string temp = this->condstr;
    while(temp.find("(") != string::npos){
        temp = st_substr(temp, temp.find("(")+1, st_compliment(temp, temp.find("(")-1));
    }
    if(temp.find("+") != string::npos){
        output = '+';
    } else if(temp.find("-") != string::npos){
        output = '-';
    } else if(temp.find("*") != string::npos){
        output = '*';
    } else if(temp.find("/") != string::npos){
        output = '/';
    }
    return output;
}
string Condition::getValue(vector<st_Object> input){
    string temp = this->condstr;
    for(int i = 0; i < input.size(); i++){
        while(temp.find("#"+to_string(i)) != string::npos){
            //get value
            int ilen = 1;
            if( i > 0){
                int ilen = floor(log(i)/log(10))+1;
            }
            string prop = st_substr(temp, temp.find("#"+to_string(i))+ilen+2, temp.find(string(1, this->operand)));
            string value = input.at(i).getPropertyValue(prop);
            //replace
            if(this->operand != '\0'){
                temp.replace(temp.find("#"+to_string(i)), temp.find(string(1,this->operand))-temp.find("#"+to_string(i)), value);
            } else {
                temp.replace(temp.find("#"+to_string(i)), temp.length()-temp.find("#"+to_string(i)), value);
            }
        }
    }
    return temp;
}
// vector<st_Object> getObjects(){
//     if(this->subconditions.first.empty()){
//         vector<st_Object> objects;
//         int len = st_match(this->condstr, "#");
//         for(int i = 0; i < len; i++){
//             objects.push_back(st_Object(this->language));
//             vector<string> delims;
//             delims.push_back("=");
//             delims.push_back("<");
//             delims.push_back(">");
//             vector<string> dirtVarNames = st_multiSplit(this->condstr, delims).first;
//             vector<Var> properties;
//             for(string dirtVarName : dirtVarNames){
//                 if(st_match(dirtVarName, "#") > 0){
//                      properties.push_back(Var(st_substr(dirtVarName, dirtVarName.find("."), dirtVarName.length()), this->language));
//                 }
//             }
//             objects.at(i).addProperties(properties);
//         }
//         return objects;
//     } else{
//        vector<vector<st_Object>> subconditionsObjects;
//        for(Condition subcondition : this->subconditions.first){
//            subconditionsObjects.push_back(subcondition.needObjects);
//        }
//        for(int i = 0; i < subconditionsObjects.at(0).size(); i++){
//            for(int j = 1; j < subconditionsObjects.size(); j++){
//                if(subconditionsObjects.at(j).size() > i){
//                    subconditionsObjects.at(0).at(i).addProperties(subconditionsObjects.at(j).at(i).properties);
//                }
//            }
//        }
//        return subconditionsObjects.at(0);
//     }
// }
