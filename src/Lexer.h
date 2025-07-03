#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Lexer {
private:
    std::string input;
    size_t position;
    size_t current;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_set<std::string> builtinMethods;
    
    void initializeKeywords();
    void initializeBuiltinMethods();
    char peek();
    char peekNext();
    char advance();
    void skipWhitespace();
    void skipComment();
    Token string();
    Token number();
    Token identifier();
    Token stringLiteral(char quote);
    Token templateString();
    bool isAtEnd();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    bool match(char expected);
    
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();
    Token nextToken();
}; 