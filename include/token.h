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
    RETURN,
    IMPORT,
    STRING,
    NUMBER,

    IF,
    WHILE,
    AND,
    OR,

    VAR,
    INT,
    FLOAT,
    STRING_TYPE,

    COMMA,
    SEMICOLON,

    EQUALS,
    EQUALS_EQUALS,
    NOT_EQUALS,
    GT,
    GTE,
    LT,
    LTE,

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
