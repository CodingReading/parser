#include "REToNFA.h"
#include "NFAToDFA.h"
#include "parseToken.h"
#include "global.h"
#include <chrono>

int main() {

    auto start = std::chrono::system_clock::now();     
    init();
    auto end = std::chrono::system_clock::now(); 
    std::chrono::duration<double> diff = end - start;
    std::cout << "run " << diff.count() * 1000 << "ms" << std::endl;
    system("pause");
    return 0;
}








