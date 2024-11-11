#ifndef HEADER_UI_INCLUDED
#define HEADER_UI_INCLUDED

#include <stdbool.h>


struct UI_ui_ {
    bool showMenu;
    bool showMap;
    bool showGame;
};
extern struct UI_ui_ UI_ui;


void UI_init();

#endif