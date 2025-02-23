#ifndef POPUP_H
#define POPUP_H

#include <stdbool.h>
#include <stdint.h>

void drawPopupIfActive();
void createPopup(char* text);
void closePopup();
bool isPopupOpen();

#endif