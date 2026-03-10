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
    MODULO,

    IDENTIFIER,
    RETURN,
    IMPORT,
    STRING,
    NUMBER,

    IF,
    ELSE,
    WHILE,
    AND,
    OR,

    VAR,
    INT,
    FLOAT,
    STRING_TYPE,
    OBJECT_TYPE,
    ARRAY_TYPE,

    COMMA,
    COLON,
    PIPE,
    ARROW,
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

    PLUS_EQUALS,
    MINUS_EQUALS,
    MULTIPLY_EQUALS,
    DIVIDE_EQUALS,
    PLUS_PLUS,
    MINUS_MINUS,

    END_FILE
};

struct Token {
    enum TokenType type;
    unsigned int line;
    char lexeme[256];
    struct MayValue* literal;
};
