import socket

class Player:
  sock: socket
  addr: tuple
  encoding = 'utf-8'

  room = None
  ready: bool = False

  def __init__(self, uid, socket: socket, addr, name: str, room) -> None:
    self.sock = socket
    self.addr = addr
    self.uid = uid
    self.name = name
    if room:
      room.add_player(self)

  def send(self, msg: str):
    if not self.sock:
      return
    self.sock.send(b"{" + bytes(msg, self.encoding) + b'}')

  def recv(self) -> str:
    if not self.sock:
      return ""
    return self.sock.recv(1024).decode(self.encoding)

class Room:
  Host: Player
  Name: str
  UID: int
  Players: list[Player]

  def __init__(self, name: str, host: Player, uid: int) -> None:
    self.Name = name
    self.Players = [host]
    self.Host = host
    self.UID = uid
    if(host.room != None):
      host.room.exit_room(host)
    host.room = self

  def add_player(self, player: Player) -> None:
    if player in self.Players:
      return
    if player.room != None:
      player.room.exit_room(player)
    player.room = self
    self.Players.append(player)
    for p in self.Players:
      p.send('show_message Server "  ' + player.name + ' joined room."')

  def exit_room(self, player: Player) -> None:
    if player not in self.Players:
      return
    self.Players.remove(player)
    for p in self.Players:
      p.send('show_message Server "  ' + player.name + ' left room."')
    player.send('show_message Server "  You left room ' + self.Name + '."')

  def is_full(self) -> bool:
    return len(self.Players) >= 16

  # TODO
  def start(self) -> None:
    pass

  # TODO
  def stop(self) -> None:
    pass