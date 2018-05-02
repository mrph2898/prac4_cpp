#include "lexer.h"
#include <list>
#include <utility>
class Parser
{
    TokenSequence ts;
    TokenIterator ti;
    std::string c_lexem;
    type_of_lex c_type;
    long c_position;
    std::list<std::pair<std::string, std::string>> expressions;
public:
    Parser(const TokenSequence &tok_s) : ts(tok_s)
    {
        ti = ts.begin();
        c_lexem = (*ti).get_lexem();
        c_type = (*ti).get_type();
        c_position = (*ti).get_pos();
    }
    void gl()
    {
        ti++;
        c_lexem = (*ti).get_lexem();
        c_type = (*ti).get_type();
        c_position = (*ti).get_pos();
    }
    void S();
    void Expr();
    int Ident();
    int Array();
    void Function(char c = 0);
    void analyse()
    {
//        std::cout << (*ti).get_lexem() << std::endl;
        S();
        std::cout << "Correct!" << std::endl;
    }
};

void Parser::S()
{
    if (c_type != LEX_END) {
        Expr();
    }
}

void Parser::Expr()
{
    if (c_type == LEX_NUM) {
        expressions.push_back({c_lexem, "Integer"});
    } else if (c_type == LEX_ID) {
        Ident();
        gl();
        while (c_type == LEX_LBR) {
            gl();
            Expr();
            gl();
            if (c_type != LEX_RBR) {
                std::string h = "Expected <<]>> in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
        }
        while (c_type == LEX_ASSIGN) {
            gl();
            Expr();
        }
    } else {
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
}

int Parser::Ident() // return value = 0, if Ident != Array|Function,
                    //              = n, where n - dimension of Array
                    //              = -1, if Ident = Function
{
    auto c = *c_lexem.begin();
    switch (c) {
    case 'i':
    case 'j':
    case 'k':
    case 's':
    case 't':
        if (c_lexem.begin()++ != c_lexem.end()) {
            std::string h = "Incorrect expression in position ";
            h.append(std::to_string(c_position));
            throw std::runtime_error(h);
        } else {
            if ((c != 's') && (c != 't')) {
                expressions.push_back({c_lexem, "Integer"});
            } else {
                expressions.push_back({c_lexem, "String"});
            }
            return 0;
        }
        break;
    case 'a':
        return Array();
        break;
    case 'f':
        Function();
        return -1;
        break;
    default:
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
        break;
    }
    return -2;
}

int Parser::Array()
{
    int n = 1;
    c_lexem.erase(0, c_lexem.find('a'));
    auto c = c_lexem.begin();
    while (c != c_lexem.end()) {
        switch (*c) {
        case 'a':
            n++;
            c++;
            break;
        case 'i':
        case 'j':
        case 'k':
        case 's':
        case 't':
            if (c++ != c_lexem.end()) {
                std::string h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            } else {
                std::string tl;
                tl.append(std::to_string(n));
                tl.append("-dimensional array of ");
                if ((*c != 's') && (*c != 't')) {
                    tl.append("integers");
                } else {
                    tl.append("strings");
                }
                expressions.push_back({c_lexem, tl});
                return n;
            }
            break;
        case 'f':
            //std::string t2 = " ";
            //t2.append(std::to_string(n));
            //t2.append("-dimensional array of functions, that return ");
            Function(*c);
            return n;
            break;
        default:
            std::string m = "Incorrect expression in position ";
            c_position += n;
            m.append(std::to_string(c_position));
            throw std::runtime_error(m);
            break;
        }
   }
   return -1;
}
/*
void Parser::Function(char c = 0)
{
    c_lexem.erase(0, c_lexem.find('f') + 1);
    auto ci = c_lexem.begin();
    while (ci != c_lexem.end()) {
        switch (*ci) {
        case 'a':
            if (c != 0) {
                std::string h = "Incorrect expression in position ";
                c_position += (c_lexem.begin() - ci);
                h.append(std::to_string(c_position));
            } else {
                Array();
            }
            break;
        case 'i':
        case 'j':
        case 'k':
        case 's':
        case 't':

}
*/
int main() {
    TokenSequence t;
    Parser p(t);
    p.analyse();
}
