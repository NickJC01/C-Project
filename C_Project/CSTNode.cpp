#include "CSTNode.h"
#include <iostream>
#include <string>

void CSTNode::addChild(CSTNode* child) {
    if (!leftChild) {
        leftChild = child;
    } else {
        CSTNode* sibling = leftChild;
        while (sibling->rightSibling) {
            sibling = sibling->rightSibling;
        }
        sibling->rightSibling = child;
    }
}

void CSTNode::addSibling(CSTNode* sibling) {
    if (!rightSibling) {
        rightSibling = sibling;
    } else {
        CSTNode* lastSibling = rightSibling;
        while (lastSibling->rightSibling) {
            lastSibling = lastSibling->rightSibling;
        }
        lastSibling->rightSibling = sibling;
    }
}
