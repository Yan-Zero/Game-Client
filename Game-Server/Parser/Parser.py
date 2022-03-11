from Lexer import *

class Parser:
  __lexer: Lexer

  def __init__(self, lexer) -> None:
    self.__lexer = lexer
  
  def command(self) -> str:
    command = self.__lexer.get_next_token()
    if command == "":
      return None
    return command