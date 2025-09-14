import argparse
from subprocess import Popen, PIPE, STDOUT

parser = argparse.ArgumentParser()
parser.add_argument("engine", help="path to engine")
parser.add_argument("perft", help="path to perft suite")

args = parser.parse_args()

engine_path = args.engine
perft_path = args.perft

p = Popen(engine_path, stdin=PIPE, stdout=PIPE, stderr=STDOUT, universal_newlines=True)

with open(perft_path, "r") as perft:
    for test in perft:
        fen, *rest = test.split(";")
        rest = [int(token[3:]) for token in rest]

        for depth, nodes in enumerate(rest, start=1):
            p.stdin.write(f"position fen {fen}\n")
            p.stdin.write(f"perft {depth}\n")
            p.stdin.flush()

            print(f"{fen:<70} | {depth} | {nodes:<11} | ", end="")
            found = int(p.stdout.readline().strip())
            print("PASS" if found == nodes else f"FAIL {found}")

p.stdin.close()
p.wait()