"""
Time [s],Value
0.013520500000000,0x000000D260803000
1.013520000000000,0x000000D2E0803000
2.013526000000000,0x000000D210803000
2.513831500000000,0x000000D290803000
"""
data = []
with open('remote_testcases_reversed.txt') as f:
    data = [l.split(',') for l in f.read().split('\n')[1:-1]]

def diff(a, b):
    out = ''
    for i, j in zip(a, b):
        if i!=j: out += 'v'
        if i==j: out += ' '
    return out

last = None
for line in data:
    current = bin(int(line[1], 16))[2:]
    if last:
        print diff(last, current)
    print current
    last = current
