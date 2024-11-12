#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <stdint.h> //uint8_t
#include <string.h> //memcpy

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

#include "IN_input.h"
struct IN_input_ IN_input;


bool IN_MBJustPressed(int mb) {
    if (IN_input.event.button.button == mb) {
        if (IN_input.mouseLastST[mb] == false) {
            return true;
            IN_input.mouseLastST[mb] = true;
        }
    }
    else {
        IN_input.mouseLastST[mb] = false;
    }
    return false;
}

bool IN_keyJustPressed(int key) {
    if (IN_input.KBState[key] == 1) {
        if (IN_input.KBLastState[key] == 0) {
            IN_input.KBLastState[key] = 1;
            return true;
        }
    }
    else {
        IN_input.KBLastState[key] = 0;
    }
    return false;
}


void IN_checkMovement() {
    if (IN_input.KBState[SDL_SCANCODE_UP] || IN_input.KBState[SDL_SCANCODE_W]) {
        PL_forward(PL_player.accel);
    }
    if (IN_input.KBState[SDL_SCANCODE_DOWN] || IN_input.KBState[SDL_SCANCODE_S]) {
        PL_backward(PL_player.accel);
    }
    if (IN_input.KBState[SDL_SCANCODE_A]) {
        PL_left(PL_player.accel);
    }
    if (IN_input.KBState[SDL_SCANCODE_D]) {
        PL_right(PL_player.accel);
    }

    if (IN_input.KBState[SDL_SCANCODE_LEFT]) {
        PL_rotate(PL_player.rSpeed * -1);  
    }
    if (IN_input.KBState[SDL_SCANCODE_RIGHT]) {
        PL_rotate(PL_player.rSpeed);  
    }
}


void IN_allActions() {
    if (IN_input.event.type == SDL_QUIT) {
        ST_state.quit = true;
    }
}

void IN_mapActions() {
    if (IN_input.event.type == SDL_MOUSEBUTTONDOWN) {
        if (IN_MBJustPressed(SDL_BUTTON(SDL_BUTTON_LEFT))) {
            int mapX = (IN_input.mouseX * MP_map.width) / WI_window.windowWidth;
            int mapY = (IN_input.mouseY * MP_map.height) / WI_window.windowHeight;

            MP_changeWall(mapX, mapY);
        }
            
        if (IN_input.event.button.button == SDL_BUTTON(SDL_BUTTON_RIGHT)) {}    
    }
}

void IN_gameActions() {
    if (IN_input.event.type == SDL_MOUSEMOTION) {
        PL_mouseRotate(IN_input.event.motion.xrel);
    }


}

void IN_menuActions() {

}


void IN_allKBActions() {
    if (IN_keyJustPressed(SDL_SCANCODE_M)) {
        MP_saveMap();    
    }
}

void IN_mapKBActions() {
    IN_checkMovement();

    if (IN_keyJustPressed(SDL_SCANCODE_R)) {
        UI_ui.showMap = false;
        SDL_SetRelativeMouseMode(true);
    }

}

void IN_gameKBActions() {
    IN_checkMovement();

    if (IN_keyJustPressed(SDL_SCANCODE_R)) {
        UI_ui.showMap = true;
        SDL_SetRelativeMouseMode(false);
    }

    if (IN_keyJustPressed(SDL_SCANCODE_F)) {
        if (RD_rend.fishEye) {
            RD_rend.fishEye = false;
        }
        else {
            RD_rend.fishEye = true;
        }
    }
}

void IN_menuKBActions() {

}


void IN_checkInputs() {
    
    SDL_GetMouseState(&IN_input.mouseX, &IN_input.mouseY); //get mouse pos
    while (SDL_PollEvent(&IN_input.event)) {
        IN_allActions();

        if (UI_ui.showMenu) {
            IN_menuActions();
        }
        else if (UI_ui.showMap) {
            IN_mapActions();
        }
        else if (UI_ui.showGame) {
            IN_gameActions();
        }
        
    }


    IN_input.KBState = SDL_GetKeyboardState(NULL); //get keyboard state

    IN_allKBActions();
    if (UI_ui.showMenu) {
        IN_menuKBActions();
    }
    else if (UI_ui.showMap) {
        IN_mapKBActions();
    }
    else if (UI_ui.showGame) {
        IN_gameKBActions();
    }
}


void IN_init() {
    SDL_GetMouseState(&IN_input.mouseX, &IN_input.mouseY);
    SDL_SetRelativeMouseMode(true);

    IN_input.KBState = SDL_GetKeyboardState(&IN_input.KBLen);
    IN_input.KBLen *= sizeof(uint8_t);
    IN_input.KBLastState = (uint8_t*)malloc(IN_input.KBLen);
    memcpy(IN_input.KBLastState, IN_input.KBState, IN_input.KBLen);
}

void IN_destroy() {
    free(IN_input.KBLastState);
}
