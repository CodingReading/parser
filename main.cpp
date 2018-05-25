#include "REToNFA.h"

int main() {
    std::string s;
    std::cout << "输入正规式： ";
    std::cin >> s;
    REToNFA r = REToNFA(s);
    r.thompson();
    r.printNFA();
    system("pause");
    return 0;
}



