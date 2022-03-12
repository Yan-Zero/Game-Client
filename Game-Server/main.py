from Server import *
import threading, Game.DDP, socket
from Parser.Lexer import *
from Parser.Parser import *

Lock = threading.Lock()
Players: dict[int, Player] = {
}
UnusedID: dict[str, set[int]] = {}
Rooms: dict[int, Room] = {
  0: Room("Server", Player(0, None, None, "Server")),
}
Rooms[0].UID = 0


def check_name(str) -> bool:
  if str == "" or len(str) > 32:
    return False
  for c in str:
    if c in [" ", "\n", "\t", "\r", "\0", '\f', '\v', '\b']:
      return False
  return True

def list_info(player: Player, args: list[Token]):
  if len(args) == 0:
    return 'error "Invalid arguments!"'
  if args[0].value == "player":
    ret = '"  uid\t\tname" '
    for uid in Players:
      ret += '"  ' + str(uid) + '\t\t' + Players[uid].name + '" '
    return "show_message Server " + ret
  elif args[0].value == "room":
    ret = '"  uid\t\tname" '
    for uid in Rooms:
      ret += '"  ' + str(uid) + '\t\t' + Rooms[uid].Name + '" '
    return "show_message Server " + ret
  else:
    return 'error "Invalid arguments!"'

def info(player: Player, args: list[Token]):
  if len(args) == 0:
    return 'error "The argument of info must be room or player!"'
  if args[0] == "room":
    if len(args) == 1:
      if player.room.UID != 0:
        return 'show_message Server "  You are in room ' + player.room.Name + '."'
      else:
        return list_info(player, args)
    else:
      value = int(args[1].value)
      if value in Rooms:
        ret  = '"  name: ' + Rooms[value].Name + '" '
        ret += '"  host:" '
        ret += '"      uid : ' + str(Rooms[value].Host.uid) + '" '
        ret += '"      name: ' + Rooms[value].Host.name + '" '
        ret += '"  players:" '
        for player in Rooms[value].Players:
          ret += '"      uid : ' + str(player.uid) + '" '
          ret += '"      name: ' + player.name + '" '
          ret += '"      ready: ' + str(player.ready) + '" '
          ret += '"      "'
        return 'show_message Server ' + ret
      else:
        return 'error "Room ' + args[1].value + ' does not exist!"'
  elif args[0] == "player":
    if len(args) == 1:
      value = str(player.uid)
    else:
      value = args[1].value
    try:
      uid = int(value)
      ret  = '"  name: ' + Players[uid].name + '" '
      ret += '"  uid:  ' + str(uid) + '" '
      return "show_message Server " + ret
    except Exception as e:
      print(f"Error(Player: {player.name}[{player.uid}]): ", e)
      player.send('error "' + str(e) + '"')
      return 'error "Invalid player id!"'
  else:
    return 'error "The argument of info must be room or player!"'

def client_exit(player: Player, args: list[Token]):
  return None

def rename(player: Player, args: list[Token]):
  if len(args) == 0:
    return 'error "rename - Invalid arguments!"'
  if not check_name(args[0].value):
    player.send('error "Invalid name!"')
    return 'set name ' + player.name + ' -f'
  player.name = args[0].value
  if len(args) > 1 and args[1].value == "-s":
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

def help(player: Player, args: list[Token]):
  if len(args) == 0:
    return 'error "No command specified!"'
  if args[0].value in Help_Info:
    return f'show_help "{args[0].value}:" "' + '" "'.join(Help_Info[args[0].value]) + "\" ' '"
  else:
    return f'error "HELP - Unknown command: {args[0].value}!\n"'

def update_help(player: Player, args: list[Token]):
  for key in Help_List:
    player.send(f'update_help "{key}" "{Help_List[key]}"')
  return None

Function_Map = {
  "info": info,
  "exit": client_exit,
  "help": help,
  "rename": rename,
  "update_help": update_help,
  "list": list_info,

}

def Player_Command(player: Player, data: str):
  parser = Parser(data)
  try:
    parser.parse()
    for cmd in parser.PCMDs:
      if cmd.func_name in Function_Map:
        return Function_Map[cmd.func_name](player, cmd.args)
      else:
        return 'error "Unknown command: ' + cmd.func_name + '!"'
  except Exception as e:
    print(f"Error(Player: {player.name}[{player.uid}]): ", e)
    return 'error "' + str(e) + '"'

def Handler(clientsocket, addr):
  # 对接
  data = clientsocket.recv(1024).decode('utf-8')
  parser = Parser(data)
  parser.parse()
  if parser.PCMDs[0].func_name != "encode":
    clientsocket.close()
    return
  encoding = parser.PCMDs[0].args[0].value
  clientsocket.send('OK'.encode(encoding))

  # 校验身份，分配 id，并重命名
  data = clientsocket.recv(1024).decode(encoding)
  player = Player(UnusedID['player'].pop(), clientsocket, addr, "Anonymous")
  player.encoding = encoding
  Players[player.uid] = player
  Player_Command(player, data)
  Player_Command(player, 'update_help')
  player.send('set __id__ ' + str(player.uid))
  player.send(info(player, [Token(TokenType.String, "player")]))
  Rooms[0].add_player(player)

  # 正式运行
  while True:
    try:
      data = player.recv()
    except Exception as e:
      print(player.uid, e)
      try:
        player.send('error "' + str(e) + '"')
      except Exception as ex:
        del Players[player.uid]
        UnusedID['player'].add(player.uid)
        print(ex)
        break
    else:
      ret = Player_Command(player, data)
      if ret != None:
        player.send(ret)
      if data == "exit":
        del Players[player.uid]
        UnusedID['player'].add(player.uid)
        player.sock.close()

def Accepting(socket):
  while True:
    # 建立客户端连接
    clientsocketaddr = socket.accept()
    threading.Thread(target = Handler, args = clientsocketaddr).start()

def main(port: int, max_players: int, max_rooms: int):
  # 创建 socket 对象
  serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  serversocket.bind(("127.0.0.1", port))
  serversocket.listen(max_players)
  UnusedID['player'] = set(range(1, max_players + 1))
  UnusedID['room'] = set(range(1, max_rooms + 1))

  acceptingthread = threading.Thread(target = Accepting, args = (serversocket, ))
  acceptingthread.daemon = True
  acceptingthread.start()
  while True:
    command = input()
    for player in Players.values():
      player.send(command)

if __name__ == "__main__":
  main(23333, 50, 10)
