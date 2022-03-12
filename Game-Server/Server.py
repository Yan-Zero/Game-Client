import socket

class Player:
  sock: socket
  addr: tuple
  encoding = 'utf-8'

  room = None
  ready: bool = False

  def __init__(self, uid, socket: socket, addr, name: str) -> None:
    self.sock = socket
    self.addr = addr
    self.uid = uid
    self.name = name

  def send(self, msg: str):
    self.sock.send(b"{" + bytes(msg, self.encoding) + b'}')

  def recv(self) -> str:
    return self.sock.recv(1024).decode(self.encoding)

class Room:
  Host: Player
  Name: str
  UID: id
  Players: list[Player]

  def __init__(self, name: str, host: Player) -> None:
    self.Name = name
    self.Players = [host]
    self.Host = host
    self.UID = hash(name + str(host.uid))

  def add_player(self, player: Player) -> None:
    if player in self.Players:
      return
    if player.room != None:
      player.room.remove_player(player)
    player.room = self
    self.Players.append(player)
