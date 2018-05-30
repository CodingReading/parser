#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include "lexer.h"

#define EPSILON 0   

// NFA结点
// 每个节点最多两个出边
class stateNode {
public:
    stateNode() :id(statnum++) {}
    std::map<char, std::set<stateNode*>> m;

    static int statnum;

    int get_id() {
        return id;
    }
private:
    int id;
};

int stateNode::statnum = 0;

void combine(stateNode* pre, char c, stateNode* next) {
    (pre->m)[c].insert(next);
}

void print_relationships(stateNode* node) {
    for (auto& p: node->m) {
        for (auto& s : p.second) {
            std::cout << node->get_id() << " --> ";
            if (p.first == EPSILON)
                std::cout << "ε";
            else
                std::cout << p.first;
            std::cout << " --> " << s->get_id() << std::endl;
        }
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

    stateNode* get_start() {
        return start;
    }

    stateNode* get_end() {
        return end;
    }
    void thompson();
    void printNFA();
private:
    lexer* lex;
    stateNode* start;       // 指向NFA头部
    stateNode* end;         // 指向NFA尾部
    // 存储所有创建的节点
    std::vector<stateNode*> v;
};

void REToNFA::printNFA() {
    std::cout << "------------------RE to NFA------------------" << std::endl;
    for (auto node : v)
        print_relationships(node);
    std::cout << std::endl;
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
    st_char.push('$');
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
                if (ch != '#')
                    combine(prev, ch, next);
                st_char.pop();
                ch = st_char.top();
            }

            st_char.pop();
            // ()一个完整语句，用#分隔
            st_char.push('#');
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
                if (ch != '#')
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

    combine(st_start.top(), EPSILON, end);

    char ch = st_char.top();
    // 连接剩余的节点
    while (ch != '$') {
        stateNode* next = st_start.top();
        st_start.pop();
        stateNode* prev = st_start.top();
        if (ch != '#')
            combine(prev, ch, next);
        st_char.pop();
        ch = st_char.top();
    }
}
