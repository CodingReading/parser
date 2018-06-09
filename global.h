#pragma once
#include <string>
#include <map>
#include <sstream>

const std::string digit_without_zero = "1|2|3|4|5|6|7|8|9";
const std::string digit = "0|1|2|3|4|5|6|7|8|9";
const std::string letter = "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";

const std::string identifier = letter + "(" + letter + "|" + digit + ")*";
const std::string zero = "0";
const std::string num = zero + "|" + "(" + digit_without_zero + ")" + "(" + digit + ")*";

const std::string text = "begin x:=9;if x>0 then x:=2*x+1/3;end#";

int start = 0;

std::map<std::string, int> mapping{
    {"#", ++start},
    {identifier, ++start},     // 写在前面，后面有相同的在parse函数中会替换
    {num, ++start},
    {"begin", ++start},
    {"if", ++start},
    {"len", ++start},
    {"while", ++start},
    {"do", ++start},
    {"end", ++start},
    {"+", ++start},
    {"-", ++start},
    {"\\*", ++start},
    {"/", ++start},
    {":", ++start},
    {":=", ++start},
    {"<", ++start},
    {"<>", ++start},
    {"<=", ++start},
    {">", ++start},
    {">=", ++start},
    {"=", ++start},
    {";", ++start},
    {"<", ++start},
    {">", ++start},
    {"\\(", ++start},
    {"\\)", ++start},
    {"\\|", ++start}
};


NFAToDFA* createDFA(std::string s) {
    REToNFA* r = new REToNFA(s);
    r->thompson();
    // r->printNFA();
    NFAToDFA* n = new NFAToDFA(r);
    n->subset_construction();
    // n->printDFA();
    n->hopcroft();
    // n->printf_simplifiedDFA();
    return n;
}

void parse(std::map<NFAToDFA*, int>& m, std::vector<std::pair<std::string, int >>& v) {

    std::istringstream iss(text);
    std::string s;
    while (iss >> s) {
        // 每次读入一个用空格分开的字符串分析
        // 把字符扔到每个自动机去分析，取出分析出的最长的字符
        int curpos = 0;
        NFAToDFA* nfa = nullptr;
        while (curpos < s.length()) {
            int max_length = 0;
            for (auto& p : m) {
                ParseToken parse_token = ParseToken(p.first);
                int len = parse_token.get_token_length(s.substr(curpos, s.length()));
                // 求出能解析出的最长的串
                if (len >= max_length) {
                    max_length = len;
                    nfa = p.first;
                }
            }

            // 分析出的token存到v中
            v.push_back(std::make_pair(s.substr(curpos, max_length), m[nfa]));
            curpos += max_length;
        }
    }
}

void print_tokens(std::vector<std::pair<std::string, int>>& v) {
    for (auto& p : v)
        std::cout << "<" << p.second << ", " << p.first << " >" << std::endl;
}


// 初始化所有自动机
void init() {
    std::map<NFAToDFA*, int> m;
    for (auto& p : mapping)
        m[createDFA(p.first)] = mapping[p.first];

    // 存储识别出的token
    std::vector<std::pair<std::string, int >> v;

    parse(m, v);
    print_tokens(v);
}



