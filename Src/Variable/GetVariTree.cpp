//
// Created by liaohy on 12/19/25.
//

#include "GetVariTree.hpp"
#include "VariDie.hpp"
#include "TypeDie.hpp"
#include "Helper.hpp"
std::shared_ptr<VariTree> getVariTree(std::string path) {
    using namespace dwarf;
    Dwarf dwarfFile(path.c_str());
    auto variTree = std::make_shared<VariTree>();
    if (dwarfFile==nullptr) {std::clog << "False" <<std::endl;return variTree;}
    else std::clog << "True" <<std::endl;
    std::size_t count=0;
    while (true) {
        auto cuNode = new VariNode(nullptr);
        auto cu = dwarfFile.getNextCU();
        if (cu==std::nullopt) {std::clog << "cu error." << std::endl; continue;}
        if (*cu == nullptr) {std::clog << "cu over." << std::endl; break;}
        cuNode->setName(cu->getName().value_or(""));
        std::clog << "got a right cu, count: " << ++count << "." << std::endl;
        cu->recursion([](Die& die, void* userData) -> bool {
            auto tag = *die.getTag();
            auto variTree = static_cast<VariNode*>(userData);
            if (tag==DW_TAG_variable) {
                auto vari = VariDie(&die);
                auto name = vari.getName();
                if (name==std::nullopt)return false;
                auto loc = vari.getLoc();
                if (loc==std::nullopt)return false;
                auto typeDie = vari.getTypeDie();
                if (typeDie==std::nullopt)return false;
                auto type = TypeDie(&*typeDie);
                auto typeName = type.getName();
                if (typeName==std::nullopt)return false;
                auto node = new VariNode(nullptr);
                variTree->addChild(node);
                node->setName(VariTree::contextStr() + *name);
                node->setOffset(*loc);
                node->setType(*typeName);

                auto size = type.getSize();
                if (size!=std::nullopt) {node->setSize(*size);}
                auto deepType = die.getDeepTypeDie();
                if (deepType==std::nullopt)return false;
                getMember(*deepType,node);
                return false;
            }
            if (tag==DW_TAG_namespace) {
                std::clog << '@';
                auto name = die.getName();
                if (name==std::nullopt) return false;
                std::clog << *name;
                VariTree::context.push_back(*name);
                return true;
            }
            if (tag==DW_TAG_typedef) {
                auto typeDie = TypeDie(&die);
                if (typeDie.getName()==std::nullopt) return false;
                return true;
            }
            if (isTypeTag(tag)) {
                auto typeDie = TypeDie(&die);
                auto name = typeDie.getName();
                if (name!=std::nullopt&&name->find('<')==std::string::npos) {
                    auto node = std::make_shared<VariNode>(nullptr);
                    node->setName(VariTree::contextStr() + *name);
                    getMember(die,node.get());
                    // VariTree::typeMap.emplace(node->getName(), node);
                    if (isSpaceTypeTag(tag)) {
                        VariTree::context.push_back(*typeDie.getName());
                    }
                }
                std::clog << '$';
                return isComplexTypeTag(tag);
            }
            return false;
        },
        [](Die& die, void* userData) -> void {
            auto tag = *die.getTag();
            auto variTree = static_cast<VariNode*>(userData);
            if (tag == DW_TAG_namespace) {
                if (die.getName()==std::nullopt) return;
                VariTree::context.pop_back();
            } else if (isSpaceTypeTag(tag)) {
                auto typeDie = TypeDie(&die);
                auto name = typeDie.getName();
                if (name==std::nullopt||name->find('<')!=std::string::npos) return;
                if (VariTree::context.empty()) return;
                // if (*name==variTree->context[variTree->context.size()-1]) {
                VariTree::context.pop_back();
                // }
            }
        }
        ,cuNode);
        if (cuNode->getChildSize()==0) delete cuNode;
        else variTree->addChild(cuNode);
        std::clog << std::endl;
    }
    variTree->updateAbsolute();
    return variTree;
}
