#include "input.h"

uint8_t keysLast[NUM_KEYS];
uint8_t keys[NUM_KEYS];

uint8_t handleInput()
{
    uint8_t running = 1;

    for(uint8_t i = 0; i < NUM_KEYS; i++)
    {
        keysLast[i] = keys[i];
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            running = 0;
        }
        else if(event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        {
            continue;
        }

        uint8_t state = 0;
        if(event.type == SDL_KEYDOWN)
        {
            state = 1;
        }
        switch(event.key.keysym.sym)
        {
            #ifdef FUNKEY
            case SDLK_u:
            {
                keys[B_UP] = state;
                break;
            }
            case SDLK_d:
            {
                keys[B_DOWN] = state;
                break;
            }
            case SDLK_l:
            {
                keys[B_LEFT] = state;
                break;
            }
            case SDLK_r:
            {
                keys[B_RIGHT] = state;
                break;
            }
            case SDLK_a:
            {
                keys[B_A] = state;
                break;
            }
            case SDLK_b:
            {
                keys[B_B] = state;
                break;
            }
            case SDLK_x:
            {
                keys[B_X] = state;
                break;
            }
            case SDLK_y:
            {
                keys[B_Y] = state;
                break;
            }
            case SDLK_m:
            {
                keys[B_TL] = state;
                break;
            }
            case SDLK_n:
            {
                keys[B_TR] = state;
                break;
            }
            case SDLK_q:
            {
                running = 0;
                break;
            }
            case SDLK_k:
            {
                keys[B_SELECT] = state;
                break;
            }
            case SDLK_s:
            {
                keys[B_START] = state;
                break;
            }
            #else
            case SDLK_UP:
            {
                keys[B_UP] = state;
                break;
            }
            case SDLK_DOWN:
            {
                keys[B_DOWN] = state;
                break;
            }
            case SDLK_LEFT:
            {
                keys[B_LEFT] = state;
                break;
            }
            case SDLK_RIGHT:
            {
                keys[B_RIGHT] = state;
                break;
            }
            case SDLK_a:
            {
                keys[B_A] = state;
                break;
            }
            case SDLK_s:
            {
                keys[B_B] = state;
                break;
            }
            case SDLK_x:
            {
                keys[B_X] = state;
                break;
            }
            case SDLK_y:
            {
                keys[B_Y] = state;
                break;
            }
            case SDLK_u:
            {
                keys[B_TL] = state;
                break;
            }
            case SDLK_i:
            {
                keys[B_TR] = state;
                break;
            }
            case SDLK_q:
            {
                running = 0;
                break;
            }
            case SDLK_n:
            {
                keys[B_SELECT] = state;
                break;
            }
            case SDLK_m:
            {
                keys[B_START] = state;
                break;
            }
            #endif
        }
    }
    return running;
}

uint8_t keyPressed(Key key)
{
    return keys[key];
}

uint8_t keyUp(Key key)
{
    if(keysLast[key] == 1 && keys[key] == 0)
    {
        return 1;
    }
    return 0;
}