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

  # 这是相当于静态的，因为全局对该成员引用都是同一个 {}
  ROOMS = {
    
  }

  def __init__(self, name: str, host: Player, uid: int) -> None:
    if uid in self.ROOMS:
      raise ValueError("Error: uid has been used.")
    if host.room.UID != 0:
      raise PermissionError("Error: you are not in ROOM 0.")
    self.ROOMS[self.UID] = self
    self.Name = name
    self.Players = [host]
    self.Host = host
    self.UID = uid
    host.room = self
    
    self.ROOMS[0].leave_room(host)

  def join_room(self, player: Player) -> None:
    if player in self.Players:
      p.send(f'show_message "Room {self.Name}" "  You has been in room."')
      return
    for p in self.Players:
      p.send(f'show_message "Room {self.Name}" "  {player.name} joined room."')
    
    self.Players.append(player)
    t_room = player.room
    player.room = self
    if t_room != None:
      t_room.leave_room(player)

  def leave_room(self, player: Player) -> None:
    # 校验
    if player not in self.Players:
      return
    if self.UID == 0:
      player.send('error "Error: you are not in any room.\n"')
      return

    # 发送信息
    for p in self.Players:
      p.send(f'show_message "Room {self.Name}" "  {player.name} left room."')
    player.send('show_message Server "  You left room ' + self.Name + '."')

    if player.room == self:
      # 大厅添加玩家
      player.room = None
      self.Players.remove(player)
      self.ROOMS[0].add_player(player)
    else:
      self.Players.remove(player)

  def kick(self, player: Player) -> None:
    if player == self.Host:
      raise ValueError("You are the host of this room.\n" + \
                       "  If you want to leave this room, you could type 'room leave'.\n" + \
                       "  Or you want to close this room, please type 'room close'\n")
    if player not in self.Players:
      raise ValueError(f"{player.name} isn't in this room.")

    for p in self.Players:
      p.send(f'show_message "Room {self.Name}" "  {player.name} was kicked out."')

    player.room = None
    self.Players.remove(player)
    self.ROOMS[0].join_room(player)

  def is_full(self) -> bool:
    return len(self.Players) >= 16

  # TODO
  def start(self) -> None:
    pass

  # TODO
  def stop(self) -> None:
    pass

  # 关闭房间
  def close(self)-> None:
    for p in self.Players:
      p.send(f'show_message "Room {self.Name}" "  The room {self.UID} is been close."')
      p.room = None
      self.ROOMS[0].add_player(p)

    del self.ROOMS[self.UID]