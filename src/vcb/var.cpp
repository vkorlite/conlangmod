#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "st/st_Exception.h"
#include "st/st_string.h"
#include "vcb/var.h"
using namespace std;

//constructors
Var::Var(string name, vector<string> values, map<string, string> parentVarNames_needValues, string language){ //create new var
    Var::treeWrite = false;
    this->name = name;
    this->values = values;
    this->language = language;

    for(auto parentVarName_needValue : parentVarNames_needValues){
        Var parentVar(parentVarName_needValue.first, this->language);
        parentVar.get();
        parentVar.addDependency(parentVarName_needValue.second, this->name);
    }

    this->update();
}
Var::Var(string name, string language){ //initialize existing var
    this->name = name;
    this->language = language;
    this->cannon = false;
}
//mutator functions
void Var::set(string value){
    bool inValues = false;
    for(int i = 0; i < this->values.size(); i++){
        if(this->values.at(i) == value){
            this->value=value;
            inValues = true;
            break;
        }
    }
    if(!inValues){
        cout << "tried setting " << this->name << " to " << value << " ERROR: INVALID VAR SET, THROWING" << endl;
    }
}
void Var::irrSet(string value){
    bool inValues = false;
    for(int i = 0; i < this->values.size(); i++){
        if(this->values.at(i) == value){
            this->value=value;
            inValues = true;
            break;
        }
    }
    if(!inValues){
        this->addValue(value);
        this->value = value;
    }
}
void Var::addDependency(string value, string varName){
    Var var(varName, this->language);
    if(!this->hasDependent(varName)){
        if(!this->dependencies[value].empty()){
            this->dependencies[value].push_back(var);
        } else {
            vector<Var> clump;
            clump.push_back(var);
            this->dependencies[value] = clump;
        }
        if(this->cannon){
            this->update();
        }
    } 
}
void Var::addDependencies(map<string, vector<string>> dependencies){
    for(auto dependency: dependencies){
        for(string dependentVar : dependency.second){
            Var var(dependentVar, this->language);
            if(!this->hasDependent(dependentVar)){
                this->dependencies[value].push_back(var);
            }
        }
    }
    if(this->cannon){
        this->update();
    }     
}
void Var::removeDependency(string value, string varName){
    vector<string> varNames;
    for(Var dependency: this->dependencies[value]){
        varNames.push_back(dependency.name);
    }
    int indexOfVar = st_in(varNames, varName);
    if(indexOfVar != string::npos){
        this->dependencies[value].erase(this->dependencies[value].begin()+indexOfVar);
        if(this->cannon){
            this->update();
        }
    }
}
void Var::removeDependencies(map<string, vector<string>> dependencies){
    for(auto dependency : dependencies){
        for(string dependentVar : dependency.second){
            vector<string> varNames;
            for(Var dependency: this->dependencies[dependency.first]){
                varNames.push_back(dependency.name);
            }
            int indexOfVar = st_in(varNames, dependentVar);
            if(indexOfVar != string::npos){
                this->dependencies[dependency.first].erase(this->dependencies[dependency.first].begin()+indexOfVar);
            }
        }
    }
    if(this->cannon){
        this->update();
    }
}
void Var::addValue(string value){
    if( st_in(this->values, value) == string::npos){
        this->values.push_back(value);
        this->dependencies.insert({value, {}});
        if(this->cannon){
            this->update();
        }
    }
}
void Var::addvalues(vector<string> values){
    for(string value : values){
        if( st_in(this->values, value) == string::npos){
            this->values.push_back(value);
            this->dependencies.insert({value, {}});
        }
    }
    if(this->cannon){
        this->update();
    }
}
void Var::removeValue(string value){
    if(st_in(this->values, value) != string::npos){
        this->values.erase(this->values.begin()+st_in(this->values,value));
        this->dependencies.erase(value);
        if(this->cannon){
            this->update();
        }
    }
}
void Var::removeValues(vector<string> values){
    for(string value: values){
        if(st_in(this->values, value) != string::npos){
            this->values.erase(this->values.begin()+st_in(this->values,value));
            this->dependencies.erase(value);
        }   
    }
    if(this->cannon){
        this->update();
    }
}
//accessor function
string Var::valueDependency(Var var){ //Given a dependent var, check which value is required from the Var to beget the dependent Var
    for(auto dependency : this->dependencies){
        for(Var dependentVar : dependency.second){
            if(dependentVar.name == var.name || dependentVar.hasDependent(var)){ //Vars can be identified by name
                return dependency.first;
            }
        }
    }
    return "";
}
string Var::valueDependency(string varName){ //Given a dependent var, check which value is required from the Var to beget the dependent Var
    for(auto dependency : this->dependencies){
        for(Var dependentVar : dependency.second){
            if(dependentVar.name == varName || dependentVar.hasDependent(varName)){ //Vars can be identified by name
                return dependency.first;
            }
        }
    }
    return "";
}
bool Var::hasDependent(Var var){
    for(auto dependency : this->dependencies){
        for(Var dependentVar : dependency.second){
            if(dependentVar.name == var.name || dependentVar.hasDependent(var)){
                return true;
            } 
        }
    }
    return false;
}
bool Var::hasDependent(string varName){
    for(auto dependency : this->dependencies){
        for(Var dependentVar : dependency.second){
            if(dependentVar.name == varName || dependentVar.hasDependent(varName)){
                return true;
            } 
        }
    }
    return false;
}
//file functions
void Var::get(){
    string inLine;
    try{
        ifstream fin;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fin.open(rec_dir+this->language+"/vars_raw");
        if(fin.is_open()){
            string line;
            while(getline(fin, line)){
                if(st_split(line, ' ').at(0) == this->name){
                    inLine = line;
                }
            }
            fin.close();
        } else { throw(st_Exception("FILE IO ERROR: could not read vars_raw"));}
    } catch(st_Exception e){ cout << e.error << endl;}
    if(inLine != ""){
        this->values = st_split(st_substr(inLine, inLine.find('[')+1, inLine.find(']')), ' ');
        vector<string> dependenciesLine = st_split(st_substr(inLine, st_find(inLine, '[', 1)+1, st_compliment(inLine, st_find(inLine, '[', 1))), ',');
        for(int i = 0; i < dependenciesLine.size(); i++){
            string dependencyStr = dependenciesLine.at(i);
            if(dependencyStr != ""){
                vector<string> dependencyVarNames = st_split(st_substr(dependencyStr, 1, dependencyStr.length()-1), ' ');
                for(string dependencyVarName : dependencyVarNames){
                    Var dependentVar(dependencyVarName, this->language);
                    dependentVar.get();
                    if(this->dependencies[this->values.at(i)].empty()){
                        vector<Var> dependentVars;
                        dependentVars.push_back(dependentVar);
                        this->dependencies[this->values.at(i)] = dependentVars;
                    } else {
                        this->dependencies[this->values.at(i)].push_back(dependentVar);
                    }
                }
            }
        }
    }
    this->cannon = true;
}
void Var::update(){
    Var::treeWrite = false;
    vector<string> allLines;
    try{
        ifstream fin;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fin.open(rec_dir+this->language+"/vars_raw", ios::in);
        if(fin.is_open()){
            string line = "";
            while( getline(fin, line)){
                if(st_split(line, ' ').at(0) != this->name){
                    allLines.push_back(line);
                }
            }
            fin.close();
        } else { throw(st_Exception("FILE IO ERROR: could not open vars_raw"));}
    } catch(st_Exception e){ cout << e.error << endl;}

    string outLine = this->name + " [";
    for(int i = 0; i < this->values.size(); i++){
        outLine += this->values.at(i);
        if( i < this->values.size()-1){
            outLine+= " ";
        } else {
            outLine+="] ";
        }
    }
    outLine += "[";
    for(int i = 0; i < this->values.size(); i++){
        outLine += "[";
        for(int j = 0; j < this->dependencies[this->values.at(i)].size(); j++){
            outLine += this->dependencies[this->values.at(i)].at(j).name;
            if(j < this->dependencies[this->values.at(i)].size()-1){
                outLine += " ";
            }
        }
        outLine += "]";
        if(i < this->values.size()-1){
            outLine+= ",";
        } else {
            outLine += "]";
        }
    }
    try{
        ofstream fout;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fout.open(rec_dir+this->language+"/vars_raw", ios::out);
        if(fout.is_open()){
            for(string line : allLines){
                fout << line << endl;
            }
            fout << outLine;
            fout.close();
        } else { throw(st_Exception("FILE IO ERROR: could not write to vars_raw"));}
    } catch (st_Exception e) {cout << e.error << endl;}

    this->cannon=true;
}
void Var::remove(){
    vector<string> allLines;
    try{
        ifstream fin;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fin.open(rec_dir+this->language+"/vars_raw");
        if(fin.is_open()){
            string line;
            while(getline(fin, line)){
                if(st_split(line, ' ').at(0) != this->name){
                    allLines.push_back(line);
                }
            }
            fin.close();
        } else{ throw(st_Exception("FILE IO ERROR: could not open vars_raw"));}
    } catch(st_Exception e) { cout << e.error << endl;}
    try{
        ofstream fout;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fout.open(rec_dir+this->language+"/vars_raw", ios::out);
        if(fout.is_open()){
            for(int i = 0; i < allLines.size(); i++){
                fout << allLines.at(i);
                if(i < allLines.size()-1){
                    fout << endl;
                }
            }
            fout.close();
        } else { throw(st_Exception("FILE IO ERROR: could not open vars_raw")); }
    } catch(st_Exception e){ cout << e.error << endl;}

    this->cannon = false;
}
bool Var::treeWrite = false;

//tree functions
string makeTree_rec_helper(Var var){
    if(var.values.empty()){
        string values = "";
        for(string value : var.values){
            values += " " + value + ":[]";
        }
        return "(" + var.name + ")" + values;
    }
    string values = "";
    for(auto dependency: var.dependencies){
        values += " " + dependency.first + ":[";
        for(int i = 0; i < dependency.second.size(); i++){
            values += makeTree_rec_helper(dependency.second.at(i));
            if(i < dependency.second.size()-1){
                values += ",";
            }
        }
        values += "]";
    }
    return "(" + var.name + ")" + values;
}

void Var::makeTree(string language){
    //add io write
    ifstream fin;
    bool openWorked = false;
    vector<string> allLines;
    try{
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fin.open(rec_dir+language+"/vars_raw", ios::in);
        if(fin.is_open()){
            openWorked = true;
            string line;
            while(getline(fin, line)){
                allLines.push_back(line);
            }
        } else {
            throw(st_Exception("FILE IO ERROR: failed to open vars_raw"));
        }
        fin.close();
    } catch(st_Exception e){
        cout << e.error << endl;
    }
    if(openWorked){
        //get all vars
        vector<Var> allVars;
        for(auto line : allLines){
            Var var(st_split(line, ' ').at(0), language);
            var.get();
            allVars.push_back(var);
        }
        //find top level vars
        vector<Var> parentVars;
        for(Var varChecking: allVars){
            bool isDependent = false;
            for(Var varChecker: allVars){
                if(varChecker.hasDependent(varChecking)){
                    isDependent = true;
                    break;
                }
            }
            if(!isDependent){
                parentVars.push_back(varChecking);
            }
        }
        string outLine = "";
        for(int i = 0; i < parentVars.size(); i++){
            outLine += makeTree_rec_helper(parentVars.at(i));
            if(i < parentVars.size()-1){
                outLine += "\n";
            }
        }
        ofstream fout;
        string rec_dir = st_con(getenv("CONLANGMOD_REC_DIR"));
        fout.open(rec_dir+language+"/vars_tree");
        try{
            if(fout.is_open()){
                Var::treeWrite = true;
                fout << outLine;
            } else {
                throw(st_Exception("FILE IO ERROR: couldn't access vars_tree"));
            }

        } catch(st_Exception e){
            cout << e.error << endl;
        }
        fout.close();
    }
}

//debug

// int main(){
//     map<string, string> needValues;
//     needValues["DEPENDENT_VARNAME1"] = "VALUE5";
//     vector<string> values;
//     values.push_back("VALUE1");
//     values.push_back("VALUE2");
//     Var var("DEPENDENT2_VAR1", values, needValues, "format_example_language");
//     Var::makeTree("format_example_language");
//     return 0;
// }

//2025-10-13 debug worked
// int main(int argc, char* argv[]){
//     if(argc > 1){
//         if(strcmp(argv[1], "-h") == 0){
//             cout << "var.cpp debug bin" << endl;
//             cout << "Options: " << endl;
//             cout << "-h: help" << endl;
//             cout << "-i: Check if var.cpp correctly reads example file" << endl;
//             cout << "-o: Check if var.cpp correctly writes into example files"  << endl;
//             cout << "-if [language_name]: Check if var.cpp correctly reads specified file " << endl;
//             cout << "-of [language_name]: Check if var.cpp correctly writes specified file " << endl;
//             cout << "-t: Check if var.cpp correctly creates a tree" << endl;
//         } else if(strcmp(argv[1],"-i") == 0){
//             Var test("NAME", "format_example_language");
//             cout << test.name << endl;
//             cout << endl;
//             cout << "Values: " << endl;
//             for(string value : test.values){
//                 cout << value << endl;
//             }
//             cout << endl;
//             cout << "Dependencies: " << endl;
//             for(int i = 0; i < test.dependencies.size(); i++){
//                 cout << test.values.at(i) << ": " << endl;
//                 for(Var var : test.dependencies[test.values.at(i)]){
//                     cout << var.name << endl;
//                 }
//                 cout << endl;
//             } 
//         } else if(strcmp(argv[1],"-o") == 0){
//             vector<string> values;
//             values.push_back("VALUE5");
//             values.push_back("VALUE9");
//             map<string, string> depends;
//             depends["VALUE1"] = "NAME";
//             Var test("DEPENDENT_VARNAME1", values, depends, "format_example_language");
//         } else if(strcmp(argv[1],"-if") == 0){
//             string languagename = "";
//             Var test("NAME", st_con(argv[2]));
//             cout << test.name << endl;
//             cout << endl;
//             cout << "Values: " << endl;
//             for(string value : test.values){
//                 cout << value << endl;
//             }
//             cout << endl;
//             cout << "Dependencies: " << endl;
//             for(int i = 0; i < test.dependencies.size(); i++){
//                 cout << test.values.at(i) << ": " << endl;
//                 for(Var var : test.dependencies[test.values.at(i)]){
//                     cout << var.name << endl;
//                 }
//                 cout << endl;
//             }
//         } else if(strcmp(argv[1],"-of") == 0){
//             vector<string> values;
//             values.push_back("VALUE5");
//             values.push_back("VALUE9");
//             map<string, string> depends;
//             depends["VALUE1"] = "NAME";
//             Var test("DEPENDENT_VARNAME1", values, depends, st_con(argv[2]));
//         } else if(strcmp(argv[1],"-t") == 0){
//             Var::makeTree("format_example_language");
//         }else {
//             cout << "error: unknown argument, write '-h' for help" << endl;
//         }
//     } else {
//         cout << "error: no arguments, write '-h' for help" << endl;
//     }
//     return 0;
// }
