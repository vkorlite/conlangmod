#pragma once
#include <string>
#include <utility>
#include <vector>

/**
 * @dir include/st
 * @brief Headers for utility functions and classes 
 *
 * Contains the headers for utility functions and classes like st_string and st_Exception
 *
 * All st_* components in conlangmod stand for standard, pronounced: saint.
 *
 * @author vkorlite
 * @date 2025-10-25
*/
/**
 * @file st_string.h
 * @brief header file for st_string functions
 *
 * The header file for st_string functions, which are utilities for strings that either don't exist in the c++ standard library or are not as easy to use.
 * 
 * @author vkorlite
 * @date 2025-10-25
 */

/**
 * @brief substring from startpos to endpos, instead of startpos and length
 *
 * A simple function that makes it easier to get substrings of strings.
 * @param startpos The position of the first char in the substring (inclusive)
 * @param endpos The position of the last char in the substring (exclusive)
 */
std::string st_substr(std::string input, int startpos, int endpos);
/**
 * @brief finds the index of a string in a vector
 *
 * The function finds at what index does the string first show up.
 */
int st_in(std::vector<std::string> input, std::string match);
/**
 * @brief finds the n'th occurance of a char delimeter 
 *
 * The function finds the n'th occurance of a char delimeter in a string. 
 * @param num Array numbers, i.e. num=0 -> 1st occurance.
 */
int st_find(std::string input, char delimeter, int num);
/**
 * @brief finds the n'th occurance of a string delimeter
 *
 * the function finds the n'th occurance of a string delimeter in a string.
 * If the delimeter is one character long, turn it into a char, as the other st_find is more efficient.
 * @param num Array number. i.e. num=0 -> 1st occurance.
 */
int st_find(std::string input, std::string delimeter, int num);
/**
 * @brief finds how many times a char appears in a string
 *
 * the function finds how many times a char delimeter appears in a given string.
 */
int st_match(std::string input, char delimeter);
/**
 * @brief finds how many times a substring appears in a string
 *
 * the function finds how many times a string delimeter appears in a string.
 */
int st_match(std::string input, std::string delimeter);
/**
 * @brief splits a string by char delimeter
 * 
 * The function splits a string by a delimeter (exclusive)
 *
 * @return vector<string>
 */
std::vector<std::string> st_split(std::string input, char delimeter);
/**
 * @brief splits string by string delimeter
 *
 * the function splits a string by a string delimeter (exclusive)
 *
 * @return vector<string>
 */
std::vector<std::string> st_split(std::string input, std::string delimeter);
/**
 * @brief lowers all chars in a string
 *
 * the function lowers every char in a string
 */
std::string st_toLower(std::string input);
/**
 * @brief converts char* to string
 *
 * the function converts char* to string
 *
 * @param input char*
 * @return string
 */
std::string st_con(char* input);
/**
 * @brief converts string to char*
 *
 * the function converts string to char*
 *
 * @param input string
 * @return char*
 */
char* st_con(std::string input);
/**
 * @brief finds the complimentary bracket of a given brackets in a string
 *
 * The function finds a complimentary bracket of a given brackets in a string.
 * The function takes all kinds of open brackets: (, [, {
 * @param pos which bracket in the string to find the compliment for. if pos = -1 then it finds the first open parentheses
 */
int st_compliment(std::string input, int pos);
/**
 * @brief splits the string by multiple delimeters
 *
 * The function splits the string by a vector of char delimeters.
 * @return pair<vector<string>, vector<char>> the first is the list of all the strings, second is an ordered list of which delimeter was in that place
 */
std::pair<std::vector<std::string>, std::vector<char>> st_multiSplit(std::string input, std::vector<char> delimeters);
/**
 * @brief splits the string by multiple delimeters
 * 
 * The function splits the string by a vector of string delimeters
 * @return pair<vector<string>, vector<char>> the first is the list of all the split strings, second is an ordered list of which delimeter was in that place.
 */
std::pair<std::vector<std::string>, std::vector<std::string>> st_multiSplit(std::string input, std::vector<std::string> delimeters);
/**
 * @brief removes all instances of a certain char in a string
 *
 * The function removes all instances of a char remover in a string
 */
std::string st_remove(std::string input, char remover);
/**
 * @brief removes all instances of a certain substring in a string
 *
 * The function removes all instances of a substring in a string
 */
std::string st_remove(std::string input, std::string remover);

