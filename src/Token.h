#pragma once
#include <string>
#include <iostream>

enum class TokenType {
    // Literals
    TOK_EOF = -1,
    TOK_IDENTIFIER = -2,
    TOK_NUMBER = -3,
    TOK_STRING = -4,
    
    // Keywords
    TOK_FUNC = -5,
    TOK_LET = -6,
    TOK_RETURN = -7,
    TOK_IF = -8,
    TOK_ELSE = -9,
    TOK_CLASS = -10,
    TOK_NEW = -11,
    TOK_IMPORT = -12,
    TOK_FROM = -13,
    TOK_PUB = -14,
    TOK_SELF = -15,
    TOK_INT = -16,
    TOK_PRINTLN = -17,
    TOK_EXIT_SUCCESS = -18,
    
    // Operators
    TOK_PLUS = -20,
    TOK_MINUS = -21,
    TOK_MULTIPLY = -22,
    TOK_DIVIDE = -23,
    TOK_ASSIGN = -24,
    TOK_EQUAL = -25,
    TOK_NOT_EQUAL = -26,
    TOK_LESS = -27,
    TOK_GREATER = -28,
    TOK_ARROW = -29, // ->
    
    // Delimiters
    TOK_LPAREN = -30,
    TOK_RPAREN = -31,
    TOK_LBRACE = -32,
    TOK_RBRACE = -33,
    TOK_SEMICOLON = -34,
    TOK_COMMA = -35,
    TOK_COLON = -36,
    TOK_DOT = -37,
    TOK_DOUBLE_QUOTE = -38,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l = 0, int c = 0) 
        : type(t), value(v), line(l), column(c) {}
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::TOK_EOF: return "EOF";
        case TokenType::TOK_IDENTIFIER: return "IDENTIFIER";
        case TokenType::TOK_NUMBER: return "NUMBER";
        case TokenType::TOK_STRING: return "STRING";
        case TokenType::TOK_FUNC: return "FUNC";
        case TokenType::TOK_LET: return "LET";
        case TokenType::TOK_RETURN: return "RETURN";
        case TokenType::TOK_IF: return "IF";
        case TokenType::TOK_ELSE: return "ELSE";
        case TokenType::TOK_CLASS: return "CLASS";
        case TokenType::TOK_NEW: return "NEW";
        case TokenType::TOK_IMPORT: return "IMPORT";
        case TokenType::TOK_FROM: return "FROM";
        case TokenType::TOK_PUB: return "PUB";
        case TokenType::TOK_SELF: return "SELF";
        case TokenType::TOK_INT: return "INT";
        case TokenType::TOK_PRINTLN: return "PRINTLN";
        case TokenType::TOK_EXIT_SUCCESS: return "EXIT_SUCCESS";
        case TokenType::TOK_PLUS: return "PLUS";
        case TokenType::TOK_MINUS: return "MINUS";
        case TokenType::TOK_MULTIPLY: return "MULTIPLY";
        case TokenType::TOK_DIVIDE: return "DIVIDE";
        case TokenType::TOK_ASSIGN: return "ASSIGN";
        case TokenType::TOK_EQUAL: return "EQUAL";
        case TokenType::TOK_NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::TOK_LESS: return "LESS";
        case TokenType::TOK_GREATER: return "GREATER";
        case TokenType::TOK_ARROW: return "ARROW";
        case TokenType::TOK_LPAREN: return "LPAREN";
        case TokenType::TOK_RPAREN: return "RPAREN";
        case TokenType::TOK_LBRACE: return "LBRACE";
        case TokenType::TOK_RBRACE: return "RBRACE";
        case TokenType::TOK_SEMICOLON: return "SEMICOLON";
        case TokenType::TOK_COMMA: return "COMMA";
        case TokenType::TOK_COLON: return "COLON";
        case TokenType::TOK_DOT: return "DOT";
        case TokenType::TOK_DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        default: return "UNKNOWN";
    }
} 