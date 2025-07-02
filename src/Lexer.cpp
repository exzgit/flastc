#include "Lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {
    initKeywords();
}

void Lexer::initKeywords() {
    keywords["func"] = TokenType::TOK_FUNC;
    keywords["let"] = TokenType::TOK_LET;
    keywords["return"] = TokenType::TOK_RETURN;
    keywords["if"] = TokenType::TOK_IF;
    keywords["else"] = TokenType::TOK_ELSE;
    keywords["class"] = TokenType::TOK_CLASS;
    keywords["new"] = TokenType::TOK_NEW;
    keywords["import"] = TokenType::TOK_IMPORT;
    keywords["from"] = TokenType::TOK_FROM;
    keywords["pub"] = TokenType::TOK_PUB;
    keywords["self"] = TokenType::TOK_SELF;
    keywords["int"] = TokenType::TOK_INT;
    keywords["println"] = TokenType::TOK_PRINTLN;
    keywords["EXIT_SUCCESS"] = TokenType::TOK_EXIT_SUCCESS;
}

char Lexer::peek(size_t offset) {
    size_t pos = position + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

char Lexer::advance() {
    if (position >= source.length()) {
        return '\0';
    }
    
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (position < source.length() && std::isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        // Skip single line comment
        while (position < source.length() && peek() != '\n') {
            advance();
        }
        if (peek() == '\n') {
            advance();
        }
    }
}

Token Lexer::readNumber() {
    std::string number;
    size_t startColumn = column;
    
    while (position < source.length() && (std::isdigit(peek()) || peek() == '.')) {
        number += advance();
    }
    
    return Token(TokenType::TOK_NUMBER, number, line, startColumn);
}

Token Lexer::readString() {
    std::string str;
    size_t startColumn = column;
    
    advance(); // Skip opening quote
    
    while (position < source.length() && peek() != '"') {
        if (peek() == '\\') {
            advance(); // Skip backslash
            char escaped = advance();
            switch (escaped) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case 'r': str += '\r'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                default: str += escaped; break;
            }
        } else {
            str += advance();
        }
    }
    
    if (peek() == '"') {
        advance(); // Skip closing quote
    } else {
        throw std::runtime_error("Unterminated string literal at line " + std::to_string(line));
    }
    
    return Token(TokenType::TOK_STRING, str, line, startColumn);
}

Token Lexer::readIdentifier() {
    std::string identifier;
    size_t startColumn = column;
    
    while (position < source.length() && 
           (std::isalnum(peek()) || peek() == '_')) {
        identifier += advance();
    }
    
    // Check if it's a keyword
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token(it->second, identifier, line, startColumn);
    }
    
    return Token(TokenType::TOK_IDENTIFIER, identifier, line, startColumn);
}

Token Lexer::readOperator() {
    size_t startColumn = column;
    char c = peek();
    
    switch (c) {
        case '+': advance(); return Token(TokenType::TOK_PLUS, "+", line, startColumn);
        case '*': advance(); return Token(TokenType::TOK_MULTIPLY, "*", line, startColumn);
        case '/': advance(); return Token(TokenType::TOK_DIVIDE, "/", line, startColumn);
        case '(': advance(); return Token(TokenType::TOK_LPAREN, "(", line, startColumn);
        case ')': advance(); return Token(TokenType::TOK_RPAREN, ")", line, startColumn);
        case '{': advance(); return Token(TokenType::TOK_LBRACE, "{", line, startColumn);
        case '}': advance(); return Token(TokenType::TOK_RBRACE, "}", line, startColumn);
        case ';': advance(); return Token(TokenType::TOK_SEMICOLON, ";", line, startColumn);
        case ',': advance(); return Token(TokenType::TOK_COMMA, ",", line, startColumn);
        case ':': advance(); return Token(TokenType::TOK_COLON, ":", line, startColumn);
        case '.': advance(); return Token(TokenType::TOK_DOT, ".", line, startColumn);
        case '=':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::TOK_EQUAL, "==", line, startColumn);
            }
            return Token(TokenType::TOK_ASSIGN, "=", line, startColumn);
        case '-':
            advance();
            if (peek() == '>') {
                advance();
                return Token(TokenType::TOK_ARROW, "->", line, startColumn);
            }
            return Token(TokenType::TOK_MINUS, "-", line, startColumn);
        case '<': advance(); return Token(TokenType::TOK_LESS, "<", line, startColumn);
        case '>': advance(); return Token(TokenType::TOK_GREATER, ">", line, startColumn);
        case '!':
            advance();
            if (peek() == '=') {
                advance();
                return Token(TokenType::TOK_NOT_EQUAL, "!=", line, startColumn);
            }
            break;
    }
    
    throw std::runtime_error("Unknown character '" + std::string(1, c) + "' at line " + 
                           std::to_string(line) + ", column " + std::to_string(column));
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (position >= source.length()) {
        return Token(TokenType::TOK_EOF, "", line, column);
    }
    
    // Skip comments
    if (peek() == '/' && peek(1) == '/') {
        skipComment();
        return nextToken();
    }
    
    char c = peek();
    
    if (std::isdigit(c)) {
        return readNumber();
    }
    
    if (c == '"') {
        return readString();
    }
    
    if (std::isalpha(c) || c == '_') {
        return readIdentifier();
    }
    
    return readOperator();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    Token token = nextToken();
    while (token.type != TokenType::TOK_EOF) {
        tokens.push_back(token);
        token = nextToken();
    }
    tokens.push_back(token); // Add EOF token
    
    return tokens;
} 