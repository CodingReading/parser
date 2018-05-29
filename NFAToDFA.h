#pragma once
// 子集构造算法，工作表算法
#include <queue>
#include <set>
#include <map>
#include <utility>
#include "REToNFA.h"


// 递归造成死循环
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

// 因为定义了 EPSILON为1 所以对于char = 1忽视,无法识别
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
    stateNode* nfa_start;       // NFA的起始节点
    stateNode* nfa_end;         // NFA的终结节点
    // 存放连接对 node1-->char-->node2
    std::multimap<stateNode*, std::pair<char, stateNode*>> m;
    // 用来存放终结节点
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

// 工作表算法
void NFAToDFA::subset_construction() {
    stateNode::statnum = 0;
    // 用来存储集合，用来判断是否出现过以及对应的节点 O(N)
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
        // 检查所有ASICC 字符
        // 少写了unsigned 构成死循环
        for (unsigned char c = 0; c < UCHAR_MAX; ++c) {
            std::vector<stateNode*> vec;
            // 把s中所有出边为c的节点放入vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<stateNode*> t_set;
            // 求所有出边为c的ε闭包
            for (auto n : vec)
                eps_closure(n, t_set);
         
            // 如果指向自己，则后续不用遍历这个节点，否则造成死循环
            if (t_set != s)
                worklist.push(t_set);
            // 如果这个子集存在,不用创造新节点
            if (Q.find(t_set) != Q.end()) {
                m.insert(std::make_pair(Q[s].first, std::make_pair(c, Q[t_set].first)));  
            }
            else {
                // 保存节点
                stateNode* next = new stateNode();
                // 如果集合中包含nfa_end，则为终止节点
                if (t_set.find(nfa_end) != t_set.end())
                    s_end.insert(next);
                Q.insert(make_pair(t_set, std::make_pair(next, false)));
                m.insert(std::make_pair(Q[s].first, std::make_pair(c, next)));
            }        
        }
    }
}
