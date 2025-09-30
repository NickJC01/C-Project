#ifndef PARSER_H
#define PARSER_H

#include "TokenStream.h"
#include "CSTNode.h"
#include "Tokenizer.h"
#include "ErrorHandler.h"
#include "SymbolTable.h"

class Parser {
private:
    TokenStream& tokenStream;
    ErrorHandler& errorHandler;
    SymbolTable symbolTable; 

    CSTNode* parseProcedure(const std::string& nodeType, const Token& returnTypeToken);
    CSTNode* parseStatement();
    CSTNode* parseExpression(CSTNode* leftHandSide = nullptr, bool stopAtParen = false);
    CSTNode* parseBooleanExpression();
    CSTNode* parseTerm();
    CSTNode* parseFactor();
    CSTNode* parseDeclaration();
    CSTNode* parseAssignment();

    void reportError(const std::string& message, int lineNumber);

public:
    Parser(TokenStream& tokenStream, ErrorHandler& errorHandler);
    CSTNode* parseProgram();
    SymbolTable& getSymbolTable() { return symbolTable; }

};

#endif
