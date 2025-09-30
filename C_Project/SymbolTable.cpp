#include "SymbolTable.h"
#include <iostream>
#include <iomanip>

void SymbolTable::addEntry(const SymbolTableEntry& entry)
{
    //case:same‑scope duplicates
    if (isDefinedInCurrentScope(entry.identifierName, entry.scope)) {
        throw std::runtime_error("variable \"" + entry.identifierName +
                                 "\" is already defined locally");
    }

    ///case:duplicates a parameter of this procedure 
    if (isInParameterList(entry.identifierName, entry.scope)) {
        throw std::runtime_error("variable \"" + entry.identifierName +
                                 "\" is already defined locally");
    }

    //case:shadowing a global name
    if (entry.scope != 0 && isDefinedGlobally(entry.identifierName)) {
        throw std::runtime_error("variable \"" + entry.identifierName +
                                 "\" is already defined globally");
    }

    entries.push_back(entry);
}

void SymbolTable::addFunctionParameter(const std::string& functionName, const SymbolTableEntry& param) {
    for (auto& pair : parameterLists) {
        if (pair.first == functionName) {
            pair.second.push_back(param);
            return;
        }
    }
    parameterLists.push_back({functionName, {param}});
}

void SymbolTable::printTable(std::ostream& out) const {
    for (const auto& entry : entries) {
        out << "IDENTIFIER_NAME: " << entry.identifierName << "\n";
        out << "IDENTIFIER_TYPE: " << entry.identifierType << "\n";
        out << "DATATYPE: " << entry.dataType << "\n";
        out << "DATATYPE_IS_ARRAY: " << (entry.isArray ? "yes" : "no") << "\n";
        out << "DATATYPE_ARRAY_SIZE: " << entry.arraySize << "\n";
        out << "SCOPE: " << entry.scope << "\n" << "\n";
    }

    for (const auto& paramList : parameterLists) {
        out << "PARAMETER LIST FOR: " << paramList.first << "\n";
        for (const auto& param : paramList.second) {
            out << "IDENTIFIER_NAME: " << param.identifierName << "\n";
            out << "DATATYPE: " << param.dataType << "\n";
            out << "DATATYPE_IS_ARRAY: " << (param.isArray ? "yes" : "no") << "\n";
            out << "DATATYPE_ARRAY_SIZE: " << param.arraySize << "\n";
            out << "SCOPE: " << param.scope << "\n" << "\n";
        }
    }
}

void SymbolTable::enterScope()
{
    scopeStack.push_back(currentScope);   // remember where we came from
    currentScope = nextScopeId++;         // assign a unique number
}

void SymbolTable::exitScope()
{
    if (!scopeStack.empty()) {
        currentScope = scopeStack.back(); // restore caller’s scope
        scopeStack.pop_back();
    }
 
}

int SymbolTable::getCurrentScopeLevel() const {
    return currentScope;
}

bool SymbolTable::isDefinedInCurrentScope(const std::string& name,
    int current) const
{
for (const auto& e : entries) {
if (e.identifierName == name && e.scope == current) {
return true;            // duplicate in the same block
}
}
return false;
}

bool SymbolTable::isDefinedGlobally(const std::string& name) const
{
for (const auto& e : entries) {
if (e.identifierName == name && e.scope == 0) {
return true;            // already a global symbol
}
}
return false;
}

bool SymbolTable::isInParameterList(const std::string& name, int currentScope) const
{
    //  which procedure/function owns currentScope  
    std::string owner; // procedure/function name
    for (const auto& e : entries) {
        if ((e.identifierType == "procedure" || e.identifierType == "function") &&
            e.scope == currentScope) {
            owner = e.identifierName;
            break;
        }
    }
    if (owner.empty()) return false;

    // search that parameter list
    for (const auto& pair : parameterLists) {
        if (pair.first == owner) {
            for (const auto& p : pair.second) {
                if (p.identifierName == name) return true;
            }
        }
    }
    return false;
}