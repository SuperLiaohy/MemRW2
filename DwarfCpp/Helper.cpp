//
// Created by liaohy on 12/19/25.
//

#include "Helper.hpp"

#include "VariDie.hpp"




auto getMemberHelper(dwarf::Die& die, void* userData) -> bool;

void getMember(dwarf::Die& die, VariNode *node) {
    using namespace dwarf;
    auto tag = die.getTag().value_or(0);
    if (tag==DW_TAG_array_type) {
        auto len = die.getArrayLen();
        if (len==std::nullopt) return;
        if (*len>256) {return;}
        std::size_t size = node->getSize()/(*len);
        auto realType = die.getTypeDie();
        if (realType==std::nullopt) {return;};
        auto type = TypeDie(&*realType);
        auto str = type.getName();
        if (str==std::nullopt) {return;};
        auto memNode = new VariNode(nullptr);
        memNode->setName("[0]");
        memNode->setOffset(0);
        memNode->setSize(size);
        memNode->setType(*str);
        auto deepType = die.getDeepTypeDie();
        if (deepType!=std::nullopt) getMember(*deepType,memNode);
        node->addChild(memNode,*len);
        for (std::size_t i=1;i<len;i++) {
            auto c = node->getChild(i);
            c->setName('['+std::to_string(i)+']');
            c->setOffset(i*(size));
        }
    }
    die.recursion(getMemberHelper,node);
}

auto getMemberHelper(dwarf::Die& die, void* userData) -> bool {
    using namespace dwarf;
    auto node = static_cast<VariNode*>(userData);
    auto tag = die.getTag().value_or(0);
    if (tag==DW_TAG_member) {
        auto mem = VariDie(&die);
        auto memName = mem.getName();
        if (memName==std::nullopt)return false;
        auto memOff = mem.getMemLoc();
        if (memOff==std::nullopt)return false;
        auto typeDie = mem.getTypeDie();
        if (typeDie==std::nullopt)return false;
        auto type = TypeDie(&*typeDie);
        auto typeName = type.getName();
        if (typeName==std::nullopt)return false;
        auto memNode = new VariNode(nullptr);
        node->addChild(memNode);
        memNode->setName(*memName);
        memNode->setOffset(*memOff);
        memNode->setType(*typeName);
        auto size = type.getSize();
        if (size!=std::nullopt) {memNode->setSize(*size);}
        auto deepType = die.getDeepTypeDie();
        if (deepType==std::nullopt)return false;
        getMember(*deepType,memNode);
    } else if (tag==DW_TAG_inheritance) {
        getMember(die,node);
    }
    return false;
}