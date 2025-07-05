#include "ErrorHandler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

ErrorHandler g_errorHandler;

ErrorHandler::ErrorHandler() : useColors(true) {
    const char* term = std::getenv("TERM");
    if (!term || std::string(term) == "dumb") {
        useColors = false;
    }
}

std::string ErrorHandler::getColorCode(const std::string& color) const {
    if (!useColors) return "";
    if (color == "red") return "\033[31m";
    if (color == "green") return "\033[32m";
    if (color == "yellow") return "\033[33m";
    if (color == "blue") return "\033[34m";
    if (color == "magenta") return "\033[35m";
    if (color == "cyan") return "\033[36m";
    if (color == "white") return "\033[37m";
    if (color == "bold") return "\033[1m";
    if (color == "underline") return "\033[4m";
    if (color == "reset") return "\033[0m";
    return "";
}

std::string ErrorHandler::resetColor() const {
    return getColorCode("reset");
}

std::string ErrorHandler::formatErrorLevel(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::INFO:
            return getColorCode("cyan") + "info" + resetColor();
        case ErrorLevel::WARNING:
            return getColorCode("yellow") + "warning" + resetColor();
        case ErrorLevel::ERROR:
            return getColorCode("red") + "error" + resetColor();
        case ErrorLevel::FATAL:
            return getColorCode("red") + getColorCode("bold") + "fatal error" + resetColor();
        default:
            return "unknown";
    }
}

std::string ErrorHandler::formatErrorCode(ErrorCode code) {
    return "E???"; // You can expand this to map to your error codes
}

std::string ErrorHandler::getErrorMessage(ErrorCode code, const std::string& context) {
    switch (code) {
        case ErrorCode::UNEXPECTED_TOKEN:
            return "Unexpected token '" + context + "'";
        case ErrorCode::UNTERMINATED_STRING:
            return "Unterminated string literal";
        case ErrorCode::UNTERMINATED_COMMENT:
            return "Unterminated comment";
        case ErrorCode::INVALID_NUMBER:
            return "Invalid number format";
        case ErrorCode::INVALID_IDENTIFIER:
            return "Invalid identifier '" + context + "'";
        case ErrorCode::INVALID_EXPRESSION:
            return "Invalid expression";
        default:
            return "Unknown error";
    }
}

std::string ErrorHandler::getSuggestion(ErrorCode code, const std::string& context) {
    switch (code) {
        case ErrorCode::UNEXPECTED_TOKEN:
            if (context == "import") {
                return "Did you mean 'use'? In FLAST, we use 'use' for importing modules.";
            }
            return "Check the syntax and ensure all tokens are properly placed.";
        case ErrorCode::UNTERMINATED_STRING:
            return "Add a closing quote (\") to terminate the string.";
        default:
            return "Review the code around the error location for syntax issues.";
    }
}

std::vector<std::string> ErrorHandler::getHints(ErrorCode code, const std::string& context) {
    std::vector<std::string> hints;
    switch (code) {
        case ErrorCode::UNEXPECTED_TOKEN:
            hints.push_back("FLAST uses 'use' instead of 'import' for module imports");
            hints.push_back("No semicolons are required at the end of statements");
            hints.push_back("Check for balanced parentheses, braces, and brackets");
            break;
        default:
            hints.push_back("Refer to the FLAST documentation for syntax guidelines");
            hints.push_back("Use --help for compiler options and examples");
    }
    return hints;
}

std::string ErrorHandler::formatErrorLocation(const ErrorContext& context) {
    std::stringstream ss;
    ss << getColorCode("cyan") << context.fileName << resetColor() << ":";
    ss << getColorCode("yellow") << context.line << resetColor() << ":";
    ss << getColorCode("yellow") << context.column << resetColor();
    return ss.str();
}

std::string ErrorHandler::formatErrorLine(const ErrorContext& context) {
    if (context.lineContent.empty()) return "";
    std::stringstream ss;
    ss << "  " << context.lineContent;
    return ss.str();
}

std::string ErrorHandler::formatErrorPointer(const ErrorContext& context) {
    if (context.lineContent.empty() || context.column <= 0) return "";
    std::stringstream ss;
    ss << "  ";
    for (int i = 1; i < context.column; ++i) {
        if (i < context.lineContent.length() && context.lineContent[i-1] == '\t') {
            ss << "    ";
        } else {
            ss << " ";
        }
    }
    ss << getColorCode("red") << getColorCode("bold") << "^" << resetColor();
    if (!context.tokenValue.empty()) {
        for (size_t i = 1; i < context.tokenValue.length(); ++i) {
            ss << getColorCode("red") << getColorCode("bold") << "~" << resetColor();
        }
    }
    return ss.str();
}

void ErrorHandler::reportError(ErrorCode code, const std::string& message, const ErrorContext& context) {
    std::string errorMsg = message.empty() ? getErrorMessage(code, context.tokenValue) : message;
    std::string suggestion = getSuggestion(code, context.tokenValue);
    std::vector<std::string> hints = getHints(code, context.tokenValue);
    CompilerError error(ErrorLevel::ERROR, code, errorMsg, context);
    error.context.suggestion = suggestion;
    error.hints = hints;
    errors.push_back(error);
    hasErrors = true;
}

void ErrorHandler::reportWarning(ErrorCode code, const std::string& message, const ErrorContext& context) {
    std::string warningMsg = message.empty() ? getErrorMessage(code, context.tokenValue) : message;
    std::string suggestion = getSuggestion(code, context.tokenValue);
    std::vector<std::string> hints = getHints(code, context.tokenValue);
    CompilerError warning(ErrorLevel::WARNING, code, warningMsg, context);
    warning.context.suggestion = suggestion;
    warning.hints = hints;
    warnings.push_back(warning);
    hasWarnings = true;
}

void ErrorHandler::reportFatal(ErrorCode code, const std::string& message, const ErrorContext& context) {
    std::string fatalMsg = message.empty() ? getErrorMessage(code, context.tokenValue) : message;
    std::string suggestion = getSuggestion(code, context.tokenValue);
    std::vector<std::string> hints = getHints(code, context.tokenValue);
    CompilerError fatal(ErrorLevel::FATAL, code, fatalMsg, context);
    fatal.context.suggestion = suggestion;
    fatal.hints = hints;
    errors.push_back(fatal);
    hasErrors = true;
}

ErrorContext ErrorHandler::createContext(const std::string& fileName, int line, int column, const std::string& lineContent, const std::string& tokenValue) {
    ErrorContext context(fileName, line, column);
    context.lineContent = lineContent;
    context.tokenValue = tokenValue;
    return context;
}

void ErrorHandler::printErrors() {
    if (errors.empty()) return;
    std::cout << "\n" << getColorCode("red") << getColorCode("bold") << "=== COMPILATION ERRORS ===" << resetColor() << "\n\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        const auto& error = errors[i];
        std::cout << getColorCode("red") << getColorCode("bold") << "error[" << formatErrorCode(error.code) << "]: " << resetColor() << error.message << "\n";
        if (!error.context.fileName.empty()) {
            std::cout << "  --> " << formatErrorLocation(error.context) << "\n";
        }
        if (!error.context.lineContent.empty()) {
            std::cout << formatErrorLine(error.context) << "\n";
            std::cout << formatErrorPointer(error.context) << "\n";
        }
        if (!error.context.suggestion.empty()) {
            std::cout << "\n" << getColorCode("cyan") << "help: " << resetColor() << error.context.suggestion << "\n";
        }
        if (!error.hints.empty()) {
            std::cout << "\n" << getColorCode("yellow") << "hints:" << resetColor() << "\n";
            for (const auto& hint : error.hints) {
                std::cout << "  • " << hint << "\n";
            }
        }
        if (i < errors.size() - 1) {
            std::cout << "\n";
        }
    }
}

void ErrorHandler::printWarnings() {
    if (warnings.empty()) return;
    std::cout << "\n" << getColorCode("yellow") << getColorCode("bold") << "=== COMPILATION WARNINGS ===" << resetColor() << "\n\n";
    for (size_t i = 0; i < warnings.size(); ++i) {
        const auto& warning = warnings[i];
        std::cout << getColorCode("yellow") << getColorCode("bold") << "warning[" << formatErrorCode(warning.code) << "]: " << resetColor() << warning.message << "\n";
        if (!warning.context.fileName.empty()) {
            std::cout << "  --> " << formatErrorLocation(warning.context) << "\n";
        }
        if (!warning.context.lineContent.empty()) {
            std::cout << formatErrorLine(warning.context) << "\n";
            std::cout << formatErrorPointer(warning.context) << "\n";
        }
        if (!warning.context.suggestion.empty()) {
            std::cout << "\n" << getColorCode("cyan") << "help: " << resetColor() << warning.context.suggestion << "\n";
        }
        if (i < warnings.size() - 1) {
            std::cout << "\n";
        }
    }
}

void ErrorHandler::printAllIssues() {
    printErrors();
    printWarnings();
    if (hasErrors || hasWarnings) {
        std::cout << "\n" << getErrorSummary() << "\n";
    }
}

std::string ErrorHandler::getErrorSummary() const {
    std::stringstream ss;
    if (hasErrors) {
        ss << getColorCode("red") << getColorCode("bold") << "❌ Compilation failed with " << errors.size() << " error(s)";
        if (hasWarnings) {
            ss << " and " << warnings.size() << " warning(s)";
        }
    } else if (hasWarnings) {
        ss << getColorCode("yellow") << getColorCode("bold") << "⚠️  Compilation succeeded with " << warnings.size() << " warning(s)";
    } else {
        ss << getColorCode("green") << getColorCode("bold") << "✅ Compilation successful";
    }
    ss << resetColor();
    return ss.str();
} 