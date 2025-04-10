/**
 * collision_world.h -- detect and handle line segment intersections
 * Copyright (c) 2012 the Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 **/

#ifndef COLLISIONWORLD_H_
#define COLLISIONWORLD_H_

#include "./line.h"
#include "./intersection_detection.h"
#include "./intersection_event_list.h"

struct CollisionWorld {
  // Time step used for simulation
  double timeStep;

  // Container that holds all the lines as an array of Line* lines.
  // This CollisionWorld owns the Line* lines.
  Line** lines;
  unsigned int numOfLines;

  // Record the total number of line-wall collisions.
  unsigned int numLineWallCollisions;

  // Record the total number of line-line intersections.
  unsigned int numLineLineCollisions;
};
typedef struct CollisionWorld CollisionWorld;

typedef struct {
    double x, y, width, height;
} BoundingBox;

typedef struct {
    Vec p1, p2, p3, p4;  // Parallelogram vertices
    Line* line;
} SweptLine;

typedef struct QuadTree {
    BoundingBox boundary;
    SweptLine* sweptLines;
    unsigned int numOfSweptLines;
    unsigned int capacity;
    unsigned int overflow;
    struct QuadTree *lowerLeft, *lowerRight, *upperLeft, *upperRight;
} QuadTree;

CollisionWorld* CollisionWorld_new(const unsigned int capacity);

void CollisionWorld_delete(CollisionWorld* collisionWorld);

// Return the total number of lines in the box.
unsigned int CollisionWorld_getNumOfLines(CollisionWorld* collisionWorld);

// Add a line into the box.  Must be under capacity.
// This CollisionWorld becomes owner of the Line* line.
void CollisionWorld_addLine(CollisionWorld* collisionWorld, Line *line);

// Get a line from box.
Line* CollisionWorld_getLine(CollisionWorld* collisionWorld,
                             const unsigned int index);

// Update lines' situation in the box.
void CollisionWorld_updateLines(CollisionWorld* collisionWorld);

// Update position of lines.
void CollisionWorld_updatePosition(CollisionWorld* collisionWorld);

// Handle line-wall collision.
void CollisionWorld_lineWallCollision(CollisionWorld* collisionWorld);

// Detect line-line intersection.
void CollisionWorld_detectIntersection(CollisionWorld* collisionWorld);

// Get total number of line-wall collisions.
unsigned int CollisionWorld_getNumLineWallCollisions(
    CollisionWorld* collisionWorld);

// Get total number of line-line intersections.
unsigned int CollisionWorld_getNumLineLineCollisions(
    CollisionWorld* collisionWorld);

// Update the two lines based on their intersection event.
// Precondition: compareLines(l1, l2) < 0 must be true.
void CollisionWorld_collisionSolver(CollisionWorld* collisionWorld, Line *l1,
                                    Line *l2,
                                    IntersectionType intersectionType);

QuadTree* QuadTree_create(double x, double y, double width, double height, unsigned int capacity, unsigned int overflow);
void QuadTree_free(QuadTree* qt);
bool QuadTree_insert(QuadTree* qt, Line* line);
void QuadTree_subdivide(QuadTree* qt);
void QuadTree_redistributeSweptLines(QuadTree* qt);

SweptLine createSweptLine(Line* line);
bool SweptLine_intersect(SweptLine* sl1, SweptLine* sl2);
bool SweptLine_inBoundingBox(SweptLine* sweptLine, BoundingBox* bb);

void checkCollisionsQuadTree(QuadTree* tree, IntersectionEventList* intersectionEventList, CollisionWorld* collisionWorld);
void checkChildSweptLines(QuadTree* parent, CollisionWorld* collisionWorld, IntersectionEventList* intersectionEventList, QuadTree* child);
void checkSweptLineCollision(SweptLine* sl1, SweptLine* sl2, CollisionWorld* collisionWorld, IntersectionEventList* intersectionEventList);

bool Vec_inBoundingBox(Vec* v, BoundingBox* bb);
bool lineSegmentIntersect(Vec p1, Vec p2, Vec q1, Vec q2);
double direction(Vec pi, Vec pj, Vec pk);
bool onSegment(Vec pi, Vec pj, Vec pk);

#endif  //COLLISIONWORLD_H_