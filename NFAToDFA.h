#pragma once
// �Ӽ������㷨���������㷨
#include <queue>
#include <utility>
#include <algorithm>
#include <iterator>
#include "REToNFA.h"


// �� �� �հ�
void eps_closure(stateNode* node, std::set<stateNode*>& v){
    v.insert(node);
    std::queue<stateNode*> que;
    que.push(node);
    while (que.size()) {
        stateNode *n = que.back();
        que.pop();
        // ����EPSILON��
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
    void printf_simplifiedDFA();

    // DFA����С��
    void hopcroft();
private:
    stateNode* nfa_start;       // NFA����ʼ�ڵ�
    stateNode* nfa_end;         // NFA���ս�ڵ�

    stateNode* dfa_start;
    // ���DFA�Ľڵ�
    std::vector<stateNode*> nodes;
    // ��������ս�ڵ�
    std::set<stateNode*> s_ends;
    // ���DFA��С����Ľڵ�
    std::vector<stateNode*> simplified_nodes;
    void split(std::set<std::set<stateNode*>>& s);
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

// �������㷨
void NFAToDFA::subset_construction() {
    stateNode::statnum = 0;
    // �����洢���ϣ������ж��Ƿ���ֹ��Լ��ü��϶�Ӧ��DFA�ڵ�
    std::map<std::set<stateNode*>, std::pair<stateNode*, bool>> Q;   
    std::queue<std::set<stateNode*>> worklist;
    std::set<stateNode*> s;
    // ������ʼ�ڵ�� �űհ�
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
        // �������ASICC �ַ��� 0Ϊ ��
        for (char c = 1; c < CHAR_MAX; ++c) {
            std::set<stateNode*> vec;
            // ��s�����г���Ϊc�Ľڵ����vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<stateNode*> eps_set;
            // �����г���Ϊc�Ħűհ�
            for (auto n : vec)
                eps_closure(n, eps_set);
         
            // ����Q�У�������һ���µ��Ӽ�����Ҫpush
            // ���ߣ�����һ��Q�����е��Ӽ����ǻ�û�б�����Ҳ��Ҫpush
            if (Q.find(eps_set) == Q.end() || 
                (Q.find(eps_set) != Q.end() && Q[eps_set].second == false))
                worklist.push(eps_set);

            stateNode* cur_node = Q[s].first;
            // �������Ӽ�����,���ô����½ڵ�
            if (Q.find(eps_set) != Q.end()) {
                combine(cur_node, c, Q[eps_set].first);
            }
            else {
                // ����ڵ�
                stateNode* next = new stateNode();
                nodes.push_back(next);
                // ��������а���nfa_end����Ϊ��ֹ�ڵ�
                if (eps_set.find(nfa_end) != eps_set.end())
                    s_ends.insert(next);
                Q.insert(make_pair(eps_set, std::make_pair(next, false)));
                cur_node->m[c].insert(next);
            }        
        }
    }
}


void NFAToDFA::hopcroft() {
    // �Ӽ�����Ϊaccepted ״̬���ͷ�accepted״̬
    std::set<stateNode*> accepted(s_ends);
    std::set<stateNode*> non_accepted;
    for (auto node : nodes)
        if (accepted.find(node) == accepted.end())
            non_accepted.insert(node);

    std::set<std::set<stateNode*>> s;
    s.insert(accepted);
    s.insert(non_accepted);

    // ����Ӽ������ˣ���һ�����࣬size�ı�
    int old_size = s.size();
    do {
        old_size = s.size();
        split(s);
    } while (old_size != s.size());

    // �����ڵ�
    std::map<std::set<stateNode*>, stateNode*> mapping; // ÿ���Ӽ���Ӧ����СDFA�Ľڵ�
    stateNode::statnum = 0;
    for (auto& se : s) {
        stateNode* t = new stateNode();
        simplified_nodes.push_back(t);
        mapping[se] = t;
    }

    // �����Ӽ����ӽڵ�
    for (auto& se : mapping)
        for (auto node : se.first)
            for (auto& pa : node->m)
                for (auto t : pa.second)
                    if (se.first.find(t) == se.first.end()) {
                        // ��������Լ������Ӽ����ҵ����������Ӽ�
                        for (auto& p1 : mapping)
                            if (p1.first.find(t) != p1.first.end())
                                combine(mapping[se.first], pa.first, mapping[p1.first]);
                    }
                    else {
                        // ���Լ������Ӽ�,�ر�
                        combine(mapping[se.first], pa.first, mapping[se.first]);
                    }


}

void NFAToDFA::split(std::set<std::set<stateNode*>>& s) {
    
    for (auto p = s.begin(); p != s.end(); ++p) {
        if (p->size() == 1)
            continue;
        // ��һ��setΪ���������Ӽ���charΪ���ߵ��ַ�
        // vector��ž�����ͬ�ĳ����ַ�ͬʱ������ڵ�����ͬ���Ӽ�
        std::map<std::pair<std::set<stateNode*>,char>, std::vector<stateNode*>> mmp;
        bool changed = false;

        for (auto ss : *p) {
            for (auto edge : ss->m)
                for (auto out : edge.second) {
                    // ����������Լ������Ӽ�����
                    if (p->find(out) != p->end())
                        continue;
                    // ��������Լ������Ӽ����ҵ����������Ӽ�
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

        // ����Ӽ��仯�ˣ������Ӽ�����ֹѭ��
        std::set<stateNode*> undivided(*p);     // δ���ָ�ģ�ʣ��Ľڵ�ļ���
        s.erase(p);                             // ɾ��ԭ����
        
        for (auto& pa : mmp) {
            std::set<stateNode*> temp(pa.second.begin(), pa.second.end());
            s.insert(temp);

            // ���ϵ��������undividedΪʣ��ڵ�
            std::set<stateNode*> s1;
            std::set_difference(undivided.begin(), undivided.end(),
                temp.begin(), temp.end(), std::inserter(s1, s1.begin()));
            undivided.swap(s1);
        }
        if (undivided.size())
            s.insert(undivided);
        break;
    }
}