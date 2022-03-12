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
    elif __o is TokenType:
      return self.type == __o
    elif self.type == TokenType.String and isinstance(__o, str):
      return self.value == __o
    return False

class Lexer:
  __ch: str

  def __init__(self, context):
    self.context = context
    self.pos = 0

  def scan(self) -> Token:
    self.read_char()
    self.ignore_space()
    if self.__ch == None:
      return Token(TokenType.EOF)
    elif self.__ch in ['"', "'"]:
      return Token(TokenType.String, self.get_string())
    elif self.__ch == '{':
      return Token(TokenType.LeftBracket)
    elif self.__ch == '}':
      return Token(TokenType.RightBracket)
    elif self.__ch == '(':
      return Token(TokenType.LeftParenthesis)
    elif self.__ch == ')':
      return Token(TokenType.RightParenthesis)
    elif self.__ch == ',':
      return Token(TokenType.Comma)
    ret = ""
    while self.__ch not in [' ' , '\t', '\n', '\r', '\f', \
                            '\v', None, ",", "}", ")", "{", "("]:
      if self.__ch == '\\':
        self.read_char()
        if self.__ch == '"':
          ret += '"'
        elif self.__ch == '\\':
          ret += '\\'
        elif self.__ch == 'n':
          ret += '\n'
        elif self.__ch == '(':
          ret += '('
        elif self.__ch == ')':
          ret += ')'
        elif self.__ch == 't':
          ret += '\t'
        elif self.__ch == '{':
          ret += '{'
        elif self.__ch == '}':
          ret += '}'
        elif self.__ch == 'r':
          ret += '\r'
        elif self.__ch == "'":
          ret += "'"
        else:
          ret += self.__ch
      else:
        ret += self.__ch
      self.read_char()
    if self.__ch != None:
      self.pos -= 1
    return Token(TokenType.String, ret)

  def get_string(self):
    ch = self.__ch
    ret = ""
    self.read_char()
    while self.__ch != None and self.__ch != ch:
      if self.__ch == '\\':
        self.read_char()
        if self.__ch == '"':
          ret += '"'
        elif self.__ch == '\\':
          ret += '\\'
        elif self.__ch == 'n':
          ret += '\n'
        elif self.__ch == 't':
          ret += '\t'
        elif self.__ch == 'r':
          ret += '\r'
        elif self.__ch == "'":
          ret += "'"
        else:
          ret += self.__ch
      else:
        ret += self.__ch
      self.read_char()
    return ret

  def ignore_space(self):
    while self.__ch != None and self.__ch in [' ', '\t', '\n', '\r', '\f', '\v']:
      self.read_char()

  def read_char(self, ch: str = None):
    if self.pos >= len(self.context):
      self.__ch = None
      return None
    self.__ch = self.context[self.pos]
    self.pos += 1

    # read_char(ch) -> bool
    if not isinstance(ch, str):
      return None
    if ch != self.__ch:
      return False
    self.__ch == ' '
    return True
