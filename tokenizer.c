#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token* tokenize(const char *line, int line_num) {
    (void)line_num; 
    size_t len = strlen(line);
    Token *tokens = malloc(sizeof(Token) * (len + 1)); 
    if (!tokens) return NULL;
    int count = 0;
    
    const char *p = line;
    while (*p) {
        while (*p && isspace(*p)) p++;
        if (!*p) break;
        
        if (isdigit(*p) || (*p == '-' && isdigit(*(p+1)))) {
            const char *start = p;
            while (*p && (isdigit(*p) || *p == '.')) p++;
            size_t len_num = p - start;
            tokens[count].type = TOKEN_NUMBER;
            tokens[count].text = malloc(len_num + 1);
            if (!tokens[count].text) goto error;
            memcpy(tokens[count].text, start, len_num);
            tokens[count].text[len_num] = '\0';
            count++;
            continue;
        }
        
        if (*p == '"') {
            p++; 
            const char *start = p;
            while (*p && *p != '"') p++;
            if (*p != '"') { 
                tokens[count].type = TOKEN_ERROR;
                tokens[count].text = strdup("Unterminated string");
                count++;
                goto done;
            }
            size_t len_str = p - start;
            tokens[count].type = TOKEN_STRING;
            tokens[count].text = malloc(len_str + 1);
            if (!tokens[count].text) goto error;
            memcpy(tokens[count].text, start, len_str);
            tokens[count].text[len_str] = '\0';
            p++; 
            count++;
            continue;
        }
        
        if (*p == ',') {
            tokens[count].type = TOKEN_COMMA;
            tokens[count].text = strdup(",");
            count++;
            p++;
            continue;
        }
        if (*p == '(') {
            tokens[count].type = TOKEN_LPAREN;
            tokens[count].text = strdup("(");
            count++;
            p++;
            continue;
        }
        if (*p == ')') {
            tokens[count].type = TOKEN_RPAREN;
            tokens[count].text = strdup(")");
            count++;
            p++;
            continue;
        }
        if (*p == '=') {
            tokens[count].type = TOKEN_EQ;
            tokens[count].text = strdup("=");
            count++;
            p++;
            continue;
        }
        
        if (isalpha(*p) || *p == '_') {
            const char *start = p;
            while (*p && (isalnum(*p) || *p == '_')) p++;
            size_t len_word = p - start;
            tokens[count].type = TOKEN_WORD;
            tokens[count].text = malloc(len_word + 1);
            if (!tokens[count].text) goto error;
            memcpy(tokens[count].text, start, len_word);
            tokens[count].text[len_word] = '\0';
            count++;
            continue;
        }
        
        tokens[count].type = TOKEN_ERROR;
        tokens[count].text = malloc(16);
        if (!tokens[count].text) goto error;
        snprintf(tokens[count].text, 16, "Unexpected '%c'", *p);
        count++;
        p++;
    }
    
done:
    tokens[count].type = TOKEN_EOF;
    tokens[count].text = NULL;
    return tokens;

error:
    for (int i = 0; i < count; i++) free(tokens[i].text);
    free(tokens);
    return NULL;
}

void free_tokens(Token *tokens) {
    if (!tokens) return;
    int i = 0;
    while (tokens[i].type != TOKEN_EOF) {
        free(tokens[i].text);
        i++;
    }
    free(tokens);
}

Token* peek_token(Token *tokens, int *pos) {
    return &tokens[*pos];
}

void consume_token(Token *tokens, int *pos) {
    (void)tokens; 
    (*pos)++;
}

bool match_token(Token *tokens, int *pos, TokenType type, const char *text) {
    Token *t = &tokens[*pos];
    if (t->type != type) return false;
    if (text && strcmp(t->text, text) != 0) return false;
    (*pos)++;
    return true;
}