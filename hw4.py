#!/usr/bin/env python3

import threading
import queue
import sys


#s = 0
#
#def go(q, foo):
#    global s
#    q.put(foo)
#    s += foo
#
#
#q = queue.Queue()
#
#for u in range(0, 10000):
#    t = threading.Thread(target=go, args = (q, u))
#    t.daemon = True
#    t.start()
#
#
#while (not q.empty()):
#    g = q.get()
#    print(g)
#
#print("done {} {}".format(s, sum(range(0, 10000))))

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

    while True:
        user_list.append(fin.readline())
        print(user_list[-1])


user_list = []

def main():
    print("start")
    read_input()

if __name__ == '__main__':
    main()
