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

        // ���һ���ַ���Ӧ������ߣ�
        if (state->m.find(c) != state->m.end()) {
            state = *(state->m[c].begin());
            ++length;
        }
        else
            state = nullptr;

        st.push(state);
    }

    // ��state���ǽ���״̬ʱ
    while (dfa_end_nodes.find(state) != dfa_end_nodes.end()) {
        st.pop();
        state = st.top();
        // rollback
        --length;
    }

    return length;
}

void ParseToken::generate_simplifiedDFA_start_node() {
    // û����ߵľ�����ʼ�ڵ�
    std::set<StateNode*> s;
    for (auto node : nodes)
        for (auto& p : node->m)
            s.insert(p.second.begin(), p.second.end());
    // �϶�����ֻ��һ������s��
    for (auto node : nodes)
        if (s.find(node) == s.end()) {
            start = node;
            break;
        }
}
