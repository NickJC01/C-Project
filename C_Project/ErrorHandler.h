#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class ErrorHandler {
private:
    std::vector<std::pair<int, std::string>> errors;

public:
    void addError(int line, const std::string& message);
    void printErrors() const;
    void clearErrors();
    void writeErrorsToFile(const std::string& filename) const;
    bool hasErrors() const { return !errors.empty(); }
    
};

// Declare the global instance
extern ErrorHandler errorHandler;

#endif // ERROR_HANDLER_H
