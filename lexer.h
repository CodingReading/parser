#pragma once
#include <string>

class Lexer{
public:
    Lexer(std::string s):input(s), index(0){}
    
    int get_next_char() {
        return input[index++];
    }

    std::string get_substr(int i, int j) {
        return input.substr(0, j);
    }

    int get_input_length() {
        return input.length();
    }

    void set_pos(int i) {
        index = i;
    }

    int get_pos() {
        return index;
    }
private:
    std::string input;
    int index;
};
