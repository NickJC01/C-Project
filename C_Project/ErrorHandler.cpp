#include "ErrorHandler.h"

// Define the global instance
ErrorHandler errorHandler;

void ErrorHandler::addError(int line, const std::string& message) {
    errors.push_back({line, message});
}

void ErrorHandler::printErrors() const {
    if (errors.empty()) return;

    std::cerr << "Errors encountered:\n";

    for (const auto& error : errors) {
        std::cerr << "Line " << error.first << ": " << error.second << "\n";
    }

    writeErrorsToFile("errors.txt");  // Ensure errors get logged to file
}

void ErrorHandler::writeErrorsToFile(const std::string& filename) const {
    if (errors.empty()) {
        return;
    }

    std::ofstream errorFile(filename, std::ios::app); // Append errors
    if (!errorFile) {
        std::cerr << "[ERROR] Unable to open error log file: " << filename << "\n";
        return; 
    }


    for (const auto& error : errors) {
        errorFile << "Line " << error.first << ": " << error.second << "\n";
    }

    errorFile.close();
}

void ErrorHandler::clearErrors() {
    errors.clear();
}

