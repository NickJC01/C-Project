#include "Parser.h"
#include "CSTNode.h"
#include "TokenStream.h"
#include "ErrorHandler.h"
#include "Tokenizer.h"
#include "SymbolTable.h"

Parser::Parser(TokenStream& tokenStream, ErrorHandler& errorHandler)
    : tokenStream(tokenStream), errorHandler(errorHandler) {}

void Parser::reportError(const std::string& message, int lineNumber) {
    errorHandler.addError(lineNumber, message);
}

CSTNode* Parser::parseProgram() {
    CSTNode* root = new CSTNode("Program");

    while (tokenStream.hasMoreTokens()) {
        Token token = tokenStream.getNextToken();
        
        if (token.type == TOKEN_PROCEDURE || token.value == "function") { 
            bool isFunction = (token.value == "function");

            // if it's a function, grab the return type
            std::string typeName = "Procedure";  // default type
            Token returnTypeToken;
            if (isFunction) {
                typeName = "Function";
            
                returnTypeToken = tokenStream.getNextToken();
                if (returnTypeToken.type != TOKEN_TYPE) {
                    reportError("Expected return type after 'function'.", returnTypeToken.lineNumber);
                    delete root;
                    return nullptr;
                }
            }

            // parse the procedure or function declaration
            CSTNode* procedureNode = parseProcedure(typeName, returnTypeToken);
            if (procedureNode) {
                root->addChild(procedureNode);
            }
        } 
        else {
            tokenStream.rewind();  // put the token back for parseStatement to process it.
            CSTNode* statementNode = parseStatement();
            
            if (statementNode) {
                root->addChild(statementNode);  // add to the root node
            } 
            else {
                reportError("Expected valid statement at start of program.", token.lineNumber);
                delete root;
                return nullptr;
            }
        }
    }
    return root;
}


CSTNode* Parser::parseProcedure(const std::string& nodeType, const Token& returnTypeToken) {  // nodeType is "procedure" or "function"
    Token token = tokenStream.getNextToken();

    if (token.type != TOKEN_IDENTIFIER) {
        if (keywords.find(token.value) != keywords.end()) {
            reportError("Syntax error: cannot define a function with reserved word '" + token.value + "'", token.lineNumber);
            return nullptr;
        }
        else{
            reportError("Expected procedure or function name.", token.lineNumber);
            return nullptr;
        }
    }
    
    symbolTable.enterScope();
    int currentScope = symbolTable.getCurrentScopeLevel();  // store the scope


    
    // create a node for the procedure itself
    CSTNode* procedureNode = new CSTNode(nodeType, token.value, token.lineNumber);
    SymbolTableEntry procEntry;
    procEntry.identifierName = token.value;
    procEntry.identifierType = (nodeType == "Function") ? "function" : "procedure";
    procEntry.dataType = (nodeType == "Function") ? returnTypeToken.value : "NOT APPLICABLE";
    procEntry.isArray = false;
    procEntry.arraySize = 0;
    procEntry.scope = currentScope;
    try {
        symbolTable.addEntry(procEntry);      
    }
    catch (const std::runtime_error& e) {
        reportError(e.what(), token.lineNumber);
        return nullptr;                   
    }
    
    if (nodeType == "Function" && returnTypeToken.type != TOKEN_UNKNOWN) {
        procedureNode->addChild(new CSTNode("ReturnType", returnTypeToken.value, returnTypeToken.lineNumber));
    }
    
    // parse the opening parenthesis '('
    token = tokenStream.getNextToken();
    if (token.type != TOKEN_L_PAREN) {
        reportError("Expected '(' after procedure or function name.", token.lineNumber);
        delete procedureNode;
        return nullptr;
    }
    procedureNode->addChild(new CSTNode("Symbol", "(", token.lineNumber));  // add '(' to CST

    // parse the parameter list (if any)
    while (tokenStream.hasMoreTokens()) {
        token = tokenStream.getNextToken();
        
        if (token.type == TOKEN_R_PAREN) {  // end of parameter list
            procedureNode->addChild(new CSTNode("Symbol", ")", token.lineNumber));  // add ')' to CST
            break;
        }

        if (token.type == TOKEN_TYPE) {
            if (token.value == "void") {
                // accept 'void' as the only parameter, followed immediately by ')'
                procedureNode->addChild(new CSTNode("ParameterType", "void", token.lineNumber));
        
                token = tokenStream.getNextToken();
                if (token.type != TOKEN_R_PAREN) {
                    reportError("Expected ')' after 'void'.", token.lineNumber);
                    delete procedureNode;
                    return nullptr;
                }
        
                procedureNode->addChild(new CSTNode("Symbol", ")", token.lineNumber));
                
                break;  // end parsing parameter list
                
            }
        
            CSTNode* paramTypeNode = new CSTNode("ParameterType", token.value, token.lineNumber);
            token = tokenStream.getNextToken();
        
            if (token.type == TOKEN_IDENTIFIER) {          // ← outer test already true
                bool isArrayParam = false;
                int  arraySize    = 0;
            
                /* look for “[ size ]” */
                Token afterName = tokenStream.peekNextToken();
                if (afterName.type == TOKEN_L_BRACKET) {
                    tokenStream.getNextToken();                 // consume '['
                    Token sizeTok = tokenStream.getNextToken(); // expect integer
                    if (sizeTok.type != TOKEN_INTEGER) {
                        reportError("Expected integer size for array parameter.", sizeTok.lineNumber);
                        delete procedureNode;
                        return nullptr;
                    }
                    isArrayParam = true;
                    arraySize    = std::stoi(sizeTok.value);
            
                    Token closeBr = tokenStream.getNextToken(); // expect ']'
                    if (closeBr.type != TOKEN_R_BRACKET) {
                        reportError("Expected ']' after array size.", closeBr.lineNumber);
                        delete procedureNode;
                        return nullptr;
                    }
                }
            
                /* build CST */
                CSTNode* paramNode = new CSTNode("Parameter", token.value, token.lineNumber);
                if (isArrayParam) {
                    paramNode->addChild(new CSTNode("ArraySize", std::to_string(arraySize), token.lineNumber)); // optional
                }
                paramTypeNode->addChild(paramNode);
                procedureNode->addChild(paramTypeNode);
            
                // symbol‑table entry – store only in parameter list 
                SymbolTableEntry paramEntry;
                paramEntry.identifierName = token.value;
                paramEntry.identifierType = "parameter";
                paramEntry.dataType       = paramTypeNode->value;
                paramEntry.isArray        = isArrayParam;
                paramEntry.arraySize      = arraySize;
                paramEntry.scope          = currentScope;
            
                symbolTable.addFunctionParameter(procEntry.identifierName, paramEntry);
            }
            else {
                if (keywords.find(token.value) != keywords.end()) {
                    reportError("Syntax error: reserved word '" + token.value +
                                "' cannot be used as a parameter name.", token.lineNumber);
                    delete procedureNode;
                    return nullptr;
                } else {
                    reportError("Expected parameter name after type.", token.lineNumber);
                    delete procedureNode;
                    return nullptr;
                }
            }
        }

        Token lookAhead = tokenStream.peekNextToken();      // NEW
        if (lookAhead.type == TOKEN_COMMA) {                // use lookAhead
            tokenStream.getNextToken();                     // consume the comma
            procedureNode->addChild(new CSTNode("Symbol", ",", lookAhead.lineNumber));
        }
    }

    // parse the opening brace '{'
    token = tokenStream.getNextToken();
    if (token.type != TOKEN_L_BRACE) {
        reportError("Expected '{' at the start of procedure or function body.", token.lineNumber);
        delete procedureNode;
        return nullptr;
    }
    procedureNode->addChild(new CSTNode("Symbol", "{", token.lineNumber));  // add '{' to CST

    // parse the procedure body (statements)
    while (tokenStream.hasMoreTokens()) {
        token = tokenStream.getNextToken();
        if (token.type == TOKEN_R_BRACE) {  // end of procedure body
            procedureNode->addChild(new CSTNode("Symbol", "}", token.lineNumber));  // add '}' to CST
            break;
        }

        tokenStream.rewind();
        CSTNode* statementNode = parseStatement();

        if (statementNode) {
            procedureNode->addChild(statementNode);
        }
    }
    symbolTable.exitScope();

    return procedureNode;
}

CSTNode* Parser::parseStatement() {
    Token token = tokenStream.getNextToken();

    if (token.type == TOKEN_KEYWORD && token.value == "for") {  // handling "for" loops        
        CSTNode* forNode = new CSTNode("ForStatement", "for", token.lineNumber);
    
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_PAREN) {
            reportError("Expected '(' after 'for'.", token.lineNumber);
            delete forNode;
            return nullptr;
        }
    
        // parse Initialization (e.g., i = 0;)
        CSTNode* initNode = parseStatement();
        if (!initNode) {
            delete forNode;
            return nullptr;
        }
        forNode->addChild(initNode);
    
        // parse Condition (e.g., (i < 4) && (digit > -1))
        CSTNode* conditionNode = parseExpression();
        if (!conditionNode) {
            delete forNode;
            return nullptr;
        }
        forNode->addChild(conditionNode);
    
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_SEMICOLON) {  
            std::cout << "Unexpected token: " << token.value << " on Line: " << token.lineNumber << std::endl;
            reportError("Expected ';' after 'for' loop condition.", token.lineNumber);
            delete forNode;
            return nullptr;
        } 
    
        Token idToken = tokenStream.getNextToken();
        Token nextToken = tokenStream.peekNextToken();

        // handle increment/decrement operators like "i++" or "i--"
        if (idToken.type == TOKEN_IDENTIFIER && (nextToken.type == TOKEN_PLUS || nextToken.type == TOKEN_MINUS)) {
            tokenStream.getNextToken();  // consume the increment or decrement operator
            CSTNode* incrementNode = new CSTNode("Increment", idToken.value, idToken.lineNumber);
            incrementNode->addChild(new CSTNode("Operator", nextToken.value, nextToken.lineNumber));

            forNode->addChild(incrementNode);
            std::cout << "Successfully parsed increment or decrement operator in for loop.\n";
        } 
        // handle assignment expressions like "i = i + 1"
        else if (idToken.type == TOKEN_IDENTIFIER && nextToken.type == TOKEN_ASSIGNMENT_OPERATOR) {
            tokenStream.getNextToken();  // consume the '=' operator
            CSTNode* incrementNode = new CSTNode("Assignment", idToken.value, idToken.lineNumber);

            CSTNode* expr = parseExpression();
            if (!expr) {
                delete incrementNode;
                delete forNode;
                return nullptr;
            }
            incrementNode->addChild(expr);

            forNode->addChild(incrementNode);
        } else {
            std::cout << "Failed to parse the increment section of the 'for' loop.\n";
            reportError("Expected increment expression (i++, i--, or assignment) in 'for' loop increment.", idToken.lineNumber);
            delete forNode;
            return nullptr;
        }

        token = tokenStream.getNextToken();
        if (token.type != TOKEN_R_PAREN) {
            std::cout << "Expected ')'. Found: " << token.value << " on Line: " << token.lineNumber << std::endl;
            reportError("Expected ')' after 'for' loop header.", token.lineNumber);
            delete forNode;
            return nullptr;
        }

        // expect the opening brace for the 'for' loop body
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_BRACE) {
            reportError("Expected '{' after 'for' loop header.", token.lineNumber);
            delete forNode;
            return nullptr;
        }

        // parse the block of the 'for' loop
        while (tokenStream.hasMoreTokens()) {
            token = tokenStream.getNextToken();
            if (token.type == TOKEN_R_BRACE) break;

            tokenStream.rewind();
            CSTNode* statementNode = parseStatement();

            if (statementNode) {
                forNode->addChild(statementNode);
            }
        }
        return forNode;

    }
    

    if (token.type == TOKEN_TYPE) {  // handling declarations (e.g., "int", "boolean", "char")
        CSTNode* declarationNode = new CSTNode("Declaration", token.value, token.lineNumber);
    
        while (true) {
            token = tokenStream.getNextToken();
    
            if (token.type == TOKEN_IDENTIFIER) {  //standard variable or array name
                CSTNode* variableNode = new CSTNode("Variable", token.value, token.lineNumber);
    
                // check if it's an array declaration
                Token nextToken = tokenStream.peekNextToken();
                if (nextToken.type == TOKEN_L_BRACKET) {  // detects '[' for arrays
                    // Error if identifier name is a reserved type (e.g., "char char;")
                    if (keywords.find(token.value) != keywords.end()) {
                        reportError("Syntax error: reserved word '" + token.value + "' cannot be used as a variable name.", token.lineNumber);
                        delete declarationNode;
                        return nullptr;
                    }

                    tokenStream.getNextToken();  // consume the '['
    
                    Token signToken = tokenStream.peekNextToken();
                    std::string sizeValue;
                    Token numberToken;

                    if (signToken.type == TOKEN_PLUS || signToken.type == TOKEN_MINUS) {
                        tokenStream.getNextToken();  // consume '+' or '-'
                        numberToken = tokenStream.getNextToken();
                        if (numberToken.type != TOKEN_INTEGER) {
                            reportError("Expected integer after '+' or '-' in array size.", numberToken.lineNumber);
                            delete declarationNode;
                            return nullptr;
                        }
                        sizeValue = (signToken.value + numberToken.value);
                    } else {
                        numberToken = tokenStream.getNextToken();
                        if (numberToken.type != TOKEN_INTEGER) {
                            reportError("Expected integer size for array declaration.", numberToken.lineNumber);
                            delete declarationNode;
                            return nullptr;
                        }
                        sizeValue = numberToken.value;
                    }
                    // validate array size is a positive integer
                    try {
                        int size = std::stoi(sizeValue);
                        if (size <= 0) {
                            reportError("Syntax error: array declaration size must be a positive integer.", numberToken.lineNumber);
                            delete declarationNode;
                            return nullptr;
                        }
                    } catch (...) {
                        reportError("Syntax error: array declaration size must be a positive integer.", numberToken.lineNumber);
                        delete declarationNode;
                        return nullptr;
                    }
                    
                    CSTNode* sizeNode = new CSTNode("ArraySize", sizeValue, signToken.lineNumber);
                    variableNode->addChild(sizeNode);                    
    
                    // now expect a closing bracket ']'
                    nextToken = tokenStream.getNextToken();
                    if (nextToken.type != TOKEN_R_BRACKET) {
                        reportError("Expected ']' after array size.", nextToken.lineNumber);
                        delete declarationNode;
                        return nullptr;
                    }
    
                    // mark this as an ArrayDeclaration instead of a regular Variable
                    variableNode->name = "ArrayDeclaration";
                }
    
                declarationNode->addChild(variableNode);  // add to declaration node

                SymbolTableEntry varEntry;                // add to symbol table
                varEntry.identifierName = token.value;
                varEntry.identifierType = "datatype";
                varEntry.dataType = declarationNode->value;
                varEntry.isArray = (variableNode->name == "ArrayDeclaration");
                varEntry.arraySize = (varEntry.isArray && variableNode->leftChild) ? std::stoi(variableNode->leftChild->value) : 0;
                varEntry.scope = symbolTable.getCurrentScopeLevel();
                try {
                    symbolTable.addEntry(varEntry);    
                }
                catch (const std::runtime_error& e) {
                    reportError(e.what(), token.lineNumber);  
                    return nullptr;                         
                }
                
            } 
            else {
                if (keywords.find(token.value) != keywords.end()) {
                    reportError("Syntax error: reserved word '" + token.value + "' cannot be used as a variable name.", token.lineNumber);
                    delete declarationNode;
                    return nullptr;
                }
                else{
                    reportError("Expected variable name after type.", token.lineNumber);
                    delete declarationNode;
                    return nullptr;
                }
            }
    
            token = tokenStream.peekNextToken();
    
            if (token.type == TOKEN_COMMA) {  // handle multiple declarations in one line
                tokenStream.getNextToken();  // consume the comma
                continue;  // continue processing additional variables
            } 
            else if (token.type == TOKEN_SEMICOLON) {  // end of declaration line
                tokenStream.getNextToken();  // consume the semicolon
                break;
            } 
            else {  // unexpected token
                reportError("Expected ';' after variable declaration.", token.lineNumber);
                delete declarationNode;
                return nullptr;
            }
        }
    
        return declarationNode;
    }  
    
    if (token.type == TOKEN_IDENTIFIER) {

        // look ahead without consuming 
        Token look = tokenStream.peekNextToken();

        if (look.type == TOKEN_L_BRACKET) {
            tokenStream.getNextToken();   // consume '['

            CSTNode* indexExpr = parseExpression(); // parse the index
            if (!indexExpr) return nullptr;

            Token closeBr = tokenStream.getNextToken(); // expect ']'
            if (closeBr.type != TOKEN_R_BRACKET) {
                reportError("Expected ']' after array index.", closeBr.lineNumber);
                delete indexExpr;
                return nullptr;
            }

            Token assignTok = tokenStream.getNextToken(); // expect '='
            if (assignTok.type != TOKEN_ASSIGNMENT_OPERATOR) {
                reportError("Expected '=' after array element.", assignTok.lineNumber);
                delete indexExpr;
                return nullptr;
            }

            CSTNode* rhs = parseExpression(); // right side
            if (!rhs) { delete indexExpr; return nullptr; }

            Token semi = tokenStream.getNextToken(); // expect ';'
            if (semi.type != TOKEN_SEMICOLON) {
                reportError("Expected ';' after assignment.", semi.lineNumber);
                delete indexExpr; delete rhs;
                return nullptr;
            }

            // build CST Assignments
            CSTNode* arrayAccess = new CSTNode("ArrayAccess", token.value, token.lineNumber);
            arrayAccess->addChild(indexExpr);

            CSTNode* assignNode = new CSTNode("Assignment", "[]", assignTok.lineNumber);
            assignNode->addChild(arrayAccess);
            assignNode->addChild(rhs);
            return assignNode;
        }

        if (look.type == TOKEN_ASSIGNMENT_OPERATOR) {
            tokenStream.getNextToken();     // consume '='

            CSTNode* assignNode = new CSTNode("Assignment", token.value, token.lineNumber);
            CSTNode* expr = parseExpression();
            if (!expr) { delete assignNode; return nullptr; }
            assignNode->addChild(expr);

            Token semi = tokenStream.getNextToken();
            if (semi.type != TOKEN_SEMICOLON) {
                reportError("Expected ';' after assignment statement.", semi.lineNumber);
                delete assignNode;
                return nullptr;
            }
            return assignNode;
        }

        if (look.type == TOKEN_L_PAREN) {
            tokenStream.getNextToken();      // consume '('
            CSTNode* callNode = new CSTNode("FunctionCall", token.value, token.lineNumber);

            // skip or parse arguments until ')' 
            while (tokenStream.hasMoreTokens()) {
                Token t = tokenStream.getNextToken();
                if (t.type == TOKEN_R_PAREN) break;
            }

            Token semi = tokenStream.getNextToken();
            if (semi.type != TOKEN_SEMICOLON) {
                reportError("Expected ';' after function call.", semi.lineNumber);
                delete callNode;
                return nullptr;
            }
            return callNode;
        }

        // anything else after an identifier is invalid here 
        reportError("Invalid statement.", token.lineNumber);
        return nullptr;
    }
    
    if (token.type == TOKEN_KEYWORD && token.value == "if") {  // handling "if" statements
        CSTNode* ifNode = new CSTNode("IfStatement", token.value, token.lineNumber);

        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_PAREN) {
            reportError("Expected '(' after 'if' keyword.", token.lineNumber);
            delete ifNode;
            return nullptr;
        }

        // parse the condition within the parentheses using parseExpression
        CSTNode* conditionNode = parseExpression();
        if (!conditionNode) {
            delete ifNode;
            return nullptr;
        }
        ifNode->addChild(conditionNode);

        token = tokenStream.getNextToken();
        if (token.type != TOKEN_R_PAREN) {
            reportError("Expected ')' after 'if' condition.", token.lineNumber);
            delete ifNode;
            return nullptr;
        }

        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_BRACE) {
            reportError("Expected '{' after 'if' condition.", token.lineNumber);
            delete ifNode;
            return nullptr;
        }

        // parse the statements inside the if block
        while (tokenStream.hasMoreTokens()) {
            token = tokenStream.getNextToken();
            if (token.type == TOKEN_R_BRACE) break;

            tokenStream.rewind();
            CSTNode* statementNode = parseStatement();

            if (statementNode) {
                ifNode->addChild(statementNode);
            }
        }
        // now check for an `else` statement after the closing brace of the `if` block
        Token nextToken = tokenStream.peekNextToken();
        if (nextToken.type == TOKEN_KEYWORD && nextToken.value == "else") {
            tokenStream.getNextToken();  // Consume the 'else'
            
            CSTNode* elseNode = new CSTNode("ElseStatement", "else", nextToken.lineNumber);

            // the else block should start with a '{'
            token = tokenStream.getNextToken();
            if (token.type != TOKEN_L_BRACE) {
                reportError("Expected '{' after 'else' keyword.", token.lineNumber);
                delete ifNode;
                delete elseNode;
                return nullptr;
            }

            // parse the statements inside the else block
            while (tokenStream.hasMoreTokens()) {
                token = tokenStream.getNextToken();
                if (token.type == TOKEN_R_BRACE) break;

                tokenStream.rewind();
                CSTNode* elseStatementNode = parseStatement();

                if (elseStatementNode) {
                    elseNode->addChild(elseStatementNode);
                }
            }
            
            ifNode->addChild(elseNode);  // attach the 'else' block to the 'if' node
        }
        
        return ifNode;
    }

    if (token.type == TOKEN_KEYWORD && token.value == "return") {  // handling return statements
        CSTNode* returnNode = new CSTNode("Return", "return", token.lineNumber);

        token = tokenStream.getNextToken();
        if (token.type != TOKEN_SEMICOLON) {  // if there's something before the semicolon, treat it as an expression
            tokenStream.rewind();

            // parse the expression following the return statement
            CSTNode* expressionNode = parseExpression();

            if (expressionNode) {
                returnNode->addChild(expressionNode);
            }

            token = tokenStream.getNextToken();
            if (token.type != TOKEN_SEMICOLON) {
                reportError("Expected ';' after return statement.", token.lineNumber);
                delete returnNode;
                return nullptr;
            }
        }

        return returnNode;
    }

    if (token.type == TOKEN_KEYWORD && token.value == "while") {  // handling "while" statements
        CSTNode* whileNode = new CSTNode("WhileStatement", token.value, token.lineNumber);
    
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_PAREN) {
            reportError("Expected '(' after 'while' keyword.", token.lineNumber);
            delete whileNode;
            return nullptr;
        }
    
        // parse the condition within the parentheses using parseExpression
        CSTNode* conditionNode = parseExpression();
        if (!conditionNode) {
            delete whileNode;
            return nullptr;
        }
        whileNode->addChild(conditionNode);
    
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_R_PAREN) {
            reportError("Expected ')' after 'while' condition.", token.lineNumber);
            delete whileNode;
            return nullptr;
        }
    
        token = tokenStream.getNextToken();
        if (token.type != TOKEN_L_BRACE) {
            reportError("Expected '{' after 'while' condition.", token.lineNumber);
            delete whileNode;
            return nullptr;
        }
    
        // parse the statements inside the while block
        while (tokenStream.hasMoreTokens()) {
            token = tokenStream.getNextToken();
            if (token.type == TOKEN_R_BRACE) break;
    
            tokenStream.rewind();
            CSTNode* statementNode = parseStatement();
    
            if (statementNode) {
                whileNode->addChild(statementNode);
            }
        }
    
        return whileNode;
    }
    
    reportError("Invalid statement.", token.lineNumber);
    return nullptr;
}

CSTNode* Parser::parseDeclaration() {
    Token typeToken = tokenStream.getNextToken();
    Token nameToken = tokenStream.getNextToken();

    if (nameToken.type != TOKEN_IDENTIFIER) {
        reportError("Expected identifier in declaration.", nameToken.lineNumber);
        return nullptr;
    }

    CSTNode* declarationNode = new CSTNode("Declaration", nameToken.value, nameToken.lineNumber);
    return declarationNode;
}

CSTNode* Parser::parseAssignment() {
    Token identifierToken = tokenStream.getNextToken();
    Token assignmentToken = tokenStream.getNextToken();

    if (assignmentToken.type != TOKEN_ASSIGNMENT_OPERATOR) {
        reportError("Expected '=' in assignment statement.", assignmentToken.lineNumber);
        return nullptr;
    }

    CSTNode* assignmentNode = new CSTNode("Assignment", identifierToken.value, identifierToken.lineNumber);
    CSTNode* expressionNode = parseExpression();

    if (expressionNode) assignmentNode->leftChild = expressionNode;

    return assignmentNode;
}

CSTNode* Parser::parseExpression(CSTNode* leftHandSide, bool stopAtParen) {
    if (!leftHandSide) {  // if no left-hand side is passed, we parse the first part of the expression
        Token token = tokenStream.getNextToken();

        // case 1: integer literal
        if (token.type == TOKEN_INTEGER) {
            leftHandSide = new CSTNode("Operand", token.value, token.lineNumber);
        }
        // case 2: negative integer handling (e.g., -1, -42, etc.)
        else if (token.type == TOKEN_MINUS) {  // check if it's a minus sign
            Token nextToken = tokenStream.peekNextToken();

            if (nextToken.type == TOKEN_INTEGER) {  // check if the next token is an integer
                tokenStream.getNextToken();  // consume the integer token
                leftHandSide = new CSTNode("Operand", "-" + nextToken.value, nextToken.lineNumber);
            }
            else {
                // treat as a subtraction operator in the larger expression
                tokenStream.rewind();  // rewind if it's not part of a negative number
            }
        } else if (token.type == TOKEN_BOOLEAN_NOT) {  // unary logical NOT
            CSTNode* operatorNode = new CSTNode("Operator", token.value, token.lineNumber);
        
            CSTNode* operand = parseExpression();
            if (!operand) {
                delete operatorNode;
                return nullptr;
            }
        
            operatorNode->addChild(operand);
            leftHandSide = operatorNode;
        }
        
        // case 3: identifier (variable or fumction Call)
        else if (token.type == TOKEN_IDENTIFIER) {
            Token nextToken = tokenStream.peekNextToken();

            if (nextToken.type == TOKEN_L_PAREN) {  // function call detected
                tokenStream.getNextToken();  // consume '('
                CSTNode* functionCallNode = new CSTNode("FunctionCall", token.value, token.lineNumber);

                while (tokenStream.hasMoreTokens()) {
                    Token argToken = tokenStream.peekNextToken();
                    if (argToken.type == TOKEN_R_PAREN) {
                        tokenStream.getNextToken();  // consume ')'
                        break;
                    }

                    CSTNode* argumentNode = parseExpression();
                    if (argumentNode) functionCallNode->addChild(argumentNode);

                    argToken = tokenStream.peekNextToken();

                    if (argToken.type == TOKEN_COMMA) {
                        tokenStream.getNextToken();  // consume the comma
                    } else if (argToken.type != TOKEN_R_PAREN) {
                        reportError("Expected ',' or ')' in function call argument list.", argToken.lineNumber);
                        delete functionCallNode;
                        return nullptr;
                    }
                }
                leftHandSide = functionCallNode;
            }
            // case 4: array access detected (e.g., hexnum[i])
            else if (nextToken.type == TOKEN_L_BRACKET) {  // detecting array access
                tokenStream.getNextToken();  // Consume '['

                CSTNode* arrayAccessNode = new CSTNode("ArrayAccess", token.value, token.lineNumber);

                // parse the index inside the brackets (e.g., `i`)
                CSTNode* indexNode = parseExpression();
                if (!indexNode) {
                    delete arrayAccessNode;
                    return nullptr;
                }
                arrayAccessNode->addChild(indexNode);

                Token closingBracketToken = tokenStream.getNextToken();  // consume ']'
                if (closingBracketToken.type != TOKEN_R_BRACKET) {
                    reportError("Expected ']' after array index.", closingBracketToken.lineNumber);
                    delete arrayAccessNode;
                    return nullptr;
                }

                leftHandSide = arrayAccessNode;  // set the result of array access
            }
            else {
                leftHandSide = new CSTNode("Operand", token.value, token.lineNumber);
            }
        }

        // case 5: parenthesized expression
        else if (token.type == TOKEN_L_PAREN) {
            // recursively parse inner expression
            leftHandSide = parseExpression();
            
            token = tokenStream.peekNextToken();
            if (token.type == TOKEN_R_PAREN) {
                tokenStream.getNextToken();  // consume ')'
                
                // donot return yet, check if more operators follow
                Token op = tokenStream.peekNextToken();
                if (op.type == TOKEN_BOOLEAN_AND || op.type == TOKEN_BOOLEAN_OR ||
                    op.type == TOKEN_PLUS || op.type == TOKEN_MINUS ||
                    op.type == TOKEN_ASTERISK || op.type == TOKEN_DIVIDE ||
                    op.type == TOKEN_MODULO || op.type == TOKEN_BOOLEAN_EQUAL ||
                    op.type == TOKEN_BOOLEAN_NOT_EQUAL || op.type == TOKEN_LT ||
                    op.type == TOKEN_GT || op.type == TOKEN_LT_EQUAL ||
                    op.type == TOKEN_GT_EQUAL) {
                    // allow loop to continue
                } 
            } else {
                reportError("Expected ')' after expression.", token.lineNumber);
                delete leftHandSide;
                return nullptr;
            }
        }
        
        // case 6: character literal (e.g., '0', 'A', etc.)
        else if (token.type == TOKEN_CHAR_LITERAL) {
            leftHandSide = new CSTNode("Operand", token.value, token.lineNumber);
        }
        // case 7: string literal (e.g., "feed\x0")
        else if (token.type == TOKEN_STRING) {
            leftHandSide = new CSTNode("Operand", token.value, token.lineNumber);
        }
        // case 8: escape sequences (e.g., "name = 'Robert\x0';")
        else if (token.type == TOKEN_UNKNOWN && token.value == "\\") {  // handle escape sequences starting with '\'
            std::cout << "Found backslash delimiter: " << token.value << " on Line: " << token.lineNumber << std::endl;
        
            Token nextToken = tokenStream.getNextToken();
        
            if (nextToken.type == TOKEN_UNKNOWN && (nextToken.value == "x0" || nextToken.value == "n")) { 
                // add valid escape sequence to CST
                CSTNode* escapeNode = new CSTNode("EscapeSequence", "\\" + nextToken.value, token.lineNumber);
                leftHandSide = escapeNode;
            } else {
                reportError("Invalid or unrecognized escape sequence: \\" + nextToken.value, token.lineNumber);
                return nullptr;
            }
        }
        
        else {
            reportError("Invalid expression.", token.lineNumber);
            return nullptr;
        }
    }
    
    // now we handle operators if they follow the parsed left hand side
    while (true) {
        Token token = tokenStream.peekNextToken();

        // stop if we encounter a ')' when 'stopAtParen' is true
        if (stopAtParen && token.type == TOKEN_R_PAREN) {
            std::cout << "Stopping expression parsing due to encountering ')'.\n";
            break;
        }

        if (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS ||
            token.type == TOKEN_ASTERISK || token.type == TOKEN_DIVIDE ||
            token.type == TOKEN_MODULO || token.type == TOKEN_BOOLEAN_EQUAL ||
            token.type == TOKEN_BOOLEAN_NOT_EQUAL || token.type == TOKEN_LT ||
            token.type == TOKEN_GT || token.type == TOKEN_LT_EQUAL ||
            token.type == TOKEN_GT_EQUAL || token.type == TOKEN_BOOLEAN_OR ||
            token.type == TOKEN_BOOLEAN_AND || token.type == TOKEN_LOGICAL_OR) {

            tokenStream.getNextToken();  // consume the operator

            // create the operator node
            CSTNode* operatorNode = new CSTNode("Operator", token.value, token.lineNumber);
            operatorNode->addChild(leftHandSide);

            // special handling for logical operators (&&, ||) to ensure they are grouped
            if (token.type == TOKEN_BOOLEAN_AND || token.type == TOKEN_BOOLEAN_OR) {                
                // parse the right-hand side
                CSTNode* rightHandSide = parseExpression();
                if (!rightHandSide) {
                    delete operatorNode;
                    return nullptr;
                }

                operatorNode->addChild(rightHandSide);
                leftHandSide = operatorNode;  // update the leftHandSide to be the new operator node
            } 
            else {
                // normal handling for other operators
                CSTNode* rightHandSide = parseExpression();
                if (!rightHandSide) {
                    delete operatorNode;
                    return nullptr;
                }

                operatorNode->addChild(rightHandSide);
                leftHandSide = operatorNode;
            }
        } 
        else if (stopAtParen && token.type == TOKEN_R_PAREN) {
            break;  // stop parsing if we encounter a ')' when requested
        } 
        else {
            break;  // no operator follows, return what we have
        }
    }


    return leftHandSide;
}

CSTNode* Parser::parseTerm() {
    Token token = tokenStream.getNextToken();
    if (token.type == TOKEN_INTEGER || token.type == TOKEN_IDENTIFIER) {
        return new CSTNode("Term", token.value, token.lineNumber);
    }
    reportError("Expected integer or identifier in expression.", token.lineNumber);
    return nullptr;
}
