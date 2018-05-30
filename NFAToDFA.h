#pragma once
// 子集构造算法，工作表算法
#include <queue>
#include <utility>
#include "REToNFA.h"


// 求 ε 闭包
void eps_closure(stateNode* node, std::set<stateNode*>& v){
    v.insert(node);
    std::queue<stateNode*> que;
    que.push(node);
    while (que.size()) {
        stateNode *n = que.back();
        que.pop();
        // 存在EPSILON边
        if (n->m.find(EPSILON) != n->m.end())
            for (auto p : (n->m)[EPSILON])
                if (v.find(p) == v.end()) {
                    que.push(p);
                    v.insert(p);
                }
    }
}

void delta(std::set<stateNode*>& s, char c, std::set<stateNode*>& v) {
    for (auto node : s)
        if (node->m.find(c) != node->m.end())
            v.insert(node->m[c].begin(), node->m[c].end());
}

class NFAToDFA {
public:
    NFAToDFA() :nfa_start(nullptr), nfa_end(nullptr) , dfa_start(nullptr){}
    NFAToDFA(REToNFA& r) :nfa_start(r.get_start()), nfa_end(r.get_end()), dfa_start(nullptr) {}
    void subset_construction();
    void printDFA();
private:
    stateNode* nfa_start;       // NFA的起始节点
    stateNode* nfa_end;         // NFA的终结节点

    stateNode* dfa_start;
    // 存放DFA的节点
    std::vector<stateNode*> nodes;
    // 用来存放终结节点
    std::set<stateNode*> s_ends;
};

void NFAToDFA::printDFA() {
    std::cout << "------------------NFA to DFA------------------" << std::endl;
    for (auto node : nodes) 
        print_relationships(node);
    std::cout << std::endl;
}

// 工作表算法
void NFAToDFA::subset_construction() {
    stateNode::statnum = 0;
    // 用来存储集合，用来判断是否出现过以及该集合对应的DFA节点
    std::map<std::set<stateNode*>, std::pair<stateNode*, bool>> Q;   
    std::queue<std::set<stateNode*>> worklist;
    std::set<stateNode*> s;
    // 创建起始节点的 ε闭包
    eps_closure(nfa_start, s);
    worklist.push(s);

    dfa_start = new stateNode();
    Q.insert(std::make_pair(s, std::make_pair(dfa_start, false)));
    nodes.push_back(dfa_start);

    while (worklist.size()) {
        s = worklist.front();
        worklist.pop();
        if (Q[s].second == true)
            continue;
        Q[s].second = true;
        // 检查所有ASICC 字符， 0为 ε
        for (char c = 1; c < CHAR_MAX; ++c) {
            std::set<stateNode*> vec;
            // 把s中所有出边为c的节点放入vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<stateNode*> eps_set;
            // 求所有出边为c的ε闭包
            for (auto n : vec)
                eps_closure(n, eps_set);
         
            // 不在Q中，即这是一个新的子集，需要push
            // 或者，这是一个Q中已有的子集但是还没有遍历，也需要push
            if (Q.find(eps_set) == Q.end() || 
                (Q.find(eps_set) != Q.end() && Q[eps_set].second == false))
                worklist.push(eps_set);

            stateNode* cur_node = Q[s].first;
            // 如果这个子集存在,不用创造新节点
            if (Q.find(eps_set) != Q.end()) {
                cur_node->m[c].insert(Q[eps_set].first);
            }
            else {
                // 保存节点
                stateNode* next = new stateNode();
                nodes.push_back(next);
                // 如果集合中包含nfa_end，则为终止节点
                if (eps_set.find(nfa_end) != eps_set.end())
                    s_ends.insert(next);
                Q.insert(make_pair(eps_set, std::make_pair(next, false)));
                cur_node->m[c].insert(next);
            }        
        }
    }
}
