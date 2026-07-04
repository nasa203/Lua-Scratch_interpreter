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