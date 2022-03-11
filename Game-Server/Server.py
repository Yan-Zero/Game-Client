import socket

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

class Room:
  Host: Player
  Name: str
  UID: id

  def __init__(self, name: str, host: Player) -> None:
    self.Name = name
    self.players = []
    self.Host = host
    self.UID = hash(name + str(host.uid))