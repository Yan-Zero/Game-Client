from enum import Enum


class TokenType(Enum):
  EOF = 0
  String = 1
  LeftBracket = 2
  RightBracket = 3
  LeftParenthesis = 4
  RightParenthesis = 5
  Comma = 6

class Token:
  def __init__(self, type: TokenType, value: str = ""):
    self.type = type
    self.value = value

  def __str__(self):
    return f"{self.type}: {self.value}"

  def __repr__(self):
    return self.__str__()

  def __eq__(self, __o: object) -> bool:
    if isinstance(__o, Token):
      return self.type == __o.type and self.value == __o.value
    if self.type == TokenType.String:
      if __o is str:
        return self.value == __o
      return False
    elif __o is TokenType:
        return self.type == __o
    return False

class Lexer:
  def __init__(self, context):
    self.context = context
    self.pos = 0

  def get_next_token(self) -> Token:
    self.ignore_space()
    if self.pos >= len(self.context):
      return Token(TokenType.EOF)
    if self.context[self.pos] in ['"', "'"]:
      return Token(TokenType.String, self.get_string())
    elif self.context[self.pos] == '{':
      self.pos += 1
      return Token(TokenType.LeftBracket)
    elif self.context[self.pos] == '}':
      self.pos += 1
      return Token(TokenType.RightBracket)
    elif self.context[self.pos] == '(':
      self.pos += 1
      return Token(TokenType.LeftParenthesis)
    elif self.context[self.pos] == ')':
      self.pos += 1
      return Token(TokenType.RightParenthesis)
    elif self.context[self.pos] == ',':
      self.pos += 1
      return Token(TokenType.Comma)
    ret = ""
    while  (self.pos < len(self.context)) and \
           (self.context[self.pos] != ' ' 
        and self.context[self.pos] != '\t'
        and self.context[self.pos] != '\n' 
        and self.context[self.pos] != '\r' 
        and self.context[self.pos] != '\f' 
        and self.context[self.pos] != '\v'):
      if(self.context[self.pos] == '\\'):
        self.pos += 1
        if self.pos >= len(self.context):
          return Token(TokenType.String, ret)
        if self.context[self.pos] == '"':
          ret += '"'
        elif self.context[self.pos] == '\\':
          ret += '\\'
        elif self.context[self.pos] == 'n':
          ret += '\n'
        elif self.context[self.pos] == '(':
          ret += '('
        elif self.context[self.pos] == ')':
          ret += ')'
        elif self.context[self.pos] == 't':
          ret += '\t'
        elif self.context[self.pos] == '{':
          ret += '{'
        elif self.context[self.pos] == '}':
          ret += '}'
        elif self.context[self.pos] == 'r':
          ret += '\r'
        elif self.context[self.pos] == "'":
          ret += "'"
        else:
          ret += self.context[self.pos]
      else:
        self.pos += 1
    return Token(TokenType.String, ret)

  def get_string(self):
    ch = self.context[self.pos]
    self.pos += 1
    ret = ""
    while self.pos < len(self.context) and self.context[self.pos] != ch:
      if self.context[self.pos] == '\\':
        self.pos += 1
        if self.pos >= len(self.context):
          return ret
        if self.context[self.pos] == '"':
          ret += '"'
        elif self.context[self.pos] == '\\':
          ret += '\\'
        elif self.context[self.pos] == 'n':
          ret += '\n'
        elif self.context[self.pos] == "'":
          ret += "'"
        elif self.context[self.pos] == 't':
          ret += '\t'
        elif self.context[self.pos] == 'r':
          ret += '\r'
        else:
          ret += self.context[self.pos]
      else:
        ret += self.context[self.pos]
      self.pos += 1
    self.pos += 1
    return ret

  def ignore_space(self):
    while (self.pos < len(self.context)) and \
          (self.context[self.pos] == ' ' 
        or self.context[self.pos] == '\t'
        or self.context[self.pos] == '\n' 
        or self.context[self.pos] == '\r' 
        or self.context[self.pos] == '\f' 
        or self.context[self.pos] == '\v'):
      self.pos += 1

