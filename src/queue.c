#include "queue.h"

#include <stdlib.h>

ChunkQueue* createChunkQueue()
{
    ChunkQueue* queue = malloc(sizeof(ChunkQueue));
    queue->first = NULL,
    queue->last = NULL;
    return queue;
}

void destroyChunkQueue(ChunkQueue* queue)
{
    ChunkQueueNode* node = queue->first;
    while(node != NULL)
    {
        ChunkQueueNode* next = node->next;
        free(node);
        node = next;
    }
}

void chunkEnqueue(ChunkQueue* queue, Chunk* chunk)
{
    ChunkQueueNode* node = malloc(sizeof(ChunkQueueNode));
    node->chunk = chunk;
    node->next = NULL;

    if(queue->first == NULL)
    {
        queue->first = node;
        queue->last = node;
    }
    else
    {
        queue->last->next = node;
        queue->last = node;
    }
}

void chunkEnqueueNoDup(ChunkQueue* queue, Chunk* chunk)
{
    ChunkQueueNode* node = malloc(sizeof(ChunkQueueNode));
    node->chunk = chunk;
    node->next = NULL;

    if(queue->first == NULL)
    {
        queue->first = node;
        queue->last = node;
    }
    else
    {
        ChunkQueueNode* current = queue->first;
        while(current != NULL)
        {
            if(current->chunk == chunk)
            {
                //Chunk already present
                free(node);
                return;
            }
            current = current->next;
        }

        queue->last->next = node;
        queue->last = node;
    }
}

Chunk* chunkDequeue(ChunkQueue* queue)
{
    ChunkQueueNode* node = queue->first;
    Chunk* chunk = node->chunk;

    queue->first = node->next;
    if(queue->first == NULL)
    {
        queue->last = NULL;
    }
    free(node);

    return chunk;
}

bool isChunkQueueEmpty(ChunkQueue* queue)
{
    return queue->first == NULL;
}