import argparse
import time
from subprocess import Popen, PIPE, STDOUT

parser = argparse.ArgumentParser()
parser.add_argument("engine", help="path to engine")
parser.add_argument("perft", help="path to perft suite")

args = parser.parse_args()

engine_path = args.engine
perft_path = args.perft

p = Popen(engine_path, stdin=PIPE, stdout=PIPE, stderr=STDOUT, universal_newlines=True)

def parse_token(token: str) -> tuple[int, int]:
    depth,nodes = token.split()
    return int(depth[1:]), int(nodes)

t_total = 0

with open(perft_path, "r") as perft:
    for test in perft:
        fen, *rest = test.split(";")
        rest = [parse_token(token) for token in rest]

        for depth, nodes in rest:
            t0 = time.time()
            p.stdin.write(f"position fen {fen}\n")
            p.stdin.write(f"perft {depth}\n")
            p.stdin.flush()

            found = int(p.stdout.readline().strip())
            t1 = time.time()
            t_total += t1 - t0

            print(f"{fen:<70} | {depth} | {nodes:<11} | ", end="")
            print("PASS" if found == nodes else f"FAIL {found}")

p.stdin.close()
p.wait()

print("time =", t_total)