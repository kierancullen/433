#include <stdio.h>
#include "pico/stdlib.h"
#include "font.h"
#include "ssd1306.h"

#define MAX_X 127

//this clears the entire area of each letter
void drawChar(uint8_t index, uint8_t x, uint8_t y) {
    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 8; row++) {
            ssd1306_drawPixel(x+col, y+row, (ASCII[index][col]>>row & 0b1 == 1));
        }
    }
}

void drawMessage(char* message, uint8_t x, uint8_t y, uint8_t spacingPx, uint8_t lineSpacingPx) {
    int i = 0;
    uint8_t realX = x;
    uint8_t realY = y;
    while (1) {
        char current = message[i];
        if (current != '\0' && current != 0) {
            //printf("the current char to be drawn is %c \r\n", current);
            if (realX+4 > MAX_X) { //the letter would go off the screen
                realX = x; //return to the beginning of the line
                realY += 8+lineSpacingPx;
            } 
            drawChar(current-32, realX, realY);
            i++;
            realX += 5+spacingPx;
        }
        else {
            break;
        }
    }
}