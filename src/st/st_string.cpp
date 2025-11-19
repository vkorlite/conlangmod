#include <string>
#include <cstring>
#include <cctype>
#include <vector>
#include <utility>
#include "st/st_string.h"
using namespace std;

string st_substr(string input, int startpos, int endpos){
    return input.substr(startpos, endpos-startpos);
}

int st_in(vector<string> input, string match){
    for(int i = 0; i < input.size(); i++){
        if(input.at(i) == match){
            return i;
        }
    }
    return string::npos;
}

int st_find(string input, char delimeter, int num) // get the position of the num'th instance of a search
{
    int temp = num;
    for(int i = 0; i < input.length(); i++){
        if(input.at(i) == delimeter){
            if(temp == 0){
                return i;
            } else {
                temp--;
            }
        }
    }
    return string::npos;
}

int st_find(string input, string delimeter, int num){
    int len = delimeter.length();
    int temp = num;
    for(int i = 0; i + len < input.length(); i++){
        if(input.substr(i, len) == delimeter){
            if(temp == 0){
                return i;
            } else {
                temp--;
            }
        }
    }
    return string::npos;
}
int st_match(string input, char delimeter){
    int output = 0;
    for(int i = 0; i < input.length(); i++){
       if(input.at(i) ==delimeter){
           output++;
       }
    }
    return output;
}

int st_match(string input, string delimeter){
    int output = 0;
    int len = delimeter.length();
    for(int i = 0; i+len < input.length(); i++){
        if(input.substr(i, len) == delimeter){
            output++;
        }
    }
    return output;
}
vector<string> st_split(string input, char delimeter){
    vector<string> output;
    if(st_match(input, delimeter) == 0){
        output.push_back(input);
        return output;
    }
    for(int i = 0; i < st_match(input, delimeter); i++){
        if( i > 0){
            output.push_back(st_substr(input, st_find(input, delimeter, i-1)+1, st_find(input, delimeter, i)));
        } else {
            output.push_back(st_substr(input, 0, st_find(input, delimeter, i)));
        }
    }
    output.push_back(st_substr(input, st_find(input, delimeter, st_match(input, delimeter)-1)+1, input.length()));
    return output;
}

vector<string> st_split(string input, string delimeter){
    vector<string> output;
    if(st_match(input, delimeter) == 0){
        output.push_back(input);
        return output;
    }
    for(int i = 0; i < st_match(input, delimeter); i++){
        if( i > 0){
            output.push_back(st_substr(input, st_find(input, delimeter, i-1)+delimeter.length(), st_find(input, delimeter, i)));
        } else {
            output.push_back(st_substr(input, 0, st_find(input, delimeter, i)));
        }
    }
    output.push_back(st_substr(input, st_find(input, delimeter, st_match(input, delimeter)-1)+1, input.length()));
    return output;
}

string st_toLower(string input){
    string output = "";
    for(int i = 0; i < input.length(); i++){
        output += tolower(input.at(i));
    }
    return output;
}

string st_con(char* input){
    string output = "";
    for(int i = 0; (*(input+i) != '\0'); i++){
        output += *(input+i);
    }
    return output;
}
char* st_con(string input){
    char* output = new char[input.length()];
    for(int i = 0; i < input.length(); i++){
        *(output+i)=input.at(i);
    }
    *(output+input.length())='\0';
    return output;
}

int st_compliment(string input, int pos){
    char br_open = '\0';
    char br_close = '\0';
    switch(input.at(pos)){
        case '[':
            br_open = '[';
            br_close = ']';
            break;
        case '(':
            br_open = '(';
            br_close = ')';
            break;
        case '{':
            br_open = '{';
            br_close = '}';
            break;
        default:
            return input.length();
    }
    int off = 0;
    for(int i = pos+1; i < input.length(); i++){
        if(input.at(i) == br_close){
            if(off == 0){
                return i;
            }
            off--;
        } else if(input.at(i) == br_open){
            off++;
        }
    }
    return string::npos;
}

pair<vector<string>, vector<char>> st_multiSplit(string input, vector<char> delimeters){
    pair<vector<string>, vector<char>> output;
    int splitArrLen = 0;
    for(char delimeter : delimeters){
        splitArrLen += st_match(input, delimeter);
    }
    if(splitArrLen == 0){
        output.first = {input};
        output.second = {'\0'};
        return output;
    }
    vector<string> outputSplit;
    vector<char> outputOrder;
    int off = 0;
    for(int i = 0; i < input.length(); i++){
        for(int j = 0; j < delimeters.size(); j++){
            if(input.at(i) == delimeters.at(j)){
                outputSplit.push_back(st_substr(input, off, i));
                outputOrder.push_back(delimeters.at(j));
                off = i+1;
            }
        }
    }
    outputSplit.push_back(st_substr(input, off, input.length()));
    output.first = outputSplit;
    output.second = outputOrder;
    return output;
}

pair<vector<string>, vector<string>> st_multiSplit(string input, vector<string> delimeters){
    pair<vector<string>, vector<string>> output;
    vector<string> outputSplit;
    vector<string> outputOrder;
    int splitArrLen = 0;
    for(string delimeter : delimeters){
        splitArrLen += st_match(input, delimeter);
    }
    if(splitArrLen == 0){
        outputSplit.push_back(input);
        output.first = outputSplit;
        output.second = outputOrder;
        return output;
    }
    int off = 0;
    for(int i = 0; i < input.length(); i++){
        for(int j = 0; j < delimeters.size(); j++){
            if(delimeters.at(j).length()+i < input.length()){
                if(input.substr(i, delimeters.at(j).length()) == delimeters.at(j)){
                    outputSplit.push_back(st_substr(input, off, i));
                    outputOrder.push_back(delimeters.at(j));
                    off = i+delimeters.at(j).length();
                }
            }
        }
    }
    outputSplit.push_back(st_substr(input, off, input.length()));
    output.first = outputSplit;
    output.second = outputOrder;
    return output;
}

string st_remove(string input, char remover){
    string output = "";
    for(int i = 0; i< input.length(); i++){
        if(input.at(i) != remover){
            output += input.at(i);
        }
    }
    return output;
}

string st_remove(string input, string remover){
    string output = "";
    for(int i = 0; i < input.length(); i++){
        if(input.substr(i, remover.length()) == remover){
            i+= remover.length();
        } else {
            output += input.at(i);
        }
    }
    return output;
}
