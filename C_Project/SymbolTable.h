#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <iostream>

struct SymbolTableEntry {
    std::string identifierName;
    std::string identifierType; // function, procedure, datatype, parameter
    std::string dataType;
    bool isArray;
    int arraySize;
    int scope;
};

class SymbolTable {
public:
    void addEntry(const SymbolTableEntry& entry);
    void addFunctionParameter(const std::string& functionName, const SymbolTableEntry& param);
    void printTable(std::ostream& out = std::cout) const;
    void enterScope();
    void exitScope();
    int getCurrentScopeLevel() const;
    bool isDefinedInCurrentScope(const std::string& name, int current) const;
    bool isDefinedGlobally(const std::string& name) const;
    bool isInParameterList(const std::string& name, int currentScope) const;

    
    private:
    std::vector<SymbolTableEntry> entries;
    std::vector<std::pair<std::string, std::vector<SymbolTableEntry>>> parameterLists;

    int currentScope   = 0;          
    int nextScopeId    = 1;         
    std::vector<int> scopeStack;         
};
#endif
