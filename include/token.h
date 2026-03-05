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

    SEMICOLON,

    EQUALS,
    EQUALS_EQUALS,
    EQUALS_EQUALS_EQUALS,
    EQUALS_EQUALS_EQUALS_EQUALS,
    NOT_EQUALS,

    QUESTION,

    END_FILE
};

struct Token {
    enum TokenType type;
    unsigned int line;
    char lexeme[256];
    void* literal;
};
