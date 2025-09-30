#include "CommentRemover.h"
#include "Tokenizer.h"
#include "ErrorHandler.h"
#include "TokenStream.h"
#include "Parser.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include "SymbolTable.h"

namespace fs = std::filesystem;
extern ErrorHandler errorHandler;

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TOKEN_PROCEDURE: return "PROCEDURE";
        case TOKEN_IDENTIFIER: return "IDENTIFIER"; // covers function/variable names
        case TOKEN_KEYWORD: return "KEYWORD"; // covers reserved words like for, if, while
        case TOKEN_TYPE: return "TYPE";  // now covers int, boolean, etc.
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_L_PAREN: return "L_PAREN";
        case TOKEN_R_PAREN: return "R_PAREN";
        case TOKEN_L_BRACKET: return "L_BRACKET";
        case TOKEN_R_BRACKET: return "R_BRACKET";
        case TOKEN_L_BRACE: return "L_BRACE";
        case TOKEN_R_BRACE: return "R_BRACE";
        case TOKEN_DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        case TOKEN_SINGLE_QUOTE: return "SINGLE_QUOTE";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_ASTERISK: return "ASTERISK";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_CARET: return "CARET";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_LT_EQUAL: return "LT_EQUAL";
        case TOKEN_GT_EQUAL: return "GT_EQUAL";
        case TOKEN_BOOLEAN_AND: return "BOOLEAN_AND";
        case TOKEN_BOOLEAN_OR: return "BOOLEAN_OR";
        case TOKEN_BOOLEAN_NOT: return "BOOLEAN_NOT";
        case TOKEN_BOOLEAN_EQUAL: return "BOOLEAN_EQUAL";
        case TOKEN_BOOLEAN_NOT_EQUAL: return "BOOLEAN_NOT_EQUAL";
        case TOKEN_BOOLEAN_TRUE: return "BOOLEAN_TRUE";
        case TOKEN_BOOLEAN_FALSE: return "BOOLEAN_FALSE";
        case TOKEN_STRING: return "STRING";
        default: return "UNKNOWN";
    }
}

void printCST(CSTNode* node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth * 4; ++i) std::cout << " "; 

    if (node->name == "Symbol") {
        std::cout << '"' << node->value << '"' << std::endl;  // print symbol nodes directly (e.g., "(", ")", "{", "}")
    } else {
        std::cout << node->name << " (" << node->value << ") [Line: " << node->lineNumber << "]" << std::endl;
    }

    if (node->leftChild) printCST(node->leftChild, depth + 1);
    if (node->rightSibling) printCST(node->rightSibling, depth);
}

void writeCSTToFile(CSTNode* node, std::ofstream& out, int depth = 0) {
    if (!node) return;

    for (int i = 0; i < depth * 4; ++i) out << " ";

    if (node->name == "Symbol") {
        out << '"' << node->value << '"' << std::endl;
    } else {
        out << node->name << " (" << node->value << ") [Line: " << node->lineNumber << "]" << std::endl;
    }

    if (node->leftChild) writeCSTToFile(node->leftChild, out, depth + 1);
    if (node->rightSibling) writeCSTToFile(node->rightSibling, out, depth);
}

int main() {
    
    std::string testDirectory = "testfiles/TestFiles4";
    std::string outputDirectory = "outputfiles";

    if (!fs::exists(testDirectory) || !fs::is_directory(testDirectory)) {
        std::cerr << "Test directory not found: " << testDirectory << std::endl;
        return 1;
    }

    if (!fs::exists(outputDirectory)) {
        fs::create_directory(outputDirectory);
    }

    CommentRemover remover;

    for (const auto& entry : fs::directory_iterator(testDirectory)) {
        if (entry.is_regular_file()) {
            std::string inputFilePath = entry.path().string();
            std::string outputFilePath = outputDirectory + "/" + entry.path().filename().string();
            std::string tokenOutputFile = outputDirectory + "/tokens_" + entry.path().filename().string();

            //std::cout << "Processing: " << inputFilePath << " -> " << outputFilePath << std::endl;

            int finalLineNumber = 1;
            remover.removeComments(inputFilePath, outputFilePath);

            if (errorHandler.hasErrors()) {
                errorHandler.printErrors();
                errorHandler.writeErrorsToFile("errors.txt");
                errorHandler.clearErrors();
                //std::cerr << "Skipping " << inputFilePath << " due to errors.\n\n";
                continue;
            }

            Tokenizer tokenizer(outputFilePath, outputFilePath, finalLineNumber);
            tokenizer.tokenize();

            if (errorHandler.hasErrors()) {
                errorHandler.printErrors();
                errorHandler.writeErrorsToFile("errors.txt");
            
                // remove the partially created file if it exists
                std::remove(outputFilePath.c_str());   
                std::remove(tokenOutputFile.c_str());
            
                //std::cerr << "Skipping " << inputFilePath << " due to errors.\n\n";
            
                errorHandler.clearErrors(); // Clear errors here so the next file starts clean
                continue;
            }
            
            
            // now we proceed to create the token output file only if no errors were detected
            const auto& tokens = tokenizer.getTokens();

            if (tokens.empty()) { 
                //std::cerr << "Skipping " << inputFilePath << " due to empty token list.\n\n";
                errorHandler.addError(0, "Syntax Error: Token list generation failed. See terminal or error log.");
                errorHandler.writeErrorsToFile("errors.txt");
                errorHandler.clearErrors();
                continue;  // move to the next file without creating the token file
            }
            
            if (!errorHandler.hasErrors()) {  // Only create a token file if there are no errors
                std::ofstream tokenFile(tokenOutputFile);
                if (!tokenFile) {
                    //std::cerr << "error: Unable to create token output file " << tokenOutputFile << std::endl;
                    continue;
                }
            
                tokenFile << "Token list:\n\n";
                for (const auto& token : tokens) {
                    tokenFile << "Token type: " << tokenTypeToString(token.type) << "\n";
                    tokenFile << "Token: " << token.value << "\n\n";
                }
            
                tokenFile.close();
                //std::cout << "tokens saved to: " << tokenOutputFile << std::endl;
            }
            
            errorHandler.clearErrors();
            
            // initialize TokenStream and Parser
            TokenStream tokenStream(tokens);
            Parser parser(tokenStream, errorHandler);

            CSTNode* cstRoot = parser.parseProgram();

            if (errorHandler.hasErrors()) {
                errorHandler.writeErrorsToFile("errors.txt");
                errorHandler.printErrors();
                std::remove(outputFilePath.c_str());
                std::remove(tokenOutputFile.c_str());
            
                std::string cstOutputFile = outputDirectory + "/cst_" + entry.path().filename().string();
                std::string symbolOutputFile = outputDirectory + "/symboltable_"  + entry.path().filename().string(); 
                std::remove(cstOutputFile.c_str());
                std::remove(symbolOutputFile.c_str());             

                delete cstRoot;  // just in case
                errorHandler.clearErrors();
                continue;  // skip rest of file
            }
            
            if (cstRoot) {
                std::string cstOutputFile = outputDirectory + "/cst_" + entry.path().filename().string();
                std::ofstream cstFile(cstOutputFile);
                if (cstFile) {
                    cstFile << "CST for file: " << entry.path().filename().string() << "\n";
                    writeCSTToFile(cstRoot, cstFile);
                    cstFile.close();
                }
                std::string symbolOutputFile = outputDirectory + "/symboltable_" + entry.path().filename().string();
                std::ofstream symbolFile(symbolOutputFile);
                if (symbolFile) {
                    parser.getSymbolTable().printTable(symbolFile);
                    symbolFile.close();
}

                delete cstRoot;
            }
            
            
            
        }
            
    }
    return 0;
}

