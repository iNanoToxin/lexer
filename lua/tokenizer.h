#ifndef LUA_TOKENIZER_H
#define LUA_TOKENIZER_H

#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>

enum class token_type
{
    IDENTIFIER,

    STRING_RAW,
    STRING,

    COMMENT_RAW,
    COMMENT,

    NUMBER_HEXADECIMAL,
    NUMBER_BINARY,
    NUMBER,

    KEYWORD,
    PUNCTUATION
};

struct token
{
public:
    token_type type;
    std::string literal;

    [[nodiscard]] bool is(const std::string& token_literal) const
    {
        return literal == token_literal;
    }

    [[nodiscard]] bool is(const token_type& token_type) const
    {
        return type == token_type;
    }
};


static constexpr const char* keywords[] = {
    "and",
    "break",
    "do",
    "else",
    "elseif",
    "end",
    "false",
    "for",
    "function",
    "if",
    "in",
    "local",
    "nil",
    "not",
    "or",
    "repeat",
    "return",
    "then",
    "true",
    "until",
    "while"
};

static constexpr const char* punctuations[] = {
    "...",

    "<<",
    ">>",
    "//",
    "==",
    "~=",
    "<=",
    ">=",
    "..",
    "::",

    "+",
    "-",
    "*",
    "/",
    "%",
    "^",
    "#",
    "&",
    "~",
    "|",
    "<",
    ">",
    "=",
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",
    ":",
    ";",
    ",",
    "."
};


bool is_digit(char c);
bool is_hexadecimal_digit(char c);
bool is_binary_digit(char c);
bool is_identifier_nondigit(char c);
bool is_identifier(char c);
bool is_escape_character(char c);
bool is_symbol(char c);
bool is_white_space(char c);
bool is_keyword(const std::string& str);


class token_stream
{
public:
    std::size_t length = 0;
    std::size_t index = 0;
    std::string source;
    std::string buffer;
    std::vector<token> tokens;

    void tokenize(const std::string& lua_source)
    {
        this->source = lua_source;
        this->length = lua_source.length();

        while (next())
        {
            if (starts_with("__EXIT__SAFE__"))
            {
                break;
            }

            if (is_comment_token())
            {
                parse_comment();
            }
            else if (is_word_token())
            {
                parse_word();
            }
            else if (is_string_token())
            {
                parse_string();
            }
            else if (is_numeric_token())
            {
                if (starts_with("0x") || starts_with("0X"))
                {
                    parse_hexadecimal_number();
                }
                else if (starts_with("0b") || starts_with("0B"))
                {
                    parse_binary_number();
                }
                else
                {
                    parse_number();
                }
            }
            else if (is_punctuation_token())
            {
                parse_punctuation();
            }
            else if (is_white_space(peek()))
            {
                consume();
            }
            else
            {
                throw std::invalid_argument("unknown token type");
            }
        }
    }

    template <typename T>
    void parse_numeric_notation_sequence(token_type type, T is_digit_type)
    {
        bool is_malformed = false;
        unsigned int length_sequence = 0;

        while (next() && !is_white_space(peek()))
        {
            if (next() && !starts_with('_'))
            {
                if (!starts_with('.') && is_symbol(peek()))
                {
                    break;
                }
                else if (is_digit_type(peek()))
                {
                    length_sequence++;
                }
                else
                {
                    is_malformed = true;
                }
            }
            bump();
        }

        if (is_malformed || length_sequence == 0)
        {
            throw std::invalid_argument("malformed number");
        }

        add_token(type);
    }

    void parse_hexadecimal_number()
    {
        bump(2);
        return parse_numeric_notation_sequence(token_type::NUMBER_HEXADECIMAL, &is_hexadecimal_digit);
    }

    void parse_binary_number()
    {
        bump(2);
        return parse_numeric_notation_sequence(token_type::NUMBER_BINARY, &is_binary_digit);
    }

    void parse_exponential_number()
    {
        bump();

        if (starts_with('-') || starts_with('+'))
        {
            bump();
        }
        return parse_numeric_notation_sequence(token_type::NUMBER, &is_digit);
    }

    void parse_number()
    {
        bool is_malformed = false;
        unsigned int decimal_points = 0;

        while (next() && !is_white_space(peek()))
        {
            // Handle numbers with exponents.
            if (starts_with('e') || starts_with('E'))
            {
                parse_exponential_number();
                return;
            }

            if (!starts_with('_'))
            {
                if (starts_with('.'))
                {
                    decimal_points++;
                }
                else if (is_symbol(peek()))
                {
                    break;
                }
                else if (!is_digit(peek()))
                {
                    is_malformed = true;
                }
            }
            bump();
        }

        if (is_malformed || decimal_points > 1)
        {
            throw std::invalid_argument("malformed number");
        }

        add_token(token_type::NUMBER);
    }

    void parse_long_brackets()
    {
        std::string closing_bracket;

        bump();
        closing_bracket.push_back(']');

        while (starts_with('='))
        {
            bump();
            closing_bracket.push_back('=');
        }

        bump();
        closing_bracket.push_back(']');

        while (next())
        {
            if (starts_with(closing_bracket))
            {
                break;
            }
            bump();
        }
        if (starts_with(closing_bracket))
        {
            bump(closing_bracket.length());
        }
        else
        {
            throw std::invalid_argument("invalid bracket");
        }
    }

    void parse_string()
    {
        if (starts_with_long_bracket())
        {
            parse_long_brackets();
            add_token(token_type::STRING_RAW);
        }
        else
        {
            char quote = peek();
            bump();

            while (next() && !starts_with(quote) && !starts_with('\n'))
            {
                // Handle escape characters in strings.
                if (starts_with('\\'))
                {
                    if (!next(1))
                    {
                        throw std::invalid_argument("invalid escape sequence");
                    }
                    bump();
                }
                bump();
            }

            if (!starts_with(quote))
            {
                throw std::invalid_argument("malformed string");
            }

            bump();
            add_token(token_type::STRING);
        }
    }

    void parse_comment()
    {
        bump(2);

        if (starts_with_long_bracket())
        {
            parse_long_brackets();
            add_token(token_type::COMMENT_RAW);
        }
        else
        {
            while (next() && !starts_with('\n'))
            {
                bump();
            }
            add_token(token_type::COMMENT);
        }
    }

    void parse_word()
    {
        while (next() && is_identifier(peek()))
        {
            bump();
        }

        for (const char* keyword: keywords)
        {
            if (buffer == keyword)
            {
                add_token(token_type::KEYWORD);
                return;
            }
        }
        add_token(token_type::IDENTIFIER);
    }

    void parse_punctuation()
    {
        for (auto punctuation: punctuations)
        {
            if (starts_with(punctuation))
            {
                bump(strlen(punctuation));
                add_token(token_type::PUNCTUATION);
                break;
            }
        }
    }


    bool is_comment_token()
    {
        return starts_with("--");
    }

    bool is_word_token()
    {
        return is_identifier_nondigit(peek());
    }

    bool is_string_token()
    {
        return starts_with('\"') || starts_with('\'') || starts_with_long_bracket();
    }

    bool is_numeric_token()
    {
        return starts_with('.') && next(1) && is_digit(peek(1)) || is_digit(peek());
    }

    bool is_punctuation_token()
    {
        for (const char* punctuation: punctuations)
        {
            if (starts_with(punctuation))
            {
                return true;
            }
        }
        return false;
    }

    char consume()
    {
        return source.at(index++);
    }

    void bump(std::size_t amount = 1)
    {
        for (std::size_t i = 0; i < amount; i++)
        {
            buffer.push_back(consume());
        }
    }

    [[nodiscard]] bool next(std::size_t offset = 0) const
    {
        return index + offset < length;
    }

    [[nodiscard]] char peek(std::size_t offset = 0)
    {
        return source.at(index + offset);
    }

    [[nodiscard]] bool starts_with(const std::string& str) const
    {
        return next(str.length() - 1) && source.substr(index, str.length()) == str;
    }

    [[nodiscard]] bool starts_with(char c)
    {
        return next() && peek() == c;
    }

    [[nodiscard]] bool starts_with_long_bracket()
    {
        if (starts_with('['))
        {
            int i = 1;
            while (next(i) && peek(i) == '=')
            {
                i++;
            }
            if (next(i) && peek(i) == '[')
            {
                return true;
            }
        }
        return false;
    }

    void add_token(const token_type& type)
    {
        // std::cout << buffer << "\n";
        if (type != token_type::COMMENT && type != token_type::COMMENT_RAW)
        {
            tokens.push_back(token{.type = type, .literal = buffer});
        }
        buffer.clear();
    }
};

#endif