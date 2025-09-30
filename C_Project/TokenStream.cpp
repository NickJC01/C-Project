#include "TokenStream.h"
#include "Tokenizer.h"

TokenStream::TokenStream(const std::vector<Token>& tokens)
    : tokens(tokens), currentIndex(0) {}

Token TokenStream::getNextToken() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex++];
    }
    return {TOKEN_UNKNOWN, "EOF", -1};  // return a special token indicating end of tokens
}

void TokenStream::rewind() {
    if (currentIndex > 0) currentIndex--;
}

bool TokenStream::hasMoreTokens() const {
    return currentIndex < tokens.size();
}

int TokenStream::getCurrentIndex() const {
    return currentIndex;
}

Token TokenStream::peekNextToken() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex]; // return the current token without advancing the index
    }
    return {TOKEN_UNKNOWN, "EOF", -1};  // return a special token indicating end of tokens
}
