struct MayValue;
enum TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    OR,

    VOID,
    VAR,
    INT,
    FLOAT,

    SEMICOLON,

    EQUALS,
    EQUALS_EQUALS,
    EQUALS_EQUALS_EQUALS,
    EQUALS_EQUALS_EQUALS_EQUALS,
    NOT_EQUALS,

    BANG,
    QUESTION,

    END_FILE
};

struct Token {
    enum TokenType type;
    unsigned int line;
    char lexeme[256];
    struct MayValue* literal;
};
