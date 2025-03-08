#ifndef WORLD_NOISE_H
#define WORLD_NOISE_H

#include <stdint.h>

void initWorldNoise(uint32_t seed);

/**
 * Sets a position in the X-Z plane to be used for all noise functions
 * Should be called before any getNoise() calls - unless you're sure a previous value is still valid
 */
void setNoisePosition(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z);

float getNoiseRandScale(float y, float scale);

/**
 * Noise-based RNG - this means all values are reproducible based on the initial seed
 * @param y Vertical coordinate for the 3d noise
 * @return a random value
 */
float getNoiseRand(float y);

float getNoiseBiome();
float getNoiseTerrain();

#endif