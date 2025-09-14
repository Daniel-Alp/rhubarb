# Rhubarb
Engine that supports the Universal Chess Interface. To build the engine (on Linux or MacOS)
```
git clone https://github.com/Daniel-Alp/rhubarb
cd rhubarb
mkdir build
cd build
cmake ..
cd ..
cmake --build build
```

# Features
Evaluation
* Piece-square table
* Tapered evaluation

Search
* Negamax with alpha-beta pruning
* Transposition table
* Iterative deepening
* Principal variation search
* Quiescence search
* Null move pruning
* Static null move pruning
* Aspiration windows
* Late move reduction

Move ordering
* Move from the transposition table
* Captures and then promotions (MVV-LVA)
* Killer moves
* History heuristic 
