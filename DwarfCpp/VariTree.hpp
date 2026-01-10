//
// Created by liaohy on 12/18/25.
//

#pragma once
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
class VariTree;
class VariNode {
public:
    VariNode(VariNode* parent = nullptr) : parent(parent) {}
    ~VariNode() {for (auto child: children) delete child;}
    void setParent(VariNode* p) {parent=p;}
    void addChild(VariNode* node) {children.push_back(node);node->parent=this;}
    void addChild(VariNode* node, int n);
    void addChildrenTree(VariNode* tree);

    void setName(const std::string &n) {name = n;}
    void setOffset(const std::size_t o)  {offset = o;}
    void correctAbsolute() {if (parent) absolute=parent->absolute+offset;else absolute=offset;}
    void setAbsolute(const std::size_t a)  {absolute = a;}
    void setType(const std::string &t)  {type = t;}
    void setSize(const std::size_t s)  {size = s;}
    void updateAbsolute();

    VariNode* findChild(const std::string& childName) const {
        for (auto child: children) {
            if (child->getName()==childName) return child;
        };
        return nullptr;
    }

    VariNode* getParent() const {return parent;}
    VariNode* getChild(std::size_t index) const {return children[index];}
    std::size_t getChildSize() const {return children.size();}
    std::size_t getDepth() const {
        auto p = parent; std::size_t depth = 0;
        while (p) {
            p = p->parent;
            ++depth;
        }
        return depth;
    }
    std::size_t getIndex() const {
        if (parent==nullptr) return 0;
        for (int i = 0; i < parent->children.size()-1; ++i) {
            if(parent->children[i]==this) return i;
        }
        return parent->children.size()-1;
    }

    const std::string& getName() const {return name;}
    std::size_t getOffset() const {return offset;}
    std::size_t getAbsolute() const {return absolute;}
    const std::string& getType() const {return type;}
    std::size_t getSize() const {return size;}
    std::string getRecursionName() const;

    std::string data(int column) const {
        switch (column) {
            case 0:{
                return name;
            }
            case 1:
                if (parent == nullptr) return {};;
                if (parent->parent== nullptr) return {};;
                return type;
            case 2: {
                if (parent == nullptr) return {};;
                if (parent->parent== nullptr) return {};;
                return std::format("0x{:x}", absolute);
            }
            case 3:
                if (parent == nullptr) return {};;
                if (parent->parent== nullptr) return {};;
                return std::format("{}", size);
            default:
                return {};

        }
    }

protected:
    VariNode* parent;
    std::vector<VariNode*> children;
    std::string name;
    std::size_t offset = 0;
    std::size_t absolute = 0;
    std::string type;
    std::size_t size = 0;

    void addChildrenTreeHelper(VariNode* tree);

};


class VariTree : public VariNode {
public:
    VariTree() : VariNode(nullptr) {}
    static std::unordered_map<std::string,std::shared_ptr<VariNode>> typeMap;


    static std::vector<std::string> context;
    static std::string contextStr() {
        std::string prefix;
        for (auto & str: context) {
            prefix+=str;
            prefix+="::";
        }
        return prefix;
    }
};