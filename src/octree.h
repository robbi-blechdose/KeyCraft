#ifndef _OCTREE_H
#define _OCTREE_H

#include "chunk.h"
#include "aabb.h"

typedef struct Octree {
    AABB aabb;
    Chunk* chunk;
    struct Octree* children[8];
} Octree;

Octree* createOctree(vec3 pos, float size, Chunk* chunk);
void insertOctree(Octree* octree, Chunk* chunk);
Chunk* findOctree(Octree* octree, ChunkPos* pos);

void freeOctree(Octree* octree);

void saveOctree(Octree* octree);
Octree* loadOctree();

void printOctree(Octree* octree, char* prefix);

#endif