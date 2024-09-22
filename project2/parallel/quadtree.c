#include "quadtree.h"

quadTree* quadTreeCreate(double x, double y, double width, double height, unsigned int capacity, unsigned int maxLines) {
    quadTree* tree = (quadTree*)malloc(sizeof(quadTree));
    tree->boundary = (borderBox){x, y, width, height};
    tree->sweptLines = (sweptLine**)malloc(sizeof(sweptLine*) * maxLines);
    tree->numOfLines = 0;
    tree->capacity = capacity;
    tree->maxLines = maxLines;
    tree->lowerLeft = tree->lowerRight = tree->upperLeft = tree->upperRight = NULL;
    return tree;
}

sweptLine* sweptLineCreate(Line* line) {
    sweptLine* s_line = (sweptLine*)malloc(sizeof(sweptLine));
    s_line->oldp1 = line->p1;
    s_line->oldp2 = line->p2;
    s_line->newp1 = Vec_make(line->p1.x + line->velocity.x, line->p1.y + line->velocity.y);
    s_line->newp2 = Vec_make(line->p2.x + line->velocity.x, line->p2.y + line->velocity.y);
    s_line->line = line;
    return s_line;
}

bool pointInBorderBox(quadTree* tree, Vec* vec) {
    return (
        (vec->x > tree->boundary.x && vec->x < tree->boundary.x + tree->boundary.width) &&
        (vec->y > tree->boundary.y && vec->y < tree->boundary.y + tree->boundary.height)
    );
}


bool sweptLineInBorderBox(quadTree* tree, sweptLine* s_line) {
    return (
        pointInBorderBox(tree, &s_line->oldp1) &&
        pointInBorderBox(tree, &s_line->oldp2) &&
        pointInBorderBox(tree, &s_line->newp1) &&
        pointInBorderBox(tree, &s_line->newp2)
    );
}

void quadTreeRedistibuteLines(quadTree* tree){
    for(int i = 0; i < tree->numOfLines; ++i) {
        Line* line = (*(tree->sweptLines + i))->line;
        bool inserted = false;
        if(!inserted && quadTreeInsert(tree->lowerLeft, line)) inserted = true;
        if(!inserted && quadTreeInsert(tree->lowerRight, line)) inserted = true;
        if(!inserted && quadTreeInsert(tree->upperLeft, line)) inserted = true;
        if(!inserted && quadTreeInsert(tree->upperRight, line)) inserted = true;

        if(inserted) {
            for(int j = i; j < tree->numOfLines; ++j) {
                *(tree->sweptLines + j) = *(tree->sweptLines + j + 1);
                --i;
                --tree->numOfLines;
            }
        }
    }
}


void quadTreeSubdivide(quadTree* tree) {
    double halfWidth = tree->boundary.width / 2;
    double halfHeight = tree->boundary.height / 2;
    tree->lowerLeft = quadTreeCreate(tree->boundary.x, tree->boundary.y, halfWidth, halfHeight, tree->capacity, tree->maxLines);
    tree->lowerRight = quadTreeCreate(tree->boundary.x + halfWidth, tree->boundary.y, halfWidth, halfHeight, tree->capacity, tree->maxLines);
    tree->upperLeft = quadTreeCreate(tree->boundary.x, tree->boundary.y + halfHeight, halfWidth, halfHeight, tree->capacity, tree->maxLines);
    tree->upperRight = quadTreeCreate(tree->boundary.x + halfWidth, tree->boundary.y + halfHeight, halfWidth, halfHeight, tree->capacity, tree->maxLines);
    quadTreeRedistibuteLines(tree);
}

bool quadTreeInsert(quadTree* tree, Line* line){
    sweptLine* s_line = sweptLineCreate(line);
    if(!sweptLineInBorderBox(tree, s_line)) return false;

    if(tree->numOfLines < tree->capacity && !tree->lowerLeft) {
        *(tree->sweptLines + tree->numOfLines) = s_line;
        ++tree->numOfLines;
        return true;
    }

    if(!tree->lowerLeft) {
        quadTreeSubdivide(tree);
    }

    if(quadTreeInsert(tree->lowerLeft, line)) return true;
    if(quadTreeInsert(tree->lowerRight, line)) return true;
    if(quadTreeInsert(tree->upperLeft, line)) return true;
    if(quadTreeInsert(tree->upperRight, line)) return true;

    *(tree->sweptLines + tree->numOfLines) = s_line;
    ++tree->numOfLines;

    return true;
}

void quadTreeFree(quadTree* tree) {
    if (tree == NULL) return;

    for(int i = 0; i < tree->numOfLines; ++i) {
        free(*(tree->sweptLines + i));
    }
    free(tree->sweptLines);

    quadTreeFree(tree->lowerLeft);
    quadTreeFree(tree->lowerRight);
    quadTreeFree(tree->upperLeft);
    quadTreeFree(tree->upperRight);

    free(tree);
}

