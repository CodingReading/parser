#pragma once
#include <stack>
#include <vector>
#include <set>
#include "lexer.h"
#include "node.h"


// Thompson算法
class REToNFA{
public:
    REToNFA(std::string s) {
        lex = new Lexer(s);
        start = nullptr;
        end = nullptr;
    }

    StateNode* get_start() {
        return start;
    }

    StateNode* get_end() {
        return end;
    }
    void thompson();
    void printNFA();
private:
    Lexer* lex;
    StateNode* start;       // 指向NFA头部
    StateNode* end;         // 指向NFA尾部
    // 存储所有创建的节点
    std::vector<StateNode*> v;
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
    std::stack<StateNode*> st_start;
    std::stack<StateNode*> st_end;
    std::stack<char> st_char;

    start = new StateNode();
    end = new StateNode();

    v.push_back(start);
    v.push_back(end);

    st_start.push(start);
    st_end.push(end);
    st_char.push('$');
    int len = lex->get_input_length();

    bool is_escape_char = false;

    while (len--) {
        char ch = lex->get_next_char();
        if (ch == '\\') {
            is_escape_char = true;
            continue;
        }

        if (is_escape_char) {
            is_escape_char = false;
            goto escape_char;
        }

        switch (ch)
        {
        case '(':
        {
            StateNode* n = new StateNode();
            v.push_back(n);
            st_end.push(n);
            st_char.push(ch);
            break;
        }
        case ')':
        {
            StateNode* st = st_start.top();
            StateNode* ed = st_end.top();

            combine(st, EPSILON, ed);
            ch = st_char.top();

            while (ch != '(') {
                StateNode* next = st_start.top();
                st_start.pop();
                StateNode* prev = st_start.top();
                if (ch != '@')
                    combine(prev, ch, next);
                st_char.pop();
                ch = st_char.top();
            }

            st_char.pop();
            // ()一个完整语句，用@分隔
            st_char.push('@');
            st_start.push(st_end.top());
            st_end.pop();
            break;
        }
        case '|':
        {
            StateNode* st = st_start.top();
            StateNode* ed = st_end.top();
            combine(st, EPSILON, ed);
            ch = st_char.top();
            while (ch != '(' && ch != '$') {
                StateNode* next = st_start.top();
                st_start.pop();
                StateNode* prev = st_start.top();
                if (ch != '@')
                    combine(prev, ch, next);
                st_char.pop();
                ch = st_char.top();
            }
            break;
        }
        case '*':
        {
            StateNode* next = st_start.top();
            st_start.pop();
            StateNode* prev = st_start.top();
            combine(prev, EPSILON, next);
            combine(next, EPSILON, prev);
            st_start.push(next);
            break;
        }
        default:
        escape_char:
            StateNode* n = new StateNode();
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
        StateNode* next = st_start.top();
        st_start.pop();
        StateNode* prev = st_start.top();
        if (ch != '@')
            combine(prev, ch, next);
        st_char.pop();
        ch = st_char.top();
    }
}
