#include "REToNFA.h"
#include "NFAToDFA.h"
#include "parseToken.h"

int main() {
    std::string s;
    std::cout << "输入正规式： ";
    std::cin >> s;
    REToNFA re_to_nfa = REToNFA(s);
    re_to_nfa.thompson();
    re_to_nfa.printNFA();

    NFAToDFA nfa_to_dfa = NFAToDFA(re_to_nfa);
    nfa_to_dfa.subset_construction();
    nfa_to_dfa.printDFA();

    nfa_to_dfa.hopcroft();
    nfa_to_dfa.printf_simplifiedDFA();

    std::cout << "输入待解析的字符串: ";
    std::cin >> s;
    Lexer lex = Lexer(s);
    ParseToken parse = ParseToken(&nfa_to_dfa);
    std::cout << parse.nextToken(&lex) << std::endl;

    system("pause");
    return 0;
}



