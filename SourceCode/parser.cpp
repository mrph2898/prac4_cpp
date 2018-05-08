#include "lexer.h"
#include <list>
#include <utility>
#include <memory>

struct Node;
typedef std::shared_ptr<Node> pNode;

struct Node
{
    std::pair<long, std::string> type;
    std::string lexem;
    pNode left_node;
    pNode right_node;
    Node(std::pair<long, std::string> t, std::string s = " ") : type(t), lexem(s),
            left_node(nullptr), right_node(nullptr) {}
};

struct Tree
{
    pNode cur_root;
    pNode root;
    Tree() : cur_root(nullptr), root(cur_root) {}
    void add_operator(Node &n)
    {
        n.left_node = cur_root;
        cur_root = std::make_shared<Node>(n);
    }
    void add_left(Node &n)
    {
        (*cur_root).left_node = std::make_shared<Node>(n);
    }
    void add_right(Node &n)
    {
        (*cur_root).right_node = std::make_shared<Node>(n);
    }
    void first_node(Node &n) { cur_root = std::make_shared<Node>(n); }
    void print_expr(pNode);
    void print_tree(pNode);
};

void Tree::print_expr(pNode pn)
{
    if (pn->left_node == nullptr && pn->right_node == nullptr) {
        std::cout << pn->lexem;
        return;
    }
    print_expr(pn->left_node);
    if (pn->type.second == "BR") {
        std::cout << '[';
    } else if (pn->type.second == "AS") {
        std::cout << '=';
    }
    print_expr(pn->right_node);
    if (pn->type.second == "BR") {
        std::cout << ']';
    }
}

void Tree::print_tree(pNode pn)
{
    if (pn == nullptr) {return;}
    if ((pn->left_node == nullptr) && (pn->right_node == nullptr) ) { return; }
    print_tree(pn->left_node);
    print_expr(pn->left_node);
    std::cout << "\t:" << pn->left_node->type.second << std::endl;
    print_tree(pn->right_node);
    print_expr(pn->right_node);
    std::cout << "\t:" << pn->right_node->type.second << std::endl;
    if (pn == root) {
        print_expr(pn);
        std::cout << "\t:" << pn->right_node->type.second << std::endl;
    }
}

class Parser
{
    TokenSequence ts;
    TokenIterator ti;
    std::string c_lexem;
    type_of_lex c_type;
    long c_position;
    Tree abstract_syntax_tree;
    bool first_elem = true;
    long main_assign_sequence = 0;
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
    std::pair<bool, std::string> Subexpr();
    std::pair<long, std::string> check_id();
    void analyse()
    {
        S();
        std::cout << "Correct!" << std::endl;
        abstract_syntax_tree.print_tree(abstract_syntax_tree.root);
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
    if (c_type == LEX_NUM || c_type == LEX_ID) {
        auto left_type = Subexpr();
        while (c_type == LEX_ASSIGN) {
            if (left_type.first == true) {
                std::string h = "Assign to const value in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            Node n(std::make_pair<long, std::string>(0, "AS"), c_lexem);
            abstract_syntax_tree.add_operator(n);
            if (main_assign_sequence == 0 && first_elem) {
                abstract_syntax_tree.root = abstract_syntax_tree.cur_root;
            }
            main_assign_sequence++;
            gl();
            left_type = Subexpr();
            main_assign_sequence--;
            if (main_assign_sequence == 0 && first_elem) {
                abstract_syntax_tree.cur_root = abstract_syntax_tree.root;
            }
        }
    } else {
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
}

std::pair<bool, std::string> Parser::Subexpr()
{
    std::pair<long, std::string> type;
    if (c_type == LEX_NUM) {
        Node n(std::make_pair<long, std::string>(0, "Int"), c_lexem);
        if (abstract_syntax_tree.root == nullptr) {
            abstract_syntax_tree.first_node(n);
        } else {
            abstract_syntax_tree.add_right(n);
        }
        gl();
        return std::make_pair(true, "Int");
    } else if (c_type == LEX_ID) {
        type = check_id();
        Node n(type, c_lexem);
        if (abstract_syntax_tree.cur_root == nullptr) {
            abstract_syntax_tree.first_node(n);
        } else {
            abstract_syntax_tree.add_right(n);
            abstract_syntax_tree.cur_root = abstract_syntax_tree.cur_root->right_node;
        }
        gl();
//        if (c_type == LEX_END) { return false; }
        while (c_type == LEX_LBR) {
            first_elem = false;
            if (abstract_syntax_tree.cur_root->type.first == 0) {
                std::string h = "Can't take an index in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            Node n(std::make_pair<long, std::string>((abstract_syntax_tree.cur_root->type.first - 1), "BR"), c_lexem);
            abstract_syntax_tree.add_operator(n);
            if (main_assign_sequence == 0) {
                abstract_syntax_tree.root = abstract_syntax_tree.cur_root;
            }
            gl();
            Expr();
            if (c_type != LEX_RBR) {
                std::string h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            if (main_assign_sequence == 1) {
                abstract_syntax_tree.cur_root = abstract_syntax_tree.root->right_node;
            } else if (main_assign_sequence == 0) {
                abstract_syntax_tree.cur_root = abstract_syntax_tree.root;
            }
            first_elem = true;
            gl();
        }
    } else {
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
    return std::make_pair(false, type.second) ;
}

std::pair<long, std::string> Parser::check_id()
{
    std::string h;
    auto lex_it = c_lexem.begin();
    std::string type;
    long dim = 0;
    bool undef_array = false;
    long dim_in_func;
    char state = 'B';
    bool undef_brackets = false;
    int parametrs = 0;
    while (lex_it != c_lexem.end()) {
        switch (state) {
        case 'B':
            switch (*lex_it) {
            case 'i':
            case 'j':
            case 'k':
                type = "Int";
                state = 'I';
                break;
            case 's':
            case 't':
                type = "String";
                state = 'S';
                break;
            case 'a':
                dim++;
                state = 'A';
                undef_array = true;
                break;
            case 'f':
                type = "Function";
                state = 'F';
                break;
            default:
                h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            lex_it++;
            c_position++;
            break;
        case 'I':  //Here and in 'S' automat come, if there is no empty string after 'i',...,'t'
        case 'S':
            h = "Incorrect expression in position ";
            h.append(std::to_string(c_position));
            throw std::runtime_error(h);
            break;
        case 'A':
            while (*lex_it == 'a') {
                dim++;
                lex_it++;
                c_position++;
                if (lex_it == c_lexem.end()) {
                    h = "Undefined array in poooosition ";
                    h.append(std::to_string(c_position));
                    throw std::runtime_error(h);
                }
            }
            switch (*lex_it) {
            case 'i':
            case 'j':
            case 'k':
                type = std::to_string(dim);
                type.append("-dimensional array of Ints");
                state = 'I';
                break;
            case 's':
            case 't':
                type = std::to_string(dim);
                state = 'S';
                type.append("-dimensional array of Strings");
                break;
            case 'f':
                type = std::to_string(dim);
                type.append("-dimensional array of Functions");
                state = 'F';
                break;
            default:
                h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            undef_array = false;
            lex_it++;
            c_position++;
            break;
        case 'F':
            switch (*lex_it) {
            case 'i':
            case 'j':
            case 'k':
                undef_brackets = false;
                if (parametrs == 0) {
                    type.append(", returning Int");
                    parametrs = 1;
                } else if (parametrs == 1) {
                    type.append(" with parametrs: Int");
                    parametrs = 2;
                } else {
                    type.append(", Int");
                }
                break;
            case 's':
            case 't':
                undef_brackets = false;
                if (parametrs == 0) {
                    type.append(", returning String");
                    parametrs = 1;
                } else if (parametrs == 1) {
                    type.append(" with parametrs: String");
                    parametrs = 2;
                } else {
                    type.append(", String");
                }
                break;
            case 'a':
                state = 'O';
                type.append(", returning");
                break;
            default:
                h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            lex_it++;
            c_position++;
            break;
        case 'O':
            dim_in_func = 1;
            while (*lex_it == 'a') {
                dim_in_func++;
                lex_it++;
                c_position++;
                if (lex_it == c_lexem.end()) {
                    h = "Undefined array in position ";
                    h.append(std::to_string(c_position));
                    throw std::runtime_error(h);
                }
            }
            switch (*lex_it) {
            case 'i':
            case 'j':
            case 'k':
                type = std::to_string(dim_in_func);
                type.append("-dimensional array of Ints");
                break;
            case 's':
            case 't':
                type = std::to_string(dim_in_func);
                type.append("-dimensional array of Strings");
                break;
            case 'f':
            default:
                h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            state = 'F';
            lex_it++;
            c_position++;
            undef_brackets = true;
            break;
        }
    }
    if (undef_array) {
        h = "Undefined array in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
    if (undef_brackets) {}
    return std::make_pair(dim, type);
}

int main() {
    TokenSequence t;
    Parser p(t);
    try {
        p.analyse();
    } catch (std::runtime_error &re) {
        std::cout << re.what() << std::endl;
    }
/*    try {
        p.check_id();
    } catch (std::runtime_error &re) {
        std::cout << re.what() << std::endl;
    }*/
    /*Tree a;
    Node n({0, "int"}, "i");
    Node k({0, "BR"}, "[]");
    Node l({0, "int"}, "j");
    Node m({0, "AS"}, "=");
    Node b({0, "int"}, "k");
    a.first_node(n);
    a.add_operator(k);
    a.root = a.cur_root;
    a.add_right(m);
    a.cur_root = a.cur_root->right_node;
    a.add_left(l);
    a.add_right(b);
    a.print_tree(a.root);*/
}
