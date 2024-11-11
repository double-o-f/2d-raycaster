#ifndef HEADER_IN_INCLUDED
#define HEADER_IN_INCLUDED

#include <stdbool.h> //bool
#include <stdint.h> //uint8_t
#include <string.h> //memcmp

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include "PL_player.h"
#include "MP_map.h"
#include "RD_rend.h"
#include "WI_window.h"
#include "ST_state.h"
#include "UI_ui.h"


struct IN_input_{
    SDL_Event event;
    
    int mouseX;
    int mouseY;
    bool mouseLastST[5];

    const uint8_t* KBState;
    const uint8_t* KBLastState;
    int KBLen;
};
extern struct IN_input_ IN_input;


bool IN_MBJustPressed(int mb);

bool IN_keyJustPressed(int key);

void IN_mouseUpdate();

void IN_KBUpdate();


void IN_allActions();

void IN_mapActions();

void IN_gameActions();

void IN_menuActions();


void IN_allKBActions();

void IN_mapKBActions();

void IN_gameKBActions();

void IN_menuKBActions();


void IN_checkInputs();


void IN_init();

#endif
