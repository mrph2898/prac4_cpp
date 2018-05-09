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
    char operation;
    pNode left_node;
    pNode right_node;
    Node(std::pair<long, const std::string &> t, char op = 'N', std::string s = " ") : type(t), lexem(s), operation(op),
            left_node(nullptr), right_node(nullptr) {}
};

struct Tree
{
    pNode root;
    Tree() : root(nullptr) {}
    void print_expr(pNode &);
    void print_tree(pNode &);
};

void add_operator(pNode &cur_root, Node &n)
{
    n.left_node = cur_root;
    cur_root = std::make_shared<Node>(n);
}
void add_left(pNode &cur_root, Node &n)
{
    (*cur_root).left_node = std::make_shared<Node>(n);
}
void add_right(pNode &cur_root, Node &n)
{
    (*cur_root).right_node = std::make_shared<Node>(n);
}
void first_node(pNode &cur_root, Node &n) { cur_root = std::make_shared<Node>(n); }

void Tree::print_expr(pNode &pn)
{
    if (pn->left_node == nullptr && pn->right_node == nullptr) {
        std::cout << pn->lexem;
        return;
    }
    print_expr(pn->left_node);
    if (pn->operation == 'B') {
        std::cout << '[';
    } else if (pn->operation == 'A') {
        std::cout << '=';
    }
    print_expr(pn->right_node);
    if (pn->operation == 'B') {
        std::cout << ']';
    }
}

void Tree::print_tree(pNode &pn)
{
    if (pn == nullptr) { return; }
    if ((pn->left_node == nullptr) && (pn->right_node == nullptr) ) {
        if (pn == root) {
            std::cout<< pn->lexem << "\t:" << pn->type.second << std::endl;
        }
        return;
    }
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
    Node Expr();
    std::pair<bool, Node> Subexpr();
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
        Node r = Expr();
        if (c_type != LEX_END) {
            std::string h = "Incorrect expression after position ";
            h.append(std::to_string(c_position));
            throw std::runtime_error(h);
        }
        abstract_syntax_tree.root = std::make_shared<Node>(r);
    }
}

Node Parser::Expr()
{
    pNode c_root;
    if (c_type == LEX_NUM || c_type == LEX_ID) {
        auto n = Subexpr();
        c_root = std::make_shared<Node>(n.second);
        while (c_type == LEX_ASSIGN) {
            if (n.first == true) {
                std::string h = "Assign to const value in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            Node u(std::make_pair(0, n.second.type.second), 'A');
            add_operator(c_root, u);
            gl();
            auto r_n = Subexpr();
            if (c_root->type.second != r_n.second.type.second) {
                std::string h = "Not equal types in assignment in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            add_right(c_root, r_n.second);
        }
    } else {
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
    return *c_root;
}

std::pair<bool, Node> Parser::Subexpr()
{
    pNode c_root;
    std::pair<long, std::string> type;
    if (c_type == LEX_NUM) {
        Node n(std::make_pair<long, std::string>(0, "Int"), 'N', c_lexem);
        gl();
        return std::make_pair(true, n);
    } else if (c_type == LEX_ID) {
        type = check_id();
        Node n(type, 'N', c_lexem);
        c_root = std::make_shared<Node>(n);
        gl();
        while (c_type == LEX_LBR) {
            if (c_root->type.first == 0) {
                std::string h = "Can't take an index in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            std::string new_type = c_root->type.second;
            std::string t;
            if ((c_root->type.first - 1) != 0) {
                new_type.erase(0, new_type.find('-'));
                t = std::to_string(c_root->type.first - 1);
                t.append(new_type);
            } else {
                t = new_type.erase(0, (new_type.find('f') + 2));
            }
            Node u(std::make_pair((c_root->type.first - 1), t), 'B');
            add_operator(c_root, u);
            gl();
            Node sub_exp = Expr();
            if (c_type != LEX_RBR) {
                std::string h = "Incorrect expression in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            if (sub_exp.type.second != "Int") {
                std::string h = "Indexing by not-int expression in brackets, that is ended in position ";
                h.append(std::to_string(c_position));
                throw std::runtime_error(h);
            }
            add_right(c_root, sub_exp);
            gl();
        }
    } else {
        std::string h = "Incorrect expression in position ";
        h.append(std::to_string(c_position));
        throw std::runtime_error(h);
    }
    return std::make_pair(false, *c_root) ;
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
                    h = "Undefined array in position ";
                    h.append(std::to_string(c_position));
                    throw std::runtime_error(h);
                }
            }
            switch (*lex_it) {
            case 'i':
            case 'j':
            case 'k':
                type = std::to_string(dim);
                type.append("-dimensional array of Int");
                state = 'I';
                break;
            case 's':
            case 't':
                type = std::to_string(dim);
                state = 'S';
                type.append("-dimensional array of String");
                break;
            case 'f':
                type = std::to_string(dim);
                type.append("-dimensional array of Function");
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
                if (parametrs == 0) {
                    type.append(", returning ");
                    parametrs = 1;
                } else if (parametrs == 1) {
                    type.append(" with parametrs: ");
                    parametrs = 2;
                } else {
                    type.append(", ");
                }
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
                type.append(std::to_string(dim_in_func));
                type.append("-dimensional array of Int");
                break;
            case 's':
            case 't':
                type.append(std::to_string(dim_in_func));
                type.append("-dimensional array of String");
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
