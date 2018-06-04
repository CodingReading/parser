#pragma once
#include <map>
#include <iostream>

#define EPSILON 0   


// ½áµã
class StateNode {
public:
    StateNode() :id(statnum++) {}
    ~StateNode() {
        m.clear();
    }
    std::map<char, std::set<StateNode*>> m;

    static int statnum;

    int get_id() {
        return id;
    }
private:
    int id;
};

int StateNode::statnum = 0;

void combine(StateNode* pre, char c, StateNode* next) {
    (pre->m)[c].insert(next);
}

void print_relationships(StateNode* node) {
    for (auto& p : node->m) {
        for (auto& s : p.second) {
            std::cout << node->get_id() << " --> ";
            if (p.first == EPSILON)
                std::cout << "¦Å";
            else
                std::cout << p.first;
            std::cout << " --> " << s->get_id() << std::endl;
        }
    }
}
