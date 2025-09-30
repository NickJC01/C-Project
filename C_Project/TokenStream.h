#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include "Tokenizer.h"
#include <vector>

class TokenStream {
private:
    std::vector<Token> tokens;
    size_t currentIndex;

public:
    TokenStream(const std::vector<Token>& tokens);

    Token getNextToken();
    void rewind();
    bool hasMoreTokens() const;
    int getCurrentIndex() const;
    Token peekNextToken();
};

#endif
