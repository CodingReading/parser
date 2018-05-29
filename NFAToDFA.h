#pragma once
// �Ӽ������㷨���������㷨
#include <queue>
#include <set>
#include <map>
#include <utility>
#include "REToNFA.h"


// �ݹ������ѭ��
void eps_closure(stateNode* node, std::set<stateNode*>& v){
    v.insert(node);
    std::queue<stateNode*> que;
    que.push(node);
    while (que.size()) {
        stateNode *n = que.back();
        que.pop();
        if (n->next1 != nullptr && n->edge1 == EPSILON && v.find(n) != v.end()) {
            que.push(n->next1);
            v.insert(n->next1);
        }

        if (n->next2 != nullptr && n->edge2 == EPSILON && v.find(n) != v.end()) {
            que.push(n->next2);
            v.insert(n->next2);
        }
    }
}

// ��Ϊ������ EPSILONΪ1 ���Զ���char = 1����,�޷�ʶ��
void delta(std::set<stateNode*>& s, char c, std::vector<stateNode*>& v) {
    for (auto node : s) {
        if (node->next1 != nullptr && node->edge1 != EPSILON && node->edge1 == c)
            v.push_back(node->next1);
        if (node->next2 != nullptr && node->edge2 != EPSILON && node->edge2 == c)
            v.push_back(node->next2);
    }
}

class NFAToDFA {
public:
    NFAToDFA() :nfa_start(nullptr), nfa_end(nullptr) {}
    NFAToDFA(REToNFA& r) :nfa_start(r.get_start()), nfa_end(r.get_end()) {}
    void subset_construction();
    void printDFA();
private:
    stateNode* nfa_start;       // NFA����ʼ�ڵ�
    stateNode* nfa_end;         // NFA���ս�ڵ�
    // ������Ӷ� node1-->char-->node2
    std::multimap<stateNode*, std::pair<char, stateNode*>> m;
    // ��������ս�ڵ�
    std::set<stateNode*> s_end;
    void combine_nodes();
};

void NFAToDFA::printDFA() {
    for (auto& node : m) 
        print_relationships(node.first, node.second.first, node.second.second);
}

void NFAToDFA::combine_nodes() {
    for (auto& n : m)
        combine(n.first, n.second.first, n.second.second);
}

// �������㷨
void NFAToDFA::subset_construction() {
    stateNode::statnum = 0;
    // �����洢���ϣ������ж��Ƿ���ֹ��Լ���Ӧ�Ľڵ� O(N)
    std::map<std::set<stateNode*>, std::pair<stateNode*, bool>> Q;   
    std::queue<std::set<stateNode*>> worklist;
    std::set<stateNode*> s;
    eps_closure(nfa_start, s);
    worklist.push(s);

    stateNode* cur = new stateNode();
    nfa_start = cur;
    Q.insert(std::make_pair(s, std::make_pair(cur, false)));

    while (worklist.size()) {
        s = worklist.front();
        worklist.pop();
        if (Q[s].second == true)
            continue;
        Q[s].second = true;
        // �������ASICC �ַ�
        // ��д��unsigned ������ѭ��
        for (unsigned char c = 0; c < UCHAR_MAX; ++c) {
            std::vector<stateNode*> vec;
            // ��s�����г���Ϊc�Ľڵ����vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<stateNode*> t_set;
            // �����г���Ϊc�Ħűհ�
            for (auto n : vec)
                eps_closure(n, t_set);
         
            // ���ָ���Լ�����������ñ�������ڵ㣬���������ѭ��
            if (t_set != s)
                worklist.push(t_set);
            // �������Ӽ�����,���ô����½ڵ�
            if (Q.find(t_set) != Q.end()) {
                m.insert(std::make_pair(Q[s].first, std::make_pair(c, Q[t_set].first)));  
            }
            else {
                // ����ڵ�
                stateNode* next = new stateNode();
                // ��������а���nfa_end����Ϊ��ֹ�ڵ�
                if (t_set.find(nfa_end) != t_set.end())
                    s_end.insert(next);
                Q.insert(make_pair(t_set, std::make_pair(next, false)));
                m.insert(std::make_pair(Q[s].first, std::make_pair(c, next)));
            }        
        }
    }
}
