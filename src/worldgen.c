#include "worldgen.h"

#include <stdlib.h>

#define FNL_IMPL
#include "engine/includes/FastNoiseLite.h"
#include "engine/util.h"

fnl_state noise;

void initWorldgen(uint32_t seed)
{
    //Init FastNoiseListe
    noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.fractal_type = FNL_FRACTAL_NONE;
    noise.seed = seed;
    //Init random
    srand(seed);
}

void generateChunk(Chunk* chunk)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t k = 0; k < CHUNK_SIZE; k++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 8 * 5
            uint8_t height = (fnlGetNoise2D(&noise, (x * CHUNK_SIZE + i) * 4, (z * CHUNK_SIZE + k) * 4) + 1) * 10;

            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                uint8_t pos = j + (y * CHUNK_SIZE);

                if(pos == height && randr(128) == 0)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
                }
                else if(pos == height - 1)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_GRASS;
                }
                else if(pos < height && pos >= height / 2)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
                }
                else if(pos < height / 2)
                {
                    if(pos == 0)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_BEDROCK;
                    }
                    else
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_STONE;
                    }
                }
            }
        }
    }
    chunk->modified = 1;
}