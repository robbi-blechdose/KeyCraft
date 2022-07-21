#include "input.h"

bool keysLast[NUM_KEYS];
bool keys[NUM_KEYS];

bool handleInput()
{
    bool running = true;

    for(uint8_t i = 0; i < NUM_KEYS; i++)
    {
        keysLast[i] = keys[i];
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            running = false;
        }
        else if(event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        {
            continue;
        }

        bool state = false;
        if(event.type == SDL_KEYDOWN)
        {
            state = true;
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
                keys[B_MENU] = state;
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
            default:
            {
                break;
            }
        }
    }
    return running;
}

bool keyPressed(Key key)
{
    return keys[key];
}

bool keyUp(Key key)
{
    if(keysLast[key] == 1 && keys[key] == 0)
    {
        return 1;
    }
    return 0;
}