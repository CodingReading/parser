#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include "lexer.h"

#define EPSILON 0   

// NFA���
// ÿ���ڵ������������
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
                std::cout << "��";
            else
                std::cout << p.first;
            std::cout << " --> " << s->get_id() << std::endl;
        }
    }
}

// Thompson�㷨
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
    stateNode* start;       // ָ��NFAͷ��
    stateNode* end;         // ָ��NFAβ��
    // �洢���д����Ľڵ�
    std::vector<stateNode*> v;
};

void REToNFA::printNFA() {
    std::cout << "------------------RE to NFA------------------" << std::endl;
    for (auto node : v)
        print_relationships(node);
    std::cout << std::endl;
}

/* thompson�㷨��REתNFA
   REֻ��������������ӡ�ѡ�񡢱հ�
   ()|*�������ַ���Ҫ���⴦��

   �����ַ��������½ڵ�ֱ��ѹջ
   ����(��˵���϶���)��ԣ�����һ���½ڵ�Ϊβ�ڵ㣬����)���æű�����
   ����)�����ӽڵ㣬֮ǰ����(������β�ڵ�Ϊ���ڵ���ʼ�ڵ�
   ����*�����������ű߼���
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
            // ()һ��������䣬��#�ָ�
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
    // ����ʣ��Ľڵ�
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
