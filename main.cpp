#include "REToNFA.h"

int main() {
    std::string s;
    std::cout << "��������ʽ�� ";
    std::cin >> s;
    REToNFA r = REToNFA(s);
    r.thompson();
    r.printNFA();
    system("pause");
    return 0;
}



