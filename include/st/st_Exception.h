#pragma once
#include <string>
using namespace std;

/**
 * @file st_Exception.h
 * @brief The header file for the st_Exception class
 *
 * The header file for the st_Exception class, which very dumbly handles errors, by just being a container for a string.
 *
 * @author vkorlite
 * @date 2025-10-25
 */

class st_Exception{
    public:
        string error;
        st_Exception(string error);
};
