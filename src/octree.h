#ifndef OCTREE_H
#define OCTREE_H

#include <stdbool.h>

#include "chunk.h"
#include "aabb.h"

//#define OCTREE_DEBUG

typedef struct Octree {
    AABB aabb;
    Chunk* chunk;
    struct Octree* children[8];
} Octree;

Octree* createOctree(vec3 pos, float size, Chunk* chunk);
void insertOctree(Octree* octree, Chunk* chunk);
Chunk* findOctree(Octree* octree, ChunkPos* pos);
bool octreeEmpty(Octree* octree);

void freeOctree(Octree* octree);

void saveOctree(Octree* octree);
Octree* loadOctree();

#ifdef OCTREE_DEBUG
void printOctree(Octree* octree, char* prefix);
#endif

#endif