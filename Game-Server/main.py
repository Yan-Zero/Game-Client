from Server import *
import socket, sys, time
import threading
from Parser.Lexer import *
sys.path.append("./Game")
from ddp import * 

Lock = threading.Lock()
Players = {}
Rooms = {}

def check_name(str) -> bool:
  if str == "" or len(str) > 32:
    return False
  for c in str:
    if c in [" ", "\n", "\t", "\r", "\0", '\f', '\v', '\b']:
      return False
  return True

def info(player: Player, lexer: Lexer):
  value = lexer.get_next_token()
  if value == "room":
    pass
  elif value == "player":
    value = lexer.get_next_token()
    if value == "":
      value = str(player.uid)
    try:
      uid = int(value)
      ret  = '"  name: ' + Players[uid].name + '" '
      ret += '"  uid:  ' + str(uid) + '" '
      return "show_message Server " + ret
    except Exception as e:
      print(player.uid, e)
      player.send('recv 2')
      player.send('error "' + str(e) + '"')
      return 'error "Invalid player id!"'
  else:
    return 'error "The argument of info must be room or player!"'

def client_exit(player: Player, lexer: Lexer):
  return None

def rename(player: Player, lexer: Lexer):
  tk = lexer.get_next_token()
  if not check_name(tk):
    player.send('error "Invalid name!"')
    return 'set name ' + player.name + ' -f'
  player.name = tk
  if lexer.get_next_token() == "-s":
    return None
  return 'show_message Server "  Your name is ' + player.name + ' now."'

Help_List = {
  "room":   "room:     the command of room. Please type 'help room' to see more.",
}

Help_Info = {
  "room" : [
    "  Running HOST command needs that the player is host of the room.",
    "      room list:               list all rooms",
    "      room create <room name>: create a room",
    "      room join <room name>:   join a room",
    "      room leave:              leave a room",
    " HOST room kick <player id>:   kick a player",
    " HOST room kick <player name>: kick a player",
    " HOST room start:              start the game",
    " HOST room stop:               stop the game",
  ]
}

def help(player: Player, lexer: Lexer):
  tk = lexer.get_next_token()
  if tk in Help_Info:
    return f'show_help "{tk}:" "' + '" "'.join(Help_Info[tk]) + "\""
  else:
    return f'error "HELP - Unknown command: {tk}!\n"'

def update_help(player: Player, lexer: Lexer):
  for key in Help_List:
    player.send(f'update_help "{key}" "{Help_List[key]}"')
  return None

Function_Map = {
  "info": info,
  "exit": client_exit,
  "help": help,
  "rename": rename,
  "update_help": update_help,
}

def Player_Command(player: Player, data: str):
  tk = Lexer(data)
  command = tk.get_next_token()
  if command == "":
    return None
  if command in Function_Map:
    return Function_Map[command](player, tk)
  return f'error "Unknown command: {command}!"'

def Handler(clientsocket: socket, addr):
  # 对接
  data = clientsocket.recv(1024).decode('utf-8')
  tk = Lexer(data)
  if tk.get_next_token() != "encode":
      clientsocket.close()
      return
  encoding = tk.get_next_token()
  clientsocket.send('OK'.encode(encoding))

  # 校验身份，分配 id，并重命名
  data = clientsocket.recv(1024).decode(encoding)
  uid = hash(data + str(addr))
  player = Player(uid, clientsocket, addr, "Anonymous")
  player.encoding = encoding
  Players[uid] = player
  Player_Command(player, data)
  player.send('set __id__ ' + str(player.uid))
  Player_Command(player, 'update_help')

  # 正式运行
  while True:
    try:
      data = player.recv()
    except Exception as e:
      print(player.uid, e)
      try:
        player.send('error "' + str(e) + '"')
      except Exception as ex:
        print(ex)
        break
    else:
      ret = Player_Command(player, data)
      if ret != None:
        player.send(ret)
      if data == "exit":
        del Players[uid]
        player.sock.close()

def Accepting(socket: socket):
  while True:
    # 建立客户端连接
    clientsocketaddr = socket.accept()
    threading.Thread(target = Handler, args = clientsocketaddr).start()

def main(port: int, max_players: int, max_rooms: int):
  # 创建 socket 对象
  serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  serversocket.bind(("127.0.0.1", port))
  serversocket.listen(max_players)

  acceptingthread = threading.Thread(target = Accepting, args = (serversocket))
  acceptingthread.daemon = True
  acceptingthread.start()
  while True:
    command = input()

if __name__ == "__main__":
  main(2256, 50, 10)
