from Parser.Lexer import *

class PCommand:
  func_name: str = ""
  args: list[Token]

  def __init__(self):
    self.args = []

  def GetArgs(self):
    ret = '"'
    for arg in self.args:
      ret += arg.value.replace('"', '\\"') + '" '
    return ret[:-1]

  def __str__(self):
    return self.func_name + " " + self.GetArgs()

class Parser:
  __lexer: Lexer
  __lookahead: Token

  __p_cmd: PCommand = None
  PCMDs: list[PCommand] = []

  def __init__(self, lexer) -> None:
    if isinstance(lexer, Lexer):
      self.__lexer = lexer
    elif isinstance(lexer, str):
      self.__lexer = Lexer(lexer)
    else:
      raise TypeError("Expected Lexer or str, but got: " + str(type(lexer)))
    self.move()

  def move(self):
    self.__lookahead = self.__lexer.scan()

  def match(self, token_type: TokenType):
    if self.__lookahead.type != token_type:
      raise TypeError("Expected token type: " + str(token_type) + ", but got: " + str(self.__lookahead.type))
    self.move()

  def parse(self):
    self.PCMDs = []
    self.stmt()

  # command -> id(params) | id params
  def command(self):
    self.__p_cmd = PCommand()
    self.id()
    if self.__lookahead.type == TokenType.LeftParenthesis:
      self.match(TokenType.LeftParenthesis)
      self.params()
      self.match(TokenType.RightParenthesis)
    else:
      self.params()
    self.PCMDs.append(self.__p_cmd)

  # stmt -> block block* | command (, command)*
  def stmt(self):
    if self.__lookahead.type == TokenType.LeftBracket:
      self.block()
      while self.__lookahead.type == TokenType.LeftBracket:
        self.block()
    else:
      self.command()
      while self.__lookahead.type == TokenType.Comma:
        self.match(TokenType.Comma)
        self.command()

  # block -> { stmt* }
  def block(self):
    self.match(TokenType.LeftBracket)
    while self.__lookahead.type != TokenType.RightBracket:
      self.stmt()
    self.match(TokenType.RightBracket)

  # param -> string
  def param(self):
    self.__p_cmd.args.append(self.__lookahead)
    self.match(TokenType.String)

  # params_space -> param params
  # params_comma -> param (, params)
  # params -> params_comma | params_space | param | ϵ 
  def params(self):
    if self.__lookahead.type == TokenType.String:
      self.param()
      # params_comma
      if self.__lookahead.type == TokenType.Comma:
        while self.__lookahead.type == TokenType.Comma:
          self.match(TokenType.Comma)
          self.param()
      # params_space
      elif self.__lookahead.type == TokenType.String:
        while self.__lookahead.type == TokenType.String:
          self.param()
    # ϵ

  # id -> [a-zA-Z_][a-zA-Z0-9_]*
  def id(self):
    _id = self.__lookahead
    self.match(TokenType.String)
    if not _id.value[0].isalpha() and _id.value[0] != '_':
      raise TypeError("Id must start with a letter or underscore")
    for ch in _id.value[1:]:
      if not ch.isalnum() and ch != '_':
        raise TypeError("id must contain only letters, numbers, or underscores")
    self.__p_cmd.func_name = _id.value