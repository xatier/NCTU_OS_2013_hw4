#!/usr/bin/env python3

import threading
import queue
import sys



def go(q, ts, u):
    global user_list
    global mark
    acct = q[u]
    mark[u] += 1

    if acct.trans:
        instr = acct.trans.pop(0)
        print("format: {}".format(instr))
        print("Timestamp: {}".format(ts))

        if instr[1] == 'B':
            print("{} {}".format("B: ", acct.balance))
            print(acct.balance)
            mark[u] += 1

        else:
            print("{} {}".format("T: ", acct.balance))
            to = (int(instr[2][11:]) - 1)
            amount = int(instr[3])
            if acct.balance >= amount:
                #acct.balance -= amount
                #user_list[to].balance += amount
                mark[u] += 1
                pass
            else:
                pass



def getone():
    global user_list
    for i in range(len(user_list)):
        if len(user_list[i].trans) != 0:
            return i

    return -1


def run():
    global user_list
    global ts
    global mark
    q = []
    for _ in range(10):
        q.append(user_list[_])
        mark.append(-1)

    while q:
        ts += 1
        for u in range(0, len(q)):
            t = threading.Thread(target=go, args = (q, ts, u))
            t.daemon = True
            t.start()

        for _ in range(10):
            if mark[_] == 0 or mark[_] > 2:
                q[_] = user_list[getone()]
                mark[_] = -1


class Account:
    def __init__(self, ID, balance, no_transactions):
        self.ID = int(ID[11:])
        self.balance = int(balance)
        self.no_transactions = no_transactions
        self.trans = []

    def print(self):
        print("ID:{}\n{}\n{}".format(self.ID, self.balance, self.no_transactions))
        print(self.trans)

def read_input():
    global user_list
    input_file = "transactions.txt"
    output_file = "transaction_log.txt"

    try:
        fin = open(input_file, "r")
    except:
        sys.stderr.write("{}: No such file!".format(input_file))
        sys.exit(1)

    try:
        fout = open(output_file, "w")
    except:
        sys.stderr.write("{}: error while opening file to write!\n", output_file);
        sys.exit(1)


    no_accounts = int(fin.readline())
    _ = no_accounts
    while _:
        __ = fin.readline().split()
        acct = Account(__[0], __[1], __[2])
        no_t = int(__[2])

        while no_t:
            __ = fin.readline().split()
            acct.trans.append(__)
            no_t -= 1

        user_list.append(acct)
        _ -= 1



user_list = []
mark = []
ts = 0


def main():
    print("start")
    read_input()
    user_list.sort(key=lambda x: x.ID)
    #for _ in user_list: _.print()
    run()
    print("done")

if __name__ == '__main__':
    main()
