#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;

/**
 * @dir include/vcb
 * @brief Headers for the vcb component of conlangmod
 * 
 * Contains the header files for the vcb component of conlangmod.
 * vcb stands for vars coms and bombs.
 * vars hold information about objects
 * coms get and process information about objects
 * bombs get and change information about objects
 *
 * @author vkorlite
 * @date 2025-10-25
 */

/**
 * @file var.h
 * @brief Header file for the Var class
 *
 * It is the header file for the Var class, which stores information about properties of an object, keeps track of what each Var is dependent on, and writes it to files.
 * Full information on each Var is stored in vars_raw, which is read and written for cannonical vars.
 * A tree describing all the dependencies is in vars_tree, managed by the tree functions.
 *
 * @author vkorlite
 * @date 2025-10-25
 */

/**
 * @class Var
 * @brief encapsulates information about vars
 *
 * The Var class contains information about vars, and a way to interface with vars_raw and vars_tree.
 * A cannonical (yes it is misspelled, deal with it) Var is one that defines how all vars with that name should work.
 * Unless you are trying to change the vars_raw file or need to know every last bit of information about a var, it's not a good idea to cannonize it.
 * Cannonizing a var involves calling the functions get() or update().
 *
 * @author vkorlite
 * @date 2025-10-25
 */
class Var{
    public:
        string name; /**< name of the var */
        string value; /**< current value of the var*/
        vector<string> values; /**< list of possible values of the var*/
        map<string, vector<Var>> dependencies; /**< map of dependencies of the var, where the key is a possible value and the value is a list of dependent Vars*/
        string language; /**< the language that the var is applicable to*/
        static bool treeWrite; /**< is the tree up to date? */
        bool cannon; /**< is this var cannonical? */
        /**@defgroup constructors Constructors
         *
         * @brief the constructors for the Var class
         *
         * @{
         */
        /**@brief constructor for a new var
         * Constructor for an entirely new var
         * @param parentVarNames_needValues A map where the key is the name of the var that the created one depends on, while the value is the value that the var needs to have in order to beget the var's existance.
         */
        Var(string name, vector<string> values, map<string, string> parentVarNames_needValues, string language); 
        /**@brief constructor for an existing var
         * Constructor for an existing var
         */
        Var(string name, string language);
        /** @} */
        /**@defgroup mutators Mutator functions
         * @brief Mutator functions of the Var class
         *
         * Mutator functions of the Var class, they call this->update() if the var is cannonical.
         * @{
         */
        /**@brief sets the value of the Var object
         * If the value is in this->values, it sets it, otherwise it "throws" it
         */
        void set(string value);
        /**@brief sets the value of the Var object
         * If the value is in this->values, it sets it, otherwise it adds the value to this->values
         */
        void irrSet(string value);
        /**@brief adds a dependency to this->dependencies
         * If the dependency isn't already in this->dependencies, it adds it, otherwise it does nothing
         * @param varName The name of the dependent var
         */
        void addDependency(string value, string varName);
        /**@brief adds dependencies to this->dependencies
         * If the dependencies aren't already in this->dependencies, it adds them, otherwise it does nothing.
         * @param dependencies a vector of the names of the dependencies to be added.
         */
        void addDependencies(map<string, vector<string>> dependencies);
        /**@brief removes a dependency from this->dependencies
         * If the dependency is in this->dependencies, it removes it, otherwise it does nothing.
         * @param varName The name of the dependent var to be removed.
         */
        void removeDependency(string value, string varName);
        /**@brief removes dependencies from this->dependencies
         * If the dependencies are in this->dependencies, it removes them, otherwise it does nothing.
         * @param dependencies A vector of the names of the dependencies to be removed.
         */
        void removeDependencies(map<string, vector<string>> dependencies);
        /**@brief adds a value to this->values
         * If the value isn't already in this->values, it adds it, otherwise it does nothing.
         * It also adds a key accordingly to this->dependencies
         */
        void addValue(string value);
        /**@brief adds values to this->values
         * If the values aren't already in this->values, it adds them, otherwise it does nothing.
         * It also adds keys accordingly to this->dependencies
         * @param values a vector of values to be added
         */
        void addvalues(vector<string> values);
        /**@brief removes a value from this->values
         * If the value is in this->values, it removes it, otherwise it does nothing.
         * It also removes a key accordingly from this->dependencies.
         */
        void removeValue(string value);
        /**@brief removes values from this->values
         * If the values are in this->values, it removes them, otherwise it does nothing.
         * It also removes keys accordingly from this->dependencies.
         * @param values a vector of values to be removed
         */
        void removeValues(vector<string> values);
        /** @} */
        /**@defgroup accessor Accessor functions
         * @brief Accessor functions for the Var class
         * Accessor functions for the Var class. Since all of the properties in the Var class are public, they are wrapper functions for common operations with vars dealing with dependencies
         * @{
         */
        /**@brief gets the value needed to beget the dependent var
         * Gets the value needed to beget the dependent var. It uses recursion so that even if the var is indirectly dependent, it still gives an answer.
         * @param var the dependent Var object 
         * @return value needed to beget the var, or to beget the vars that the var is dependent on. An empty string if it is not dependent.
         */
        string valueDependency(Var var);
        /**@brief gets the value needed to beget the dependent var
         * Gets the value needed to beget the dependent var. It uses recursion so that even if the var is indirectly dependent, it still gives an answer.
         * @param var the dependent var name 
         * @return value needed to beget the var, or to beget the vars that the var is dependent on. An empty string if it is not dependent.
         */
        string valueDependency(string varName);
        /**@brief checks if the var is dependent on this var
         * Checks if the var is dependent on this var. It uses recursion to find indirect dependencies.
         * @param var the dependent Var object
         * @return true if it is dependent false if it isn't
         */
        bool hasDependent(Var var);
        /**@brief checks if the var is dependent on this var
         * Checks if the var is dependent on this var. It uses recursion to find indirect dependencies.
         * @param var the dependent var name
         * @return true if it is dependent false if it isn't
         */
        bool hasDependent(string varName);
        /** @} */
        /** @defgroup file File functions
         * @brief functions to interract with vars_raw
         * Functions that interact with vars_raw. get() and update() make the var cannonical.
         * All of them make treeWrite false;
         * @{
         */
        /**@brief gets the information from vars_raw
         * Gets all the information about a var from vars_raw
         * Makes the var cannonical.
         * @throws st_Exception for file input errors
         */
        void get();
        /**@brief writes the information of this var to vars_raw
         * Writes the information of this var to vars_raw
         * Makes the var cannonical.
         * @throws st_Exception for file input/output errors.
         */
        void update();
        /**@brief removes the var from vars_raw
         * Removes the current var from vars_raw, removing it from the language.
         * @throws st_Exception for file input/output errors.
         */
        void remove();
        /**@}*/
        /**@brief updates the vars_tree file
         * Updates the vars_tree file from the dual vars_raw file.
         * Sets treeWrite to true.
         * @throws st_Exception for file input/output errors.
         */
        static void makeTree(string language);
};
