#ifndef COMMENT_REMOVER_H
#define COMMENT_REMOVER_H

#include <string>
#include "ErrorHandler.h"  // Include ErrorHandler

class CommentRemover {
public:
    void removeComments(const std::string& inputFilename, const std::string& outputFilename);
};

#endif // COMMENT_REMOVER_H
