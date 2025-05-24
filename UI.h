#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <utility>
#include <vector>
#include <string>

using namespace std;

class UI
{   
    public:
        struct Button
        {
            Rectangle rectangle;
            Texture2D icon;
            bool hovered = false;
            bool clicked = false;
        };

        UI(int numberOfButtons, int buttonSize, Color colour, Color gridColour, Color hoverColour);

        void draw(int initialX, int initialY);
        void drawButton(Texture2D icon, int buttonIndex, Color tint);
        void drawGrid(int initialX, int initialY);
        void drawHoverColour(int hoverIndex);
        int getButton(Vector2 position, int initialX, int initialY);
        Texture2D loadButton(string filename);

    private:
        int numberOfButtons = 0;
        int buttonSize = 0;
        Color colour = DARKGRAY;
        Color gridColour = BLACK;
        Color hoverColour = WHITE;
        vector<Button> buttons;
};


#endif // UI.h