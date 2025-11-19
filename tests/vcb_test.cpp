#include <iostream>
#include <string>
#include <vector>
#include "vcb/var.h"
#include "vcb/st_Object.h"
#include "vcb/com.h"

int main(int argc, char* argv[]){
    string comstr = "{(#0.NUM > 0), (#0.DEPENDENT_VARNAME1 = VALUE5) & ((#1.NUM) > (#0.NUM))}{all}{(#0.NUM < #1.NUM)}";
    Com com = Com(comstr, "format_example_language");
    Var name =  Var("DEPENDENT2_VAR1", "format_example_language");
    Var name2 = Var("NAME", "format_example_language");
    Var arpr =  Var("NUM", "format_example_language");
    Var arpr2 = Var("NUM", "format_example_language");
    Var arpr3 = Var("NUM", "format_example_language");
    arpr.irrSet("2");
    arpr2.irrSet("3");
    arpr3.irrSet("5");
    name.irrSet("VALUE1");
    name2.irrSet("VALUE2");
    vector<Var> properties;
    properties.push_back(name);
    properties.push_back(arpr);
    vector<Var> properties2;
    name.addValue("VALUE1");
    name2.addValue("VALUE2");
    arpr.addValue("2");
    arpr2.addValue("3");
    properties2.push_back(name2);
    properties2.push_back(arpr2);
    vector<Var> properties3;
    properties3.push_back(name);
    properties3.push_back(arpr3);
    st_Object obj = st_Object("format_example_language", properties);
    st_Object obj2 = st_Object("format_example_language", properties2);
    st_Object obj3 = st_Object("format_example_language", properties3);
    obj.get();
    obj2.get();
    obj3.get();
    cout << "listing props for obj" << endl;
    vector<Var> objproperties = obj.treeTraversal();
    for(Var prop : objproperties){
        cout << prop.name << ": " << prop.value << endl;
    }
    cout << "end" << endl;
    cout << "listing props for obj2" << endl;
    vector<Var> obj2properties = obj2.treeTraversal();
    for(Var prop : obj2properties){
        cout << prop.name << ": " << prop.value << endl;
    }
    cout << "end" << endl;
    vector<st_Object> objects;
    objects.push_back(obj);
    objects.push_back(obj2);

    cout << com.comstr << endl;

    vector<vector<st_Object>> domains = com.domain.get(objects);
    for(vector<st_Object> domain : domains){
        for(int i = 0; i < domain.size(); i++){
            cout << domain.at(i).getPropertyValue("NAME");
            if(i < domain.size()-1){
                cout << " : ";
            } else {
                cout << endl;
            }
        }
    }

    bool* ans = static_cast<bool*>(com.get(objects));
    if(*ans){
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    // vector<vector<st_Object>> domain = dom.get(objects);
    // for(vector<st_Object> list : domain){
    //     for(int i = 0; i < list.size(); i++){
    //         cout << list.at(i).getPropertyValue("NAME");
    //         if( i < list.size()-1){
    //             cout << " : ";
    //         } else {
    //             cout << endl;
    //         }
    //     }
    //     cout << "CHECKING subcond 0: " ;
    //     bool* ans = static_cast<bool*>(dom.subconditions.at(0).check(objects));
    //     if(*ans){
    //         cout << "true" << endl;
    //     } else {
    //         cout << "false" << endl;
    //     }
    //     cout << "CHECKING subcond 1: " ;
    //     ans = static_cast<bool*>(dom.subconditions.at(1).check(objects));
    //     if(*ans){
    //         cout << "true" << endl;
    //     } else {
    //         cout << "false" << endl;
    //     }
    // }
    return 0;
}
