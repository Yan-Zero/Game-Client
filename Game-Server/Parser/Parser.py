from Lexer import *

class Parser:
  __lexer: Lexer
  __lookahead: Token

  def __init__(self, lexer) -> None:
    self.__lexer = lexer
  
  def commands(self) -> str:

    command = self.__lexer.scan()
    if command == "":
      return None
    return command