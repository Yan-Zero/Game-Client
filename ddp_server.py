import random
from ddp import *
import socket, sys, time
import threading

# 创建 socket 对象
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# 获取本地主机名
host = "127.0.0.1"
port = 23345
# 绑定端口号
serversocket.bind((host, port))
# 最大房间数量
max_room = 5
max_player_of_room = 5
# 设置最大连接数，超过后排队
serversocket.listen(max_player_of_room * max_room)

sockets = {}
isoffline = {}
iswantedclose = {}
msgs = {}

lock = threading.Lock()

requires = {}
names = {}
readys = []
starting = False

Players = {
}

class Lexer:
    def __init__(self, context):
        self.context = context
        self.pos = 0

    def get_next(self):
        self.ignore_space()
        if self.pos >= len(self.context):
            return ""
        if self.context[self.pos] == '"':
            return self.get_string()
        i = self.context.find(' ', self.pos)
        if i == -1:
            i = len(self.context)
        ret = self.context[self.pos : i]
        self.pos += len(ret)
        return ret

    def get_string(self):
        self.pos += 1
        ret = ""
        while self.pos < len(self.context) and self.context[self.pos] != '"':
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

class Player:
    sock: socket
    addr: tuple
    encoding = 'utf-8'

    def __init__(self, uid, socket: socket, addr, name: str) -> None:
        self.sock = socket
        self.addr = addr
        self.uid = uid
        self.name = name

    def send(self, msg: str):
        self.sock.send(b"{" + bytes(msg, self.encoding) + b'}')

    def recv(self) -> str:
        return self.sock.recv(1024).decode(self.encoding)

def check_name(str) -> bool:
    if str == "" or len(str) > 32:
        return False
    for c in str:
        if c in [" ", "\n", "\t", "\r", "\0", '\f', '\v', '\b']:
            return False
    return True

def info(player: Player, lexer: Lexer):
    value = lexer.get_next()
    if value == "room":
        pass
    elif value == "player":
        value = lexer.get_next()
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
    tk = lexer.get_next()
    if tk in Help_Info:
        return f'show_help "{tk}:" "' + '" "'.join(Help_Info[tk]) + "\""
    else:
        return f'error "HELP - Unknown command: {tk}!\n"'

Help_List = {
    "room":   "room:     the command of room. Please type 'help room' to see more.",
}

def update_help(player: Player, lexer: Lexer):
    for key in Help_List:
        player.send(f'update_help "{key}" "{Help_List[key]}"')
    return None

def rename(player: Player, lexer: Lexer):
    tk = lexer.get_next()
    if not check_name(tk):
        player.send('error "Invalid name!"')
        return 'set name ' + player.name + ' -f'
    player.name = tk
    if lexer.get_next() == "-s":
        return None
    return 'show_message Server "  Your name is ' + player.name + ' now."'

Function_Map = {
    "info": info,
    "exit": client_exit,
    "help": help,
    "rename": rename,
    "update_help": update_help,
}

def Player_Command(player: Player, data: str):
    tk = Lexer(data)
    command = tk.get_next()
    if command == "":
        return None
    if command in Function_Map:
        return Function_Map[command](player, tk)
    return f'error "Unknown command: {command}!"'

def Handler(clientsocket: socket, addr):
    # 对接
    data = clientsocket.recv(1024).decode('utf-8')
    tk = Lexer(data)
    if tk.get_next() != "encode":
        clientsocket.close()
        return
    encoding = tk.get_next()
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

def Accepting():
    while True:
        # 建立客户端连接
        clientsocketaddr = serversocket.accept()
        threading.Thread(target = Handler, args = clientsocketaddr).start()

acceptingthread = threading.Thread(target = Accepting)
acceptingthread.daemon = True
acceptingthread.start()
while True:
    i = input()
    if i == "exit":
        break
    elif i == 'q':
        break
    elif i[0] == 'E':
        for player in Players.values():
            player.send(i[1:])
    else:
        for player in Players.values():
            player.send("show_message Server \"" + i + "\"")
