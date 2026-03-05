enum TokenType {
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,

    STRING,
    NUMBER,

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
