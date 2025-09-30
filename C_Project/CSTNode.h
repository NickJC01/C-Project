#ifndef CSTNODE_H
#define CSTNODE_H

#pragma once
#include <string>
#include <vector>
#include <iostream>

struct CSTNode {
    std::string name;
    std::string value;
    int lineNumber;
    CSTNode* leftChild;
    CSTNode* rightSibling;

    CSTNode(const std::string& name, const std::string& value = "", int lineNumber = -1)
        : name(name), value(value), lineNumber(lineNumber), leftChild(nullptr), rightSibling(nullptr) {}

    void addChild(CSTNode* child);
    void addSibling(CSTNode* sibling);
};

#endif
