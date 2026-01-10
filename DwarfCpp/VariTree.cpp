//
// Created by liaohy on 12/18/25.
//

#include "VariTree.hpp"

#include <ranges>

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

void VariNode::updateAbsolute() {
    for (auto child: children) {
        child->correctAbsolute();
        child->updateAbsolute();
    }
}

std::string VariNode::getRecursionName() const {
    std::vector<const std::string*> names;
    const VariNode *p = this;
    std::size_t len = 0;
    if (p->parent!=nullptr) {
        while (p->parent->parent) {
            names.push_back(&p->getName());
            len+=p->getName().size();
            p = p->getParent();
        }
    }
    std::string recursionName;
    recursionName.reserve(len+1);
    for (auto name : std::ranges::views::reverse(names)) {
        if (name!=*(names.end()-1)&&name->data()[0]!='[') {recursionName.append(".");}
        recursionName.append(*name);
    }
    return recursionName;
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
