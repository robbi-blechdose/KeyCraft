#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>

#include "chunk.h"

typedef struct ChunkQueueNode {
    Chunk* chunk;
    struct ChunkQueueNode* next;
} ChunkQueueNode;

typedef struct {
    ChunkQueueNode* first;
    ChunkQueueNode* last;
} ChunkQueue;

ChunkQueue* createChunkQueue();
void destroyChunkQueue(ChunkQueue* queue);

void chunkEnqueue(ChunkQueue* queue, Chunk* chunk);
void chunkEnqueueNoDup(ChunkQueue* queue, Chunk* chunk);
Chunk* chunkDequeue(ChunkQueue* queue);
void chunkQueueRemove(ChunkQueue* queue, Chunk* chunk);
bool isChunkQueueEmpty(ChunkQueue* queue);

#endif