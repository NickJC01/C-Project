#include "Tokenizer.h"
#include <cctype>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include "ErrorHandler.h"  


Tokenizer::Tokenizer(const std::string& filename, const std::string& outputFile, int startLine)
    : outputFilename(outputFile), lineNumber(startLine) {
    inputFile.open(filename);
    if (!inputFile) {
        errorHandler.addError(0, "Unable to open file: " + filename);
        return;
    }
    declaredIdentifiers.clear();
}

void Tokenizer::addToken(TokenType type, const std::string& value, int tokenLine) {
    Token token(type, value, tokenLine);  // properly initialize with constructor
    tokens.push_back(token);
}


void Tokenizer::skipWhitespace() {
    char c;
    while (inputFile.get(c)) {
        if (c == '\n') {
            lineNumber++;  // count all newline characters
        }
        if (!std::isspace(c)) {
            inputFile.putback(c);
            return;
        }
    }
}


void Tokenizer::processIdentifierOrKeyword(char firstChar) {
    std::string value(1, firstChar);
    char c;

    while (inputFile.get(c) && (std::isalnum(c) || c == '_')) {
        value += c;
    }
    inputFile.putback(c);

    //  reject identifiers that start with a digit and aren't declared
    if (std::isdigit(value[0]) && declaredIdentifiers.find(value) == declaredIdentifiers.end()) {
        errorHandler.addError(lineNumber, "Syntax error: invalid identifier '" + value + "'");
        std::remove(outputFilename.c_str());  

        inputFile.close();
        std::remove(outputFilename.c_str()); //  delete the incomplete output file
        return; // stop processing
    }

    //  handle keywords and identifiers properly
    if (value == "true") {
        addToken(TOKEN_BOOLEAN_TRUE, value, lineNumber);
    } 
    else if (value == "false") {
        addToken(TOKEN_BOOLEAN_FALSE, value, lineNumber);
    } 
    else if (value == "procedure") {
        addToken(TOKEN_PROCEDURE, value, lineNumber);
    } 
    else if (value == "function") {  // recognize 'function' as a keyword
        addToken(TOKEN_FUNCTION, value, lineNumber);  
    }
    
    else if (value == "int" || value == "bool" || value == "char" || 
             value == "float" || value == "double" || value == "void") {
        addToken(TOKEN_TYPE, value, lineNumber);
    }
    else if (value == "string") {
        // treat "string" as an identifier unless you've defined it as a built-in type
        addToken(TOKEN_IDENTIFIER, value, lineNumber);
    }
    else if (keywords.find(value) != keywords.end()) {
        addToken(TOKEN_KEYWORD, value, lineNumber);
    } 
    else {
        addToken(TOKEN_IDENTIFIER, value, lineNumber);
        declaredIdentifiers.insert(value); //  store this as a declared variable
    }
}

void Tokenizer::processNumber(char firstChar) {
    std::string value(1, firstChar);
    char c;
    bool invalid = false;
    int tokenLine = lineNumber;  // capture the correct line number for error reporting

    while (inputFile.get(c)) {
        if (std::isdigit(c)) {
            value += c;
        } else if (std::isalpha(c)) {  //  found a letter inside a number
            invalid = true;
            value += c;
        } else {
            inputFile.putback(c);
            break;
        }
    }

    if (invalid) {
        errorHandler.addError(tokenLine, "Syntax error: invalid integer '" + value + "'");
        tokens.clear();
        inputFile.close();
        std::remove(outputFilename.c_str()); //  delete the incomplete output file
        return; // stop processing
    }

    addToken(TOKEN_INTEGER, value, lineNumber);
}

void Tokenizer::processOperator(char firstChar) {
    std::string value(1, firstChar);
    char c;
    if (inputFile.get(c)) {
        std::string potentialOp = value + c;
        if (potentialOp == "==") { addToken(TOKEN_BOOLEAN_EQUAL, potentialOp, lineNumber); return; }
        if (potentialOp == "!=") { addToken(TOKEN_BOOLEAN_NOT_EQUAL, potentialOp, lineNumber); return; }
        if (potentialOp == "&&") { addToken(TOKEN_BOOLEAN_AND, potentialOp, lineNumber); return; }
        if (potentialOp == "||") { addToken(TOKEN_BOOLEAN_OR, potentialOp, lineNumber); return; }
        if (potentialOp == ">=") { addToken(TOKEN_GT_EQUAL, potentialOp, lineNumber); return; }
        if (potentialOp == "<=") { addToken(TOKEN_LT_EQUAL, potentialOp, lineNumber); return; }
        inputFile.putback(c);
    }
    
    switch (firstChar) {
        case '=': addToken(TOKEN_ASSIGNMENT_OPERATOR, "=", lineNumber); break;
        case '+': addToken(TOKEN_PLUS, "+", lineNumber); break;
        case '-': addToken(TOKEN_MINUS, "-", lineNumber); break;
        case '*': addToken(TOKEN_ASTERISK, "*", lineNumber); break;
        case '/': addToken(TOKEN_DIVIDE, "/", lineNumber); break;
        case '%': addToken(TOKEN_MODULO, "%", lineNumber); break;
        case '<': addToken(TOKEN_LT, "<", lineNumber); break;
        case '>': addToken(TOKEN_GT, ">", lineNumber); break;
        case '!': addToken(TOKEN_BOOLEAN_NOT, "!", lineNumber); break;
        default: addToken(TOKEN_UNKNOWN, std::string(1, firstChar), lineNumber); break;
    }
}

void Tokenizer::processPunctuation(char firstChar) {
    switch (firstChar) {
        case '(': addToken(TOKEN_L_PAREN, "(", lineNumber); break;
        case ')': addToken(TOKEN_R_PAREN, ")", lineNumber); break;
        case '{': addToken(TOKEN_L_BRACE, "{", lineNumber); break;
        case '}': addToken(TOKEN_R_BRACE, "}", lineNumber); break;
        case '[': addToken(TOKEN_L_BRACKET, "[", lineNumber); break;
        case ']': addToken(TOKEN_R_BRACKET, "]", lineNumber); break;
        case ';': addToken(TOKEN_SEMICOLON, ";", lineNumber); break;
        case ',': addToken(TOKEN_COMMA, ",", lineNumber); break;
        default: addToken(TOKEN_UNKNOWN, std::string(1, firstChar), lineNumber); break;
    }
}

void Tokenizer::processStringLiteral(char delimiter) {
    std::string value(1, delimiter);  // start with the delimiter (either '"' or '\'')
    char c;
    bool unterminated = true;
    int tokenLine = lineNumber;

    if (delimiter == '\'') {  // handling character literal case
        if (inputFile.get(c)) {
            value += c;  // add the character itself

            if (inputFile.get(c) && c == '\'') {  // check for closing single quote
                value += c;
                unterminated = false;
                addToken(TOKEN_CHAR_LITERAL, value, tokenLine);
                return;
            }
        }
        errorHandler.addError(tokenLine, "Syntax error: unterminated character literal.");
        return;
    }

    while (inputFile.get(c)) {
        if (c == '\\') {  // if we encounter a backslash, handle escape sequences
            if (inputFile.get(c)) {  // get the next character
                value += '\\';  // add the backslash to the string

                if (c == 'x') {  // hexadecimal escape sequence
                    std::string hexValue(1, c);
                    char nextChar;

                    while (inputFile.get(nextChar) && std::isxdigit(nextChar)) {
                        hexValue += nextChar;
                    }

                    if (!hexValue.empty()) {
                        value += hexValue;  // add the hex sequence to the string
                        inputFile.putback(nextChar);  // return the non-hex character
                    }
                } 
                else {  // regular escape characters
                    value += c;  // just add the character to the string
                }
            }
        } 
        else if (c == delimiter) {
            // check if the previous character was a backslash — i.e., escaped quote
            if (!value.empty() && value.back() == '\\') {
                value += c;  // treat as escaped quote — don't end the string
            } else {
                value += c;  // real (unescaped) quote ends the string
                unterminated = false;
                break;
            }
        }
        
        else {
            value += c;
        }
    }

    if (unterminated) {
        errorHandler.addError(tokenLine, "Syntax error: unterminated string literal starting here.");
        inputFile.close();
        std::remove(outputFilename.c_str()); // delete the incomplete output file
        return;
    }

    addToken(TOKEN_STRING, value, tokenLine);  // store the entire string as a single token
}

void Tokenizer::processCharLiteral() {
    std::string value = "'";          // keep the opening quote
    int tokenLine = lineNumber;
    char c;
    bool escape = false;

    while (inputFile.get(c)) {
        value += c;

        if (escape) {                 // we’re in an escape sequence
            if (c == 'x') {           // hex escape: read hex digits
                char h;
                while (inputFile.get(h) && std::isxdigit(h)) {
                    value += h;
                }
                inputFile.putback(h);
            }
            escape = false;           // escape sequence finished
            continue;
        }

        if (c == '\\') {              // start of an escape sequence
            escape = true;
            continue;
        }

        if (c == '\'') {              // found the real closing quote
            addToken(TOKEN_CHAR_LITERAL, value, tokenLine);
            return;
        }

        if (c == '\n') {              // newline before closing quote
            errorHandler.addError(tokenLine,
                "Syntax error: unterminated character literal.");
            return;
        }
    }

    // fell off the end of file without a closing quote
    errorHandler.addError(tokenLine,
        "Syntax error: unterminated character literal at end of file.");
}



void Tokenizer::tokenize() {
    tokens.clear();  // ensure fresh token list

    char c;
    while (inputFile.get(c)) {
        if (std::isspace(c)) {
            if (c == '\n') {
                lineNumber++;  // count newline ONCE
            }
            skipWhitespace();
            continue;
        }
    

        if (std::isalpha(c) || c == '_') {
            processIdentifierOrKeyword(c);
            continue;
        } else if (std::isdigit(c)) {
            processNumber(c);
            continue;
        } else if (c == '+' || c == '-' || c == '=' || c == '<' || c == '>' || 
                   c == '!' || c == '&' || c == '|' || c == '*' || c == '/' || c == '%') {
            if (c == '/') { 
                char nextChar;
                if (inputFile.get(nextChar)) {
                    if (nextChar == '/' || nextChar == '*') {
                        inputFile.putback(nextChar);
                        continue;
                    } else {
                        inputFile.putback(nextChar);
                    }
                }
            }
            processOperator(c);
            continue;
        } else if (c == '(' || c == ')' || c == '{' || c == '}' || 
                   c == ';' || c == ',' || c == '[' || c == ']') {
            processPunctuation(c);
            continue;
        } else if (c == '"') {
            processStringLiteral(c);
            continue;
        } else if (c == '\'') {
            processCharLiteral();
            continue;
        } else {
            processUnknown(c);
        }
    }

    // ensure errors are logged to file
    if (errorHandler.hasErrors()) {
        errorHandler.printErrors();
        
    }
}

void Tokenizer::processUnknown(char c) {
    std::string value(1, c);  // convert character to string
    errorHandler.addError(lineNumber, "Unknown token encountered: '" + value + "'");
}


void Tokenizer::printTokens() const {
    for (const auto& token : tokens) {
        std::cout << "Token type: " << token.type << "\n";
        std::cout << "Token: " << token.value << "\n";
        std::cout << "Line number: " << token.lineNumber << "\n";
    }
}
