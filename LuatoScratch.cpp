#include <bits/stdc++.h>
using namespace std;
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

};