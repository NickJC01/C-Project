#include "CommentRemover.h"
#include "ErrorHandler.h"
#include <iostream>
#include <fstream>

extern ErrorHandler errorHandler;

enum State {
    NORMAL,               // Default state (reading code normally)
    SINGLE_LINE_COMMENT,  // Inside `// ...`
    MULTI_LINE_COMMENT,   // Inside `/* ... */`
    STRING_LITERAL        // Inside `"..."` or `'...'`
};

void CommentRemover::removeComments(const std::string& inputFilename, const std::string& outputFilename) {
    std::ifstream inputFile(inputFilename);
    if (!inputFile) {
        errorHandler.addError(0, "Error: Unable to open input file " + inputFilename);
        return;
    }

    std::ofstream outputFile(outputFilename);
    if (!outputFile) {
        errorHandler.addError(0, "Error: Unable to create output file " + outputFilename);
        return;
    }

    char current, next;
    State currentState = NORMAL;
    int lineNumber = 1;
    int commentStartLine = -1;
    bool containsNonCommentCode = false;
    char stringDelimiter = '\0'; // Tracks if inside " or ' string

    while (inputFile.get(current)) {
        if (current == '\n') {
            lineNumber++;
            if (currentState == SINGLE_LINE_COMMENT) {
                currentState = NORMAL;
            }
            outputFile.put(current);
            continue;
        }

        switch (currentState) {
            case NORMAL:
                if (current == '/') {
                    if (inputFile.get(next)) {
                        if (next == '/') {
                            currentState = SINGLE_LINE_COMMENT; // `//` found
                        } else if (next == '*') {
                            currentState = MULTI_LINE_COMMENT; // `/*` found
                            commentStartLine = lineNumber;
                        } else {
                            outputFile.put(current);
                            outputFile.put(next);
                            containsNonCommentCode = true;
                        }
                    }
                } 
                else if (current == '*') {
                    if (inputFile.get(next) && next == '/') {
                        // Error: Closing comment */ found without opening /*
                        errorHandler.addError(lineNumber, "Lexical Error: Unmatched closing comment '*/'.");
                        inputFile.close();
                        outputFile.close();
                        std::remove(outputFilename.c_str());
                        return;
                    } else {
                        outputFile.put(current);
                        inputFile.putback(next);
                        containsNonCommentCode = true;
                    }
                } 
                else if (current == '"' || current == '\'') {
                    // Entering a string literal
                    stringDelimiter = current;
                    currentState = STRING_LITERAL;
                    outputFile.put(current);
                } 
                else {
                    outputFile.put(current);
                    containsNonCommentCode = true;
                }
                break;

            case SINGLE_LINE_COMMENT:
                // Ignore characters until the end of the line
                break;

            case MULTI_LINE_COMMENT:
                if (current == '*') {
                    // **Detect proper block closing** while skipping extra `*`
                    while (inputFile.peek() == '*') {
                        inputFile.get(current); // Skip extra asterisks
                    }

                    if (inputFile.get(next) && next == '/') {
                        currentState = NORMAL; // End `/* ... */`
                    } else {
                        inputFile.putback(next);
                    }
                }
                break;

            case STRING_LITERAL:
                outputFile.put(current);
                if (current == stringDelimiter) {
                    currentState = NORMAL; // End string literal
                    stringDelimiter = '\0';
                }
                break;
        }
    }

    inputFile.close();
    outputFile.close();

    if (currentState == MULTI_LINE_COMMENT) {
        errorHandler.addError(commentStartLine, "Lexical Error: Unterminated block comment.");
        std::remove(outputFilename.c_str());  
        return;
    }

    if (!containsNonCommentCode) {
        errorHandler.addError(1, "Lexical Error: Entire file was enclosed in a comment.");
        std::remove(outputFilename.c_str());  
        return;
    }

}
