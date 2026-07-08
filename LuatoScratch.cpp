#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
//==========SETUP==========//


enum TokenType{
    TOKEN_LOCAL,       // "local"
    TOKEN_IDENTIFIER,  // Variable names like "x" or "message"
    TOKEN_ASSIGN,      // "="
    TOKEN_DIVIDER,     // ","
    TOKEN_STRING,      // "Hello world"
    TOKEN_NUMBER,      // '10'
    TOKEN_LPAREN,      // '('
    TOKEN_RPAREN,      // ')'
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
            if (current == ','){
                tokens.push_back({TOKEN_DIVIDER, ","});
                advance();
                continue;
            }
            if (isdigit(current)){
                string num(1, advance());
                while (isdigit(peek())) num += advance();
                tokens.push_back({TOKEN_NUMBER, num});
                continue;
            }
            if (isalpha(current) || current == '_'){
                string str(1, advance());
                while (isalnum(peek()) || peek() == '_') str += advance();
                if (str == "local") tokens.push_back({TOKEN_LOCAL, ""}); 
                else tokens.push_back({TOKEN_IDENTIFIER, str});
                continue;
            }
            if (current == '"'){
                advance();
                string str = "";
                while (peek() != '"'){
                    str += advance();
                    if (peek() == '\0') throw runtime_error("I think you forgot to close a string");
                }
                advance();
                tokens.push_back({TOKEN_STRING, str});
                continue;
                
            }
            if (current == '('){
                advance();
                tokens.push_back({TOKEN_LPAREN, "("});
                continue;
            }
            if (current == ')'){
                advance();
                tokens.push_back({TOKEN_RPAREN, ")"});
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
                consume(TOKEN_ASSIGN, "equals sign (=)");
                Token val_tok = peek();
                if (val_tok.type == TOKEN_NUMBER || val_tok.type == TOKEN_STRING) index++;
                else throw runtime_error("Expected a number or string value after '='");
                target_sprite.local_variables[name_tok.value] = val_tok.value;
            } else if (peek().type == TOKEN_IDENTIFIER){
                Token name_tok = consume(TOKEN_IDENTIFIER, "variable or token name");
                if (peek().type == TOKEN_ASSIGN){
                    consume(TOKEN_ASSIGN, "equals sign(=)");
                    Token val_tok = peek();
                    if (val_tok.type == TOKEN_STRING && (name_tok.value == "x" || name_tok.value == "y" || name_tok.value == "direction")) throw runtime_error("strings can't be used for x, y, or direction");
                    if (val_tok.type == TOKEN_NUMBER || val_tok.type == TOKEN_STRING) index++;
                    else throw runtime_error("Expected number or string after '='");
                    if (name_tok.value == "x") target_sprite.x = stod(val_tok.value);
                    else if (name_tok.value == "y") target_sprite.y = stod(val_tok.value);
                    else if (name_tok.value == "direction") target_sprite.direction = stod(val_tok.value);
                    else target_sprite.local_variables[name_tok.value] = val_tok.value;
                } else if (peek().type == TOKEN_LPAREN){
                    consume(TOKEN_LPAREN, "opening parenthesis '('");
                    vector<Token> arguments;
                    while (peek().type != TOKEN_RPAREN && peek().type != TOKEN_EOF){
                        Token val_tok = peek();
                        if (val_tok.type == TOKEN_NUMBER || val_tok.type == TOKEN_STRING) {
                            arguments.push_back(val_tok);
                            index++;
                        }
                        else if (val_tok.type == TOKEN_DIVIDER) {
                            index++;
                        } 
                        else {
                            throw runtime_error("Unexpected token inside function arguments");
                        }
                    }
                    consume(TOKEN_RPAREN, "closing parenthesis ')'");
                    if (name_tok.value == "move_steps") {
                        if (arguments.empty()) throw runtime_error("move_steps requires a number!");                       
                        target_sprite.x += stod(arguments[0].value);
                    } 
                    else {
                        throw runtime_error("Unknown function: " + name_tok.value);
                    }
                } else throw runtime_error("idk what to put here but either I or you did something wrong");
            } else index++;
        }
    }
};

string generatejson(const vector<Sprite*>& all_sprites) {
    json project;
    project["meta"]["semver"] = "3.0.0";
    project["meta"]["vm"] = "0.2.0";
    json targets_list = json::array();
    json stage;
    stage["isStage"] = true;
    stage["name"] = "Stage";
    stage["variables"] = json::object();
    targets_list.push_back(stage); 
    for (Sprite* sprite_ptr : all_sprites) {
        if (sprite_ptr == nullptr) continue;
        json sprite_obj;
        sprite_obj["isStage"] = false;
        sprite_obj["name"] = sprite_ptr->name;
        sprite_obj["x"] = sprite_ptr->x;
        sprite_obj["y"] = sprite_ptr->y;
        sprite_obj["direction"] = sprite_ptr->direction;
        sprite_obj["variables"] = json::object();
        for (const auto& pair : sprite_ptr->local_variables) {
            string var_name = pair.first;   
            string var_value = pair.second; 
            string unique_id = "var_" + sprite_ptr->name + "_" + var_name; 
            sprite_obj["variables"][unique_id] = { var_name, var_value };
        }
        targets_list.push_back(sprite_obj);
    }
    project["targets"] = targets_list;       
    return project.dump(4);
}