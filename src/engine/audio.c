#include "audio.h"

Mix_Music** music;
uint8_t numMusic;
Mix_Chunk** samples;
uint8_t numSamples;
uint8_t sampleIndex;

void initAudio(uint8_t volume, uint8_t nm, uint8_t ns)
{
    numMusic = nm;
    numSamples = ns;

    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_AllocateChannels(NUM_MIX_CHANNELS);
    Mix_VolumeMusic(volume);
    Mix_Volume(-1, volume);

    music = malloc(sizeof(Mix_Music*) * numMusic);
    for(uint8_t i = 0; i < numMusic; i++)
    {
        music[i] = NULL;
    }

    samples = malloc(sizeof(Mix_Chunk*) * numSamples);
    for(uint8_t i = 0; i < numSamples; i++)
    {
        samples[i] = NULL;
    }
    sampleIndex = 0;
}

void quitAudio()
{
    for(uint8_t i = 0; i < numMusic; i++)
    {
        if(music[i] != NULL)
        {
            Mix_FreeMusic(music[i]);
        }
    }
    free(music);

    for(uint8_t i = 0; i < numSamples; i++)
    {
        if(samples[i] != NULL)
        {
            Mix_FreeChunk(samples[i]);
        }
    }
    free(samples);

    Mix_CloseAudio();
    Mix_Quit();
}

//Music functions

void loadMusic(uint8_t index, char* filename)
{
    if(music[index] != NULL)
    {
        Mix_FreeMusic(music[index]);
    }
    music[index] = Mix_LoadMUS(filename);
}

void playMusic(uint8_t index, uint16_t fadeIn)
{
    if(fadeIn)
    {
        Mix_FadeInMusic(music[index], -1, fadeIn);
    }
    else
    {
        Mix_PlayMusic(music[index], -1);
    }
}

void stopMusic(uint16_t fadeOut)
{
    if(fadeOut)
    {
        Mix_FadeOutMusic(fadeOut);
    }
    else
    {
        Mix_HaltMusic();
    }
}

//Sample functions

uint8_t loadSample(char* filename)
{
    samples[sampleIndex] = Mix_LoadWAV(filename);
    return sampleIndex++;
}

void loadSampleIndex(uint8_t index, char* filename)
{
    samples[index] = Mix_LoadWAV(filename);
}

void playSample(uint8_t index)
{
    Mix_PlayChannel(-1, samples[index], 0);
}