#include "UI.h"

UI::UI(int width, int height, int gridSize, Color colour, Color gridColour, Color hoverColour)
{
    this->width = width;
    this->height = height;
    this->gridSize = gridSize;
    this->colour = colour;
    this->gridColour = gridColour;
    this->hoverColour = hoverColour;
}

void UI::draw()
{
    DrawRectangle(0, 0, width, height, colour);
}

void UI::drawButton(Texture2D icon, int buttonIndex, int cellSize)
{
    DrawTexture(icon, 0, (buttonIndex * (cellSize * 2)), WHITE);
}

void UI::drawGrid()
{
    int cellSize = (height / gridSize);

    vector<Color> colours = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, WHITE, BLACK };
    int numberOfColours = colours.size();

    for (int y = 0; y < numberOfColours; y++)
    {
        DrawRectangle(0, y * cellSize, width, cellSize, colours[y]); 
    }

    for (int y = 0; y < height; y += cellSize)
    {
        DrawRectangleLines(0, y, width, cellSize, gridColour); //Draws button outlines
    }      
}

void UI::drawHoverColour(int hoverIndex)
{
    if (hoverIndex < 0 || hoverIndex >= gridSize)
    {
        return;
    }

    int cellSize = (height / gridSize);
    int y = hoverIndex * cellSize;

    DrawRectangleLines(0, y, width, cellSize, hoverColour);
}

int UI::getButton(Vector2 position)
{
    if (position.x < 0 || position.x > width || position.y < 0 || position.y > height)
    {
        return -1;
    }

    int cellSize = height / gridSize;
    int currentButton = position.y / cellSize;

    return currentButton;
}

Texture2D UI::loadButton(string filename, int cellSize)
{
    Image image = LoadImage(filename.c_str());
    ImageResize(&image, (cellSize * 2), (cellSize * 2));
    Texture2D icon = LoadTextureFromImage(image);
    UnloadImage(image);

    return icon;
}