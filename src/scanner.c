#include <string.h>
#include "scanner.h"

void init_scanner(struct Scanner *scanner, const char *source) {
    scanner->source = source;
    scanner->current = source;
    scanner->start = source;
    scanner->line = 1;
}

static char peek(struct Scanner *scanner) {
    return scanner->current[0];
}

static bool is_at_end(struct Scanner *scanner) {
    return peek(scanner) == '\0';
}

static char peek_next(struct Scanner *scanner) {
    if (is_at_end(scanner)) {
        return '\0';
    }
    return scanner->current[1];
}

static char advance(struct Scanner *scanner) {
    scanner->current++;
    return scanner->current[-1];
}

static void skip_whitespace(struct Scanner *scanner) {
    while (true) {
        char c = peek(scanner);
        switch (c) {
            case ' ':
            case '\t':
                scanner->current++;
                break;
            case '\n':
                scanner->current++;
                scanner->line++;
                break;
            default:
                return;
        }
    }
}

struct Token make_token(struct Scanner *scanner, enum TokenType type) {
    struct Token token = {
        .type = type,
        .start = scanner->start,
        .length = (scanner->current - scanner->start),
        .line = scanner->line,
    };
    return token;
}

static bool is_digit(char c) {
    return '0' <= c && c <= '9'; 
}

static bool is_alpha(char c) {
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_';
}

static bool is_alpha_digit(char c) {
    return is_digit(c) || is_alpha(c);
}

static void number(struct Scanner *scanner) {
    while(is_digit(peek(scanner))) {
        advance(scanner);
    }
    if (peek(scanner) == '.' && is_digit(peek_next(scanner))) {
        advance(scanner);
        while (is_digit(peek(scanner))) {
            advance(scanner);
        }
    }
}

static void identifier(struct Scanner *scanner) {
    while(is_alpha_digit(peek(scanner))) {
        advance(scanner);
    }
}

static struct Token check_keyword(struct Scanner *scanner, const char *rest, u32 length, enum TokenType type) {
    u32 token_length = scanner->current - scanner->start;
    if (token_length != length || memcmp(scanner->start+1, rest, length-1)) {
        return make_token(scanner, TOKEN_IDENTIFIER);
    }
    return make_token(scanner, type);
}

static struct Token check_next(struct Scanner *scanner, char next, enum TokenType type1, enum TokenType type2) {
    if (peek(scanner) == next) {
        advance(scanner);
        return make_token(scanner, type1);
    }
    return make_token(scanner, type2);
}

struct Token next_token(struct Scanner *scanner) {
    skip_whitespace(scanner);
    scanner->start = scanner->current;
    if (is_at_end(scanner)) {
        return make_token(scanner, TOKEN_EOF);
    }
    char c = advance(scanner);
    switch (c) {
        case '+': return make_token(scanner, TOKEN_PLUS);
        case '-': return make_token(scanner, TOKEN_MINUS);
        case '*': return make_token(scanner, TOKEN_STAR);
        case '/': return make_token(scanner, TOKEN_SLASH);
        case '(': return make_token(scanner, TOKEN_LEFT_PAREN);
        case ')': return make_token(scanner, TOKEN_RIGHT_PAREN);
        case '{': return make_token(scanner, TOKEN_LEFT_BRACE);
        case '}': return make_token(scanner, TOKEN_RIGHT_BRACE);
        case ';': return make_token(scanner, TOKEN_SEMICOLON);
        case '=': return check_next(scanner, '=', TOKEN_EQUAL_EQUAL, TOKEN_EQUAL);
        case '<': return check_next(scanner, '=', TOKEN_LESS_EQUAL, TOKEN_LESS);
        case '>': return check_next(scanner, '=', TOKEN_GREATER, TOKEN_LESS);
        case '&': return check_next(scanner, '&', TOKEN_AND, TOKEN_ERROR);
        case '|': return check_next(scanner, '|', TOKEN_OR, TOKEN_ERROR);
        case '!': return check_next(scanner, '=', TOKEN_NOT_EQUAL, TOKEN_NOT);
        default:
            if (is_digit(c)) {
                number(scanner);
                return make_token(scanner, TOKEN_NUMBER);
            } else if (is_alpha(c)) {
                identifier(scanner);
                switch (c) {
                    case 'v': return check_keyword(scanner, "ar", 3, TOKEN_VAR);
                    case 'i': return check_keyword(scanner, "f", 2, TOKEN_IF);
                    case 'e': return check_keyword(scanner, "lse", 4, TOKEN_ELSE);
                    case 't': return check_keyword(scanner, "rue", 4, TOKEN_TRUE);
                    case 'f': return check_keyword(scanner, "alse", 5, TOKEN_FALSE);
                    default:  return make_token(scanner, TOKEN_IDENTIFIER);
                }
            } else {
                return make_token(scanner, TOKEN_ERROR);
            }
    };
}