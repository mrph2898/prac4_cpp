#include "lexer.h"
int main(){
    try
    {
        TokenSequence ts;
        for (const auto &s : ts) {
          std::cout << "Position number " << s.get_pos() << '\t';
          std::cout << s.get_lexem() << std::endl;
        }
    } catch (std::runtime_error &re) {
        std::cout << "\x1b[31m" << re.what() << "\x1b[0m" << std::endl;
    }
}
