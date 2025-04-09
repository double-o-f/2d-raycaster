#ifndef HEADER_RC_UI_INCLUDED
#define HEADER_RC_UI_INCLUDED

#include <stdbool.h>


typedef struct {
    bool showMenu;
    bool showMap;
    bool showGame;
} UI_ui_;

extern UI_ui_ UI_ui;


void UI_init();

#endif