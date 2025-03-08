#include "world.h"

#include "fk-engine-core/image.h"
#include "fk-engine-core/savegame.h"

#include "chunk.h"
#include "worldgen/worldgen.h"
#include "blocks/blocklogic.h"
#include "blocks/blockactions.h"
#include "octree.h"
#include "queue.h"

#define VIEW_CHUNK(i, j, k) chunks[(i) + ((j) * VIEW_DISTANCE) + ((k) * VIEW_DISTANCE * VIEW_DISTANCE)]
#define WORLD_CHUNK(i, j, k) VIEW_CHUNK((i) - chunkPos.x, (j) - chunkPos.y, (k) - chunkPos.z)

uint32_t seed;

ChunkPos chunkPos;
Chunk* chunks[VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE];
Octree* modifiedChunks;

ChunkQueue* chunkQueue;

GLuint terrainTexture;

uint32_t worldTicks;

void initWorld(uint32_t pSeed)
{
    seed = pSeed;
    initWorldgen(pSeed);

    //Initial worldgen
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                VIEW_CHUNK(i, j, k) = calloc(1, sizeof(Chunk));
                VIEW_CHUNK(i, j, k)->position = (ChunkPos) {i, j, k};
                generateChunk(VIEW_CHUNK(i, j, k));
            }
        }
    }
    //Propagate structure data
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                propagateChunkStructureData(VIEW_CHUNK(i, j, k), chunks, VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE);
            }
        }
    }
    //Generate structures
    //If there are "source" structures outside the current view distance, the chunks within view distance won't be populated with structures (as in, will be missing parts)
    //This is pretty unavoidable though, and also not really a problem
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                generateChunkStructures(VIEW_CHUNK(i, j, k));
            }
        }
    }

    chunkPos = (ChunkPos) {0, 0, 0};

    modifiedChunks = createOctree((vec3) {0, VIEW_DISTANCE / 2, 0}, INT16_MAX * 2, NULL);
    chunkQueue = createChunkQueue();

    worldTicks = 0;

    terrainTexture = loadRGBTexture(RESOURCE("res/tex/terrain.png"));
    glBindTexture(GL_TEXTURE_2D, terrainTexture);
}

void quitWorld()
{
    deleteRGBTexture(terrainTexture);
    freeOctree(modifiedChunks);
    destroyChunkQueue(chunkQueue);
}

typedef enum {
    SWP_FB,
    SWP_LR,
    SWP_TB
} SwapSide;

typedef enum {
    SWP_FORE,
    SWP_BACK
} SwapDir;

void swapChunks(SwapSide side, SwapDir direction)
{
    //We need these for structure propagation + generation
    uint8_t newChunkIndex = 0;
    Chunk* newChunks[VIEW_DISTANCE * VIEW_DISTANCE];
    uint8_t adjacentChunkIndex = 0;
    Chunk* adjacentChunks[VIEW_DISTANCE * VIEW_DISTANCE];

    uint8_t out, in;
    if(direction == SWP_FORE)
    {
        out = 0;
        in = VIEW_DISTANCE - 1;
    }
    else
    {
        out = VIEW_DISTANCE - 1;
        in = 0;
    }

    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            //Destroy chunk "behind" the player
            Chunk* toDestroy;
            if(side == SWP_FB)
            {
                toDestroy = VIEW_CHUNK(out, i, j);
            }
            else if(side == SWP_LR)
            {
                toDestroy = VIEW_CHUNK(i, j, out);
            }
            else
            {
                toDestroy = VIEW_CHUNK(i, j, out);
            }
            //Check if chunk has been modified
            if(CHUNK_GET_FLAG(toDestroy, CHUNK_IS_INITIAL))
            {
                chunkQueueRemove(chunkQueue, toDestroy);
                destroyChunk(toDestroy);
                free(toDestroy);
            }
            else
            {
                insertOctree(modifiedChunks, toDestroy);
            }

            //Shift other chunks
            if(direction == SWP_FORE)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE - 1; k++)
                {
                    if(side == SWP_FB)
                    {
                        VIEW_CHUNK(k, i, j) = VIEW_CHUNK(k + 1, i, j);
                    }
                    else if(side == SWP_LR)
                    {
                        VIEW_CHUNK(i, j, k) = VIEW_CHUNK(i, j, k + 1);
                    }
                    else
                    {
                        VIEW_CHUNK(i, j, k) = VIEW_CHUNK(i, j, k + 1);
                    }
                }
            }
            else
            {
                for(uint8_t k = VIEW_DISTANCE - 1; k > 0; k--)
                {
                    if(side == SWP_FB)
                    {
                        VIEW_CHUNK(k, i, j) = VIEW_CHUNK(k - 1, i, j);
                    }
                    else if(side == SWP_LR)
                    {
                        VIEW_CHUNK(i, j, k) = VIEW_CHUNK(i, j, k - 1);
                    }
                    else
                    {
                        VIEW_CHUNK(i, j, k) = VIEW_CHUNK(i, j, k - 1);
                    }
                }
            }

            //Load new chunk "in front of" player
            //Check in octree
            ChunkPos newPos;
            if(side == SWP_FB)
            {
                newPos = (ChunkPos) {chunkPos.x + in, chunkPos.y + i, chunkPos.z + j};
            }
            else if(side == SWP_LR)
            {
                newPos = (ChunkPos) {chunkPos.x + i, chunkPos.y + j, chunkPos.z + in};
            }
            else
            {
                newPos = (ChunkPos) {chunkPos.x + i, chunkPos.y + j, chunkPos.z + in};
            }
            Chunk* newChunk = findOctree(modifiedChunks, &newPos);

            //Nothing in octree, generate
            if(newChunk == NULL)
            {
                newChunk = calloc(1, sizeof(Chunk));
                newChunk->position = newPos;
                generateChunk(newChunk);
            }
            else
            {
                //Mark geometry to be rebuilt (in case adjacent chunks changed)
                CHUNK_SET_FLAG(newChunk, CHUNK_MODIFIED);
            }
            
            //Grab adjacent chunk for structure propagation + generation, mark it to be rebuilt, set new chunk
            int8_t dir = direction == SWP_FORE ? -1 : 1;
            if(side == SWP_FB)
            {
                adjacentChunks[adjacentChunkIndex++] = VIEW_CHUNK(in + dir, i, j);
                CHUNK_SET_FLAG(VIEW_CHUNK(in + dir, i, j), CHUNK_MODIFIED);
                VIEW_CHUNK(in, i, j) = newChunk;
            }
            else if(side == SWP_LR)
            {
                adjacentChunks[adjacentChunkIndex++] = VIEW_CHUNK(i, j, in + dir);
                CHUNK_SET_FLAG(VIEW_CHUNK(i, j, in + dir), CHUNK_MODIFIED);
                VIEW_CHUNK(i, j, in) = newChunk;
            }
            else
            {
                adjacentChunks[adjacentChunkIndex++] = VIEW_CHUNK(i, j, in + dir);
                CHUNK_SET_FLAG(VIEW_CHUNK(i, j, in + dir), CHUNK_MODIFIED);
                VIEW_CHUNK(i, j, in) = newChunk;
            }

            //Grab new chunk for structure propagation + generation
            newChunks[newChunkIndex++] = newChunk;
        }
    }

    //TODO: structures may reach more than one chunk - what do we do in that case???

    //Propagate data into new chunks and from new chunks
    for(uint8_t i = 0; i < adjacentChunkIndex; i++)
    {
        propagateChunkStructureData(adjacentChunks[i], newChunks, newChunkIndex);
    }
    for(uint8_t i = 0; i < newChunkIndex; i++)
    {
        propagateChunkStructureData(newChunks[i], newChunks, newChunkIndex);
        propagateChunkStructureData(newChunks[i], adjacentChunks, adjacentChunkIndex);
    }

    //Finally, generate structures in both of these
    for(uint8_t i = 0; i < adjacentChunkIndex; i++)
    {
        generateChunkStructures(adjacentChunks[i]);
    }
    for(uint8_t i = 0; i < newChunkIndex; i++)
    {
        generateChunkStructures(newChunks[i]);
    }
}

void calcWorld(vec3* playerPos, uint32_t ticks)
{
    //Load chunks around the player in/out
    ChunkPos playerChunkPos = {
        (playerPos->x) / CHUNK_SIZE,
        (playerPos->y) / CHUNK_SIZE,
        (playerPos->z) / CHUNK_SIZE
    };

    if(playerChunkPos.x - chunkPos.x != 0)
    {
        if(playerChunkPos.x > chunkPos.x)
        {
            chunkPos.x++;
            swapChunks(SWP_FB, SWP_FORE);
        }
        else
        {
            chunkPos.x--;
            swapChunks(SWP_FB, SWP_BACK);
        }
    }
    if(playerChunkPos.z - chunkPos.z != 0)
    {
        if(playerChunkPos.z > chunkPos.z)
        {
            chunkPos.z++;
            swapChunks(SWP_LR, SWP_FORE);
        }
        else
        {
            chunkPos.z--;
            swapChunks(SWP_LR, SWP_BACK);
        }
    }
    //TODO: Z

    //Queue visible and modified chunks for geometry (re)calculation
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                Chunk* chunk = VIEW_CHUNK(i, j, k);
                if(CHUNK_GET_FLAG(chunk, CHUNK_MODIFIED))
                {
                    chunkEnqueueNoDup(chunkQueue, chunk);
                }
            }
        }
    }

    //Work chunk queue
    if(!isChunkQueueEmpty(chunkQueue))
    {
        uint8_t counter = MAX_CHUNKS_PER_FRAME;
        while(!isChunkQueueEmpty(chunkQueue) && counter > 0)
        {
            calcChunk(chunkDequeue(chunkQueue));
            counter--;
        }
    }

    //Tick visible chunks
    worldTicks += ticks;
    if(worldTicks >= TICK_RATE)
    {
        worldTicks = 0;

        for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
        {
            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
                {
                    //Don't need to tick empty chunks or initial chunks (the latter because tickable blocks aren't part of worldgen)
                    if(!CHUNK_GET_FLAG(VIEW_CHUNK(i, j, k), CHUNK_IS_EMPTY | CHUNK_IS_INITIAL))
                    {
                        tickChunk(VIEW_CHUNK(i, j, k));
                    }
                }
            }
        }
    }
}

void drawWorld(vec3* playerPosition, vec3* playerRotation)
{
    //y stays 0, we ignore the height for chunk culling
    vec3 chunkCenter = {0, 0, 0};
    vec3 chunkCenterRot = {0, 0, 0};

    //Draw visible chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        //Calculate X in outer loop
        chunkCenter.x = i * CHUNK_SIZE + (chunkPos.x * CHUNK_SIZE - playerPosition->x) - CHUNK_SIZE * 2;

        for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
        {
            //Calculate Z in inner loop
            chunkCenter.z = k * CHUNK_SIZE + (chunkPos.z * CHUNK_SIZE - playerPosition->z) - CHUNK_SIZE * 2;

            //Calculate "rotated" position (so that z is always in the camera direction)
            chunkCenterRot.x = chunkCenter.x * cosf(-playerRotation->y + M_PI) - chunkCenter.z * sinf(-playerRotation->y + M_PI);
            chunkCenterRot.z = chunkCenter.x * sinf(-playerRotation->y + M_PI) + chunkCenter.z * cosf(-playerRotation->y + M_PI);

            //Skip drawing chunks behind the camera
            if(chunkCenterRot.z < -CHUNK_SIZE)
            {
                continue;
            }
            //Skip drawing chunks left/right of the camera
            if(chunkCenterRot.z < 0 && (chunkCenterRot.x < -CHUNK_SIZE || chunkCenterRot.x > CHUNK_SIZE))
            {
                continue;
            }

            //Translating here and then adding to it in the inner loop saves a few push/pops
            glPushMatrix();
            glTranslatef((i + chunkPos.x) * CHUNK_SIZE - VIEW_TRANSLATION,
                        chunkPos.y * CHUNK_SIZE - VIEW_TRANSLATION,
                        (k + chunkPos.z) * CHUNK_SIZE - VIEW_TRANSLATION);

            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                //Discard empty chunks and chunks with no draw data
                if(!CHUNK_GET_FLAG(VIEW_CHUNK(i, j, k), CHUNK_IS_EMPTY | CHUNK_NO_DRAW_DATA))
                {
                    drawChunk(VIEW_CHUNK(i, j, k));
                }
                glTranslatef(0, CHUNK_SIZE, 0);
            }
            glPopMatrix();
        }
    }
}

void normalizeBlockPos(BlockPos* pos)
{
    if(pos->x < 0)
    {
        pos->x += CHUNK_SIZE;
        pos->chunk.x--;
    }
    else if(pos->x >= CHUNK_SIZE)
    {
        pos->x -= CHUNK_SIZE;
        pos->chunk.x++;
    }

    if(pos->y < 0)
    {
        pos->y += CHUNK_SIZE;
        pos->chunk.y--;
    }
    else if(pos->y >= CHUNK_SIZE)
    {
        pos->y -= CHUNK_SIZE;
        pos->chunk.y++;
    }

    if(pos->z < 0)
    {
        pos->z += CHUNK_SIZE;
        pos->chunk.z--;
    }
    else if(pos->z >= CHUNK_SIZE)
    {
        pos->z -= CHUNK_SIZE;
        pos->chunk.z++;
    }
}

Chunk* getWorldChunk(BlockPos* pos)
{
    normalizeBlockPos(pos);
    //Bounds check
    if(pos->chunk.x >= chunkPos.x + VIEW_DISTANCE||
        pos->chunk.x  < chunkPos.x ||
        pos->chunk.y >= chunkPos.y + VIEW_DISTANCE ||
        pos->chunk.y  < chunkPos.y ||
        pos->chunk.z >= chunkPos.z + VIEW_DISTANCE ||
        pos->chunk.z  < chunkPos.z)
    {
        return NULL;
    }
    return WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
}

Block* getWorldBlock(BlockPos* pos)
{
    normalizeBlockPos(pos);
    //Bounds check
    if(pos->chunk.x >= chunkPos.x + VIEW_DISTANCE||
        pos->chunk.x  < chunkPos.x ||
        pos->chunk.y >= chunkPos.y + VIEW_DISTANCE ||
        pos->chunk.y  < chunkPos.y ||
        pos->chunk.z >= chunkPos.z + VIEW_DISTANCE ||
        pos->chunk.z  < chunkPos.z)
    {
        return NULL;
    }
    return &CHUNK_BLOCK(WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z), pos->x, pos->y, pos->z);
}

void setWorldBlock(BlockPos* pos, Block block)
{
    normalizeBlockPos(pos);
    //Bounds check
    if(pos->chunk.x >= chunkPos.x + VIEW_DISTANCE||
        pos->chunk.x  < chunkPos.x ||
        pos->chunk.y >= chunkPos.y + VIEW_DISTANCE ||
        pos->chunk.y  < chunkPos.y ||
        pos->chunk.z >= chunkPos.z + VIEW_DISTANCE ||
        pos->chunk.z  < chunkPos.z)
    {
        return;
    }

    //Place block and mark chunk as modified
    Chunk* chunk = WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
    CHUNK_BLOCK(chunk, pos->x, pos->y, pos->z) = block;
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    //Mark chunk as player-modified
    CHUNK_CLEAR_FLAG(chunk, CHUNK_IS_INITIAL);

    //Mark adjacent chunks as modified if necessary
    if(pos->x == 0 && pos->chunk.x > chunkPos.x)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x - 1, pos->chunk.y, pos->chunk.z), CHUNK_MODIFIED);
    }
    else if(pos->x == CHUNK_SIZE - 1 && pos->chunk.x < chunkPos.x + VIEW_DISTANCE - 1)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x + 1, pos->chunk.y, pos->chunk.z), CHUNK_MODIFIED);
    }
    if(pos->y == 0 && pos->chunk.y > chunkPos.y)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x, pos->chunk.y - 1, pos->chunk.z), CHUNK_MODIFIED);
    }
    else if(pos->y == CHUNK_SIZE - 1 && pos->chunk.y < chunkPos.y + VIEW_DISTANCE - 1)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x, pos->chunk.y + 1, pos->chunk.z), CHUNK_MODIFIED);
    }
    if(pos->z == 0 && pos->chunk.z > chunkPos.z)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z - 1), CHUNK_MODIFIED);
    }
    else if(pos->z == CHUNK_SIZE - 1 && pos->chunk.z < chunkPos.z + VIEW_DISTANCE - 1)
    {
        CHUNK_SET_FLAG(WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z + 1), CHUNK_MODIFIED);
    }

    //Place door upper (yes, this is a special case)
    if(block.type == BLOCK_DOOR && !(block.data & BLOCK_DATA_PART))
    {
        pos->y++;
        setWorldBlock(pos, (Block) {BLOCK_DOOR, block.data + BLOCK_DATA_PART});
        pos->y--;
    }
    else if(block.type == BLOCK_COMPUTER)
    {
        //Create computer data
        for(uint8_t i = 0; i < NUM_COMPUTERS; i++)
        {
            if(chunk->computers[i] == NULL)
            {
                chunk->computers[i] = createComputer();
                CHUNK_BLOCK(chunk, pos->x, pos->y, pos->z).data |= TO_COMPUTER_INDEX(i);
                break;
            }
        }
    }
}

bool actWorldBlock(BlockPos* pos)
{
    Block* block = getWorldBlock(pos);
    if(block == NULL)
    {
        return false;
    }

    Chunk* chunk = WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
    bool ret = actBlock(chunk, block);

    if(block->type == BLOCK_DOOR)
    {
        if(block->data & BLOCK_DATA_PART)
        {
            //Upper, act lower too
            pos->y--;
            block = getWorldBlock(pos);
            chunk = WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
            ret += actBlock(chunk, block);
        }
        else
        {
            //Lower, act upper too
            pos->y++;
            block = getWorldBlock(pos);
            chunk = WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
            ret += actBlock(chunk, block);
        }
    }
    return ret;
}

AABBSide intersectsRayWorld(vec3* origin, vec3* direction, BlockPos* block, float* distance)
{
    AABBSide minSide = AABB_NONE;
    BlockPos minBlock;
    float minDistance = 512;

    //Exclude outer chunks
    for(uint8_t i = MIN_RAY_CHUNK; i <= MAX_RAY_CHUNK; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = MIN_RAY_CHUNK; k <= MAX_RAY_CHUNK; k++)
            {
                //Discard empty chunks
                if(!CHUNK_GET_FLAG(VIEW_CHUNK(i, j, k), CHUNK_IS_EMPTY))
                {
                    AABBSide result = intersectsRayChunk(VIEW_CHUNK(i, j, k), origin, direction, block, distance);
                    if(result != AABB_NONE)
                    {
                        if(*distance < minDistance)
                        {
                            minSide = result;
                            minBlock = *block;
                            minDistance = *distance;
                        }
                    }
                }
            }
        }
    }

    if(minDistance > MAX_RAY_DISTANCE)
    {
        return AABB_NONE;
    }

    *block = minBlock;
    *distance = minDistance;
    return minSide;
}

bool intersectsAABBWorld(AABB* aabb)
{
    //Exclude outer chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                if(intersectsAABBChunk(VIEW_CHUNK(i, j, k), aabb))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool isWorldUnmodified()
{
    //Check all chunks in view distance
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                if(!CHUNK_GET_FLAG(VIEW_CHUNK(i, j, k), CHUNK_IS_INITIAL))
                {
                    return false;
                }
            }
        }
    }
    //Check octree
    return octreeEmpty(modifiedChunks);
}

void saveWorld()
{
    writeElement(&chunkPos, sizeof(ChunkPos));

    //Store all modified chunks into octree
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                if(!CHUNK_GET_FLAG(VIEW_CHUNK(i, j, k), CHUNK_IS_INITIAL))
                {
                    insertOctree(modifiedChunks, VIEW_CHUNK(i, j, k));
                }
            }
        }
    }

    saveOctree(modifiedChunks);

    writeElement(&seed, sizeof(uint32_t));
}

void loadWorld(SaveVersionCompat svc)
{
    readElement(&chunkPos, sizeof(ChunkPos));

    if(modifiedChunks != NULL)
    {
        freeOctree(modifiedChunks);
    }

    modifiedChunks = loadOctree(svc);

    //Load chunks in
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                //Destroy existing world
                destroyChunk(VIEW_CHUNK(i, j, k));
                free(VIEW_CHUNK(i, j, k));
                
                //Load chunk from octree or generate if not present
                ChunkPos pos = {chunkPos.x + i, chunkPos.y + j, chunkPos.z + k};
                Chunk* chunk = findOctree(modifiedChunks, &pos);
                if(chunk != NULL)
                {
                    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
                    VIEW_CHUNK(i, j, k) = chunk;
                }
                else
                {
                    VIEW_CHUNK(i, j, k) = calloc(1, sizeof(Chunk));
                    VIEW_CHUNK(i, j, k)->position = pos;
                    generateChunk(VIEW_CHUNK(i, j, k));
                }
            }
        }
    }
    //TODO: this is the same code as in initWorld(), find some way to combine it?
    //Propagate structure data
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                propagateChunkStructureData(VIEW_CHUNK(i, j, k), chunks, VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE);
            }
        }
    }
    //Generate structures
    //If there are "source" structures outside the current view distance, the chunks within view distance won't be populated with structures (as in, will be missing parts)
    //This is pretty unavoidable though, and also not really a problem
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                generateChunkStructures(VIEW_CHUNK(i, j, k));
            }
        }
    }

    if(svc == SV_COMPAT_OK)
    {
        readElement(&seed, sizeof(uint32_t));
    }
    else if(svc == SV_COMPAT_MINOR)
    {
        seed = 0;
    }
    //Reinit worldgen with the new seed
    //This function allocates no memory so cleanup of the old worldgen isn't necessary
    initWorldgen(seed);
}