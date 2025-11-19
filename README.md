# Minefield Chain Reaction Simulator

A C program that simulates mine detonation chain reactions using graph theory and Monte Carlo methods. This project demonstrates practical applications of graph algorithms, depth-first search (DFS), and probabilistic area estimation.

## Overview

The program models a minefield where each mine is represented as a circle with coordinates (x, y) and radius r. When a mine detonates, it triggers a chain reaction of all mines within its blast radius, which in turn can trigger other mines, creating cascading explosions.

## Features

- **Graph-based mine relationships**: Mines are represented as nodes in a directed graph, where edges indicate "can trigger" relationships
- **Chain reaction simulation**: Uses depth-first search to calculate all mines affected by a detonation
- **Rocket explosion simulation**: Simulate custom explosions at any coordinates with any blast radius
- **Efficiency analysis**: Find which single mine would cause the maximum chain reaction
- **Area calculation**: Uses Monte Carlo integration to estimate the total area covered by detonated mines
- **File-based minefield loading**: Load mine configurations from text files

## How It Works

### Graph Representation
- Each mine is a node with properties: position (x, y) and blast radius (r)
- A directed edge from mine A to mine B exists if mine B is within mine A's blast radius
- The adjacency matrix is dynamically updated when mine properties change

### Chain Reaction Algorithm
Uses a stack-based DFS traversal:
1. Start with mines directly hit by the initial explosion
2. For each triggered mine, add all mines it can trigger to the stack
3. Continue until no new mines are triggered
4. Track visited mines to avoid infinite loops

### Area Estimation
Monte Carlo method for calculating total detonated area:
1. Find bounding box of all detonated mines
2. Generate random points within the bounding box
3. Count points that fall within any mine's radius
4. Estimate area as: (bounding box area) × (hits / total samples)

## Input File Format

```
<number_of_mines>
<x1> <y1> <radius1>
<x2> <y2> <radius2>
...
```

Example (`graf1.txt`):
```
3
0 0 2
3 0 1.5
5 0 1
```

## Menu Options

1. **Find efficiency of selected mine** - Calculate how many mines would detonate if a specific mine explodes
2. **Find max efficiency mine** - Identify which mine causes the largest chain reaction
3. **Rocket explosion** - Simulate a custom explosion at any location with any radius
4. **Calculate max efficiency mine detonated area** - Estimate total area covered by the most efficient mine's chain reaction
5. **Back to start** - Return to main menu

## Algorithm Complexity

- **Load graph**: O(n²) - checks all mine pairs for overlap
- **Efficiency calculation**: O(n + e) - DFS traversal where e is number of edges
- **Max efficiency**: O(n × (n + e)) - runs efficiency check for each mine
- **Area calculation**: O(n × s) - where s is number of Monte Carlo samples (default: 1,000,000)

## Sample Test Files

The repository includes several test files:
- `graf1.txt` - Simple 3-mine configuration
- `graf2.txt` - Medium complexity scenario
- `graf3.txt` - Complex minefield
- `graf4.txt` - Edge cases and boundary testing

## Technical Implementation

- **Dynamic memory management**: Graph structure uses dynamic allocation for scalability
- **Stack-based DFS**: Custom stack implementation for explosion propagation
- **Adjacency matrix**: O(1) edge lookup for efficient graph queries
- **Monte Carlo integration**: Probabilistic approach for complex area calculations

