# Overview

This is a solver for the popular game [Flow Free](https://en.wikipedia.org/wiki/Flow_Free). The code in this repository was adapted from the open-source terminal version made available by [mzucker](https://github.com/mzucker/flow_solver).

## Rules

![](https://static.au.edusercontent.com/files/cEsSDRKv1xYVNoLCBZ8cjzze)

The game presents a grid with colored dots occupying some of the squares. The objective is to connect dots of the same color by drawing pipes between them such that the entire grid is occupied by pipes. However, pipes may not intersect.

The diffculty is determined by the size of the grid, ranging from 5x5 to 15x15 squares.

## Science behind Flow Free

The game can be studied using the theory of computational complexity. It is an instance of an NP-complete game, and solving them efficiently can have a huge impact on routing problems. For example, engineers have to design circuit placements and interconnections in Very Large Scale Integration (VLSI) designs, i.e. microchip designs.

NP-complete problems are difficult to solve, and the best algorithms to solve NP-complete problems run in exponential time as a function of the size of the problem and the shortest accepted solution. Hence, the AI solver will struggle more as the size of the grid increases.

It is difficult not only because of its large branching factor, but also because of its large search tree depth. Human players rely on heuristics and discard unsolvable states by recognizing patterns, dead-ends, and subgoals which reduces the amount of search involved.

Other single-agent search AI algorithms, like A*, can improve search times which makes use of domain-specific knowledge.

## Dijkstra's

The solver uses Dijkstra's algorithm to solve each puzzle.

Each possible configuration of the grid is called a *state*. Each position in the grid can be free or contain a colour. The Flow Free Graph $G = <V, E>$ is implicit defined (graph not completely loaded into memory at once - it starts with the initial state and the graph is discovered while searching for a solution). The vertex set $V$ is defined as all the possible states and the edges $E$ connecting two vertexes are defined by the legal movements for each colour (left, right, up, down).

A move is legal only if it extends a colour pipe. A colour cannot be placed if it is not adjacent to the pipe of the same colour. Furthermore, to decrease the number of edges in the Graph, the pipe can only be started from one of the two initial colours, making one of them the start and the other the goal of the pipe.

All edges have a weight of one.

The solver uses Dijkstra's algorithm finds the path traversing the Graph from the initial state leading to a state where all the grid cells contains a colour, and therefore finding a solution to the puzzle. 

The initial node corresponds to the initial configuration (line 1). The algorithm selects a node to expand (generate the possible pipe extensions, line 5) along with an ordering of which is the next colour the algorithm should try to connect (line 6). Therefore, when a node is expanded, it has at most 4 children. Once all the children have been added to the priority queue, the algorithm will pick again the next node to extend (line 5) as well as the next colour to consider, until a solution is found. The search will expand a maximum of 1GB of nodes (line 12).

![](https://static.au.edusercontent.com/files/J1XnySQVb9GjmccURLgyJdAd)

Dead-ends are configurations for a solution that cannot exist. Detecting unsolvable states prevents unnecessary search, and by pruning their successors, the search will take less time and memory.

## Time complexity

The given codebase uses a priority queue with the property of a binary maxheap. 

Let $V$ represent the number of vertices and $E$ the number of edges.

Constructing the maxheap priority queue with a top-down approach takes $𝑂(𝑉 \cdot 𝑙𝑜𝑔(𝑉))$ time.

Looping over the priority queue takes $𝑂(𝑉)$ time. This includes child nodes being enqueued by the algorithm.

Dequeuing the highest valued node in the heap takes $O(1)$ time. Restructuring the maxheap takes $𝑂(𝑙𝑜𝑔 (𝑉))$ time.

Looping over each vertex connected to $𝑉$ takes $𝑂(𝐸)$ time. Enqueuing a child node and relaxing the priority queue takes $𝑂(𝑉)$ time.

In total, the time it would take to construct the priority queue, to dequeue each node 𝑉 times, and to relax the priority queue $𝑂(𝐸)$ times would be: 

$𝑂(𝑉 \cdot 𝑙𝑜𝑔(𝑉)) + 𝑂(𝑉 \cdot 𝑙𝑜𝑔(𝑉)) + 𝑂(𝐸 \cdot 𝑙𝑜𝑔(𝑉))$ 
$⟹ 𝑂(𝑉 + 𝐸) ∙ 𝑙𝑜𝑔(𝑉))$

From domain knowledge, we know that edges $𝐸$ is a constant as we are checking in 4 directions only. From this, we can infer that the time complexity could be written more concisely as: $𝑂(𝑉 \cdot log(𝑉))$

The growth would remain the same with and without dead end detection, as we are still considering the node when doing dead end checking. Also, as $𝑉$ grows bigger (as the board size increases), $𝐸$ becomes negligible.

## Usage

A makefile is provided to produce the executable `flow`.

Example input: `./flow [options] <puzzleName1> ... <puzzleNameN>

`./flow -A -F puzzles/regular_5x5_01.txt`

Additional options can be found if you use option `-h`.

```
usage: flow_solver [ OPTIONS ] BOARD1.txt
BOARD2.txt [ ... ] ]

Display options:

  -q, --quiet             Reduce output
  -d, --diagnostics       Print diagnostics when search unsuccessful
  -A, --animation         Animate solution
  -F, --fast              Speed up animation 4x
  -C, --color             Force use of ANSI color
  -S, --svg               Output final state to SVG

Node evaluation options:

  -d, --deadends          dead-end checking

Color ordering options:

  -r, --randomize         Shuffle order of colors before solving
  -c, --constrained       Disable order by most constrained

Search options:

  -n, --max-nodes N       Restrict storage to N nodes
  -m, --max-storage N     Restrict storage to N MB (default 1024)

Help:

  -h, --help              See this help text
```

Output:

Using the options flag `-q`, the solver will print:

1. Puzzle Name
2. SearchFlag
3. Total search time
4. Number of generated nodes
5. Summary

For example, the output of `./flow -q ../puzzles/regular_*` may be the following depending on efficiency:

```
../puzzles/regular_5x5_01.txt s 0.000 18 
../puzzles/regular_6x6_01.txt s 0.000 283 
../puzzles/regular_7x7_01.txt s 0.002 3,317 
../puzzles/regular_8x8_01.txt s 0.284 409,726 
../puzzles/regular_9x9_01.txt s 0.417 587,332 
5 total s 0.704 1,000,676
```

