#include <string>
#include "st/st_Exception.h"
using namespace std;

st_Exception::st_Exception(string error){
    this->error = error;
}
