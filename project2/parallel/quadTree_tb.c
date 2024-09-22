#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "quadtree.h"
#include "line.h"
#include "vec.h"

// Utility function to create a Line
Line* createLine(double x1, double y1, double x2, double y2, double vx, double vy) {
    Line* line = (Line*)malloc(sizeof(Line));
    line->p1 = Vec_make(x1, y1);
    line->p2 = Vec_make(x2, y2);
    line->velocity = Vec_make(vx, vy);
    return line;
}

// Test quadTreeCreate
void testQuadTreeCreate() {
    quadTree* tree = quadTreeCreate(0, 0, 100, 100, 4, 10);
    assert(tree != NULL);
    assert(tree->boundary.x == 0);
    assert(tree->boundary.y == 0);
    assert(tree->boundary.width == 100);
    assert(tree->boundary.height == 100);
    assert(tree->capacity == 4);
    assert(tree->maxLines == 10);
    assert(tree->numOfLines == 0);
    assert(tree->lowerLeft == NULL);
    assert(tree->lowerRight == NULL);
    assert(tree->upperLeft == NULL);
    assert(tree->upperRight == NULL);
    quadTreeFree(tree);
    printf("quadTreeCreate test passed\n");
}

// Test quadTreeInsert
void testQuadTreeInsert() {
    quadTree* tree = quadTreeCreate(0, 0, 100, 100, 4, 10);
    
    Line* line1 = createLine(10, 10, 20, 20, 1, 1);
    Line* line2 = createLine(30, 30, 40, 40, -1, -1);
    Line* line3 = createLine(50, 50, 60, 60, 0, 0);
    Line* line4 = createLine(70, 70, 80, 80, 2, 2);
    Line* line5 = createLine(90, 90, 95, 95, -2, -2);
    
    assert(quadTreeInsert(tree, line1));
    assert(quadTreeInsert(tree, line2));
    assert(quadTreeInsert(tree, line3));
    assert(quadTreeInsert(tree, line4));
    assert(tree->numOfLines == 4);
    assert(tree->lowerLeft == NULL); // Not subdivided yet
    
    assert(quadTreeInsert(tree, line5));
    assert(tree->lowerLeft != NULL); // Should be subdivided now
    
    quadTreeFree(tree);
    free(line1);
    free(line2);
    free(line3);
    free(line4);
    free(line5);
    printf("quadTreeInsert test passed\n");
}

// Test pointInBorderBox
void testPointInBorderBox() {
    quadTree* tree = quadTreeCreate(0, 0, 100, 100, 4, 10);
    
    Vec point1 = Vec_make(50, 50);
    Vec point2 = Vec_make(-10, 50);
    Vec point3 = Vec_make(110, 50);
    Vec point4 = Vec_make(50, -10);
    Vec point5 = Vec_make(50, 110);
    
    assert(pointInBorderBox(tree, &point1));
    assert(!pointInBorderBox(tree, &point2));
    assert(!pointInBorderBox(tree, &point3));
    assert(!pointInBorderBox(tree, &point4));
    assert(!pointInBorderBox(tree, &point5));
    
    quadTreeFree(tree);
    printf("pointInBorderBox test passed\n");
}

// Test sweptLineInBorderBox
void testSweptLineInBorderBox() {
    quadTree* tree = quadTreeCreate(0, 0, 100, 100, 4, 10);
    
    Line* line1 = createLine(10, 10, 20, 20, 1, 1);
    Line* line2 = createLine(-10, -10, -5, -5, 5, 5);
    Line* line3 = createLine(110, 110, 120, 120, -5, -5);
    
    sweptLine* s_line1 = sweptLineCreate(line1);
    sweptLine* s_line2 = sweptLineCreate(line2);
    sweptLine* s_line3 = sweptLineCreate(line3);
    
    assert(sweptLineInBorderBox(tree, s_line1));
    assert(!sweptLineInBorderBox(tree, s_line2));
    assert(!sweptLineInBorderBox(tree, s_line3));
    
    quadTreeFree(tree);
    free(line1);
    free(line2);
    free(line3);
    free(s_line1);
    free(s_line2);
    free(s_line3);
    printf("sweptLineInBorderBox test passed\n");
}

int main() {
    testQuadTreeCreate();
    testQuadTreeInsert();
    testPointInBorderBox();
    testSweptLineInBorderBox();
    
    printf("All tests passed!\n");
    return 0;
}