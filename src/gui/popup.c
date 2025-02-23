#include "popup.h"

#include <string.h>

#include "../fk-engine-core/text.h"

bool popupActive;
char popupText[15 * 6];

void drawPopupIfActive()
{
    if(!popupActive)
    {
        return;
    }

    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("!", 48 + 2, 72 + 2, TEXT_BLACK);
    glDrawText("!", 48, 72, TEXT_WHITE);
    glTextSize(GL_TEXT_SIZE8x8);

    glDrawText(popupText, 72 + 1, 72 + 1, TEXT_BLACK);
    glDrawText(popupText, 72, 72, TEXT_WHITE);
}

void createPopup(char* text)
{
    popupActive = true;
    strcpy(popupText, text);
}

void closePopup()
{
    popupActive = false;
}

bool isPopupOpen()
{
    return popupActive;
}