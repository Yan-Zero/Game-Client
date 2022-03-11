from abc import ABC, abstractmethod
from collections import defaultdict
from time import sleep
from random import choice
import sys

N = 100
S = 500
MC = 10000
VERSION = 20220205

def Pause(msg = "\nPress enter to continue"):
    try:
        return input(msg)
    except EOFError:
        print()
        sys.exit(0)

def InRange(x, l, r): #closed range
    return x >= l and x <= r

def GetSatisfied(f, log = "", msg = "", default = None):
    while True:
        x = Pause(msg)
        if x == "" and default != None:
            return default
        try:
            x = int(x)
            assert(f(x))
            return x
        except:
            if log != "":
                print(log)


class PlayerMode:
    @abstractmethod
    def IsHuman(self):
        pass
    
    def IsAsync(self):
        return False

    @abstractmethod
    def LetMakeDecision(self, player, cards):
        pass

    @abstractmethod
    def LetSelectCard(self, player):
        pass

    def Announce(self, player, announcement):
        pass
    
    def StartToSelectCard(self, player):
        pass

class CardContainer:
    def __init__(self):
        self.cards = {}

    def AddCard(self, card, num = 1): # int, int, int, int
        self.cards[card] = self.cards.get(card, 0) + num

    def RemoveCard(self, card, num = 1):
        card_num = self.cards[card]
        if card_num > num:
            self.cards[card] -= num
        elif card_num == num:
            del self.cards[card]
        else:
            assert(card_num >= num)

    def HasCard(self, card):
        return card in self.cards

    def GetCardNum(self, card = -1):
        if card >= 0:
            return self.cards.get(card, 0)
        return sum(self.cards.values())

    def CardList(self):
        for card in sorted(self.cards):
            for i in range(self.cards[card]):
                yield card

    def ClearCard(self):
        self.cards.clear()


class Player:
    def __init__(self, game = None, mode = None, index = None, name = None):
        self.game = game
        self.mode = mode
        self.index = index
        self.cards = CardContainer()
        self.score = 0
        self.name = name

    def SetMode(self, mode):
        self.mode = mode
    
    def IsAsync(self):
        return False
    
    def IsHuman(self):
        return self.mode.IsHuman()

    def GainCard(self, card, num = 1):
        self.cards.AddCard(card, num)

    def LoseCard(self, card, num = 1):
        self.cards.RemoveCard(card, num)

    def GetCardNum(self, card = -1):#-1 to get all cardnum
        return self.cards.GetCardNum(card)

    def CardList(self):
        return self.cards.CardList()

    def LetMakeDecision(self, cards):
        return self.mode.LetMakeDecision(self, cards)

    def LetSelectCard(self):
        return self.mode.LetSelectCard(self)
    
    def StartToSelectCard(self):
        return self.mode.StartToSelectCard(self)

    def Announce(self, announcement):
        return self.mode.Announce(self, announcement)

    def IsAlive(self):
        return self.cards.GetCardNum() > 0


class Game:
    nums = 0
    def __init__(self, num_players, max_card, round = -1, delay = 0):
        self.num_players = num_players
        self.max_card = max_card
        self.round = round
        self.delay = delay
        self.players = [Player(self) for i in range(0, num_players+1)]
        Game.nums += 1
        self.id = Game.nums
        assert(InRange(num_players, 2, N) and InRange(max_card, 2, S))
        for i in range(1, num_players+1):
            self.players[i].index = i
            self.players[i].name = f"P{i}"
        for i in range(1, num_players+1):
            for j in range(1, max_card+1):
                self.players[i].GainCard(j)

    def Announce(self, announcement):
        for i in range(1, self.num_players+1):
            self.players[i].Announce(announcement)

    def AnnounceAllCards(self):
        msg = "\n" + "\n".join([("%s > %s" % (self.players[i].name, " ".join([str(j) for j in self.players[i].CardList()]))) for i in filter(lambda i: self.players[i].IsAlive(), range(1, self.num_players+1))])
        self.Announce(msg)

    def IsAllHumanOut(self):
        for i in range(1, self.num_players+1):
            if self.players[i].IsHuman() and self.players[i].IsAlive():
                return False
        return True

    def GetAliveNum(self):
        return sum([int(self.players[i].IsAlive()) for i in range(1, self.num_players+1)])

    def WhenOut(self, index):
        self.players[index].score = sum([self.players[i].GetCardNum() for i in range(1, self.num_players+1)]) + 2 #penalty

    def GameRule(self, sel):
        dealing = CardContainer()
        owners = defaultdict(list)
        sing = []
        n = self.num_players
        s = self.max_card
        for i in range(1, n+1):
            if sel[i] < 0:
                continue
            dealing.AddCard(sel[i])
            owners[sel[i]].append(i)
            self.players[i].LoseCard(sel[i])
        for i in range(s, -1, -1):
            if dealing.GetCardNum(i) == 1:
                sing.append(owners[i][0])
        len_sing = len(sing)
        if len_sing == 1:
            decision = self.players[sing[0]].LetMakeDecision(dealing)
            num = dealing.GetCardNum(decision)
            self.players[sing[0]].GainCard(decision, num)
            self.Announce("%s gets %d %ds." % (self.players[sing[0]].name, num, decision))
        else:
            for i in range(0, (len_sing + 1) // 2):
                self.players[sing[i]].GainCard(sel[sing[len_sing - 1 - i]])
                self.Announce("%s gets a %d." % (self.players[sing[i]].name, sel[sing[len_sing - 1 - i]]))
        if self.delay > 0:
            sleep(self.delay)
        elif self.delay < 0:
            Pause()

    def StartToSelectCard(self):
        for i in range(1, self.num_players+1):
            if self.players[i].IsAlive():
                self.players[i].StartToSelectCard()

    def OneTurn(self):
        n = self.num_players
        human_all_out = self.IsAllHumanOut()
        sel = [0] * (n+1)
        self.Announce("\n" + "\n".join([("%s has %d cards." % (self.players[i].name, self.players[i].GetCardNum())) for i in filter(lambda i: self.players[i].IsAlive(), range(1, n+1))]))
        self.StartToSelectCard()
        for i in range(1, n+1):
            if self.players[i].IsAlive():
                sel[i] = self.players[i].LetSelectCard()
            else:
                sel[i] = -1
        self.Announce("\n" + "\n".join([("%s > %d" % (self.players[i].name, sel[i])) for i in filter(lambda i: sel[i] >= 0, range(1, n+1))]))
        if human_all_out:
            if self.delay > 0:
                sleep(self.delay)
            elif self.delay < 0:
                Pause()
        self.GameRule(sel)
        return self.GetAliveNum() <= 1

    def CalcScore(self):
        return [self.players[i].score for i in range(self.num_players+1)]

    def Start(self):
        if (self.round >= 0):
            self.Announce("[Round %d]" % self.round)
        n = self.num_players
        score = [0] * (n+1)
        while True:
            game_over = self.OneTurn()
            for i in range(1, n+1):
                if not self.players[i].IsAlive() and self.players[i].score == 0:
                    self.WhenOut(i)
            if game_over:
                break
        self.AnnounceAllCards()
        return self.CalcScore()


class Human(PlayerMode):
    def IsHuman(self):
        return True

    def LetMakeDecision(self, player, cards):
        print("\nTime for you to make a decision!")
        strcards = " ".join([str(j) for j in cards.CardList()])
        print(f"Select one type from these cards: {strcards}")
        x = GetSatisfied(
            lambda x: InRange(x, 0, player.game.max_card) and cards.GetCardNum(x) > 0,
            "You will get 0 cards. Illegal!"
        )
        return x

    def LetSelectCard(self, player):
        print("\nTime for you to select a card!")
        strcards = " ".join([str(j) for j in player.CardList()])
        print(f"You have {player.GetCardNum()} cards: {strcards}")
        x = GetSatisfied(
            lambda x: InRange(x, 0, player.game.max_card) and player.GetCardNum(x) > 0,
            "You don't have this card!"
        )
        return x
    
    def Announce(self, player, announcement):
        print(announcement)
    

class SmartA(PlayerMode):
    def IsHuman(self):
        return False
    def LetMakeDecision(self, player, cards):
        cur = player.game.max_card
        for i in range(cur-1, -1, -1):
            if cards.GetCardNum(i) > cards.GetCardNum(cur):
                cur = i
        return cur

    def LetSelectCard(self, player):
        x = choice(list(player.CardList()))
        return x


if __name__ == "__main__":
    print("Dui Dui Peng (version %d)\n" % VERSION);

    n = GetSatisfied(
        lambda x: InRange(x, 2, N),
        "Error!",
        "number of players (default 3): ",
        3
    )
    s = GetSatisfied(
        lambda x: InRange(x, 2, S),
        "Error!",
        "number of cards (default 7): ",
        7
    )
    delay = GetSatisfied(
        lambda x: True,
        "Error!",
        "computer delay, -1 for pause (default 0.5): ",
        0.5
    )
    max_score = GetSatisfied(
        lambda x: InRange(x, 0, MC),
        "Error!",
        "max score (default %d): " % (6 * n),
        6 * n
    )
    print("\nHint: the game ends when one's score reaches %d, and the player with minimum score wins.\n" % max_score)

    a = SmartA()
    h = Human()
    tot = [0] * (n+1)
    round = 0
    ranklist = []

    while True:
        round += 1
        game = Game(n, s, round, delay)
        game.players[1].SetMode(h)
        for i in range(2, n+1):
            game.players[i].SetMode(a)
        score = game.Start()

        # print score
        print()
        for i in range(1, n+1):
            tot[i] += score[i]
            print("P%d\'s score: %d" % (i, tot[i]))
        ranklist = []
        for i in range(n):
            ranklist.append(i+1)
        ranklist.sort(key = lambda x: tot[x])
        if tot[ranklist[0]] == tot[ranklist[1]] or tot[ranklist[-1]] < max_score:
            Pause()
        else:
            break

    print()
    cur_rank = 1
    for i in range(n):
        if tot[ranklist[i]] != tot[ranklist[i-1]]:
            cur_rank = i+1
        print("Rank %d: P%d" % (cur_rank, ranklist[i]))
    print("\nP%d wins!" % ranklist[0])
    Pause()
