#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <utility>
#include <vector>
#include <string>

using namespace std;

class UI
{
    private:
        int width = 0;
        int height = 0;
        int gridSize = 0;
        Color colour = DARKGRAY;
        Color gridColour = BLACK;
        Color hoverColour = WHITE;
        
    public:
        UI(int width, int height, int gridSize, Color colour, Color gridColour, Color hoverColour);

        void draw();
        void drawButton(Texture2D icon, int buttonIndex, int cellSize, Color tint);
        void drawGrid();
        void drawHoverColour(int hoverIndex);
        int getButton(Vector2 position);
        Texture2D loadButton(string filename, int cellSize);
};

#endif // UI.h