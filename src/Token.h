#pragma once
#include <string>
#include <iostream>

enum class TokenType {
    // Literals
    TOK_EOF = -1,
    TOK_IDENTIFIER = -2,
    TOK_NUMBER = -3,
    TOK_STRING = -4,
    TOK_BOOL = -5,
    TOK_NULL = -6,
    TOK_SCIENTIFIC = -7,  // For 10e1, 1.5e-3, etc.
    TOK_CHAR = -8,
    
    // Keywords - Rust-like Function Declaration
    TOK_FN = -10,
    TOK_RETURN = -11,
    TOK_PUB = -12,
    TOK_PRIV = -13,
    TOK_PROT = -14,
    TOK_STATIC = -15,
    TOK_CONST = -16,
    TOK_MUT = -17,
    TOK_UNSAFE = -18,
    
    // Keywords - Control Flow
    TOK_IF = -20,
    TOK_ELSE = -21,
    TOK_ELIF = -22,
    TOK_WHILE = -23,
    TOK_FOR = -24,
    TOK_LOOP = -25,
    TOK_BREAK = -26,
    TOK_CONTINUE = -27,
    TOK_MATCH = -28,
    TOK_CASE = -29,
    TOK_DEFAULT = -30,
    
    // Keywords - OOP (Enhanced)
    TOK_STRUCT = -35,
    TOK_CLASS = -36,
    TOK_INTERFACE = -37,
    TOK_TRAIT = -38,
    TOK_IMPL = -39,
    TOK_ENUM = -40,
    TOK_UNION = -41,
    TOK_EXTENDS = -42,
    TOK_IMPLEMENTS = -43,
    TOK_SUPER = -44,
    TOK_SELF = -45,
    TOK_SELF_TYPE = -46,  // Self (capital S)
    TOK_WHERE = -47,
    TOK_ABSTRACT = -48,
    TOK_VIRTUAL = -49,
    TOK_OVERRIDE = -50,
    TOK_FINAL = -51,
    
    // Keywords - Variables & Types
    TOK_LET = -55,
    TOK_VAR = -56,
    TOK_AUTO = -57,
    TOK_TYPEOF = -58,
    TOK_SIZEOF = -59,
    
    // Primitive Types
    TOK_I8 = -60,
    TOK_I16 = -61,
    TOK_I32 = -62,
    TOK_I64 = -63,
    TOK_I128 = -64,
    TOK_U8 = -65,
    TOK_U16 = -66,
    TOK_U32 = -67,
    TOK_U64 = -68,
    TOK_U128 = -69,
    TOK_F32 = -70,
    TOK_F64 = -71,
    TOK_CHAR_TYPE = -72,
    TOK_STR = -73,
    TOK_STRING_TYPE = -74,
    TOK_BOOL_TYPE = -75,
    TOK_VOID = -76,
    
    // Collection Types
    TOK_VEC = -80,
    TOK_ARRAY = -81,
    TOK_SLICE = -82,
    TOK_MAP = -83,
    TOK_SET = -84,
    TOK_TUPLE = -85,
    TOK_OPTION = -86,
    TOK_RESULT = -87,
    
    // Keywords - Import/Export (TypeScript-like)
    TOK_IMPORT = -90,
    TOK_FROM = -91,
    TOK_EXPORT = -92,
    TOK_MODULE = -93,
    TOK_AS = -94,
    TOK_USE = -95,
    TOK_MOD = -96,
    TOK_CRATE = -97,
    
    // Keywords - Memory & Ownership (Rust-like)
    TOK_BOX = -100,
    TOK_REF = -101,
    TOK_DEREF = -102,
    TOK_MOVE = -103,
    TOK_COPY = -104,
    TOK_CLONE = -105,
    TOK_DROP = -106,
    TOK_NEW = -107,
    TOK_DELETE = -108,
    
    // Keywords - Concurrency
    TOK_ASYNC = -110,
    TOK_AWAIT = -111,
    TOK_SPAWN = -112,
    TOK_THREAD = -113,
    TOK_MUTEX = -114,
    TOK_RWLOCK = -115,
    TOK_CHANNEL = -116,
    TOK_SEND = -117,
    TOK_SYNC = -118,
    
    // Keywords - External & FFI
    TOK_EXTERN = -120,
    TOK_C = -121,
    TOK_CPP = -122,
    TOK_CDECL = -123,
    TOK_STDCALL = -124,
    TOK_FASTCALL = -125,
    
    // Keywords - Exception Handling
    TOK_TRY = -130,
    TOK_CATCH = -131,
    TOK_FINALLY = -132,
    TOK_THROW = -133,
    TOK_PANIC = -134,
    TOK_UNWRAP = -135,
    TOK_EXPECT = -136,
    
    // Keywords - Special
    TOK_IN = -140,
    TOK_IS = -141,
    TOK_NOT = -142,
    TOK_AND = -143,
    TOK_OR = -144,
    TOK_XOR = -145,
    TOK_TRUE = -146,
    TOK_FALSE = -147,
    TOK_NONE = -148,
    TOK_SOME = -149,
    TOK_OK = -150,
    TOK_ERR = -151,
    
    // Operators - Arithmetic (High Precedence)
    TOK_MULTIPLY = -200,        // *
    TOK_DIVIDE = -201,          // /
    TOK_MODULO = -202,          // %
    TOK_POWER = -203,           // **
    
    // Operators - Arithmetic (Low Precedence)
    TOK_PLUS = -210,            // +
    TOK_MINUS = -211,           // -
    
    // Operators - Comparison (Medium Precedence)
    TOK_LESS = -220,            // <
    TOK_GREATER = -221,         // >
    TOK_LESS_EQUAL = -222,      // <=
    TOK_GREATER_EQUAL = -223,   // >=
    
    // Operators - Equality (Super Low Precedence)
    TOK_EQUAL = -230,           // ==
    TOK_NOT_EQUAL = -231,       // !=
    TOK_STRICT_EQUAL = -232,    // ===
    TOK_STRICT_NOT_EQUAL = -233, // !==
    
    // Operators - Logical
    TOK_LOGICAL_AND = -240,     // &&
    TOK_LOGICAL_OR = -241,      // ||
    TOK_LOGICAL_NOT = -242,     // !
    
    // Operators - Bitwise
    TOK_BIT_AND = -250,         // &
    TOK_BIT_OR = -251,          // |
    TOK_BIT_XOR = -252,         // ^
    TOK_BIT_NOT = -253,         // ~
    TOK_LEFT_SHIFT = -254,      // <<
    TOK_RIGHT_SHIFT = -255,     // >>
    TOK_UNSIGNED_RIGHT_SHIFT = -256, // >>>
    
    // Operators - Assignment
    TOK_ASSIGN = -260,          // =
    TOK_PLUS_ASSIGN = -261,     // +=
    TOK_MINUS_ASSIGN = -262,    // -=
    TOK_MULT_ASSIGN = -263,     // *=
    TOK_DIV_ASSIGN = -264,      // /=
    TOK_MOD_ASSIGN = -265,      // %=
    TOK_POWER_ASSIGN = -266,    // **=
    TOK_BIT_AND_ASSIGN = -267,  // &=
    TOK_BIT_OR_ASSIGN = -268,   // |=
    TOK_BIT_XOR_ASSIGN = -269,  // ^=
    TOK_LEFT_SHIFT_ASSIGN = -270,  // <<=
    TOK_RIGHT_SHIFT_ASSIGN = -271, // >>=
    
    // Operators - Unary
    TOK_INCREMENT = -280,       // ++
    TOK_DECREMENT = -281,       // --
    TOK_ADDRESS_OF = -282,      // &
    TOK_DEREFERENCE = -283,     // *
    
    // Operators - Special
    TOK_ARROW = -290,           // ->
    TOK_FAT_ARROW = -291,       // =>
    TOK_DOT = -292,             // .
    TOK_DOUBLE_DOT = -293,      // ..
    TOK_TRIPLE_DOT = -294,      // ...
    TOK_SCOPE = -295,           // ::
    TOK_QUESTION = -296,        // ?
    TOK_QUESTION_DOT = -297,    // ?.
    TOK_QUESTION_QUESTION = -298, // ??
    TOK_ELVIS = -299,           // ?:
    TOK_PIPELINE = -300,        // |>
    TOK_COMPOSE = -301,         // >>
    TOK_SPACESHIP = -302,       // <=>
    
    // Delimiters
    TOK_LPAREN = -400,          // (
    TOK_RPAREN = -401,          // )
    TOK_LBRACE = -402,          // {
    TOK_RBRACE = -403,          // }
    TOK_LBRACKET = -404,        // [
    TOK_RBRACKET = -405,        // ]
    TOK_SEMICOLON = -406,       // ;
    TOK_COMMA = -407,           // ,
    TOK_COLON = -408,           // :
    TOK_DOUBLE_COLON = -409,    // ::
    TOK_AT = -410,              // @
    TOK_HASH = -411,            // #
    TOK_DOLLAR = -412,          // $
    TOK_UNDERSCORE = -413,      // _
    TOK_PIPE = -414,            // |
    TOK_BACKSLASH = -415,       // \
    
    // String delimiters
    TOK_DOUBLE_QUOTE = -420,    // "
    TOK_SINGLE_QUOTE = -421,    // '
    TOK_BACKTICK = -422,        // `
    TOK_RAW_STRING = -423,      // r"..."
    
    // Comments
    TOK_LINE_COMMENT = -430,    // //
    TOK_BLOCK_COMMENT = -431,   // /* */
    TOK_DOC_COMMENT = -432,     // ///
    
    // Built-in Functions
    TOK_PRINTLN = -500,
    TOK_PRINT = -501,
    TOK_ASSERT = -502,
    TOK_DEBUG_ASSERT = -503,
    TOK_UNREACHABLE = -504,
    TOK_TODO = -505,
    TOK_UNIMPLEMENTED = -506,
    
    // Built-in Methods
    TOK_TYPE_METHOD = -510,     // .type()
    TOK_TO_STRING = -511,       // .to_string()
    TOK_TO_INT = -512,          // .to_int()
    TOK_TO_FLOAT = -513,        // .to_float()
    TOK_TO_BOOL = -514,         // .to_bool()
    TOK_LENGTH = -515,          // .length()
    TOK_SIZE = -516,            // .size()
    TOK_IS_EMPTY = -517,        // .is_empty()
    TOK_CONTAINS = -518,        // .contains()
    TOK_STARTS_WITH = -519,     // .starts_with()
    TOK_ENDS_WITH = -520,       // .ends_with()
    TOK_SPLIT = -521,           // .split()
    TOK_JOIN = -522,            // .join()
    TOK_TRIM = -523,            // .trim()
    TOK_REPLACE = -524,         // .replace()
    TOK_PUSH = -525,            // .push()
    TOK_POP = -526,             // .pop()
    TOK_INSERT = -527,          // .insert()
    TOK_REMOVE = -528,          // .remove()
    TOK_CLEAR = -529,           // .clear()
    TOK_SORT = -530,            // .sort()
    TOK_REVERSE = -531,         // .reverse()
    TOK_MAP_METHOD = -532,      // .map()
    TOK_FILTER = -533,          // .filter()
    TOK_REDUCE = -534,          // .reduce()
    TOK_FOLD = -535,            // .fold()
    TOK_FIND = -536,            // .find()
    TOK_ANY = -537,             // .any()
    TOK_ALL = -538,             // .all()
    TOK_COUNT = -539,           // .count()
    TOK_MIN = -540,             // .min()
    TOK_MAX = -541,             // .max()
    TOK_SUM = -542,             // .sum()
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l = 0, int c = 0) 
        : type(t), value(v), line(l), column(c) {}
};

// Operator precedence for proper mathematical evaluation
enum class Precedence {
    NONE = 0,
    ASSIGNMENT = 1,     // =, +=, -=, etc.
    TERNARY = 2,        // ?:
    LOGICAL_OR = 3,     // ||
    LOGICAL_AND = 4,    // &&
    BITWISE_OR = 5,     // |
    BITWISE_XOR = 6,    // ^
    BITWISE_AND = 7,    // &
    EQUALITY = 8,       // ==, !=, ===, !==
    COMPARISON = 9,     // <, >, <=, >=
    SHIFT = 10,         // <<, >>
    TERM = 11,          // +, -
    FACTOR = 12,        // *, /, %
    POWER = 13,         // **
    UNARY = 14,         // !, -, ++, --
    CALL = 15,          // (), [], .
    PRIMARY = 16
};

inline Precedence getOperatorPrecedence(TokenType type) {
    switch (type) {
        // Assignment operators (lowest precedence)
        case TokenType::TOK_ASSIGN:
        case TokenType::TOK_PLUS_ASSIGN:
        case TokenType::TOK_MINUS_ASSIGN:
        case TokenType::TOK_MULT_ASSIGN:
        case TokenType::TOK_DIV_ASSIGN:
        case TokenType::TOK_MOD_ASSIGN:
        case TokenType::TOK_POWER_ASSIGN:
        case TokenType::TOK_BIT_AND_ASSIGN:
        case TokenType::TOK_BIT_OR_ASSIGN:
        case TokenType::TOK_BIT_XOR_ASSIGN:
        case TokenType::TOK_LEFT_SHIFT_ASSIGN:
        case TokenType::TOK_RIGHT_SHIFT_ASSIGN:
            return Precedence::ASSIGNMENT;
            
        // Ternary operator
        case TokenType::TOK_QUESTION:
        case TokenType::TOK_ELVIS:
            return Precedence::TERNARY;
            
        // Logical OR
        case TokenType::TOK_LOGICAL_OR:
        case TokenType::TOK_OR:
            return Precedence::LOGICAL_OR;
            
        // Logical AND
        case TokenType::TOK_LOGICAL_AND:
        case TokenType::TOK_AND:
            return Precedence::LOGICAL_AND;
            
        // Bitwise OR
        case TokenType::TOK_BIT_OR:
            return Precedence::BITWISE_OR;
            
        // Bitwise XOR
        case TokenType::TOK_BIT_XOR:
        case TokenType::TOK_XOR:
            return Precedence::BITWISE_XOR;
            
        // Bitwise AND
        case TokenType::TOK_BIT_AND:
            return Precedence::BITWISE_AND;
            
        // Equality operators (super low precedence as requested)
        case TokenType::TOK_EQUAL:
        case TokenType::TOK_NOT_EQUAL:
        case TokenType::TOK_STRICT_EQUAL:
        case TokenType::TOK_STRICT_NOT_EQUAL:
            return Precedence::EQUALITY;
            
        // Comparison operators (above equality)
        case TokenType::TOK_LESS:
        case TokenType::TOK_GREATER:
        case TokenType::TOK_LESS_EQUAL:
        case TokenType::TOK_GREATER_EQUAL:
        case TokenType::TOK_SPACESHIP:
            return Precedence::COMPARISON;
            
        // Shift operators
        case TokenType::TOK_LEFT_SHIFT:
        case TokenType::TOK_RIGHT_SHIFT:
        case TokenType::TOK_UNSIGNED_RIGHT_SHIFT:
            return Precedence::SHIFT;
            
        // Addition and subtraction (low precedence)
        case TokenType::TOK_PLUS:
        case TokenType::TOK_MINUS:
            return Precedence::TERM;
            
        // Multiplication, division, modulo (high precedence)
        case TokenType::TOK_MULTIPLY:
        case TokenType::TOK_DIVIDE:
        case TokenType::TOK_MODULO:
            return Precedence::FACTOR;
            
        // Power operator (highest precedence)
        case TokenType::TOK_POWER:
            return Precedence::POWER;
            
        // Function calls, member access
        case TokenType::TOK_LPAREN:
        case TokenType::TOK_LBRACKET:
        case TokenType::TOK_DOT:
        case TokenType::TOK_QUESTION_DOT:
            return Precedence::CALL;
            
        default:
            return Precedence::NONE;
    }
}

inline bool isRightAssociative(TokenType type) {
    return type == TokenType::TOK_POWER || 
           type == TokenType::TOK_ASSIGN ||
           type == TokenType::TOK_PLUS_ASSIGN ||
           type == TokenType::TOK_MINUS_ASSIGN ||
           type == TokenType::TOK_MULT_ASSIGN ||
           type == TokenType::TOK_DIV_ASSIGN ||
           type == TokenType::TOK_MOD_ASSIGN ||
           type == TokenType::TOK_POWER_ASSIGN;
}

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Literals
        case TokenType::TOK_EOF: return "EOF";
        case TokenType::TOK_IDENTIFIER: return "IDENTIFIER";
        case TokenType::TOK_NUMBER: return "NUMBER";
        case TokenType::TOK_STRING: return "STRING";
        case TokenType::TOK_BOOL: return "BOOL";
        case TokenType::TOK_NULL: return "NULL";
        case TokenType::TOK_SCIENTIFIC: return "SCIENTIFIC";
        case TokenType::TOK_CHAR: return "CHAR";
        
        // Function keywords
        case TokenType::TOK_FN: return "FN";
        case TokenType::TOK_RETURN: return "RETURN";
        case TokenType::TOK_PUB: return "PUB";
        case TokenType::TOK_PRIV: return "PRIV";
        case TokenType::TOK_PROT: return "PROT";
        case TokenType::TOK_STATIC: return "STATIC";
        case TokenType::TOK_CONST: return "CONST";
        case TokenType::TOK_MUT: return "MUT";
        case TokenType::TOK_UNSAFE: return "UNSAFE";
        
        // Control Flow
        case TokenType::TOK_IF: return "IF";
        case TokenType::TOK_ELSE: return "ELSE";
        case TokenType::TOK_ELIF: return "ELIF";
        case TokenType::TOK_WHILE: return "WHILE";
        case TokenType::TOK_FOR: return "FOR";
        case TokenType::TOK_LOOP: return "LOOP";
        case TokenType::TOK_BREAK: return "BREAK";
        case TokenType::TOK_CONTINUE: return "CONTINUE";
        case TokenType::TOK_MATCH: return "MATCH";
        case TokenType::TOK_CASE: return "CASE";
        case TokenType::TOK_DEFAULT: return "DEFAULT";
        
        // OOP
        case TokenType::TOK_STRUCT: return "STRUCT";
        case TokenType::TOK_CLASS: return "CLASS";
        case TokenType::TOK_INTERFACE: return "INTERFACE";
        case TokenType::TOK_TRAIT: return "TRAIT";
        case TokenType::TOK_IMPL: return "IMPL";
        case TokenType::TOK_ENUM: return "ENUM";
        case TokenType::TOK_UNION: return "UNION";
        case TokenType::TOK_EXTENDS: return "EXTENDS";
        case TokenType::TOK_IMPLEMENTS: return "IMPLEMENTS";
        case TokenType::TOK_SUPER: return "SUPER";
        case TokenType::TOK_SELF: return "SELF";
        case TokenType::TOK_SELF_TYPE: return "SELF_TYPE";
        case TokenType::TOK_WHERE: return "WHERE";
        case TokenType::TOK_ABSTRACT: return "ABSTRACT";
        case TokenType::TOK_VIRTUAL: return "VIRTUAL";
        case TokenType::TOK_OVERRIDE: return "OVERRIDE";
        case TokenType::TOK_FINAL: return "FINAL";
        
        // Variables & Types
        case TokenType::TOK_LET: return "LET";
        case TokenType::TOK_VAR: return "VAR";
        case TokenType::TOK_AUTO: return "AUTO";
        case TokenType::TOK_TYPEOF: return "TYPEOF";
        case TokenType::TOK_SIZEOF: return "SIZEOF";
        
        // Primitive Types
        case TokenType::TOK_I8: return "I8";
        case TokenType::TOK_I16: return "I16";
        case TokenType::TOK_I32: return "I32";
        case TokenType::TOK_I64: return "I64";
        case TokenType::TOK_I128: return "I128";
        case TokenType::TOK_U8: return "U8";
        case TokenType::TOK_U16: return "U16";
        case TokenType::TOK_U32: return "U32";
        case TokenType::TOK_U64: return "U64";
        case TokenType::TOK_U128: return "U128";
        case TokenType::TOK_F32: return "F32";
        case TokenType::TOK_F64: return "F64";
        case TokenType::TOK_CHAR_TYPE: return "CHAR_TYPE";
        case TokenType::TOK_STR: return "STR";
        case TokenType::TOK_STRING_TYPE: return "STRING_TYPE";
        case TokenType::TOK_BOOL_TYPE: return "BOOL_TYPE";
        case TokenType::TOK_VOID: return "VOID";
        
        // Collection Types
        case TokenType::TOK_VEC: return "VEC";
        case TokenType::TOK_ARRAY: return "ARRAY";
        case TokenType::TOK_SLICE: return "SLICE";
        case TokenType::TOK_MAP: return "MAP";
        case TokenType::TOK_SET: return "SET";
        case TokenType::TOK_TUPLE: return "TUPLE";
        case TokenType::TOK_OPTION: return "OPTION";
        case TokenType::TOK_RESULT: return "RESULT";
        
        // Import/Export
        case TokenType::TOK_IMPORT: return "IMPORT";
        case TokenType::TOK_FROM: return "FROM";
        case TokenType::TOK_EXPORT: return "EXPORT";
        case TokenType::TOK_MODULE: return "MODULE";
        case TokenType::TOK_AS: return "AS";
        case TokenType::TOK_USE: return "USE";
        case TokenType::TOK_MOD: return "MOD";
        case TokenType::TOK_CRATE: return "CRATE";
        
        // Memory & Ownership
        case TokenType::TOK_BOX: return "BOX";
        case TokenType::TOK_REF: return "REF";
        case TokenType::TOK_DEREF: return "DEREF";
        case TokenType::TOK_MOVE: return "MOVE";
        case TokenType::TOK_COPY: return "COPY";
        case TokenType::TOK_CLONE: return "CLONE";
        case TokenType::TOK_DROP: return "DROP";
        case TokenType::TOK_NEW: return "NEW";
        case TokenType::TOK_DELETE: return "DELETE";
        
        // Concurrency
        case TokenType::TOK_ASYNC: return "ASYNC";
        case TokenType::TOK_AWAIT: return "AWAIT";
        case TokenType::TOK_SPAWN: return "SPAWN";
        case TokenType::TOK_THREAD: return "THREAD";
        case TokenType::TOK_MUTEX: return "MUTEX";
        case TokenType::TOK_RWLOCK: return "RWLOCK";
        case TokenType::TOK_CHANNEL: return "CHANNEL";
        case TokenType::TOK_SEND: return "SEND";
        case TokenType::TOK_SYNC: return "SYNC";
        
        // External & FFI
        case TokenType::TOK_EXTERN: return "EXTERN";
        case TokenType::TOK_C: return "C";
        case TokenType::TOK_CPP: return "CPP";
        case TokenType::TOK_CDECL: return "CDECL";
        case TokenType::TOK_STDCALL: return "STDCALL";
        case TokenType::TOK_FASTCALL: return "FASTCALL";
        
        // Exception Handling
        case TokenType::TOK_TRY: return "TRY";
        case TokenType::TOK_CATCH: return "CATCH";
        case TokenType::TOK_FINALLY: return "FINALLY";
        case TokenType::TOK_THROW: return "THROW";
        case TokenType::TOK_PANIC: return "PANIC";
        case TokenType::TOK_UNWRAP: return "UNWRAP";
        case TokenType::TOK_EXPECT: return "EXPECT";
        
        // Special Keywords
        case TokenType::TOK_IN: return "IN";
        case TokenType::TOK_IS: return "IS";
        case TokenType::TOK_NOT: return "NOT";
        case TokenType::TOK_AND: return "AND";
        case TokenType::TOK_OR: return "OR";
        case TokenType::TOK_XOR: return "XOR";
        case TokenType::TOK_TRUE: return "TRUE";
        case TokenType::TOK_FALSE: return "FALSE";
        case TokenType::TOK_NONE: return "NONE";
        case TokenType::TOK_SOME: return "SOME";
        case TokenType::TOK_OK: return "OK";
        case TokenType::TOK_ERR: return "ERR";
        
        // Operators (with proper precedence)
        case TokenType::TOK_MULTIPLY: return "MULTIPLY";
        case TokenType::TOK_DIVIDE: return "DIVIDE";
        case TokenType::TOK_MODULO: return "MODULO";
        case TokenType::TOK_POWER: return "POWER";
        case TokenType::TOK_PLUS: return "PLUS";
        case TokenType::TOK_MINUS: return "MINUS";
        case TokenType::TOK_LESS: return "LESS";
        case TokenType::TOK_GREATER: return "GREATER";
        case TokenType::TOK_LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::TOK_GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::TOK_EQUAL: return "EQUAL";
        case TokenType::TOK_NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::TOK_STRICT_EQUAL: return "STRICT_EQUAL";
        case TokenType::TOK_STRICT_NOT_EQUAL: return "STRICT_NOT_EQUAL";
        case TokenType::TOK_LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::TOK_LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::TOK_LOGICAL_NOT: return "LOGICAL_NOT";
        case TokenType::TOK_BIT_AND: return "BIT_AND";
        case TokenType::TOK_BIT_OR: return "BIT_OR";
        case TokenType::TOK_BIT_XOR: return "BIT_XOR";
        case TokenType::TOK_BIT_NOT: return "BIT_NOT";
        case TokenType::TOK_LEFT_SHIFT: return "LEFT_SHIFT";
        case TokenType::TOK_RIGHT_SHIFT: return "RIGHT_SHIFT";
        case TokenType::TOK_UNSIGNED_RIGHT_SHIFT: return "UNSIGNED_RIGHT_SHIFT";
        case TokenType::TOK_ASSIGN: return "ASSIGN";
        case TokenType::TOK_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::TOK_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::TOK_MULT_ASSIGN: return "MULT_ASSIGN";
        case TokenType::TOK_DIV_ASSIGN: return "DIV_ASSIGN";
        case TokenType::TOK_MOD_ASSIGN: return "MOD_ASSIGN";
        case TokenType::TOK_POWER_ASSIGN: return "POWER_ASSIGN";
        case TokenType::TOK_BIT_AND_ASSIGN: return "BIT_AND_ASSIGN";
        case TokenType::TOK_BIT_OR_ASSIGN: return "BIT_OR_ASSIGN";
        case TokenType::TOK_BIT_XOR_ASSIGN: return "BIT_XOR_ASSIGN";
        case TokenType::TOK_LEFT_SHIFT_ASSIGN: return "LEFT_SHIFT_ASSIGN";
        case TokenType::TOK_RIGHT_SHIFT_ASSIGN: return "RIGHT_SHIFT_ASSIGN";
        case TokenType::TOK_INCREMENT: return "INCREMENT";
        case TokenType::TOK_DECREMENT: return "DECREMENT";
        case TokenType::TOK_ADDRESS_OF: return "ADDRESS_OF";
        case TokenType::TOK_DEREFERENCE: return "DEREFERENCE";
        case TokenType::TOK_ARROW: return "ARROW";
        case TokenType::TOK_FAT_ARROW: return "FAT_ARROW";
        case TokenType::TOK_DOT: return "DOT";
        case TokenType::TOK_DOUBLE_DOT: return "DOUBLE_DOT";
        case TokenType::TOK_TRIPLE_DOT: return "TRIPLE_DOT";
        case TokenType::TOK_SCOPE: return "SCOPE";
        case TokenType::TOK_QUESTION: return "QUESTION";
        case TokenType::TOK_QUESTION_DOT: return "QUESTION_DOT";
        case TokenType::TOK_QUESTION_QUESTION: return "QUESTION_QUESTION";
        case TokenType::TOK_ELVIS: return "ELVIS";
        case TokenType::TOK_PIPELINE: return "PIPELINE";
        case TokenType::TOK_COMPOSE: return "COMPOSE";
        case TokenType::TOK_SPACESHIP: return "SPACESHIP";
        
        // Delimiters
        case TokenType::TOK_LPAREN: return "LPAREN";
        case TokenType::TOK_RPAREN: return "RPAREN";
        case TokenType::TOK_LBRACE: return "LBRACE";
        case TokenType::TOK_RBRACE: return "RBRACE";
        case TokenType::TOK_LBRACKET: return "LBRACKET";
        case TokenType::TOK_RBRACKET: return "RBRACKET";
        case TokenType::TOK_SEMICOLON: return "SEMICOLON";
        case TokenType::TOK_COMMA: return "COMMA";
        case TokenType::TOK_COLON: return "COLON";
        case TokenType::TOK_DOUBLE_COLON: return "DOUBLE_COLON";
        case TokenType::TOK_AT: return "AT";
        case TokenType::TOK_HASH: return "HASH";
        case TokenType::TOK_DOLLAR: return "DOLLAR";
        case TokenType::TOK_UNDERSCORE: return "UNDERSCORE";
        case TokenType::TOK_PIPE: return "PIPE";
        case TokenType::TOK_BACKSLASH: return "BACKSLASH";
        case TokenType::TOK_DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        case TokenType::TOK_SINGLE_QUOTE: return "SINGLE_QUOTE";
        case TokenType::TOK_BACKTICK: return "BACKTICK";
        case TokenType::TOK_RAW_STRING: return "RAW_STRING";
        case TokenType::TOK_LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::TOK_BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::TOK_DOC_COMMENT: return "DOC_COMMENT";
        
        // Built-in functions
        case TokenType::TOK_PRINTLN: return "PRINTLN";
        case TokenType::TOK_PRINT: return "PRINT";
        case TokenType::TOK_ASSERT: return "ASSERT";
        case TokenType::TOK_DEBUG_ASSERT: return "DEBUG_ASSERT";
        case TokenType::TOK_UNREACHABLE: return "UNREACHABLE";
        case TokenType::TOK_TODO: return "TODO";
        case TokenType::TOK_UNIMPLEMENTED: return "UNIMPLEMENTED";
        
        // Built-in methods
        case TokenType::TOK_TYPE_METHOD: return "TYPE_METHOD";
        case TokenType::TOK_TO_STRING: return "TO_STRING";
        case TokenType::TOK_TO_INT: return "TO_INT";
        case TokenType::TOK_TO_FLOAT: return "TO_FLOAT";
        case TokenType::TOK_TO_BOOL: return "TO_BOOL";
        case TokenType::TOK_LENGTH: return "LENGTH";
        case TokenType::TOK_SIZE: return "SIZE";
        case TokenType::TOK_IS_EMPTY: return "IS_EMPTY";
        case TokenType::TOK_CONTAINS: return "CONTAINS";
        case TokenType::TOK_STARTS_WITH: return "STARTS_WITH";
        case TokenType::TOK_ENDS_WITH: return "ENDS_WITH";
        case TokenType::TOK_SPLIT: return "SPLIT";
        case TokenType::TOK_JOIN: return "JOIN";
        case TokenType::TOK_TRIM: return "TRIM";
        case TokenType::TOK_REPLACE: return "REPLACE";
        case TokenType::TOK_PUSH: return "PUSH";
        case TokenType::TOK_POP: return "POP";
        case TokenType::TOK_INSERT: return "INSERT";
        case TokenType::TOK_REMOVE: return "REMOVE";
        case TokenType::TOK_CLEAR: return "CLEAR";
        case TokenType::TOK_SORT: return "SORT";
        case TokenType::TOK_REVERSE: return "REVERSE";
        case TokenType::TOK_MAP_METHOD: return "MAP_METHOD";
        case TokenType::TOK_FILTER: return "FILTER";
        case TokenType::TOK_REDUCE: return "REDUCE";
        case TokenType::TOK_FOLD: return "FOLD";
        case TokenType::TOK_FIND: return "FIND";
        case TokenType::TOK_ANY: return "ANY";
        case TokenType::TOK_ALL: return "ALL";
        case TokenType::TOK_COUNT: return "COUNT";
        case TokenType::TOK_MIN: return "MIN";
        case TokenType::TOK_MAX: return "MAX";
        case TokenType::TOK_SUM: return "SUM";
        
        default: return "UNKNOWN";
    }
} 