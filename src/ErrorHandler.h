#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

enum class ErrorCode {
    // Lexical errors
    UNEXPECTED_TOKEN,
    UNTERMINATED_STRING,
    UNTERMINATED_COMMENT,
    INVALID_NUMBER,
    INVALID_IDENTIFIER,
    INVALID_EXPRESSION,
    
    // Syntax errors
    MISSING_SEMICOLON,
    MISSING_BRACE,
    MISSING_PAREN,
    MISSING_BRACKET,
    UNEXPECTED_EOF,
    INVALID_STATEMENT,
    INVALID_DECLARATION,
    
    // Import/Module errors
    MODULE_NOT_FOUND,
    DUPLICATE_IMPORT,
    INVALID_IMPORT_PATH,
    CIRCULAR_DEPENDENCY,
    
    // Type errors
    TYPE_MISMATCH,
    UNDEFINED_VARIABLE,
    UNDEFINED_FUNCTION,
    UNDEFINED_TYPE,
    INVALID_TYPE_CONVERSION,
    
    // Semantic errors
    DUPLICATE_DECLARATION,
    INVALID_ACCESS_MODIFIER,
    INVALID_GENERIC_USAGE,
    INVALID_METHOD_CALL,
    
    // System errors
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    COMPILATION_FAILED,
    LINKING_FAILED
};

struct ErrorContext {
    std::string fileName;
    int line;
    int column;
    std::string lineContent;
    std::string tokenValue;
    std::string expectedToken;
    std::string suggestion;
    
    ErrorContext() : line(0), column(0) {}
    ErrorContext(const std::string& file, int l, int c) 
        : fileName(file), line(l), column(c) {}
    ErrorContext(const std::string& file, int l, int c, const std::string& content, const std::string& token) 
        : fileName(file), line(l), column(c), lineContent(content), tokenValue(token) {}
};

struct CompilerError {
    ErrorLevel level;
    ErrorCode code;
    std::string message;
    ErrorContext context;
    std::vector<std::string> hints;
    
    CompilerError(ErrorLevel l, ErrorCode c, const std::string& msg)
        : level(l), code(c), message(msg) {}
    
    CompilerError(ErrorLevel l, ErrorCode c, const std::string& msg, const ErrorContext& ctx)
        : level(l), code(c), message(msg), context(ctx) {}
};

class ErrorHandler {
private:
    std::vector<CompilerError> errors;
    std::vector<CompilerError> warnings;
    bool hasErrors = false;
    bool hasWarnings = false;
    bool warningsAsErrors = false;
    
    // Error message templates
    std::string getErrorMessage(ErrorCode code, const std::string& context = "");
    std::string getSuggestion(ErrorCode code, const std::string& context = "");
    std::vector<std::string> getHints(ErrorCode code, const std::string& context = "");
    
    // Formatting helpers
    std::string formatErrorLocation(const ErrorContext& context);
    std::string formatErrorLine(const ErrorContext& context);
    std::string formatErrorPointer(const ErrorContext& context);
    std::string formatErrorCode(ErrorCode code);
    std::string formatErrorLevel(ErrorLevel level);
    
    // Color support
    bool useColors = true;
    std::string getColorCode(const std::string& color) const;
    std::string resetColor() const;
    
public:
    ErrorHandler();
    
    // Error reporting
    void reportError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext());
    void reportWarning(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext());
    void reportFatal(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext());
    
    // Context building
    ErrorContext createContext(const std::string& fileName, int line, int column, 
                              const std::string& lineContent = "", const std::string& tokenValue = "");
    
    // Error checking
    bool hasCompilationErrors() const { return hasErrors; }
    bool hasCompilationWarnings() const { return hasWarnings; }
    int getErrorCount() const { return errors.size(); }
    int getWarningCount() const { return warnings.size(); }
    
    // Error display
    void printErrors();
    void printWarnings();
    void printAllIssues();
    
    // Configuration
    void setWarningsAsErrors(bool value) { warningsAsErrors = value; }
    void setUseColors(bool value) { useColors = value; }
    
    // Error recovery
    void clearErrors() { errors.clear(); hasErrors = false; }
    void clearWarnings() { warnings.clear(); hasWarnings = false; }
    void clearAll() { clearErrors(); clearWarnings(); }
    
    // Utility methods
    std::string getErrorSummary() const;
    std::vector<CompilerError> getErrors() const { return errors; }
    std::vector<CompilerError> getWarnings() const { return warnings; }
};

// Global error handler instance
extern ErrorHandler g_errorHandler;

// Convenience macros for error reporting
#define REPORT_ERROR(code, message, ...) \
    g_errorHandler.reportError(code, message, ##__VA_ARGS__)

#define REPORT_WARNING(code, message, ...) \
    g_errorHandler.reportWarning(code, message, ##__VA_ARGS__)

#define REPORT_FATAL(code, message, ...) \
    g_errorHandler.reportFatal(code, message, ##__VA_ARGS__)

#define CREATE_CONTEXT(file, line, col, content, token) g_errorHandler.createContext(file, line, col, content, token) 