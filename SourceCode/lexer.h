#include "iterable.h"
#include <string>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

enum type_of_lex
{
    LEX_NULL,
    LEX_NUM,
    LEX_ID,
    LEX_LBR,
    LEX_RBR,
    LEX_ASSIGN,
    LEX_END
};

class Token
{
    type_of_lex type_name;
    std::string lexem;
    long position;
public:
    Token() : type_name(LEX_NULL), lexem(" "), position(0) {}
    Token(type_of_lex type, const std::string &lex, long pos) : type_name(type), lexem(lex), position(pos) {}
    std::string get_lexem() const noexcept { return lexem; }
    type_of_lex get_type() const noexcept { return type_name; }
    void set_lexem(std::string lex) noexcept { lexem = lex; }
    void set_type(type_of_lex type_) noexcept { type_name = type_; }
    long get_pos() const noexcept { return position; }
};

class TokenIterator : public std::iterator<
                                std::input_iterator_tag,
                                Token,
                                long,
                                TokenIterator*,
                                TokenIterator&>
{
    Token token;
    static long position_of_cin;
    bool is_it_eof;
public:
    Token get_token()
    {
        int c;
        type_of_lex type;
        std::string lexem;
        long position = position_of_cin;
        char state = 'S';
        while (state != 'H') {
            c = std::cin.peek();
            switch (state) {
            case 'S':
                c = std::cin.get();
                position++;
                if (!isspace(c)){
                    //position = position_of_cin;
                    if (isdigit(c)) {
                        lexem.push_back(c);
                        state = 'N';
                    } else if (isalpha(c)) {
                        lexem.push_back(c);
                        state = 'I';
                    } else if ((c == '[') || (c == ']')) {
                        if (c == '[') {
                            type = LEX_LBR;
                        } else {
                            type = LEX_RBR;
                        }
                        lexem.push_back(c);
                        state = 'H';
                    } else if (c == '=') {
                        type = LEX_ASSIGN;
                        lexem.push_back(c);
                        state = 'H';
                    } else if (std::cin.eof()) {
                        is_it_eof = true;
                        type = LEX_END;
                        state = 'H';
                    } else {
                        std::string h = "Unexpected character <<";
                        h.push_back(c);
                        h.append(">> in position ");
                        h.append(std::to_string(position));
                        throw std::runtime_error(h);
                    }
                }
                position_of_cin = position;
                break;
            case 'N':
                if (isdigit(c)) {
                    c = std::cin.get();
                    position_of_cin++;
                    lexem.push_back(c);
                } else {
                    type = LEX_NUM;
                    state = 'H';
                }
                break;
            case 'I':
                if (isalnum(c)) {
                    c = std::cin.get();
                    position_of_cin++;
                    lexem.push_back(c);
                } else {
                    type = LEX_ID;
                    state = 'H';
                }
                break;
            }
        }
        return Token(type, lexem, position);
    }
    TokenIterator() {}
    TokenIterator(bool eof)
    {
        if (!eof) {
            Token cur_tok = get_token();
            token = cur_tok;
            is_it_eof = eof;
        }
    }
    Token operator *() const noexcept  { return token; }
    TokenIterator& operator ++() {
        Token cur_tok = get_token();
        token = cur_tok;
        return *this;
    }
    TokenIterator& operator ++(int) noexcept {
        ++(*this);
        return *this;
    }
    bool operator ==(const TokenIterator &ti) const noexcept {
        return is_it_eof == ti.is_it_eof;
    }
    bool operator !=(const TokenIterator &ti) const noexcept {
        return !(*this == ti);
    }

};

long TokenIterator::position_of_cin = 0;

class TokenSequence : public Iterable <
                      Token,
                      TokenIterator,
                      const TokenIterator>
{
public:
    TokenIterator begin() { return TokenIterator(0); }
    TokenIterator end() { return TokenIterator(1); }
    const TokenIterator begin() const { return TokenIterator(0); }
    const TokenIterator end() const { return TokenIterator(1); }
    IgnoreIterator<Token, TokenIterator>
    filter(std::function<bool(const Token&)> p) {
        return IgnoreIterator<Token, TokenIterator>((*this).begin(), (*this).end(), p);
    }
};

