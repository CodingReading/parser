#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include "lexer.h"

#define EPSILON 1       // 对应空串ε
#define EMPTY   2       // 无出边

// NFA结点
// 每个节点最多两个出边
class stateNode {
public:
    static int statnum;
    stateNode* next1, *next2;
    char edge1, edge2;
    stateNode() :id(statnum++), next1(nullptr), next2(nullptr),
    edge1(EMPTY), edge2(EMPTY){}

    int get_id() {
        return id;
    }

private:
    int id;
};

int stateNode::statnum = 0;

// 连接两个节点
void combine(stateNode* first, char type, stateNode* last) {
    if (first->next1 == nullptr) {
        first->next1 = last;
        first->edge1 = type;
    }
    else {
        first->next2 = last;
        first->edge2 = type;
    }
}

// Thompson算法
class REToNFA{
public:
    REToNFA(std::string s) {
        lex = new lexer(s);
        start = nullptr;
        end = nullptr;
    }

    void thompson();
    void printNFA();
private:
    lexer* lex;
    stateNode* start;       // 指向NFA头部
    stateNode* end;         // 指向NFA尾部
    // 存储所有创建的节点
    std::vector<stateNode*> v;

    void printNFA_core(stateNode* prev, char ch, stateNode* next);
};

void REToNFA::printNFA() {
    for (auto node : v) {
        if (node->next1 != nullptr)
            printNFA_core(node, node->edge1, node->next1);
        if (node->next2 != nullptr)
            printNFA_core(node, node->edge2, node->next2);
    }
}

void REToNFA::printNFA_core(stateNode* prev, char ch, stateNode* next) {
       
    if (ch == EMPTY)
        return;

    std::cout << prev->get_id() << " --> ";
    if (ch == EPSILON) {
        std::cout << "ε" << " --> " << next->get_id() << std::endl;
    }
    else {
        std::cout << ch << " --> " << next->get_id() << std::endl;
    }

    return;
}

/* thompson算法，RE转NFA
   RE只有三种情况，连接、选择、闭包
   ()|*这四种字符需要特殊处理

   遇到字符，创建新节点直接压栈
   遇到(，说明肯定有)配对，创建一个新节点为尾节点，遇到)后用ε边连接
   遇到)，连接节点，之前遇到(创建的尾节点为现在的起始节点
   遇到*，连接两条ε边即可
   */

void REToNFA::thompson() {
    std::stack<stateNode*> st_start;
    std::stack<stateNode*> st_end;
    std::stack<char> st_char;

    start = new stateNode();
    end = new stateNode();

    v.push_back(start);
    v.push_back(end);

    st_start.push(start);
    st_end.push(end);
    st_char.push('@');
    int len = lex->get_input_length();

    while (len--) {
        char ch = lex->get_current_char();
        
        switch (ch)
        {
        case '(':
        {
            stateNode* n = new stateNode();
            v.push_back(n);
            st_end.push(n);
            st_char.push(ch);
            break;
        }
        case ')':
        {
            stateNode* st = st_start.top();
            stateNode* ed = st_end.top();

            combine(st, EPSILON, ed);
            ch = st_char.top();

            while (ch != '(') {
                stateNode* next = st_start.top();
                st_start.pop();
                stateNode* prev = st_start.top();
                combine(prev, ch, next);
                st_char.pop();
                ch = st_char.top();
            }

            st_char.pop();
            st_start.push(st_end.top());
            st_end.pop();
            break;
        }
        case '|':
        {
            stateNode* st = st_start.top();
            stateNode* ed = st_end.top();
            combine(st, EPSILON, ed);
            ch = st_char.top();
            while (ch != '(' && ch != '@') {
                stateNode* next = st_start.top();
                st_start.pop();
                stateNode* prev = st_start.top();
                combine(prev, ch, next);
                st_char.pop();
                ch = st_char.top();
            }
            break;
        }
        case '*':
        {
            stateNode* next = st_start.top();
            st_start.pop();
            stateNode* prev = st_start.top();
            combine(prev, EPSILON, next);
            combine(next, EPSILON, prev);
            st_start.push(next);
            break;
        }
        default:
            stateNode* n = new stateNode();
            v.push_back(n);
            st_start.push(n);
            st_char.push(ch);
            break;
        }
    }

    if (st_start.top()->next1 == nullptr) {
        st_start.top()->next1 = end;
        st_start.top()->edge1 = EPSILON;
    }
    else {
        st_start.top()->next2 = end;
        st_start.top()->edge2 = EPSILON;
    }

    char ch = st_char.top();
    // 连接剩余的节点
    while (ch != '@') {
        stateNode* next = st_start.top();
        st_start.pop();
        stateNode* prev = st_start.top();
        combine(prev, ch, next);
        st_char.pop();
        ch = st_char.top();
    }
}
