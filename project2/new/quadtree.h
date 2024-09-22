#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include "./line.h"
#include "./vec.h"

struct sweptLine {
    Vec oldp1, oldp2, newp1, newp2;
    Line* line;
};
typedef struct sweptLine sweptLine;

struct borderBox{
    double x, y, width, height;
};
typedef struct borderBox borderBox;

typedef struct quadTree quadTree;

struct quadTree{
    quadTree* lowerLeft;
    quadTree* lowerRight;
    quadTree* upperLeft;
    quadTree* upperRight;
    unsigned int capacity; //at which point to split
    sweptLine** sweptLines; //lines in current
    unsigned int maxLines; //maximum possible lines
    unsigned int numOfLines;
    borderBox boundary;
};

quadTree* quadTreeCreate(double x, double y, double width, double height, unsigned int capacity, unsigned int maxLines);

sweptLine* sweptLineCreate(Line* line);

bool pointInBorderBox(quadTree* tree, Vec* vec);

bool sweptLineInBorderBox(quadTree* tree, sweptLine* line);

void quadTreeSubdivide(quadTree* tree);

void quadTreeRedistibuteLines(quadTree* tree);

bool quadTreeInsert(quadTree* tree, Line* line);

void quadTreeFree(quadTree* tree);

#endif