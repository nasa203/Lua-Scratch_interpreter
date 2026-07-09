#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <ctime>
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
    vector<Block> blocks;
    unordered_map<string, string> local_variables;
    Sprite(string sprite_name) : name(sprite_name) {}
    virtual ~Sprite() = default;
    virtual void update() = 0;
};

class Block{
    string id;
    string opcode;
    string next_id = "";
    string parent_id = "";
    vector<string> inputs;
};

//==========PREPROCESSING==========//

class lexer{
private:
    size_t pos;
    string src;
    size_t blocks = 0;
    vector<string> funny_blocks = {
        "block_of_regret",
        "im_just_a_block_standing_here",
        "sentient_but_lazy_block",
        "block_that_should_have_been_a_sphere",
        "error_404_block_not_found",
        "unpaid_intern_block",
        "emotional_support_block",
        "block_with_existential_dread",
        "congratulations_its_a_block",
        "please_dont_delete_this_block",
        "block_i_made_instead_of_sleeping",
        "load_bearing_cardboard_block",
        "just_another_block"
        "schrodingers_block"
        "definitely_a_block"
        "block_placeholder_final_v2"
        "unidentified_flying_block"
        "not_a_cube_just_a_block"
        "block_that_could_have_been_an_email",
        "i_have_no_idea_what_this_block_does",
        "blocky_mcblockface",
        "look_mom_i_made_a_block",
        "block_with_commitment_issues",
        "mediocre_block_at_best",
        "procrastination_block_alpha",
        "this_block_is_judging_you",
        "barely_functioning_block",
        "block_holding_the_whole_app_together",
        "not_my_problem_block",
        "spaghetti_code_block_v3",
        "block_asking_for_a_friend",
        "definitely_not_three_blocks_in_a_trenchcoat",
        "unwarranted_confidence_block",
        "block_under_qualified_for_this_job",
        "temporary_block_since_2022",
        "you_spin_me_right_round_baby_like_a_block",
        "block_experiencing_burnout",
        "just_trust_me_it_is_a_block",
        "passive_aggressive_block",
        "block_trying_its_best_okay",
        "it_works_on_my_machine_block",
        "the_chosen_block",
        "sweet_dreams_are_made_of_blocks"
        "block_that_refuses_to_elaborate",
        "glorified_cube_id",
        "this_block_requires_coffee",
        "shhh_the_block_is_sleeping",
        "block_with_imposter_syndrome",
        "somebody_save_this_block",
        "block_highly_likely_to_break",
        "absolute_unit_of_a_block",
        "block_is_over_it",
        "ctrl_c_ctrl_v_block",
        "block_manifesting_success",
        "legacy_block_nobody_dares_touch",
        "block_hiding_from_the_compiler",
        "wild_block_appeared",
        "block_of_netherite"
        "unsolicited_block_pic",
        "block_running_on_vibes_alone",
        "expensive_way_to_store_nothing_block",
        "block_suffering_from_success",
        "you_had_one_job_block",
        "block_complaining_about_the_weather",
        "is_it_a_block_is_it_a_plane",
        "block_that_knows_too_much",
        "parental_guidance_suggested_block",
        "final_final_actual_final_block"
        "lukewarm_block_of_tofu",
        "overpriced_artisanal_block",
        "petrified_wood_block",
        "chunk_of_expired_cheddar_block",
        "solid_lead_block_do_not_lift",
        "glowing_radioactive_uranium_block",
        "greasy_mechanics_engine_block",
        "frozen_tundra_dirt_block",
        "compressed_lint_block",
        "dense_neutron_star_block",
        "slippery_soap_block",
        "shattered_concrete_cinder_block",
        "block_made_entirely_of_smaller_blocks"
        "waxed_lightly_weathered_cut_copper_block"
    };
    string gen_unique_id(){
        mt19937 rng(static_cast<unsigned int>(time(nullptr)));
        uniform_int_distribution<int> dist(0, funny_blocks.size() - 1);
        int random_index = dist(rng);
        return funny_blocks[random_index] + "_" + to_string(blocks);
    }
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