#pragma once
// �Ӽ������㷨���������㷨
#include <queue>
#include <utility>
#include <algorithm>
#include <iterator>
#include "REToNFA.h"


// �� �� �հ�
void eps_closure(StateNode* node, std::set<StateNode*>& v){
    v.insert(node);
    std::queue<StateNode*> que;
    que.push(node);
    while (que.size()) {
        StateNode *n = que.back();
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
    // DFA����С��
    void hopcroft();
    // ���DFA��С����Ľڵ�
    std::vector<StateNode*> simplified_nodes;
    // �����С��DFA�е��ս�ڵ�
    std::set<StateNode*> simplified_end_nodes;
private:
    StateNode* nfa_start;       // NFA����ʼ�ڵ�
    StateNode* nfa_end;         // NFA���ս�ڵ�

    StateNode* dfa_start;
    // ���DFA�Ľڵ�
    std::vector<StateNode*> nodes;
    // ��������ս�ڵ�
    std::set<StateNode*> s_ends;
    
    // �����Ӽ�
    void split(std::set<std::set<StateNode*>>& s);
    // ����ӳ���ϵ������СDFA���ս�ڵ�
    void generate_end_nodes(std::map<std::set<StateNode*>, StateNode*>& mp);
    // ������СDFA��ʼ�ڵ�
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
    StateNode::statnum = 0;
    // �����洢���ϣ������ж��Ƿ���ֹ��Լ��ü��϶�Ӧ��DFA�ڵ�
    std::map<std::set<StateNode*>, std::pair<StateNode*, bool>> Q;   
    std::queue<std::set<StateNode*>> worklist;
    std::set<StateNode*> s;
    // ������ʼ�ڵ�� �űհ�
    eps_closure(nfa_start, s);
    worklist.push(s);

    dfa_start = new StateNode();
    Q.insert(std::make_pair(s, std::make_pair(dfa_start, false)));
    nodes.push_back(dfa_start);
    // ���s�а�����ֹ�ڵ㣬����
    if (s.find(nfa_end) != s.end())
        s_ends.insert(dfa_start);

    while (worklist.size()) {
        s = worklist.front();
        worklist.pop();
        if (Q[s].second == true)
            continue;
        Q[s].second = true;
        // �������ASICC �ַ��� 0Ϊ ��
        for (char c = 1; c < CHAR_MAX; ++c) {
            std::set<StateNode*> vec;
            // ��s�����г���Ϊc�Ľڵ����vec
            delta(s, c, vec);
            if (vec.empty())
                continue;
            std::set<StateNode*> eps_set;
            // �����г���Ϊc�Ħűհ�
            for (auto n : vec)
                eps_closure(n, eps_set);
         
            // ����Q�У�������һ���µ��Ӽ�����Ҫpush
            // ���ߣ�����һ��Q�����е��Ӽ����ǻ�û�б�����Ҳ��Ҫpush
            if (Q.find(eps_set) == Q.end() || 
                (Q.find(eps_set) != Q.end() && Q[eps_set].second == false))
                worklist.push(eps_set);

            StateNode* cur_node = Q[s].first;
            // �������Ӽ�����,���ô����½ڵ�
            if (Q.find(eps_set) != Q.end()) {
                combine(cur_node, c, Q[eps_set].first);
            }
            else {
                // ����ڵ�
                StateNode* next = new StateNode();
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
    std::set<StateNode*> accepted(s_ends);
    std::set<StateNode*> non_accepted;
    for (auto node : nodes)
        if (accepted.find(node) == accepted.end())
            non_accepted.insert(node);

    std::set<std::set<StateNode*>> s;
    s.insert(accepted);
    s.insert(non_accepted);

    // ����Ӽ������ˣ���һ�����࣬size�ı�
    int old_size = s.size();
    do {
        old_size = s.size();
        split(s);
    } while (old_size != s.size());

    // �����ڵ�
    std::map<std::set<StateNode*>, StateNode*> mapping; // ÿ���Ӽ���Ӧ����СDFA�Ľڵ�
    StateNode::statnum = 0;
    for (auto& se : s) {
        StateNode* t = new StateNode();
        simplified_nodes.push_back(t);
        mapping[se] = t;
    }

    // ������СDFA���ս�ڵ�
    generate_end_nodes(mapping);
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

void NFAToDFA::split(std::set<std::set<StateNode*>>& s) {
    
    for (auto p = s.begin(); p != s.end(); ++p) {
        if (p->size() == 1)
            continue;
        // ��һ��setΪ���������Ӽ���charΪ���ߵ��ַ�
        // vector��ž�����ͬ�ĳ����ַ�ͬʱ������ڵ�����ͬ���Ӽ�
        std::map<std::pair<std::set<StateNode*>,char>, std::vector<StateNode*>> mmp;
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
        std::set<StateNode*> undivided(*p);     // δ���ָ�ģ�ʣ��Ľڵ�ļ���
        s.erase(p);                             // ɾ��ԭ����
        
        for (auto& pa : mmp) {
            std::set<StateNode*> temp(pa.second.begin(), pa.second.end());
            s.insert(temp);

            // ���ϵ��������undividedΪʣ��ڵ�
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
        // ��Ϊ�ս�ڵ�ļ�����һ��û�з��ս�ڵ�
        StateNode* n = *(se.first.begin());
        if (s_ends.find(n) != s_ends.end())
            simplified_end_nodes.insert(se.second);
    }
}
