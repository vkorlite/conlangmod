#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "st/st_string.h"
#include "st/st_Exception.h"
#include "vcb/var.h"
#include "vcb/st_Object.h"
using namespace std;

st_Object::st_Object(string language, vector<Var> properties){
    this->language = language;
    this->addProperties(properties);
}

vector<Var> treeTraversal_rec_helper(Var var){
    vector<Var> output;
    output.push_back(var);
    for(auto dependency : var.dependencies){
        for(Var dependentVar : dependency.second){
            vector<Var> input = treeTraversal_rec_helper(dependentVar);
            for(int i = 0; i < input.size(); i++){
                output.push_back(input.at(i));
            }
        }
    }
    return output;
}

vector<Var> st_Object::treeTraversal(){
    vector<Var> output;
    for(int i = 0; i < this->properties.size(); i++){
        vector<Var> input = treeTraversal_rec_helper(this->properties.at(i));
        for(int j = 0; j < input.size(); j++){
            output.push_back(input.at(j));
        }
    }
    return output;
}

string getPropertyValue_rec_helper(Var parentVar, string varName){
    if(parentVar.name == varName){
        return parentVar.value;
    }
    string valueDependency = parentVar.valueDependency(varName);
    for(Var dependentVar : parentVar.dependencies[valueDependency]){
        return getPropertyValue_rec_helper(dependentVar, varName);
    }
    return "";
}

string st_Object::getPropertyValue(string varName){
    string output = "";
    bool done = false;
    for(int i = 0; i < this->properties.size(); i++){
        if(this->properties.at(i).name == varName){
            return this->properties.at(i).value;
        }
    }
    for(int i = 0; (i < this->properties.size() && !done); i++){
        if(this->properties.at(i).hasDependent(varName)){
            done = true;
            output = getPropertyValue_rec_helper(this->properties.at(i), varName);
            if(output != ""){
                return output;
            }
        }
    }
    return output;
}

bool equals_rec_helper(Var tVar, Var oVar){
    if(tVar.value == oVar.value){
        bool output = true;
        if(tVar.value == ""){
            for(auto tDependency : tVar.dependencies){
                for(int i = 0; i < tDependency.second.size(); i++){
                    if(oVar.value == ""){
                        for(auto oDependency : oVar.dependencies){
                            for(int j = 0; j < oDependency.second.size(); j++){
                                output = equals_rec_helper(tDependency.second.at(i), oDependency.second.at(j));
                                if(output == false){
                                    return false;
                                }
                            }
                        }
                    } else {
                        for(int j = 0; j < oVar.dependencies[oVar.value].size(); j++){
                            output = equals_rec_helper(tDependency.second.at(i), oVar.dependencies[oVar.value].at(j));
                            if( output == false){
                                return false;
                            }
                        }
                    }
                }
            }
        } else {
            for(int i = 0; i < tVar.dependencies[tVar.value].size(); i++){
                if(oVar.value == ""){
                    for(auto oDependency : oVar.dependencies){
                        for(int j = 0; j < oDependency.second.size(); j++){
                            output = equals_rec_helper(tVar.dependencies[tVar.value].at(i), oDependency.second.at(j));
                            if(output == false){
                                return false;
                            }
                        }
                    }
                } else {
                    for(int j = 0; j < oVar.dependencies[oVar.value].size(); j++){
                        output = equals_rec_helper(tVar.dependencies[tVar.value].at(i), oVar.dependencies[oVar.value].at(j));
                        if( output == false){
                            return false;
                        }
                    }
                }
            }
        }
        return output;
    }
    return false;
}

bool st_Object::equals(st_Object object){
    bool output = true;
    for(int i = 0; i < this->properties.size(); i++){
        bool specific = false;
        for(int j = 0; j < object.properties.size(); j++){
            if(this->properties.at(i).name == object.properties.at(j).name){
                specific = equals_rec_helper(this->properties.at(i), object.properties.at(j));
                if(!specific){
                    return false;
                }
                break;
            }
        }
    }
    return output;
}

void addProperties_rec_helper(Var parentVar, Var property){
    bool done = false;
    if(parentVar.name == property.name){
        done = true;
    } else {
        if(parentVar.value == ""){
            for(auto dependency : parentVar.dependencies){
                for(Var dependentVar : dependency.second){
                    if(dependentVar.name == property.name){
                        if(dependentVar.value == ""){
                            parentVar.addValue(dependency.first);
                            parentVar.set(dependency.first);
                        }
                        parentVar.addDependency(dependency.first, property.name);
                        done = true;
                        break;
                    }
                }
                if(done){
                    break;
                }
            }
        } else {
            for(Var dependentVar : parentVar.dependencies[parentVar.value]){
                if(dependentVar.name == property.name){
                    if(dependentVar.value != property.value){
                        parentVar.removeValue(parentVar.value);
                        string value = parentVar.valueDependency(property);
                        parentVar.addValue(value);
                        parentVar.set(value);
                    }
                    parentVar.addDependency(parentVar.value, property.name);
                    done = true;
                    break;
                }
            }
        }
    }
    if(!done){
        string valueDependency = parentVar.valueDependency(property);
        for(Var parentVars : parentVar.dependencies[valueDependency]){
            if(parentVars.hasDependent(property)){
                addProperties_rec_helper(parentVars, property);
            }
        }
    }
}

void st_Object::addProperties(vector<Var> newProperties){
    for(Var newProperty : newProperties){
        vector<int> found;
        for(int i = 0; i < this->properties.size(); i++){
            if(this->properties.at(i).hasDependent(newProperty)){
                found.push_back(i);
            }
        }
        if(found.empty()){
            this->properties.push_back(newProperty);
        } else {
            for(int i = 0; i < found.size(); i++){
                addProperties_rec_helper(this->properties.at(found.at(i)), newProperty);
            }
        }
    }
}
void st_Object::get(){
    //update tree
    if(!Var::treeWrite){
        Var::makeTree(this->language);
    }

    //get all lines
    vector<string> allLines;
    try{
        ifstream fin;
        string line;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fin.open(rec_dir+this->language+"/vars_tree", ios::in);
        if(fin.is_open()){
            while(getline(fin, line)){
                allLines.push_back(line);
            }
            fin.close();
        } else { throw(st_Exception("FILE IO ERROR: could not open vars_tree"));}
    } catch(st_Exception e) { cout << e.error << endl;}

    //add parentVars to buffer
    vector<Var> original = this->treeTraversal();
    vector<Var> buffer;
    int count = 0;
    for(int k = 0; k < original.size(); k++){
        string useName = "(" + original.at(k).name + ")";
        for(string line : allLines){
            for(int i = 0; i < st_match(line, useName) ; i++){
                int lastSpace = -1;
                int bracketOff = 0;
                int lastBracket = st_find(st_substr(line, 0, st_find(line, useName, i)), '[', st_match(line, '[')-1);
                string value;
                bool foundChain = false;
                for(int j = st_find(line, useName, i); j >= 0 && !foundChain; j--){
                    switch(line.at(j)){
                        case ']':
                            bracketOff--;
                            break;
                        case ' ':
                            if(bracketOff == 1){
                                value = st_substr(line, j+1, lastBracket-1);
                            }
                            lastSpace = j;
                            break;
                        case '[':
                            lastBracket = j;
                            if(bracketOff == 1){
                                string name = st_substr(line, j+2, lastSpace-1);
                                Var parentVar(name, this->language);
                                parentVar.addValue(value);
                                parentVar.set(value);
                                if(count > 0){
                                    parentVar.addDependency(value, buffer.at(count-1).name);
                                }
                                buffer.push_back(parentVar);
                                count++;
                            } else{
                                bracketOff++;
                            }
                            break;
                    }
                    if(j == 0 && bracketOff == 1){
                        string name = st_substr(line, j+1, lastSpace-1);
                        Var parentVar(name, this->language);
                        parentVar.addValue(value);
                        parentVar.set(value);
                        if(count > 0){
                            parentVar.addDependency(value, buffer.at(count-1).name);
                        }
                        buffer.push_back(parentVar);
                        count++;
                    }
                }
            }
        }
    }

    this->addProperties(buffer); //we don't need to worry about repeats as it gets handled by add properties
                                 //nor do we need to worry about different values as it just won't get found
}
