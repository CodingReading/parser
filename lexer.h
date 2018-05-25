#pragma once
#include <string>

class lexer {
public:
    lexer(std::string s):input(s), index(0){}
    
    int get_current_char() {
        return input[index++];
    }

    int get_input_length() {
        return input.length();
    }
private:
    std::string input;
    int index;
};
