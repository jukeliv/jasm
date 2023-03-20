#include <stdlib.h>
#include <stdio.h>

unsigned long j_strlen(const char* str)
{
    unsigned long i = 0;
    while(str[i] != '\0')
        i++;
    return i;
}
char j_strcmp(const char* str, const char* str2)
{
    unsigned long len = j_strlen(str);
    if(len < j_strlen(str2) || len > j_strlen(str2))
        return 0;
    for(int i = 0; i < len; i++)
    {
        if(str[i] != str2[i])
            return 0;
    }
    return 1;
}
void* zero(void* p, unsigned long size)
{
    unsigned char* pr = p;
    while(size--)
        *pr++ = 0;
    return p;
}
static inline char j_isnum(char c)
{
    return (c > 47 && c < 58);
}
static inline char j_isalpha(char c)
{
    return ((c > 64 && c < 91) || (c > 96 && c < 123));
}
int j_stoi(const char* str)
{
    int si = 0;
    for(unsigned long i = 0; i < j_strlen(str); i++)
    {
        if(!j_isnum(str[i]))
            return -1;
        si = si * 10 + (str[i] - '0');
    }
    return si;
}
typedef enum
{
    TOK_MEMORY_WRITE,
    TOK_MEMORY_READ,
    TOK_ADD,
    TOK_SUB,
    TOK_CALL,
    TOK_JUMP,
    TOK_JUMP_ZERO,
    TOK_COMPARE,
    TOK_NUM,
} Token_Type;
typedef struct
{
    unsigned long heap;
    unsigned long count; 
    char* content;
} String;
typedef struct
{
    Token_Type type;
    String value;
} Token;
typedef struct
{
    unsigned long heap;
    unsigned long count;
    Token* content;
} Token_List;
static Token Token_Init(Token_Type type, String value)
{
    Token token;
    token.type = type;
    token.value = value;
    return token;
}
static String String_Init()
{
    String str;
    str.count = 0;
    str.heap = 1;
    str.content = malloc(sizeof(char));
    return str;
}
static void String_Push(String* str, char c)
{
    while(str->count >= str->heap)
    {
        str->heap *= 2;
        str->content = realloc(str->content, sizeof(char)*str->heap);
    }
    str->content[str->count++] = c;
    str->content[str->count] = '\0';
}
static void String_Push_Constant_String(String* str, const char* src)
{
    for(int i = 0; i < j_strlen(src); ++i)
    {
        String_Push(str, src[i]);
    }
}
String String_Init_Str(const char* src)
{
    String str;
    str.count = 0;
    str.heap = 1;
    str.content = malloc(sizeof(char));
    String_Push_Constant_String(&str, src);
    return str;
}
void Token_List_Init(Token_List* token_list)
{
    token_list->count = 0;
    token_list->heap = 1;
    token_list->content = malloc(sizeof(Token));
}
void Token_List_Push(Token_List* token_list, Token token)
{
    while(token_list->count >= token_list->heap)
    {
        token_list->heap *= 2;
        token_list->content = realloc(token_list->content, sizeof(Token)*token_list->heap);
    }
    token_list->content[token_list->count++] = token;
}

void Tokenize(const char* source, Token_List* token_list)
{
    unsigned long i = 0;
    char lex[0xfff];
    unsigned long lex_i = 0;

    while(source[i] != '\0')
    {
        lex_i=0;
        zero(lex, 0xfff);
        
        while(source[i] == '\n' || source[i] == '\t')
            ++i;

        if(j_isnum(source[i]))
        {
            lex[lex_i++] = source[i++];
            while(j_isnum(source[i]))
            {
                lex[lex_i++] = source[i++];
            }
            Token_List_Push(token_list, Token_Init(TOK_NUM, String_Init_Str(lex)));
            ++i;
            continue;
        }
        else if(j_isalpha(source[i]))
        {
            while(j_isalpha(source[i]))
            {
                lex[lex_i++] = source[i++];
            }
            
            if(j_strcmp(lex, "mw"))
            {
                Token_List_Push(token_list, Token_Init(TOK_MEMORY_WRITE, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "mr"))
            {
                Token_List_Push(token_list, Token_Init(TOK_MEMORY_READ, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "add"))
            {
                Token_List_Push(token_list, Token_Init(TOK_ADD, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "sub"))
            {
                Token_List_Push(token_list, Token_Init(TOK_SUB, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "call"))
            {
                Token_List_Push(token_list, Token_Init(TOK_CALL, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "jmp"))
            {
                Token_List_Push(token_list, Token_Init(TOK_JUMP, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "jmpz"))
            {
                Token_List_Push(token_list, Token_Init(TOK_JUMP_ZERO, String_Init_Str(lex)));
            }
            else if(j_strcmp(lex, "cmp"))
            {
                Token_List_Push(token_list, Token_Init(TOK_COMPARE, String_Init_Str(lex)));
            }
            ++i;
            continue;
        }
        ++i;
    }
}
char Command_Len(Token_Type type)
{
    if(type == TOK_MEMORY_WRITE)
        return 2;
    else if(type == TOK_MEMORY_READ)
        return 2;
    else if(type == TOK_ADD)
        return 2;
    else if(type == TOK_SUB)
        return 2;
    else if(type == TOK_CALL)
        return 1;
    else if(type == TOK_JUMP)
        return 1;
    else if(type == TOK_JUMP_ZERO)
        return 2;
    else if(type == TOK_COMPARE)
        return 2;
    return 0;
}

int main(void)
{
    const char* source = "mw 10 45\nmw 20 50\nadd 10 20\nmw 500 1\ncall 0";
    Token_List token_list;
    Token_List_Init(&token_list);
    Tokenize(source, &token_list);
    
    for(int i = 0; i < token_list.count; i++)
    {
        Token token = token_list.content[i];
        if(Command_Len(token.type) > 0)
        {
            if(Command_Len(token_list.content[i+Command_Len(token.type)+1].type) == 0 && i+2 < token_list.count)
            {
                printf("ERROR: To many arguments\n");
                printf("%d: %s | %d\n", i, token_list.content[i+Command_Len(token.type)].value.content, token_list.content[i+Command_Len(token.type)].type);
                exit(1);
            }
        }
    }
    unsigned char memory[501]; 
    int i = 0;
    while(i < token_list.count)
    {
        Token token = token_list.content[i];
        if(token.type == TOK_MEMORY_WRITE)
        {
            memory[j_stoi(token_list.content[i+1].value.content)] = j_stoi(token_list.content[i+2].value.content);
            printf("write: %d\n", memory[j_stoi(token_list.content[i+1].value.content)]);
            i+=3;
        }
        else if(token.type == TOK_MEMORY_READ)
        {
            memory[j_stoi(token_list.content[i+1].value.content)] = memory[j_stoi(token_list.content[i+1].value.content)];
            printf("read: %d\n", memory[j_stoi(token_list.content[i+1].value.content)]);
            i+=3;
        }
        else if(token.type == TOK_ADD)
        {
            memory[j_stoi(token_list.content[i+1].value.content)] += memory[j_stoi(token_list.content[i+2].value.content)];
            printf("add: %d\n", memory[j_stoi(token_list.content[i+1].value.content)]);
            i+=3;
        }
        else if(token.type == TOK_SUB)
        {
            memory[j_stoi(token_list.content[i+1].value.content)] -= memory[j_stoi(token_list.content[i+2].value.content)];
            printf("sub: %d\n", memory[j_stoi(token_list.content[i+1].value.content)]);
            i+=3;
        }
        else if(token.type == TOK_CALL)
        {
            //TODO: implement other calls
            switch(j_stoi(token_list.content[i+1].value.content))
            {
                case 0:
                    printf("call 0(Exit): %d\n", memory[500]);
                    exit(memory[500]);
                    break;
            }
            i+=2;
        }
        else if(token.type == TOK_JUMP)
        {
            i = j_stoi(token_list.content[i+1].value.content);
            i+=2;
            printf("jmp: jumped\n");
        }
        else if(token.type == TOK_JUMP_ZERO)
        {
            if(j_stoi(token_list.content[i+1].value.content) == 0)
            {
                i = j_stoi(token_list.content[i+1].value.content);
                printf("jmpz: jumped\n");
            }
            else
            {
                printf("jmpz: continued\n");
            }
            i+=2;
        }
        else if(token.type == TOK_COMPARE)
        {
            if(memory[j_stoi(token_list.content[i+1].value.content)] == memory[j_stoi(token_list.content[i+2].value.content)])
                memory[501] = 0;
            else
                memory[501] = 1;
            
            printf("cmp: %d\n", memory[501]);
            i+=3;
        }
    }
    return 0;
}