#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <sstream>

Lexer::Lexer(const std::string& input) : input(input), position(0), current(0), line(1), column(1) {
    initializeKeywords();
    initializeBuiltinMethods();
}

void Lexer::initializeKeywords() {
    // FLAST-style function declaration
    keywords["func"] = TokenType::TOK_FUNC;
    keywords["return"] = TokenType::TOK_RETURN;
    keywords["public"] = TokenType::TOK_PUBLIC;
    keywords["private"] = TokenType::TOK_PRIVATE;
    keywords["protected"] = TokenType::TOK_PROTECTED;
    keywords["static"] = TokenType::TOK_STATIC;
    keywords["constant"] = TokenType::TOK_CONSTANT;
    keywords["mutable"] = TokenType::TOK_MUTABLE;
    keywords["unsafe"] = TokenType::TOK_UNSAFE;

    // Control flow
    keywords["if"] = TokenType::TOK_IF;
    keywords["else"] = TokenType::TOK_ELSE;
    keywords["elseif"] = TokenType::TOK_ELSEIF;
    keywords["while"] = TokenType::TOK_WHILE;
    keywords["for"] = TokenType::TOK_FOR;
    keywords["loop"] = TokenType::TOK_LOOP;
    keywords["break"] = TokenType::TOK_BREAK;
    keywords["continue"] = TokenType::TOK_CONTINUE;
    keywords["switch"] = TokenType::TOK_SWITCH;
    keywords["case"] = TokenType::TOK_CASE;
    keywords["default"] = TokenType::TOK_DEFAULT;

    // Data Types (Rust-like)
    keywords["struct"] = TokenType::TOK_STRUCT;
    keywords["enum"] = TokenType::TOK_ENUM;
    keywords["union"] = TokenType::TOK_UNION;
    keywords["impl"] = TokenType::TOK_IMPL;
    keywords["trait"] = TokenType::TOK_TRAIT;
    keywords["where"] = TokenType::TOK_WHERE;
    keywords["self"] = TokenType::TOK_SELF;
    keywords["Self"] = TokenType::TOK_SELF_TYPE;

    // Variables and types
    keywords["let"] = TokenType::TOK_LET;
    keywords["var"] = TokenType::TOK_VAR;
    keywords["auto"] = TokenType::TOK_AUTO;
    keywords["typeof"] = TokenType::TOK_TYPEOF;
    keywords["sizeof"] = TokenType::TOK_SIZEOF;
    
    // Primitive types
    keywords["int8"] = TokenType::TOK_INT8;
    keywords["int16"] = TokenType::TOK_INT16;
    keywords["int32"] = TokenType::TOK_INT32;
    keywords["int64"] = TokenType::TOK_INT64;
    keywords["int128"] = TokenType::TOK_INT128;
    keywords["uint8"] = TokenType::TOK_UINT8;
    keywords["uint16"] = TokenType::TOK_UINT16;
    keywords["uint32"] = TokenType::TOK_UINT32;
    keywords["uint64"] = TokenType::TOK_UINT64;
    keywords["uint128"] = TokenType::TOK_UINT128;
    keywords["float32"] = TokenType::TOK_FLOAT32;
    keywords["float64"] = TokenType::TOK_FLOAT64;
    keywords["char"] = TokenType::TOK_CHAR_TYPE;
    keywords["string"] = TokenType::TOK_STRING_TYPE;
    keywords["bool"] = TokenType::TOK_BOOL_TYPE;
    keywords["void"] = TokenType::TOK_VOID;
    keywords["ptr"] = TokenType::TOK_POINTER;

    // Collection types
    keywords["array"] = TokenType::TOK_ARRAY;
    keywords["list"] = TokenType::TOK_LIST;
    keywords["slice"] = TokenType::TOK_SLICE;
    keywords["map"] = TokenType::TOK_MAP;
    keywords["set"] = TokenType::TOK_SET;
    keywords["tuple"] = TokenType::TOK_TUPLE;
    keywords["option"] = TokenType::TOK_OPTION;
    keywords["result"] = TokenType::TOK_RESULT;

    // Import/Export (TypeScript-like)
    keywords["import"] = TokenType::TOK_IMPORT;
    keywords["from"] = TokenType::TOK_FROM;
    keywords["export"] = TokenType::TOK_EXPORT;
    keywords["module"] = TokenType::TOK_MODULE;
    keywords["as"] = TokenType::TOK_AS;
    keywords["use"] = TokenType::TOK_USE;
    keywords["mod"] = TokenType::TOK_MOD;
    keywords["crate"] = TokenType::TOK_CRATE;

    // Memory & Ownership (FLAST-style)
    keywords["box"] = TokenType::TOK_BOX;
    keywords["ref"] = TokenType::TOK_REF;
    keywords["deref"] = TokenType::TOK_DEREF;
    keywords["move"] = TokenType::TOK_MOVE;
    keywords["copy"] = TokenType::TOK_COPY;
    keywords["clone"] = TokenType::TOK_CLONE;
    keywords["drop"] = TokenType::TOK_DROP;
    keywords["new"] = TokenType::TOK_NEW;
    keywords["delete"] = TokenType::TOK_DELETE;

    // Concurrency
    keywords["async"] = TokenType::TOK_ASYNC;
    keywords["await"] = TokenType::TOK_AWAIT;
    keywords["spawn"] = TokenType::TOK_SPAWN;
    keywords["thread"] = TokenType::TOK_THREAD;
    keywords["mutex"] = TokenType::TOK_MUTEX;
    keywords["rwlock"] = TokenType::TOK_RWLOCK;
    keywords["channel"] = TokenType::TOK_CHANNEL;
    keywords["send"] = TokenType::TOK_SEND;
    keywords["sync"] = TokenType::TOK_SYNC;

    // External & FFI
    keywords["extern"] = TokenType::TOK_EXTERN;
    keywords["c"] = TokenType::TOK_C;
    keywords["cpp"] = TokenType::TOK_CPP;
    keywords["cdecl"] = TokenType::TOK_CDECL;
    keywords["stdcall"] = TokenType::TOK_STDCALL;
    keywords["fastcall"] = TokenType::TOK_FASTCALL;

    // Exception handling
    keywords["try"] = TokenType::TOK_TRY;
    keywords["catch"] = TokenType::TOK_CATCH;
    keywords["finally"] = TokenType::TOK_FINALLY;
    keywords["throw"] = TokenType::TOK_THROW;
    keywords["panic"] = TokenType::TOK_PANIC;
    keywords["unwrap"] = TokenType::TOK_UNWRAP;
    keywords["expect"] = TokenType::TOK_EXPECT;

    // Special keywords
    keywords["in"] = TokenType::TOK_IN;
    keywords["is"] = TokenType::TOK_IS;
    keywords["not"] = TokenType::TOK_NOT;
    keywords["and"] = TokenType::TOK_AND;
    keywords["or"] = TokenType::TOK_OR;
    keywords["xor"] = TokenType::TOK_XOR;
    keywords["true"] = TokenType::TOK_TRUE;
    keywords["false"] = TokenType::TOK_FALSE; 
    keywords["null"] = TokenType::TOK_NULL_VALUE;
    keywords["none"] = TokenType::TOK_NONE;
    keywords["some"] = TokenType::TOK_SOME;
    keywords["ok"] = TokenType::TOK_OK;
    keywords["err"] = TokenType::TOK_ERR;

    // Built-in functions
    keywords["assert"] = TokenType::TOK_ASSERT;
    keywords["debug_assert"] = TokenType::TOK_DEBUG_ASSERT;
    keywords["unreachable"] = TokenType::TOK_UNREACHABLE;
    keywords["todo"] = TokenType::TOK_TODO;
    keywords["unimplemented"] = TokenType::TOK_UNIMPLEMENTED;
}

void Lexer::initializeBuiltinMethods() {
    // Built-in methods that can be called on objects
    builtinMethods.insert("type");
    builtinMethods.insert("to_string");
    builtinMethods.insert("to_int");
    builtinMethods.insert("to_float");
    builtinMethods.insert("to_bool");
    builtinMethods.insert("length");
    builtinMethods.insert("size");
    builtinMethods.insert("is_empty");
    builtinMethods.insert("contains");
    builtinMethods.insert("starts_with");
    builtinMethods.insert("ends_with");
    builtinMethods.insert("split");
    builtinMethods.insert("join");
    builtinMethods.insert("trim");
    builtinMethods.insert("replace");
    builtinMethods.insert("push");
    builtinMethods.insert("pop");
    builtinMethods.insert("insert");
    builtinMethods.insert("remove");
    builtinMethods.insert("clear");
    builtinMethods.insert("sort");
    builtinMethods.insert("reverse");
    builtinMethods.insert("map");
    builtinMethods.insert("filter");
    builtinMethods.insert("reduce");
    builtinMethods.insert("fold");
    builtinMethods.insert("find");
    builtinMethods.insert("any");
    builtinMethods.insert("all");
    builtinMethods.insert("count");
    builtinMethods.insert("min");
    builtinMethods.insert("max");
    builtinMethods.insert("sum");
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::TOK_LINE_COMMENT && 
            token.type != TokenType::TOK_BLOCK_COMMENT) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(Token(TokenType::TOK_EOF, "", line, column));
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::TOK_EOF, "", line, column);
    }
    
    int startLine = line;
    int startColumn = column;
    char c = advance();
    
    // Single-line comments
    if (c == '/' && peek() == '/') {
        skipComment();
        return Token(TokenType::TOK_LINE_COMMENT, "", startLine, startColumn);
    }
    
    // Block comments
    if (c == '/' && peek() == '*') {
        advance(); // consume '*'
        std::string comment = "/*";
        while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
            comment += advance();
        }
        if (!isAtEnd()) {
            advance(); // consume '*'
            advance(); // consume '/'
            comment += "*/";
        }
        return Token(TokenType::TOK_BLOCK_COMMENT, comment, startLine, startColumn);
    }
    
    // Doc comments
    if (c == '/' && peek() == '/' && peekNext() == '/') {
        advance(); // consume second '/'
        advance(); // consume third '/'
        std::string comment = "///";
        while (!isAtEnd() && peek() != '\n') {
            comment += advance();
        }
        return Token(TokenType::TOK_DOC_COMMENT, comment, startLine, startColumn);
    }
    
    // Numbers (including scientific notation)
    if (isDigit(c)) {
        current--; column--; // backtrack
        return number();
    }
    
    // Strings
    if (c == '"') {
        return stringLiteral('"');
    }
    
    if (c == '\'') {
        return stringLiteral('\'');
    }
    
    // Raw strings r"..."
    if (c == 'r' && peek() == '"') {
        advance(); // consume '"'
        std::string value;
        while (!isAtEnd() && peek() != '"') {
            value += advance();
        }
        if (!isAtEnd()) advance(); // consume closing '"'
        return Token(TokenType::TOK_RAW_STRING, value, startLine, startColumn);
    }
    
    // Template strings (backticks)
    if (c == '`') {
        return templateString();
    }
    
    // Identifiers and keywords
    if (isAlpha(c) || c == '_') {
        current--; column--; // backtrack
        return identifier();
    }
    
    // Multi-character operators (must be checked before single character ones)
    if (c == '=' && peek() == '=') {
        advance();
        if (peek() == '=') {
            advance();
            return Token(TokenType::TOK_STRICT_EQUAL, "===", startLine, startColumn);
        }
        return Token(TokenType::TOK_EQUAL, "==", startLine, startColumn);
    }
    
    if (c == '!' && peek() == '=') {
        advance();
        if (peek() == '=') {
            advance();
            return Token(TokenType::TOK_STRICT_NOT_EQUAL, "!==", startLine, startColumn);
        }
        return Token(TokenType::TOK_NOT_EQUAL, "!=", startLine, startColumn);
    }
    
    if (c == '<' && peek() == '=') {
        advance();
        if (peek() == '>') {
            advance();
            return Token(TokenType::TOK_SPACESHIP, "<=>", startLine, startColumn);
        }
        return Token(TokenType::TOK_LESS_EQUAL, "<=", startLine, startColumn);
    }
    
    if (c == '>' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_GREATER_EQUAL, ">=", startLine, startColumn);
    }
    
    if (c == '<' && peek() == '<') {
        advance();
        if (peek() == '=') {
            advance();
            return Token(TokenType::TOK_LEFT_SHIFT_ASSIGN, "<<=", startLine, startColumn);
        }
        return Token(TokenType::TOK_LEFT_SHIFT, "<<", startLine, startColumn);
    }
    
    if (c == '>' && peek() == '>') {
        advance();
        if (peek() == '>') {
            advance();
            return Token(TokenType::TOK_UNSIGNED_RIGHT_SHIFT, ">>>", startLine, startColumn);
        }
        if (peek() == '=') {
            advance();
            return Token(TokenType::TOK_RIGHT_SHIFT_ASSIGN, ">>=", startLine, startColumn);
        }
        return Token(TokenType::TOK_RIGHT_SHIFT, ">>", startLine, startColumn);
    }
    
    if (c == '&' && peek() == '&') {
        advance();
        return Token(TokenType::TOK_LOGICAL_AND, "&&", startLine, startColumn);
    }
    
    if (c == '|' && peek() == '|') {
        advance();
        return Token(TokenType::TOK_LOGICAL_OR, "||", startLine, startColumn);
    }
    
    if (c == '+' && peek() == '+') {
        advance();
        return Token(TokenType::TOK_INCREMENT, "++", startLine, startColumn);
    }
    
    if (c == '-' && peek() == '-') {
        advance();
        return Token(TokenType::TOK_DECREMENT, "--", startLine, startColumn);
    }
    
    if (c == '*' && peek() == '*') {
        advance();
        if (peek() == '=') {
            advance();
            return Token(TokenType::TOK_POWER_ASSIGN, "**=", startLine, startColumn);
        }
        return Token(TokenType::TOK_POWER, "**", startLine, startColumn);
    }
    
    if (c == '-' && peek() == '>') {
        advance();
        return Token(TokenType::TOK_ARROW, "->", startLine, startColumn);
    }
    
    if (c == '=' && peek() == '>') {
        advance();
        return Token(TokenType::TOK_FAT_ARROW, "=>", startLine, startColumn);
    }
    
    if (c == ':' && peek() == ':') {
        advance();
        return Token(TokenType::TOK_SCOPE, "::", startLine, startColumn);
    }
    
    if (c == '.' && peek() == '.') {
        advance();
        if (peek() == '.') {
            advance();
            return Token(TokenType::TOK_TRIPLE_DOT, "...", startLine, startColumn);
        }
        return Token(TokenType::TOK_DOUBLE_DOT, "..", startLine, startColumn);
    }
    
    if (c == '?' && peek() == '.') {
        advance();
        return Token(TokenType::TOK_QUESTION_DOT, "?.", startLine, startColumn);
    }
    
    if (c == '?' && peek() == '?') {
        advance();
        return Token(TokenType::TOK_QUESTION_QUESTION, "??", startLine, startColumn);
    }
    
    if (c == '?' && peek() == ':') {
        advance();
        return Token(TokenType::TOK_ELVIS, "?:", startLine, startColumn);
    }
    
    if (c == '|' && peek() == '>') {
        advance();
        return Token(TokenType::TOK_PIPELINE, "|>", startLine, startColumn);
    }
    
    // Assignment operators
    if (c == '+' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_PLUS_ASSIGN, "+=", startLine, startColumn);
    }
    
    if (c == '-' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_MINUS_ASSIGN, "-=", startLine, startColumn);
    }
    
    if (c == '*' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_MULT_ASSIGN, "*=", startLine, startColumn);
    }
    
    if (c == '/' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_DIV_ASSIGN, "/=", startLine, startColumn);
    }
    
    if (c == '%' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_MOD_ASSIGN, "%=", startLine, startColumn);
    }
    
    if (c == '&' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_BIT_AND_ASSIGN, "&=", startLine, startColumn);
    }
    
    if (c == '|' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_BIT_OR_ASSIGN, "|=", startLine, startColumn);
    }
    
    if (c == '^' && peek() == '=') {
        advance();
        return Token(TokenType::TOK_BIT_XOR_ASSIGN, "^=", startLine, startColumn);
    }
    
    // Single character tokens
    switch (c) {
        case '(': return Token(TokenType::TOK_LPAREN, "(", startLine, startColumn);
        case ')': return Token(TokenType::TOK_RPAREN, ")", startLine, startColumn);
        case '{': return Token(TokenType::TOK_LBRACE, "{", startLine, startColumn);
        case '}': return Token(TokenType::TOK_RBRACE, "}", startLine, startColumn);
        case '[': return Token(TokenType::TOK_LBRACKET, "[", startLine, startColumn);
        case ']': return Token(TokenType::TOK_RBRACKET, "]", startLine, startColumn);
        case ';': return Token(TokenType::TOK_SEMICOLON, ";", startLine, startColumn);
        case ',': return Token(TokenType::TOK_COMMA, ",", startLine, startColumn);
        case ':': return Token(TokenType::TOK_COLON, ":", startLine, startColumn);
        case '.': return Token(TokenType::TOK_DOT, ".", startLine, startColumn);
        case '?': return Token(TokenType::TOK_QUESTION, "?", startLine, startColumn);
        case '@': return Token(TokenType::TOK_AT, "@", startLine, startColumn);
        case '#': return Token(TokenType::TOK_HASH, "#", startLine, startColumn);
        case '$': return Token(TokenType::TOK_DOLLAR, "$", startLine, startColumn);
        case '_': return Token(TokenType::TOK_UNDERSCORE, "_", startLine, startColumn);
        case '\\': return Token(TokenType::TOK_BACKSLASH, "\\", startLine, startColumn);
        case '=': return Token(TokenType::TOK_ASSIGN, "=", startLine, startColumn);
        case '+': return Token(TokenType::TOK_PLUS, "+", startLine, startColumn);
        case '-': return Token(TokenType::TOK_MINUS, "-", startLine, startColumn);
        case '*': return Token(TokenType::TOK_MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::TOK_DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::TOK_MODULO, "%", startLine, startColumn);
        case '<': return Token(TokenType::TOK_LESS, "<", startLine, startColumn);
        case '>': return Token(TokenType::TOK_GREATER, ">", startLine, startColumn);
        case '!': return Token(TokenType::TOK_LOGICAL_NOT, "!", startLine, startColumn);
        case '&': return Token(TokenType::TOK_BIT_AND, "&", startLine, startColumn);
        case '|': return Token(TokenType::TOK_PIPE, "|", startLine, startColumn);
        case '^': return Token(TokenType::TOK_BIT_XOR, "^", startLine, startColumn);
        case '~': return Token(TokenType::TOK_BIT_NOT, "~", startLine, startColumn);
        default:
            throw std::runtime_error("Unexpected character '" + std::string(1, c) + "' at line " + 
                                    std::to_string(line) + ", column " + std::to_string(column));
    }
}

Token Lexer::number() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    // Integer part
    while (!isAtEnd() && (isDigit(peek()) || peek() == '_')) {
        if (peek() != '_') {
            value += advance();
        } else {
            advance(); // skip underscore separators
        }
    }
    
    // Decimal part
    if (!isAtEnd() && peek() == '.' && isDigit(peekNext())) {
        hasDecimal = true;
        value += advance(); // consume '.'
        while (!isAtEnd() && (isDigit(peek()) || peek() == '_')) {
            if (peek() != '_') {
                value += advance();
            } else {
                advance(); // skip underscore separators
            }
        }
    }
    
    // Scientific notation (e.g., 1e10, 2.5e-3, 10E+5)
    if (!isAtEnd() && (peek() == 'e' || peek() == 'E')) {
        hasExponent = true;
        value += advance(); // consume 'e' or 'E'
        
        // Optional + or - after exponent
        if (!isAtEnd() && (peek() == '+' || peek() == '-')) {
            value += advance();
        }
        
        // Exponent digits
        if (!isAtEnd() && isDigit(peek())) {
            while (!isAtEnd() && (isDigit(peek()) || peek() == '_')) {
                if (peek() != '_') {
                    value += advance();
                } else {
                    advance(); // skip underscore separators
                }
            }
        } else {
            std::string lineContent = getCurrentLineContent();
            ErrorContext context("", line, column, lineContent, "");
            REPORT_ERROR(ErrorCode::INVALID_NUMBER, "Invalid scientific notation", context);
            throw std::runtime_error("Invalid scientific notation");
        }
    }
    
    if (hasExponent) {
        return Token(TokenType::TOK_SCIENTIFIC, value, startLine, startColumn);
    } else {
        return Token(TokenType::TOK_NUMBER, value, startLine, startColumn);
    }
}

Token Lexer::stringLiteral(char quote) {
    int startLine = line;
    int startColumn = column - 1; // account for opening quote
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        char c = advance();
        if (c == '\\' && !isAtEnd()) {
            // Handle escape sequences
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '\"': value += '\"'; break;
                case '\'': value += '\''; break;
                case '0': value += '\0'; break;
                default: value += escaped; break;
            }
        } else {
            value += c;
        }
    }
    
    if (isAtEnd()) {
        std::string lineContent = getCurrentLineContent();
        ErrorContext context("", startLine, startColumn, lineContent, "");
        REPORT_ERROR(ErrorCode::UNTERMINATED_STRING, "", context);
        throw std::runtime_error("Unterminated string");
    }
    
    advance(); // consume closing quote
    
    return Token(TokenType::TOK_STRING, value, startLine, startColumn);
}

Token Lexer::templateString() {
    int startLine = line;
    int startColumn = column - 1; // account for opening backtick
    std::string value;
    
    while (!isAtEnd() && peek() != '`') {
        char c = advance();
        if (c == '\\' && !isAtEnd()) {
            // Handle escape sequences in template strings
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '`': value += '`'; break;
                default: value += escaped; break;
            }
        } else {
            value += c;
        }
    }
    
    if (isAtEnd()) {
        std::string lineContent = getCurrentLineContent();
        ErrorContext context("", startLine, startColumn, lineContent, "");
        REPORT_ERROR(ErrorCode::UNTERMINATED_STRING, "Unterminated template string", context);
        throw std::runtime_error("Unterminated template string");
    }
    
    advance(); // consume closing backtick
    
    return Token(TokenType::TOK_STRING, value, startLine, startColumn);
}

Token Lexer::identifier() {
    int startLine = line;
    int startColumn = column;
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        value += advance();
    }
    
    // Check if it's a keyword
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, startLine, startColumn);
    }
    
    // Check if it's a built-in method (for semantic analysis)
    if (builtinMethods.find(value) != builtinMethods.end()) {
        // We'll handle built-in method recognition in the parser
        // For now, treat them as regular identifiers
    }
    
    return Token(TokenType::TOK_IDENTIFIER, value, startLine, startColumn);
}

char Lexer::peek() {
    if (current >= input.length()) return '\0';
    return input[current];
}

char Lexer::peekNext() {
    if (current + 1 >= input.length()) return '\0';
    return input[current + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = input[current++];
    
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    return c;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    // Skip single-line comment
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

bool Lexer::isAtEnd() {
    return current >= input.length();
}

std::string Lexer::getCurrentLineContent() const {
    std::stringstream ss(input);
    std::string line;
    int currentLine = 1;
    
    while (std::getline(ss, line)) {
        if (currentLine == this->line) {
            return line;
        }
        currentLine++;
    }
    
    return "";
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (input[current] != expected) return false;
    
    current++;
    column++;
    return true;
} 