#include "UI.h"

UI::UI(int numberOfButtons, int buttonSize, Color colour, Color gridColour, Color hoverColour)
{
    this->numberOfButtons = numberOfButtons;
    this->buttonSize = buttonSize;
    this->colour = colour;
    this->gridColour = gridColour;
    this->hoverColour = hoverColour;
}

void UI::draw(int initialX, int initialY)
{
    DrawRectangle(initialX, initialY, buttonSize, (buttonSize * numberOfButtons), colour);
}

void UI::drawButton(Texture2D icon, int buttonIndex, Color tint)
{
    DrawTexture(icon, 0, (buttonIndex * buttonSize), tint);
}

void UI::drawGrid(int initialX, int initialY)
{
    for (int i = 0; i < numberOfButtons; i++)
    {
        int y = initialY + i * buttonSize;
        DrawRectangleLines(initialX, y, buttonSize, buttonSize, gridColour); //Draws button outlines
    }      
}

void UI::drawHoverColour(int hoverIndex)
{
    if (hoverIndex < 3 || hoverIndex > 11)
    {
        return;
    }

    int y = hoverIndex * buttonSize;

    DrawRectangleLines(0, y, buttonSize, buttonSize, hoverColour);
}

int UI::getButton(Vector2 position, int initialX, int initialY)
{
    float localX = position.x - initialX;
    float localY = position.y - initialY;

    if (localX < 0 || localX > buttonSize || localY < (buttonSize * 4) || localY > (buttonSize * 12))
    {
        return -1;
    }

    int currentButton = (localY / buttonSize);
    return currentButton;
}

Texture2D UI::loadButton(string filename)
{
    Image image = LoadImage(filename.c_str());
    ImageResize(&image, buttonSize, buttonSize);
    Texture2D icon = LoadTextureFromImage(image);
    UnloadImage(image);

    return icon;
}