#ifndef _LAB2_JSON_H_
#define _LAB2_JSON_H_

#include <bits/stdc++.h>

namespace json
{

class _value_wrapper;

typedef long long       integer;
typedef double          real;
typedef bool            boolean;
typedef std::monostate  null_type;
typedef std::string     string;

const null_type null = null_type {};

typedef std::map<std::string, _value_wrapper> object;
typedef std::vector<_value_wrapper> array;

typedef std::variant<
    null_type,
    integer,
    real,
    boolean,
    string,
    object,
    array
> value;

class _value_wrapper: public value
{
public:
    template <typename... Args>
    _value_wrapper(Args&&... args)
        : value { std::forward<Args>(args)... } {}
    virtual ~_value_wrapper() = default;
};

template <typename T>
bool inside(const value &val) { return std::holds_alternative<T>(val); }
template <typename T>
T &get(value &val) { return std::get<T>(val); }
template <typename T>
T &&get(value &&val) { return std::get<T>(std::move(val)); }
template <typename T>
const T &get(const value &val) { return std::get<T>(val); }

std::string to_string(const value &val);

enum class token_type
{
    unknown = 0x0000,
    open_brace = 0x0001,         // {
    close_brace = 0x0002,        // }
    open_bracket = 0x0004,       // [
    close_bracket = 0x0008,      // ]
    colon = 0x0010,              // :
    comma = 0x0020,              // ,
    null_literal = 0x0040,       // null
    true_literal = 0x0080,       // true
    false_literal = 0x0100,      // false
    integer_literal = 0x0200,
    real_literal = 0x0400,       // -?(0|[1-9][0-9]*)((.[0-9]+)?([Ee][0-9]+)?)
    number_literal = 
        integer_literal | real_literal,
    string_literal = 0x0800,     // "([:ascii:]|\(["\/nbfrt]|u([0-9a-f]{4}|[0-9A-F]{4}))*"
    value_start = 
        open_brace | open_bracket | null_literal | 
        true_literal | false_literal | integer_literal |
        real_literal | string_literal
};

inline token_type 
operator&(token_type alpha, token_type beta)
{
    return static_cast<token_type>(
        static_cast<int>(alpha) & static_cast<int>(beta)
    );
}

inline token_type 
operator|(token_type alpha, token_type beta)
{
    return static_cast<token_type>(
        static_cast<int>(alpha) | static_cast<int>(beta)
    );
}

struct token
{
    token_type type;
    std::string contents;
};

class parse_exception: public std::runtime_error
{
public:
    parse_exception(const parse_exception &other)
        : std::runtime_error(other) {}
    parse_exception(const std::string &what)
        : std::runtime_error(what) {}    
};

class unknown_token: public parse_exception
{
private:    
    const std::string message = "unknown or incorrect token: ";
public:
    unknown_token(const unknown_token &other)
        : parse_exception(other) {}
    unknown_token(const token &token)
        : parse_exception(message + token.contents) {}
};

class incorrect_number: public parse_exception
{
private:
    const std::string message = "number is incorrect or too big: ";
public:
    incorrect_number(const incorrect_number &other)
        : parse_exception(other) {}
    incorrect_number(const token &token)
        : parse_exception(message + token.contents) {}
};

class unexpected_char_in_str: public parse_exception
{
private:
    const std::string message = "unexpected character in string: ";
public:
    unexpected_char_in_str(const unexpected_char_in_str &other)
        : parse_exception(other) {}
    unexpected_char_in_str(const std::string &c)
        : parse_exception(message + "'" + c + "'") {}
};

class incorrect_token: public parse_exception
{
private:
    static std::string expected_msg(token_type expected, const token &found);
public:
    incorrect_token(const incorrect_token &other)
        : parse_exception(other) {}
    incorrect_token(token_type expected, const token &found)
        : parse_exception(expected_msg(expected, found)) {}
};

value parse(const std::string &json);
value parse_file(const std::string &json_file);

} // namespace json

#endif // _LAB2_JSON_H_
