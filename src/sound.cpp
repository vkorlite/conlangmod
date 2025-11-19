#include <iostream>
#include <vector>
#include <var.cpp>
using namespace std;

class Subvalue{
    public:
        string name;
        int value;
        vector<Var> vars;
        Subvalue(string name, int value, Var var){
            this->name = name;
            this->value = value;
            this->var = var;
            //add io write
        }
        Subvalue(string name){
            //add io read
        }
};

class Sound{
    public:
        string symbol;
        vector<Subvalue> subvalues;
        Sound(string symbol, vector<Subvalue> subvalues){
            this->symbol = symbol;
            this->subvalues = subvalues;
            //add io write
        }
        Sound(string symbol){
            this->symbol = symbol;
            //add io read
        }
};
