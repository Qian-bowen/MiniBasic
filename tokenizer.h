#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>
#include <string>

class Tokenizer{
public:
    void token_convert(std::string str);
private:
    std::string stmt_type;
    std::vector<std::string> exp_token;
};

#endif // TOKENIZER_H
