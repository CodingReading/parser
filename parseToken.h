#pragma once
#include <stack>
#include "NFAToDFA.h"
#include "node.h"
#include "lexer.h"


class ParseToken {
public:
    ParseToken(NFAToDFA* d) :nodes(d->simplified_nodes), dfa_end_nodes(d->simplified_end_nodes){
        generate_simplifiedDFA_start_node();
    }
    int get_token_length(std::string s);
private:
    StateNode* start;
    std::vector<StateNode*> nodes;
    std::set<StateNode*> dfa_end_nodes;

    void generate_simplifiedDFA_start_node();
};

int ParseToken::get_token_length(std::string s) {
    std::stack<StateNode*> st;
    StateNode* state = start;
    int length = 0;
    int cur = 0;
    while (state != nullptr) {
        char c = s[cur++];
        if (dfa_end_nodes.find(state) != dfa_end_nodes.end()) {
            while (st.size())
                st.pop();
        }

        // 如果一个字符对应多个出边？
        if (state->m.find(c) != state->m.end()) {
            state = *(state->m[c].begin());
            ++length;
        }
        else
            state = nullptr;

        st.push(state);
    }

    // 当state不是接收状态时
    while (dfa_end_nodes.find(state) != dfa_end_nodes.end()) {
        st.pop();
        state = st.top();
        // rollback
        --length;
    }

    return length;
}

void ParseToken::generate_simplifiedDFA_start_node() {
    // 没有入边的就是起始节点
    std::set<StateNode*> s;
    for (auto node : nodes)
        for (auto& p : node->m)
            s.insert(p.second.begin(), p.second.end());
    // 肯定有且只有一个不在s中
    for (auto node : nodes)
        if (s.find(node) == s.end()) {
            start = node;
            break;
        }
}
