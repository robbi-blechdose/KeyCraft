#include "octree.h"

#include <stdlib.h>

#include "engine/savegame.h"

#define OCTREE_ACCESS(x, y, z) octree->children[(x) + (y) * 2 + (z) * 4]

Octree* createOctree(vec3 pos, float size, Chunk* chunk)
{
    Octree* octree = malloc(sizeof(Octree));
    float size2 = size / 2;
    octree->aabb = (AABB) {.min = (vec3) {.x = pos.x - size2, .y = pos.y - size2, .z = pos.z - size2},
                            .max = (vec3) {.x = pos.x + size2, .y = pos.y + size2, .z = pos.z + size2}};
    octree->chunk = chunk;
    for(uint8_t i = 0; i < 8; i++)
    {
        octree->children[i] = NULL;
    }
    return octree;
}

void insertOctree(Octree* octree, Chunk* chunk)
{
    if(octree->chunk == NULL)
    {
        octree->chunk = chunk;
    }
    else if(octree->chunk->position.x == chunk->position.x &&
            octree->chunk->position.y == chunk->position.y &&
            octree->chunk->position.z == chunk->position.z)
    {
        //Position is the same, that means the chunk was already modified and is already present in the octree
        //Thus, we don't have to do anything at all
        return;
    }
    else
    {
        //Calculate subtree to place chunk into
        float size = octree->aabb.max.x - octree->aabb.min.x; //We can use any side since the AABB is cubic

        //Check in which part of the tree the point belongs (point > center)
        uint8_t x = (chunk->position.x > octree->aabb.min.x + size / 2);
        uint8_t y = (chunk->position.y > octree->aabb.min.y + size / 2);
        uint8_t z = (chunk->position.z > octree->aabb.min.z + size / 2);

        if(OCTREE_ACCESS(x, y, z) == NULL)
        {
            vec3 newCenter = {.x = octree->aabb.min.x + (size / 2) + (x ? size / 4 : (-size / 4)),
                              .y = octree->aabb.min.y + (size / 2) + (y ? size / 4 : (-size / 4)),
                              .z = octree->aabb.min.z + (size / 2) + (z ? size / 4 : (-size / 4))};
            OCTREE_ACCESS(x, y, z) = createOctree(newCenter, size / 2, chunk);
        }
        else
        {
            insertOctree(OCTREE_ACCESS(x, y, z), chunk);
        }
    }
}

Chunk* findOctree(Octree* octree, ChunkPos* pos)
{
    vec3 posf = (vec3) {.x = pos->x, .y = pos->y, .z = pos->z};
    //Check if the chunk is inside our AABB at all
    if(!aabbIntersectsPoint(&octree->aabb, &posf))
    {
        return NULL;
    }
    //Check if we have the chunk
    else if(octree->chunk != NULL &&
            pos->x == octree->chunk->position.x &&
            pos->y == octree->chunk->position.y &&
            pos->z == octree->chunk->position.z)
    {
        return octree->chunk;
    }
    else
    {
        //Calculate subtree to search chunk in
        float size = octree->aabb.max.x - octree->aabb.min.x; //We can use any side since the AABB is cubic

        //Check in which part of the tree the point belongs (point > center)
        uint8_t x = (pos->x > octree->aabb.min.x + size / 2);
        uint8_t y = (pos->y > octree->aabb.min.y + size / 2);
        uint8_t z = (pos->z > octree->aabb.min.z + size / 2);

        if(OCTREE_ACCESS(x, y, z) == NULL)
        {
            return NULL;
        }
        else
        {
            return findOctree(OCTREE_ACCESS(x, y, z), pos);
        }
    }

    return NULL;
}

uint8_t octreeCount(Octree* octree)
{
    if(octree == NULL)
    {
        return 0;
    }
    //Since we always insert into an octree first, the children have to be NULL if the chunk is NULL
    else if(octree->chunk == NULL)
    {
        return 0;
    }
    else
    {
        uint8_t sum = 1;
        for(uint8_t i = 0; i < 8; i++)
        {
            sum += octreeCount(octree->children[i]);
        }
        return sum;
    }
}

uint8_t octreeEmpty(Octree* octree)
{
    if(octree == NULL)
    {
        return 1;
    }
    //Since we always insert into an octree first, the children have to be NULL if the chunk is NULL
    else if(octree->chunk == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void freeOctree(Octree* octree)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if(octree->children[i] != NULL)
        {
            freeOctree(octree->children[i]);
        }
    }
    free(octree);
}

void saveOctree(Octree* octree)
{
    writeElement(&octree->aabb, sizeof(AABB));
    saveChunk(octree->chunk);

    uint8_t count = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        if(octree->children[i] != NULL)
        {
            count++;
        }
    }
    writeElement(&count, sizeof(uint8_t));

    for(uint8_t i = 0; i < 8; i++)
    {
        if(octree->children[i] != NULL)
        {
            writeElement(&i, sizeof(uint8_t));
            saveOctree(octree->children[i]);
        }
    }
}

Octree* loadOctree()
{
    Octree* octree = malloc(sizeof(Octree));

    readElement(&octree->aabb, sizeof(AABB));
    octree->chunk = calloc(1, sizeof(Chunk));
    loadChunk(octree->chunk);
    
    uint8_t count;
    readElement(&count, sizeof(uint8_t));

    //Zero out memory first
    for(uint8_t i = 0; i < 8; i++)
    {
        octree->children[i] = NULL;
    }

    for(uint8_t i = 0; i < count; i++)
    {
        uint8_t index;
        readElement(&index, sizeof(uint8_t));
        octree->children[index] = loadOctree();
    }

    return octree;
}

void printOctree(Octree* octree, char* prefix)
{
    if(octree == NULL)
    {
        return;
    }

    printf(prefix);

    if(octree->chunk != NULL)
    {
        printf("%d %d %d\n", octree->chunk->position.x, octree->chunk->position.y, octree->chunk->position.z);
    }
    else
    {
        printf("- - -\n");
    }
    
    char* newPrefix = malloc(strlen(prefix) + 3);
    strcpy(newPrefix, prefix);
    strcat(newPrefix, "->");
    for(uint8_t i = 0; i < 8; i++)
    {
        printOctree(octree->children[i], newPrefix);
    }
    free(newPrefix);
}