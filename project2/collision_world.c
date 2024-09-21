/** 
 * collision_world.c -- detect and handle line segment intersections
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

#include "./collision_world.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "./intersection_detection.h"
#include "./intersection_event_list.h"
#include "./line.h"

CollisionWorld* CollisionWorld_new(const unsigned int capacity) {
  assert(capacity > 0);

  CollisionWorld* collisionWorld = malloc(sizeof(CollisionWorld));
  if (collisionWorld == NULL) {
    return NULL;
  }

  collisionWorld->numLineWallCollisions = 0;
  collisionWorld->numLineLineCollisions = 0;
  collisionWorld->timeStep = 0.5;
  collisionWorld->lines = malloc(capacity * sizeof(Line*));
  collisionWorld->numOfLines = 0;
  return collisionWorld;
}

void CollisionWorld_delete(CollisionWorld* collisionWorld) {
  for (int i = 0; i < collisionWorld->numOfLines; i++) {
    free(collisionWorld->lines[i]);
  }
  free(collisionWorld->lines);
  free(collisionWorld);
}

unsigned int CollisionWorld_getNumOfLines(CollisionWorld* collisionWorld) {
  return collisionWorld->numOfLines;
}

void CollisionWorld_addLine(CollisionWorld* collisionWorld, Line *line) {
  collisionWorld->lines[collisionWorld->numOfLines] = line;
  collisionWorld->numOfLines++;
}

Line* CollisionWorld_getLine(CollisionWorld* collisionWorld,
                             const unsigned int index) {
  if (index >= collisionWorld->numOfLines) {
    return NULL;
  }
  return collisionWorld->lines[index];
}

void CollisionWorld_updateLines(CollisionWorld* collisionWorld) {
  CollisionWorld_detectIntersection(collisionWorld);
  CollisionWorld_updatePosition(collisionWorld);
  CollisionWorld_lineWallCollision(collisionWorld);
}

void CollisionWorld_updatePosition(CollisionWorld* collisionWorld) {
  double t = collisionWorld->timeStep;
  for (int i = 0; i < collisionWorld->numOfLines; i++) {
    Line *line = collisionWorld->lines[i];
    line->p1 = Vec_add(line->p1, Vec_multiply(line->velocity, t));
    line->p2 = Vec_add(line->p2, Vec_multiply(line->velocity, t));
  }
}

void CollisionWorld_lineWallCollision(CollisionWorld* collisionWorld) {
  for (int i = 0; i < collisionWorld->numOfLines; i++) {
    Line *line = collisionWorld->lines[i];
    bool collide = false;

    // Right side
    if ((line->p1.x > BOX_XMAX || line->p2.x > BOX_XMAX)
        && (line->velocity.x > 0)) {
      line->velocity.x = -line->velocity.x;
      collide = true;
    }
    // Left side
    if ((line->p1.x < BOX_XMIN || line->p2.x < BOX_XMIN)
        && (line->velocity.x < 0)) {
      line->velocity.x = -line->velocity.x;
      collide = true;
    }
    // Top side
    if ((line->p1.y > BOX_YMAX || line->p2.y > BOX_YMAX)
        && (line->velocity.y > 0)) {
      line->velocity.y = -line->velocity.y;
      collide = true;
    }
    // Bottom side
    if ((line->p1.y < BOX_YMIN || line->p2.y < BOX_YMIN)
        && (line->velocity.y < 0)) {
      line->velocity.y = -line->velocity.y;
      collide = true;
    }
    // Update total number of collisions.
    if (collide == true) {
      collisionWorld->numLineWallCollisions++;
    }
  }
}

double min(double a, double b) {
    return (a < b) ? a : b;
}

double max(double a, double b) {
    return (a > b) ? a : b;
}

QuadTree* QuadTree_create(double x, double y, double width, double height, unsigned int capacity, unsigned int overflow) {
    QuadTree* qt = (QuadTree*)malloc(sizeof(QuadTree));
    qt->boundary = (BoundingBox){x, y, width, height};
    qt->sweptLines = (SweptLine*)malloc(sizeof(SweptLine) * capacity);
    qt->numOfSweptLines = 0;
    qt->capacity = capacity;
    qt->overflow = overflow;
    qt->lowerLeft = qt->lowerRight = qt->upperLeft = qt->upperRight = NULL;
    return qt;
}

bool Vec_inBoundingBox(Vec* v, BoundingBox* bb) {
    return (v->x >= bb->x && v->x <= bb->x + bb->width &&
            v->y >= bb->y && v->y <= bb->y + bb->height);
}

SweptLine createSweptLine(Line* line) {
    SweptLine sweptLine;
    sweptLine.line = line;
    sweptLine.p1 = line->p1;
    sweptLine.p2 = line->p2;
    sweptLine.p3 = Vec_add(line->p2, line->velocity);
    sweptLine.p4 = Vec_add(line->p1, line->velocity);
    return sweptLine;
}

void QuadTree_subdivide(QuadTree* qt) {
    double x = qt->boundary.x;
    double y = qt->boundary.y;
    double w = qt->boundary.width / 2;
    double h = qt->boundary.height / 2;

    qt->lowerLeft = QuadTree_create(x, y, w, h, qt->capacity, qt->overflow);
    qt->lowerRight = QuadTree_create(x + w, y, w, h, qt->capacity, qt->overflow);
    qt->upperLeft = QuadTree_create(x, y + h, w, h, qt->capacity, qt->overflow);
    qt->upperRight = QuadTree_create(x + w, y + h, w, h, qt->capacity, qt->overflow);
}

void QuadTree_redistributeSweptLines(QuadTree* qt) {
    for (int i = qt->numOfSweptLines - 1; i >= 1; --i) {
        SweptLine sweptLine = qt->sweptLines[i];
        bool inserted = false;

        if (!inserted && qt->lowerLeft && QuadTree_insert(qt->lowerLeft, sweptLine.line)) inserted = true;
        if (!inserted && qt->lowerRight && QuadTree_insert(qt->lowerRight, sweptLine.line)) inserted = true;
        if (!inserted && qt->upperLeft && QuadTree_insert(qt->upperLeft, sweptLine.line)) inserted = true;
        if (!inserted && qt->upperRight && QuadTree_insert(qt->upperRight, sweptLine.line)) inserted = true;

        if (inserted) {
            // Remove the sweptLine from the parent node
            for (int j = i; j < qt->numOfSweptLines - 1; ++j) {
                qt->sweptLines[j] = qt->sweptLines[j + 1];
            }
            qt->numOfSweptLines--;
        }
    }
}

void checkChildSweptLines(QuadTree* parent, CollisionWorld* collisionWorld, IntersectionEventList* intersectionEventList, QuadTree* child) {
    for (int i = 0; i < parent->numOfSweptLines; ++i) {
        for (int j = 0; j < child->numOfSweptLines; ++j) {
            checkSweptLineCollision(&parent->sweptLines[i], &child->sweptLines[j], collisionWorld, intersectionEventList);
        }
    }
}

bool SweptLine_inBoundingBox(SweptLine* sweptLine, BoundingBox* bb) {
    // Check if any of the four points of the swept line are inside the bounding box
    return (Vec_inBoundingBox(&sweptLine->p1, bb) ||
            Vec_inBoundingBox(&sweptLine->p2, bb) ||
            Vec_inBoundingBox(&sweptLine->p3, bb) ||
            Vec_inBoundingBox(&sweptLine->p4, bb));
}

bool QuadTree_insert(QuadTree* qt, Line* line) {
    SweptLine sweptLine = createSweptLine(line);
    
    if (!SweptLine_inBoundingBox(&sweptLine, &qt->boundary)) {
        return false;
    }

    if (qt->numOfSweptLines < qt->overflow && qt->lowerLeft == NULL) {
        qt->sweptLines[qt->numOfSweptLines++] = sweptLine;
        return true;
    }

    if (qt->lowerLeft == NULL) {
        QuadTree_subdivide(qt);
        QuadTree_redistributeSweptLines(qt);
    }

    bool inserted = false;
    if (!inserted && QuadTree_insert(qt->lowerLeft, line)) inserted = true;
    if (!inserted && QuadTree_insert(qt->lowerRight, line)) inserted = true;
    if (!inserted && QuadTree_insert(qt->upperLeft, line)) inserted = true;
    if (!inserted && QuadTree_insert(qt->upperRight, line)) inserted = true;

    if (!inserted) {
        qt->sweptLines[qt->numOfSweptLines++] = sweptLine;
        inserted = true;
    }

    return inserted;
}

void QuadTree_free(QuadTree* qt) {
    if (qt == NULL) return;
    
    free(qt->sweptLines);
    QuadTree_free(qt->lowerLeft);
    QuadTree_free(qt->lowerRight);
    QuadTree_free(qt->upperLeft);
    QuadTree_free(qt->upperRight);
    free(qt);
}

bool SweptLine_intersect(SweptLine* sl1, SweptLine* sl2) {
    Vec edges1[4] = {sl1->p1, sl1->p2, sl1->p3, sl1->p4};
    Vec edges2[4] = {sl2->p1, sl2->p2, sl2->p3, sl2->p4};
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (intersectLines(edges1[i], edges1[(i+1)%4], edges2[j], edges2[(j+1)%4])) {
                return true;
            }
        }
    }
    return false;
}

void checkSweptLineCollision(SweptLine* sl1, SweptLine* sl2, CollisionWorld* collisionWorld, IntersectionEventList* intersectionEventList) {
    if (compareLines(sl1->line, sl2->line) >= 0) {
        SweptLine* temp = sl1;
        sl1 = sl2;
        sl2 = temp;
    }

    if (SweptLine_intersect(sl1, sl2)) {
        IntersectionEventList_appendNode(intersectionEventList, sl1->line, sl2->line, ALREADY_INTERSECTED);
        collisionWorld->numLineLineCollisions++;
    }
}

void checkCollisionsQuadTree(QuadTree* tree, IntersectionEventList* intersectionEventList, CollisionWorld* collisionWorld) {
    for (int i = 0; i < tree->numOfSweptLines; ++i) {
        for (int j = i + 1; j < tree->numOfSweptLines; ++j) {
            checkSweptLineCollision(&tree->sweptLines[i], &tree->sweptLines[j], collisionWorld, intersectionEventList);
        }
    }

    if (tree->lowerLeft) {
        checkChildSweptLines(tree, collisionWorld, intersectionEventList, tree->lowerLeft);
        checkChildSweptLines(tree, collisionWorld, intersectionEventList, tree->lowerRight);
        checkChildSweptLines(tree, collisionWorld, intersectionEventList, tree->upperLeft);
        checkChildSweptLines(tree, collisionWorld, intersectionEventList, tree->upperRight);

        checkCollisionsQuadTree(tree->lowerLeft, intersectionEventList, collisionWorld);
        checkCollisionsQuadTree(tree->lowerRight, intersectionEventList, collisionWorld);
        checkCollisionsQuadTree(tree->upperLeft, intersectionEventList, collisionWorld);
        checkCollisionsQuadTree(tree->upperRight, intersectionEventList, collisionWorld);
    }
}

void CollisionWorld_detectIntersection(CollisionWorld* collisionWorld) {
  IntersectionEventList intersectionEventList = IntersectionEventList_make();

  QuadTree* base = QuadTree_create(BOX_XMIN, BOX_YMIN, BOX_XMAX - BOX_XMIN, BOX_YMAX - BOX_YMIN, collisionWorld->numOfLines, 8);
  
  for(int i = 0; i < collisionWorld->numOfLines; ++i) {
      QuadTree_insert(base, collisionWorld->lines[i]);
  }
  
  checkCollisionsQuadTree(base, &intersectionEventList, collisionWorld);

  // Sort the intersection event list.
  IntersectionEventNode* startNode = intersectionEventList.head;
  while (startNode != NULL) {
    IntersectionEventNode* minNode = startNode;
    IntersectionEventNode* curNode = startNode->next;
    while (curNode != NULL) {
      if (IntersectionEventNode_compareData(curNode, minNode) < 0) {
        minNode = curNode;
      }
      curNode = curNode->next;
    }
    if (minNode != startNode) {
      IntersectionEventNode_swapData(minNode, startNode);
    }
    startNode = startNode->next;
  }

  // Call the collision solver for each intersection event.
  IntersectionEventNode* curNode = intersectionEventList.head;

  while (curNode != NULL) {
    CollisionWorld_collisionSolver(collisionWorld, curNode->l1, curNode->l2,
                                   curNode->intersectionType);
    curNode = curNode->next;
  }

  IntersectionEventList_deleteNodes(&intersectionEventList);
  QuadTree_free(base);
}


unsigned int CollisionWorld_getNumLineWallCollisions(
    CollisionWorld* collisionWorld) {
  return collisionWorld->numLineWallCollisions;
}

unsigned int CollisionWorld_getNumLineLineCollisions(
    CollisionWorld* collisionWorld) {
  return collisionWorld->numLineLineCollisions;
}

void CollisionWorld_collisionSolver(CollisionWorld* collisionWorld,
                                    Line *l1, Line *l2,
                                    IntersectionType intersectionType) {
  assert(compareLines(l1, l2) < 0);
  assert(intersectionType == L1_WITH_L2
         || intersectionType == L2_WITH_L1
         || intersectionType == ALREADY_INTERSECTED);

  // Despite our efforts to determine whether lines will intersect ahead
  // of time (and to modify their velocities appropriately), our
  // simplified model can sometimes cause lines to intersect.  In such a
  // case, we compute velocities so that the two lines can get unstuck in
  // the fastest possible way, while still conserving momentum and kinetic
  // energy.
  if (intersectionType == ALREADY_INTERSECTED) {
    Vec p = getIntersectionPoint(l1->p1, l1->p2, l2->p1, l2->p2);

    if (Vec_length(Vec_subtract(l1->p1, p))
        < Vec_length(Vec_subtract(l1->p2, p))) {
      l1->velocity = Vec_multiply(Vec_normalize(Vec_subtract(l1->p2, p)),
                                  Vec_length(l1->velocity));
    } else {
      l1->velocity = Vec_multiply(Vec_normalize(Vec_subtract(l1->p1, p)),
                                  Vec_length(l1->velocity));
    }
    if (Vec_length(Vec_subtract(l2->p1, p))
        < Vec_length(Vec_subtract(l2->p2, p))) {
      l2->velocity = Vec_multiply(Vec_normalize(Vec_subtract(l2->p2, p)),
                                  Vec_length(l2->velocity));
    } else {
      l2->velocity = Vec_multiply(Vec_normalize(Vec_subtract(l2->p1, p)),
                                  Vec_length(l2->velocity));
    }
    return;
  }

  // Compute the collision face/normal vectors.
  Vec face;
  Vec normal;
  if (intersectionType == L1_WITH_L2) {
    Vec v = Vec_makeFromLine(*l2);
    face = Vec_normalize(v);
  } else {
    Vec v = Vec_makeFromLine(*l1);
    face = Vec_normalize(v);
  }
  normal = Vec_orthogonal(face);

  // Obtain each line's velocity components with respect to the collision
  // face/normal vectors.
  double v1Face = Vec_dotProduct(l1->velocity, face);
  double v2Face = Vec_dotProduct(l2->velocity, face);
  double v1Normal = Vec_dotProduct(l1->velocity, normal);
  double v2Normal = Vec_dotProduct(l2->velocity, normal);

  // Compute the mass of each line (we simply use its length).
  double m1 = Vec_length(Vec_subtract(l1->p1, l1->p2));
  double m2 = Vec_length(Vec_subtract(l2->p1, l2->p2));

  // Perform the collision calculation (computes the new velocities along
  // the direction normal to the collision face such that momentum and
  // kinetic energy are conserved).
  double newV1Normal = ((m1 - m2) / (m1 + m2)) * v1Normal
      + (2 * m2 / (m1 + m2)) * v2Normal;
  double newV2Normal = (2 * m1 / (m1 + m2)) * v1Normal
      + ((m2 - m1) / (m2 + m1)) * v2Normal;

  // Combine the resulting velocities.
  l1->velocity = Vec_add(Vec_multiply(normal, newV1Normal),
                         Vec_multiply(face, v1Face));
  l2->velocity = Vec_add(Vec_multiply(normal, newV2Normal),
                         Vec_multiply(face, v2Face));

  return;
}
