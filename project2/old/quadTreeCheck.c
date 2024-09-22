// void QuadTree_check(QuadTree* qt, int depth) {
//     if (qt == NULL) {
//         fprintf(stderr, "Error: NULL QuadTree at depth %d\n", depth);
//         exit(1);
//     }

//     printf("QuadTree at depth %d:\n", depth);
//     printf("  Boundary: (%.2f, %.2f) to (%.2f, %.2f)\n", 
//            qt->boundary.x, qt->boundary.y, 
//            qt->boundary.x + qt->boundary.width, 
//            qt->boundary.y + qt->boundary.height);
//     printf("  Capacity: %u, Overflow: %u\n", qt->capacity, qt->overflow);
//     printf("  Number of SweptLines: %d\n", qt->numOfSweptLines);

//     // Check if the number of SweptLines exceeds the capacity
//     if (qt->numOfSweptLines > qt->capacity) {
//         fprintf(stderr, "Error: Number of SweptLines (%d) exceeds capacity (%u) at depth %d\n", 
//                qt->numOfSweptLines, qt->capacity, depth);
//         exit(1);
//     }

//     // Print information about each SweptLine
//     for (int i = 0; i < qt->numOfSweptLines; i++) {
//         SweptLine* sl = &qt->sweptLines[i];
//         printf("    SweptLine %d: (%.2f, %.2f) to (%.2f, %.2f)\n", i,
//                sl->p1.x, sl->p1.y, sl->p2.x, sl->p2.y);
        
//         // Check if the SweptLine is actually within the QuadTree's boundary
//         if (!SweptLine_inBoundingBox(sl, &qt->boundary)) {
//             fprintf(stderr, "Error: SweptLine %d is outside the QuadTree boundary at depth %d\n", i, depth);
//             exit(1);
//         }
//     }

//     // Check if this node should have been subdivided
//     if (qt->numOfSweptLines > qt->overflow && qt->lowerLeft == NULL) {
//         fprintf(stderr, "Error: Node exceeds overflow limit but hasn't been subdivided at depth %d\n", depth);
//         exit(1);
//     }

//     // Recursively check child nodes
//     if (qt->lowerLeft != NULL) {
//         QuadTree_check(qt->lowerLeft, depth + 1);
//         QuadTree_check(qt->lowerRight, depth + 1);
//         QuadTree_check(qt->upperLeft, depth + 1);
//         QuadTree_check(qt->upperRight, depth + 1);
//     }
// }

// // Usage example
// void checkQuadTreeCorrectness(CollisionWorld* collisionWorld) {
//     QuadTree* base = QuadTree_create(BOX_XMIN, BOX_YMIN, BOX_XMAX - BOX_XMIN, BOX_YMAX - BOX_YMIN, collisionWorld->numOfLines, 8);
    
//     for(int i = 0; i < collisionWorld->numOfLines; ++i) {
//         QuadTree_insert(base, collisionWorld->lines[i]);
//     }
    
//     printf("Checking QuadTree correctness:\n");
//     QuadTree_check(base, 0);
    
//     QuadTree_free(base);
// }