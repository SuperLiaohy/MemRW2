//
// Created by liaohy on 12/18/25.
//

#include "VariTree.hpp"

std::unordered_map<std::string,std::shared_ptr<VariNode>> VariTree::typeMap;
std::vector<std::string> VariTree::context;

void VariNode::addChildrenTree(VariNode* tree) {
    auto origin = children.size();
    children.resize(origin+tree->getChildSize());
    for (int i = 0; i < tree->getChildSize(); ++i) {
        auto item = new VariNode(*tree->getChild(i));
        item->addChildrenTreeHelper(item);
        children[origin+i] = item;
        item->parent = this;
    }
}

void VariNode::addChildrenTreeHelper(VariNode *tree) {
    if (children.empty()) return;
    children.resize(tree->getChildSize());
    for (int i = 0; i < tree->getChildSize(); ++i) {
        auto item = new VariNode(*tree->getChild(i));
        item->addChildrenTreeHelper(item);
        children[i] = item;
        item->parent = this;
    }
}
