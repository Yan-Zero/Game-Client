#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>
#define elif else if

inline std::string string_trim(std::string &str) {
  return str.erase(0, str.find_first_not_of(" \t\n\r\f\v")).erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}

class Token
{
public:
  enum class TokenType {
    EOC,
    String,
    LeftBracket,
    RightBracket, 
    LeftParenthesis,
    RightParenthesis,
    Comma,
  };

  Token(TokenType type) : type_(type) {}
  Token(std::string str, TokenType type) : str_(str), type_(type) {};
  inline TokenType type() { return type_; }
  inline std::string value() { return str_; }
  inline bool operator==(const Token &other) {
    return type_ == other.type_ && str_ == other.str_;
  }
  inline bool operator==(const TokenType &other) {
    return type_ != TokenType::String && type_ == other;
  }
  inline bool operator==(const std::string &other) {
    return type_ == TokenType::String && str_ == other;
  }

private:
  std::string str_;
  TokenType type_;
};

class Lexer
{
public:
  Lexer(std::string &str) {
    str_ = string_trim(str);
    pos_ = 0;
  };

  Token get_next_token();
  size_t get_pos() { return pos_; }

private:
  std::string str_;
  size_t pos_;
  Token lookahead = Token(Token::TokenType::EOC);

  inline void ignore_space()
  {
    while(str_[pos_] == ' ' || str_[pos_] == '\t' || str_[pos_] == '\n' || str_[pos_] == '\r' || str_[pos_] == '\f' || str_[pos_] == '\v')
    {
      pos_++;
      if(pos_ >= str_.size())
        return;
    }
  }
  std::string get_string();

};

#endif