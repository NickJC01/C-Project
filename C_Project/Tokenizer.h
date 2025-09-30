#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include "ErrorHandler.h"

const std::unordered_set<std::string> keywords = {
    "if", "else", "while", "procedure", "function", "return",
    "int", "bool", "true", "false", "for", "char", "void"
};

enum TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_FUNCTION, 
    TOKEN_PROCEDURE, 
    TOKEN_KEYWORD, 
    TOKEN_TYPE, 
    TOKEN_INTEGER,
    TOKEN_L_PAREN,
    TOKEN_R_PAREN,
    TOKEN_L_BRACKET,
    TOKEN_R_BRACKET,
    TOKEN_L_BRACE,
    TOKEN_R_BRACE,
    TOKEN_DOUBLE_QUOTE,
    TOKEN_SINGLE_QUOTE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ASSIGNMENT_OPERATOR,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_CARET,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LT_EQUAL,
    TOKEN_GT_EQUAL,
    TOKEN_BOOLEAN_AND,
    TOKEN_LOGICAL_OR,
    TOKEN_BOOLEAN_OR,
    TOKEN_BOOLEAN_NOT,
    TOKEN_BOOLEAN_EQUAL,
    TOKEN_BOOLEAN_NOT_EQUAL,
    TOKEN_BOOLEAN_TRUE,
    TOKEN_BOOLEAN_FALSE,
    TOKEN_CHAR_LITERAL,
    TOKEN_HEX_LITERAL,
    TOKEN_BACKSLASH,
    TOKEN_STRING,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int lineNumber;

    Token() : type(TOKEN_UNKNOWN), value(""), lineNumber(-1) {} 

    Token(TokenType t, const std::string& v, int ln)
        : type(t), value(v), lineNumber(ln) {}
};


class Tokenizer {
public:
    Tokenizer(const std::string& filename, const std::string& outputFile, int startLine);
    void tokenize();
    void printTokens() const;
    std::vector<Token> getTokens() const { return tokens; }
    
private:
    std::ifstream inputFile;
    std::string outputFilename;
    std::vector<Token> tokens;
    std::unordered_set<std::string> declaredIdentifiers; // Track declared variables instead of just giving integer syntax error

    
    void processUnknown(char c);
    void addToken(TokenType type, const std::string& value, int lineNumber);
    void skipWhitespace();
    void processIdentifierOrKeyword(char firstChar);
    void processNumber(char firstChar);
    void processOperator(char firstChar);
    void processPunctuation(char firstChar);
    void processStringLiteral(char delimiter);
    void processCharLiteral();
    int lineNumber;  // Line number counter for the tokenizer

    ErrorHandler errorHandler;
};


#endif // TOKENIZER_H
