#include <bits/stdc++.h>
#include "json.h"

namespace json 
{

namespace 
{

std::string
escaped_char(char c)
{
    switch (c) {
    case '\\': return "\\\\";
    case '/' : return "\\/";
    case '\"': return "\\\"";
    case '\a': return "\\a";
    case '\b': return "\\b";
    case '\f': return "\\f";
    case '\n': return "\\n";
    case '\r': return "\\r";
    case '\t': return "\\t";
    default  : throw std::runtime_error("incorrect escape sequence");
    }
}

struct string_visitor
{
    std::string operator()(null_type) const       { return "null"; }
    std::string operator()(integer i) const       { return std::to_string(i); }
    std::string operator()(real r) const          { return std::to_string(r); }
    std::string operator()(boolean b) const       { return b ? "true" : "false"; }
    std::string operator()(const string &s) const;

    std::string operator()(const object &obj) const;
    std::string operator()(const array &arr) const;
};

template <typename Iter, typename ElemToStrFunc>
std::string
iterable_to_string(
    Iter begin, Iter end, 
    const std::string &open, const std::string &close,
    ElemToStrFunc conv
)
{
    if (begin == end)
        return open + close;

    std::string result = open + " ";
    for (auto pos = begin; pos != end; ++pos) {
        if (begin != pos)
            result += ", ";
        result += conv(*pos);
    }
    result += " " + close;
    return result;
}

std::string
escape_chars(const string &s)
{
    std::string result;
    std::size_t curr_pos = 0;
    while (std::string::npos != curr_pos) {
        std::size_t next_pos = s.find_first_of("\\/\'\"\a\b\f\n\r\t", curr_pos);
        result.append(s, curr_pos, next_pos - curr_pos);
        if (std::string::npos != next_pos)
            result += escaped_char(s[next_pos]);
        curr_pos = 
            std::string::npos == next_pos 
            ? next_pos 
            : (next_pos + 1);
    }
    return result;
}

std::string
string_visitor::operator()(const string &s) const
{
    return "\"" + escape_chars(s) + "\"";
}

std::string 
string_visitor::operator()(const object &obj) const
{
    return iterable_to_string(
        obj.cbegin(), obj.cend(), "{", "}",
        [](const object::value_type &p) 
            { return "\"" + escape_chars(p.first) + 
              "\": " + to_string(p.second); }
    );
}

std::string
string_visitor::operator()(const array &arr) const
{
    return iterable_to_string(arr.cbegin(), arr.cend(), "[", "]", to_string);
}

} // namespace 

std::string
to_string(const value &val)
{
    return std::visit(string_visitor(), val);
}

namespace
{

std::unordered_map<token_type, std::string> token_names = { 
    { token_type::open_brace,     "open_brace"     },
    { token_type::close_brace,    "close_brace"    },
    { token_type::open_bracket,   "open_bracket"   },
    { token_type::close_bracket,  "close_bracket"  },
    { token_type::colon,          "colon"          },
    { token_type::comma,          "comma"          },
    { token_type::null_literal,   "null_literal"   },
    { token_type::true_literal,   "true_literal"   },
    { token_type::false_literal,  "false_literal"  },
    { token_type::number_literal, "number_literal" },
    { token_type::string_literal, "string_literal" }
};

std::string
tokens_to_str(token_type tokens)
{
    std::string result = "";
    for (const auto &name_kv: token_names) {
        if (static_cast<int>(tokens & name_kv.first)) {
            if (!result.empty())
                result += ", ";
            result += name_kv.second; 
        }
    }
    return result;
}

struct parse_state
{
    std::size_t begin, curr, end;
    token last_token;
    const std::string &str;
    parse_state(const std::string &str)
        : begin(0), curr(0), end(str.length())
        , last_token { token_type::unknown }
        , str(str) {}
};

void
skip_ws(parse_state &state)
{
    while (state.curr != state.end && std::isspace(state.str[state.curr]))
        ++state.curr;
}

token
tokenize_simple(parse_state &state, token_type type, const std::string &pattern)
{
    int cmp_res = state.str.compare(
        state.curr, pattern.length(), pattern
    );
    if (0 != cmp_res) 
        throw incorrect_token(
            type, 
            token { 
                token_type::unknown, 
                state.str.substr(state.curr, pattern.length()) 
            }
        );
    state.curr += pattern.length();
    return token { type, pattern };
}

void
unescape_char(char &c, parse_state &state)
{
    ++state.curr;
    switch (state.str[state.curr]) {
    case '\\': ++state.curr; c = '\\';  break;
    case '/' : ++state.curr; c = '/';   break;
    case '\"': ++state.curr; c = '\"';  break;
    case 'a' : ++state.curr; c = '\a';  break;
    case 'b' : ++state.curr; c = '\b';  break;
    case 'f' : ++state.curr; c = '\f';  break;
    case 'n' : ++state.curr; c = '\n';  break;
    case 'r' : ++state.curr; c = '\r';  break;
    case 't' : ++state.curr; c = '\t';  break;
    default  : throw unexpected_char_in_str("\\" + state.str[state.curr]);
    }
}

char
to_hex_digit(int d)
{
    return (d < 10 ? (d + '0') : ((d - 10) + 'A'));
}

std::string
to_hex(char c)
{
    return "\\x" + to_hex_digit((c & 0xF0) >> 4) + to_hex_digit(c & 0x0F);
}

token
tokenize_string(parse_state &state)
{
    token result { token_type::string_literal, "" }; 
    ++state.curr;
    while (state.curr != state.end && state.str[state.curr] != '\"') {
        if (state.str[state.curr] >= 0 && state.str[state.curr] <= 0x19)
            throw unexpected_char_in_str(to_hex(state.str[state.curr]));
        if (state.str[state.curr] != '\\') {
            result.contents += state.str[state.curr]; 
            ++state.curr;
        } else {
            char c;
            unescape_char(c, state);
            result.contents += c;
        }
    }
    if (state.curr == state.end)
        throw parse_exception("end of string expected, but end of file found");
    else
        ++state.curr;
    return result;
}

token
tokenize_number(parse_state &state)
{
    std::size_t after_place = state.str.find_first_not_of(
        "0123456789+-Ee.", 
        state.curr
    );
    std::size_t point_place = state.str.find('.', state.curr);
    std::size_t exp_place = state.str.find_first_of("Ee", state.curr);
    std::string number_str = state.str.substr(state.curr, after_place - state.curr);
    token result { token_type::unknown, number_str };
    std::size_t pos;
    if (point_place < after_place || exp_place < after_place) {
        double d = std::stod(number_str, &pos);
        result.type = token_type::real_literal;
    } else {
        long long ll = std::stoll(number_str, &pos);
        result.type = token_type::integer_literal;
    }
    if (pos != number_str.length())
        throw incorrect_number(result);
    state.curr += number_str.length();
    return result;
}

token
extract_next_token(parse_state &state)
{
    if (state.last_token.type != token_type::unknown) {
        token result = state.last_token;
        state.last_token.type = token_type::unknown;
        return result;
    }
    skip_ws(state);
    if (state.curr == state.end)
        throw parse_exception("token expected");
    switch (state.str[state.curr]) {
    case '{': 
        return tokenize_simple(state, token_type::open_brace, "{");
    case '}': 
        return tokenize_simple(state, token_type::close_brace, "}");
    case '[': 
        return tokenize_simple(state, token_type::open_bracket, "[");
    case ']':
        return tokenize_simple(state, token_type::close_bracket, "]");
    case ':':
        return tokenize_simple(state, token_type::colon, ":");
    case ',':
        return tokenize_simple(state, token_type::comma, ",");
    case 'n': 
        return tokenize_simple(state, token_type::null_literal, "null");
    case 't': 
        return tokenize_simple(state, token_type::true_literal, "true"); 
    case 'f': 
        return tokenize_simple(state, token_type::false_literal, "false");
    case '"':
        return tokenize_string(state);
    case '-': 
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':    
        return tokenize_number(state);
    }
    throw unknown_token(
        token { 
            token_type::unknown, 
            std::string(1, state.str[state.curr]) 
        }
    );    
}

void
return_last_token(parse_state &state, token &t)
{
    state.last_token = std::move(t);
}

value do_parse_value(parse_state &state);

value
do_parse_object(parse_state &state)
{
    object obj;

    token next_token { token_type::unknown };
    while (next_token.type != token_type::close_brace) {
        next_token = extract_next_token(state);
        if (next_token.type != token_type::close_brace &&
            next_token.type != token_type::string_literal) { 
            throw incorrect_token(
                token_type::close_brace | token_type::string_literal,
                next_token
            );
        }
        if (next_token.type == token_type::close_brace)
            continue;

        string key(std::move(next_token.contents));
       
        next_token = extract_next_token(state);
        if (next_token.type != token_type::colon)
            throw incorrect_token(token_type::colon, next_token);

        value val = do_parse_value(state);
        obj[key] = val;

        next_token = extract_next_token(state);
        if (next_token.type != token_type::comma &&
            next_token.type != token_type::close_brace) {
            throw incorrect_token(
                token_type::close_brace | token_type::comma,
                next_token
            );
        }
    }

    return value(obj);
}

value
do_parse_array(parse_state &state)
{
    array arr;
    token next_token { token_type::unknown };
    while (next_token.type != token_type::close_bracket) {
        next_token = extract_next_token(state);
        if (next_token.type == token_type::close_bracket)
            continue;
        return_last_token(state, next_token);
        value val = do_parse_value(state);
        arr.push_back(val);
        next_token = extract_next_token(state);
        if (next_token.type != token_type::comma &&
            next_token.type != token_type::close_bracket) {
            throw incorrect_token(
                token_type::close_bracket | token_type::comma,
                next_token
            );
        }
    }
    return value(arr);
}

value
do_parse_value(parse_state &state)
{
    token next_token = extract_next_token(state);
    switch (next_token.type) {
    case token_type::open_brace: 
        return do_parse_object(state);
    case token_type::open_bracket:    
        return do_parse_array(state);
    case token_type::null_literal:
        return null;
    case token_type::true_literal:
        return boolean(true);
    case token_type::false_literal:
        return boolean(false);
    case token_type::integer_literal:
        return integer(std::stoll(next_token.contents));
    case token_type::real_literal:
        return real(std::stod(next_token.contents));
    case token_type::string_literal:
        return string(std::move(next_token.contents));
    }
    throw incorrect_token(token_type::value_start, next_token);
}

value
parse_string(const std::string &str)
{
    parse_state state(str);
    value result = do_parse_value(state);
    skip_ws(state);
    if (state.curr != state.end)
        throw parse_exception("end of document expected");
    return result;
}

} // namespace

std::string
incorrect_token::expected_msg(token_type expected, const token &found)
{
    return "\"" + tokens_to_str(expected) + 
        "\" expected, but \"" + found.contents + "\" found";
}

value
parse(const std::string &json)
{
    return parse_string(json);
}

value
parse_file(const std::string &json_file)
{
    std::ifstream str(json_file, std::ios::binary);
    std::string contents;
    std::getline(str, contents, '\0');
    if (!str.eof())
        throw parse_exception("can't read the file");
    return parse_string(contents);
}

} // namespace json
