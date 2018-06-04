#pragma once
// 子集构造算法，工作表算法
#include <queue>
#include <utility>
#include <algorithm>
#include <iterator>
#include "REToNFA.h"


// 求 ε 闭包
void eps_closure(StateNode* node, std::set<StateNode*>& v){
    v.insert(node);
    std::queue<StateNode*> que;
    que.push(node);
    while (que.size()) {
        StateNode *n = que.back();
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

void delta(std::set<StateNode*>& s, char c, std::set<StateNode*>& v) {
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
    void printf_simplifiedDFA();
    // DFA的最小化
    void hopcroft();
    // 存放DFA最小化后的节点
    std::vector<StateNode*> simplified_nodes;
    // 存放最小化DFA中的终结节点
    std::set<StateNode*> simplified_end_nodes;
private:
    StateNode* nfa_start;       // NFA的起始节点
    StateNode* nfa_end;         // NFA的终结节点

    StateNode* dfa_start;
    // 存放DFA的节点
    std::vector<StateNode*> nodes;
    // 用来存放终结节点
    std::set<StateNode*> s_ends;
    
    // 划分子集
    void split(std::set<std::set<StateNode*>>& s);
    // 根据映射关系生成最小DFA的终结节点
    void generate_end_nodes(std::map<std::set<StateNode*>, StateNode*>& mp);
    // 生成最小DFA起始节点
};

void NFAToDFA::printDFA() {
    std::cout << "------------------NFA to DFA------------------" << std::endl;
    for (auto node : nodes) 
        print_relationships(node);
    std::cout << std::endl;
}

void NFAToDFA::printf_simplifiedDFA() {
    std::cout << "------------------simplified DFA------------------" << std::endl;
    for (auto node : simplified_nodes)
        print_relationships(node);
    std::cout << std::endl;
}

// 工作表算法
void NFAToDFA::subset_construction() {
    StateNode::statnum = 0;
    // 用来存储集合，用来判断是否出现过以及该集合对应的DFA节点
    std::map<std::set<StateNode*>, std::pair<StateNode*, bool>> Q;   
    std::queue<std::set<StateNode*>> worklist;
    std::set<StateNode*> s;
    // 创建起始节点的 ε闭包
    eps_closure(nfa_start, s);
    worklist.push(s);

    dfa_start = new StateNode();
    Q.insert(std::make_pair(s, std::make_pair(dfa_start, false)));
    nodes.push_back(dfa_start);
    // 如果s中包含终止节点，插入
    if (s.find(nfa_end) != s.end())
        s_ends.insert(dfa_start);

    while (worklist.size()) {
        s = worklist.front();
        worklist.pop();
        if (Q[s].second == true)
            continue;
        Q[s].second = true;
        // 检查所有ASICC 字符， 0为 ε
        for (char c = 1; c < CHAR_MAX; ++c) {
            std::set<StateNode*> vec;
            // 把s中所有出边为c的节点放入vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<StateNode*> eps_set;
            // 求所有出边为c的ε闭包
            for (auto n : vec)
                eps_closure(n, eps_set);
         
            // 不在Q中，即这是一个新的子集，需要push
            // 或者，这是一个Q中已有的子集但是还没有遍历，也需要push
            if (Q.find(eps_set) == Q.end() || 
                (Q.find(eps_set) != Q.end() && Q[eps_set].second == false))
                worklist.push(eps_set);

            StateNode* cur_node = Q[s].first;
            // 如果这个子集存在,不用创造新节点
            if (Q.find(eps_set) != Q.end()) {
                combine(cur_node, c, Q[eps_set].first);
            }
            else {
                // 保存节点
                StateNode* next = new StateNode();
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


void NFAToDFA::hopcroft() {
    // 子集划分为accepted 状态，和非accepted状态
    std::set<StateNode*> accepted(s_ends);
    std::set<StateNode*> non_accepted;
    for (auto node : nodes)
        if (accepted.find(node) == accepted.end())
            non_accepted.insert(node);

    std::set<std::set<StateNode*>> s;
    s.insert(accepted);
    s.insert(non_accepted);

    // 如果子集划分了，则一定会变多，size改变
    int old_size = s.size();
    do {
        old_size = s.size();
        split(s);
    } while (old_size != s.size());

    // 创建节点
    std::map<std::set<StateNode*>, StateNode*> mapping; // 每个子集对应的最小DFA的节点
    StateNode::statnum = 0;
    for (auto& se : s) {
        StateNode* t = new StateNode();
        simplified_nodes.push_back(t);
        mapping[se] = t;
    }

    // 生成最小DFA的终结节点
    generate_end_nodes(mapping);
    // 根据子集连接节点
    for (auto& se : mapping)
        for (auto node : se.first)
            for (auto& pa : node->m)
                for (auto t : pa.second)
                    if (se.first.find(t) == se.first.end()) {
                        // 如果不在自己所在子集，找到出边所在子集
                        for (auto& p1 : mapping)
                            if (p1.first.find(t) != p1.first.end())
                                combine(mapping[se.first], pa.first, mapping[p1.first]);
                    }
                    else {
                        // 在自己所在子集,回边
                        combine(mapping[se.first], pa.first, mapping[se.first]);
                    }


}

void NFAToDFA::split(std::set<std::set<StateNode*>>& s) {
    
    for (auto p = s.begin(); p != s.end(); ++p) {
        if (p->size() == 1)
            continue;
        // 第一个set为出边所在子集，char为出边的字符
        // vector存放具有相同的出边字符同时出边外节点在相同的子集
        std::map<std::pair<std::set<StateNode*>,char>, std::vector<StateNode*>> mmp;
        bool changed = false;

        for (auto ss : *p) {
            for (auto edge : ss->m)
                for (auto out : edge.second) {
                    // 如果出边在自己所在子集继续
                    if (p->find(out) != p->end())
                        continue;
                    // 如果不在自己所在子集，找到出边所在子集
                    for (auto& p1 : s) {
                        if (p1.find(out) != p1.end()) {
                            auto t = std::make_pair(p1, edge.first);
                            mmp[t].push_back(ss);
                        }
                    }

                    changed = true;
                }    
        }

        if (!changed)
            continue;

        // 如果子集变化了，修正子集，终止循环
        std::set<StateNode*> undivided(*p);     // 未被分割的，剩余的节点的集合
        s.erase(p);                             // 删除原来的
        
        for (auto& pa : mmp) {
            std::set<StateNode*> temp(pa.second.begin(), pa.second.end());
            s.insert(temp);

            // 不断的求差集，最后undivided为剩余节点
            std::set<StateNode*> s1;
            std::set_difference(undivided.begin(), undivided.end(),
                temp.begin(), temp.end(), std::inserter(s1, s1.begin()));
            undivided.swap(s1);
        }
        if (undivided.size())
            s.insert(undivided);
        break;
    }
}

void NFAToDFA::generate_end_nodes(std::map<std::set<StateNode*>, StateNode*>& mp) {
    for (auto& se : mp) {
        // 因为终结节点的集合里一定没有非终结节点
        StateNode* n = *(se.first.begin());
        if (s_ends.find(n) != s_ends.end())
            simplified_end_nodes.insert(se.second);
    }
}
