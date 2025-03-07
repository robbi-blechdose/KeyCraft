#include "worldnoise.h"

#define FNL_IMPL
#include "../fk-engine-core/includes/FastNoiseLite.h"

#include "../chunk.h"

fnl_state terrainNoise;
fnl_state randNoise;
fnl_state biomeNoise;

//Evil evil globals (but if you think object-oriented and this file is a single instance of a class created by a factory it's okay!)
float noiseX;
float noiseZ;

void initWorldNoise(uint32_t seed)
{
    //Init FastNoiseLite (terrain)
    terrainNoise = fnlCreateState();
    terrainNoise.noise_type = FNL_NOISE_PERLIN;
    terrainNoise.fractal_type = FNL_FRACTAL_NONE;
    terrainNoise.seed = seed;
    //Init FastNoiseLite (random)
    randNoise = fnlCreateState();
    randNoise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    randNoise.fractal_type = FNL_FRACTAL_NONE;
    randNoise.frequency = 0.1f;
    randNoise.seed = seed;
    //Init FastNoiseLite (biome)
    biomeNoise = fnlCreateState();
    biomeNoise.noise_type = FNL_NOISE_CELLULAR;
    biomeNoise.fractal_type = FNL_FRACTAL_NONE;
    biomeNoise.cellular_distance_func = FNL_CELLULAR_DISTANCE_HYBRID;
    biomeNoise.cellular_return_type = FNL_CELLULAR_RETURN_TYPE_CELLVALUE;
    biomeNoise.frequency = 0.03f;
    biomeNoise.seed = seed;
}

void setNoisePosition(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    noiseX = (chunkX * CHUNK_SIZE + x);
    noiseZ = (chunkZ * CHUNK_SIZE + z);
}

float getNoiseRandScale(float y, float scale)
{
    //Scale noise to be within 0 to 1
    return (fnlGetNoise3D(&randNoise, noiseX * scale, y, noiseZ * scale) + 1) / 2;
}

float getNoiseRand(float y)
{
    return getNoiseRandScale(y, 5);
}

float getNoiseBiome()
{
    return (fnlGetNoise2D(&biomeNoise, noiseX, noiseZ) + 1) / 2;
}

float getNoiseTerrain()
{
    //Noise value (-1 to 1) is scaled to be within 0 to 1
    return (fnlGetNoise2D(&terrainNoise, noiseX * 4, noiseZ * 4) + 1) / 2;
}