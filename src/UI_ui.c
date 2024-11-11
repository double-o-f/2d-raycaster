#include <stdbool.h>

#include "UI_ui.h"
struct UI_ui_ UI_ui;


void UI_init() {
    UI_ui.showMenu = false;
    UI_ui.showMap = false;
    UI_ui.showGame = true;
}
