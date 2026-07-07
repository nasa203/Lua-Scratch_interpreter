#include <bits/stdc++.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
//==========SETUP==========//


enum TokenType{
    TOKEN_LOCAL,       // "local"
    TOKEN_IDENTIFIER,  // Variable names like "x" or "message"
    TOKEN_ASSIGN,      // "="
    TOKEN_STRING,      // "Hello world"
    TOKEN_NUMBER,      // '10'
    TOKEN_EOF          // End of File
};
struct Token{
    TokenType type;
    string value;
};

class Sprite{
public:
    string name;
    double x = 0.0;
    double y = 0.0;
    double size = 100.0;
    double direction = 90.0;
    unordered_map<string, string> local_variables;
    Sprite(string sprite_name) : name(sprite_name) {}
    virtual ~Sprite() = default;
    virtual void update() = 0;
};

//==========PREPROCESSING==========//

class lexer{
private:
    size_t pos;
    string src;
    char peek(){
        if (pos >= src.size()) return '\0';
        return src[pos];
    }
    char advance(){
        if (pos >= src.length()) return '\0';
        return src[pos++];
    }
public:
    lexer(string source) : src(source), pos(0) {}
    vector<Token> tokenize(){
        vector<Token> tokens;
        while (peek() != '\0'){
            char current = peek();
            if (isspace(current)){
                advance();
                continue;
            }
            if (current == '='){
                tokens.push_back({TOKEN_ASSIGN, "="});
                advance();
                continue;
            }
            if (isdigit(current)){
                string num;
                while (isdigit(peek())) num += advance();
                tokens.push_back({TOKEN_NUMBER, num});
                continue;
            }
            if (isalpha(current) || current == '_'){
                string str;
                while (isalnum(peek()) || peek() == '_') str += advance();
                if (str == "local") tokens.push_back({TOKEN_LOCAL, ""}); 
                else tokens.push_back({TOKEN_IDENTIFIER, str});
                continue;
            }
        }

        tokens.push_back({TOKEN_EOF, ""});
        return tokens;
    }

};
class parser{
private:
    size_t index = 0;
    vector<Token> tokens;
    Token peek(){
        if (index >= tokens.size()) return {TOKEN_EOF, ""};
        return tokens[index];
    }
    Token consume(TokenType expect, string tokenasastr){
        Token current = peek();
        if (current.type == expect){
            return tokens[index++];
        }
        throw runtime_error("Expected " + tokenasastr + ", got " + current.value);
    }
public:
    parser(vector<Token> tokens_list) : tokens(tokens_list), index(0) {}
    void parse(Sprite& target_sprite){
        while (peek().type != TOKEN_EOF){
            if (peek().type == TOKEN_LOCAL){
                consume(TOKEN_LOCAL, "local");
                Token name_tok = consume(TOKEN_IDENTIFIER, "variable name");
                consume(TOKEN_ASSIGN, "equal sign (=)");
                Token val_tok = peek();
                if (val_tok.type == TOKEN_NUMBER || val_tok.type == TOKEN_STRING) index++;
                else throw runtime_error("Expected a number or string value after '='");
                target_sprite.local_variables[name_tok.value] = val_tok.value;
            } else index++;
        }
    }
};

class generator{
  string json = "{\n  \"variables\": {\n";  
};