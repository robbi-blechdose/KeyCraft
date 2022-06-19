#include "world.h"

#include "engine/image.h"
#include "engine/savegame.h"

#include "chunk.h"
#include "worldgen.h"
#include "blocks/blocklogic.h"
#include "blocks/blockactions.h"
#include "octree.h"

#define VIEW_CHUNK(i, j, k) chunks[(i) + ((j) * VIEW_DISTANCE) + ((k) * VIEW_DISTANCE * VIEW_DISTANCE)]
#define WORLD_CHUNK(i, j, k) VIEW_CHUNK((i) - chunkPos.x, (j) - chunkPos.y, (k) - chunkPos.z)

ChunkPos chunkPos;
Chunk* chunks[VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE];
Octree* modifiedChunks;

GLuint terrainTexture;

uint32_t worldTicks;

void initWorld()
{
    initWorldgen(0);

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
    chunkPos = (ChunkPos) {0, 0, 0};

    modifiedChunks = createOctree((vec3) {0, 0, 0}, INT16_MAX * 2, NULL);

    worldTicks = 0;

    terrainTexture = loadRGBTexture("res/tex/terrain.png");
    glBindTexture(GL_TEXTURE_2D, terrainTexture);
}

void quitWorld()
{
    deleteRGBTexture(terrainTexture);
    freeOctree(modifiedChunks);
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
            if(toDestroy->initial)
            {
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
                newChunk->modified = 1;
            }
            
            if(side == SWP_FB)
            {
                VIEW_CHUNK(in, i, j) = newChunk;
            }
            else if(side == SWP_LR)
            {
                VIEW_CHUNK(i, j, in) = newChunk;
            }
            else
            {
                VIEW_CHUNK(i, j, in) = newChunk;
            }
        }
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

    //printf("P: %d %d %d\n", playerChunkPos.x, playerChunkPos.y, playerChunkPos.z);
    //printf("W: %d %d %d\n", chunkPos.x, chunkPos.y, chunkPos.z);

    //Calculate visible chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                calcChunk(VIEW_CHUNK(i, j, k));
            }
        }
    }

    //Tick visible chunks
    worldTicks += ticks;
    if(worldTicks > TICK_RATE)
    {
        worldTicks = 0;

        for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
        {
            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
                {
                    if(!VIEW_CHUNK(i, j, k)->isEmpty)
                    {
                        tickChunk(VIEW_CHUNK(i, j, k));
                    }
                }
            }
        }
    }
}

//TODO: Skip chunks left or right of the screen player as well
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
            //chunkCenterRot.x = chunkCenter.x * cosf(-playerRotation->y + M_PI) - chunkCenter.z * sinf(-playerRotation->y + M_PI);
            chunkCenterRot.z = chunkCenter.x * sinf(-playerRotation->y + M_PI) + chunkCenter.z * cosf(-playerRotation->y + M_PI);
            //vec3 chunkCenterRot = rotatev3(chunkCenter, (vec3) {.x = 0, .y = 1, .z = 0}, playerRotation->y - M_PI);

            //Skip drawing chunks behind the camera
            if(chunkCenterRot.z < -CHUNK_SIZE)
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
                //Discard empty chunks
                if(!VIEW_CHUNK(i, j, k)->isEmpty)
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
    chunk->modified = 1;
    //Mark chunk as player-modified
    chunk->initial = 0;

    //Mark adjacent chunks as modified if necessary
    if(pos->x == 0 && pos->chunk.x >= chunkPos.x)
    {
        WORLD_CHUNK(pos->chunk.x - 1, pos->chunk.y, pos->chunk.z)->modified = 1;
    }
    else if(pos->x == CHUNK_SIZE - 1 && pos->chunk.x < chunkPos.x + VIEW_DISTANCE)
    {
        WORLD_CHUNK(pos->chunk.x + 1, pos->chunk.y, pos->chunk.z)->modified = 1;
    }
    if(pos->y == 0 && pos->chunk.y >= chunkPos.y)
    {
        WORLD_CHUNK(pos->chunk.x, pos->chunk.y - 1, pos->chunk.z)->modified = 1;
    }
    else if(pos->y == CHUNK_SIZE - 1 && pos->chunk.y < chunkPos.y + VIEW_DISTANCE)
    {
        WORLD_CHUNK(pos->chunk.x, pos->chunk.y + 1, pos->chunk.z)->modified = 1;
    }
    if(pos->z == 0 && pos->chunk.z >= chunkPos.z)
    {
        WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z - 1)->modified = 1;
    }
    else if(pos->z == CHUNK_SIZE - 1 && pos->chunk.z < chunkPos.z + VIEW_DISTANCE)
    {
        WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z + 1)->modified = 1;
    }

    //Place door upper (yes, this is a special case)
    if(block.type == BLOCK_DOOR && !(block.data & BLOCK_DATA_PART))
    {
        pos->y++;
        setWorldBlock(pos, (Block) {BLOCK_DOOR, block.data + BLOCK_DATA_PART});
        pos->y--;
    }
}

uint8_t actWorldBlock(BlockPos* pos)
{
    Block* block = getWorldBlock(pos);
    if(block == NULL)
    {
        return 0;
    }

    Chunk* chunk = WORLD_CHUNK(pos->chunk.x, pos->chunk.y, pos->chunk.z);
    uint8_t ret = actBlock(chunk, block);

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
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                //Discard empty chunks
                if(!VIEW_CHUNK(i, j, k)->isEmpty)
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

uint8_t intersectsAABBWorld(AABB* aabb)
{
    //Exclude outer chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                //Discard empty chunks
                if(!VIEW_CHUNK(i, j, k)->isEmpty)
                {
                    if(intersectsAABBChunk(VIEW_CHUNK(i, j, k), aabb))
                    {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
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
                if(!VIEW_CHUNK(i, j, k)->initial)
                {
                    insertOctree(modifiedChunks, VIEW_CHUNK(i, j, k));
                }
            }
        }
    }

    //Don't save anything if we didn't modify anything
    if(octreeEmpty(modifiedChunks))
    {
        return;
    }

    saveOctree(modifiedChunks);
}

void loadWorld()
{
    readElement(&chunkPos, sizeof(ChunkPos));

    if(modifiedChunks != NULL)
    {
        freeOctree(modifiedChunks);
    }

    modifiedChunks = loadOctree();

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
}