#include "canvas.h"
#include <iostream>
#include <raylib.h>
#include <vector>
#include <stack>

using namespace std;

bool coloursAreEqual(Color firstColour, Color secondColour);
void exportToPNG(Canvas currentCanvas, pair<int, int> currentCell);
string GetColorName(Color currentColour);
void handleToolbarClick(int column, Color& selectedColour, bool& paintBucketActive, Vector2 mousePosition, vector<vector<Color>>& gridColours);
void paintBucket(int initialCellColumn, int initialCellRow, Color newColour, vector<vector<Color>>& gridColours, int columns, int rows);
Color setColour(Vector2 mousePosition, int cellSize);

Color UIHoverColour = WHITE;
Color gridColour = RED;
Color selectedColour = BLACK;
Color transparentColour = {0, 0, 0, 0};
Color backgroundColour = {50, 50, 50, 255};
Color UIGridColour = {40, 40, 40, 255};
Color UIColour = DARKGRAY;

int screenWidth =  512;
int screenHeight = 512;
int cellSize = 16;

int UIWidth = 32;
int UIHeight = screenHeight;
int UIGridSize = screenHeight / 32;

class UI
{
    private:
        int width = 0;
        int height = 0;
        int gridSize = 0;
        Color colour = DARKGRAY;
        Color gridColour = UIGridColour;
        Color hoverColour = WHITE;
        
    public:
        UI(int width, int height, int gridSize, Color colour, Color gridColour, Color hoverColour)
        {
            this->width = width;
            this->height = height;
            this->gridSize = gridSize;
            this->colour = colour;
            this->gridColour = gridColour;
            this->hoverColour = hoverColour;
        }

        void draw()
        {
            DrawRectangle(0, 0, width, height, colour);
        }

        void drawGrid()
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

        void drawHoverColour(int hoverIndex)
        {
            if (hoverIndex < 0 || hoverIndex >= gridSize)
            {
                return;
            }

            int cellSize = (height / gridSize);
            int y = hoverIndex * cellSize;

            DrawRectangleLines(0, y, width, cellSize, hoverColour);
        }

        int getButton(Vector2 position)
        {
            if (position.x < 0 || position.x > width || position.y < 0 || position.y > height)
            {
                return -1;
            }

            int cellSize = height / gridSize;
            int currentButton = position.y / cellSize;

            return currentButton;
        }

        void setButtonColour(Color colour)
        {
            int cellSize = (height / gridSize);
            DrawRectangle(0, 0, width, cellSize, colour);
        }
};

int main() 
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "PixelPaint :)");

    int columns = screenWidth / cellSize; 
    int rows = screenHeight / cellSize;
    bool paintBucketActive = false;

    vector<vector<Color>> gridColours(rows, vector<Color>(columns));

    int canvasWidth = 256;
    int canvasHeight = 256;

    Canvas currentCanvas(canvasWidth, canvasHeight, WHITE, cellSize);
    currentCanvas.createGrid();

    UI UI(UIWidth, UIHeight, UIGridSize, UIColour, UIGridColour, UIHoverColour);

    while(!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(backgroundColour);

            int xOffset = screenWidth / 4;
            int yOffset = screenHeight / 4;

            currentCanvas.drawCanvas(xOffset, yOffset);
            UI.draw();

            UI.drawGrid();

            Vector2 mousePosition = GetMousePosition();
            pair <int, int> currentCell = currentCanvas.getCellCoordinates(mousePosition, xOffset, yOffset);
            
            UI.drawHoverColour(UI.getButton(mousePosition));
            cout << UI.getButton(mousePosition) << endl;
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    if (currentCanvas.isWithinCanvas(currentCell))
                    {
                        currentCanvas.setCellColour(currentCell, selectedColour);
                    }
                }

                vector<Color> colourButtons = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, WHITE, BLACK };
                int buttonIndex = UI.getButton(mousePosition);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (buttonIndex >= 0 && buttonIndex < colourButtons.size())
                    {
                        selectedColour = colourButtons[buttonIndex];
                    }

                    if (buttonIndex == 10)
                    {
                        exportToPNG(currentCanvas, currentCell);
                    }
                }

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
                {
                    if (currentCanvas.isWithinCanvas(currentCell))
                    {
                        currentCanvas.setCellColour(currentCell, transparentColour);
                    }
                }

                currentCanvas.drawCanvas(xOffset, yOffset);
                currentCanvas.updateCanvasGridColours(xOffset, yOffset);

                DrawFPS(screenWidth - 150, screenHeight - 50);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

bool coloursAreEqual(Color firstColour, Color secondColour)
{
    return ((firstColour.r == secondColour.r) &&
            (firstColour.g == secondColour.g) &&
            (firstColour.b == secondColour.b) &&
            (firstColour.a == secondColour.a));
}

void exportToPNG(Canvas currentCanvas, pair<int, int> currentCell)
{
    int columns = currentCanvas.getWidth() / cellSize;
    int rows = currentCanvas.getHeight() / cellSize;

    RenderTexture2D canvas = LoadRenderTexture(currentCanvas.getWidth(),  currentCanvas.getHeight());

    BeginTextureMode(canvas);
        ClearBackground(transparentColour);
        for (int row = 0; row < rows; row++)
        {
            for (int column = 0; column < columns; column++)
            {
                pair <int, int> cell = {column, row};
                bool isInsideCanvas = currentCanvas.isWithinCanvas(cell);
                Color colour = currentCanvas.getCellColour(cell, isInsideCanvas);
                DrawRectangle(column * cellSize, (row - 1) * cellSize, cellSize, cellSize, colour);
            }
        }
    EndTextureMode();

    Image image = LoadImageFromTexture(canvas.texture);
    ImageFlipVertical(&image);
    ExportImage(image, "PixelPaint.png");
    
    UnloadImage(image);
    UnloadRenderTexture(canvas);
}

string GetColorName(Color currentColour) 
{
    if (coloursAreEqual(currentColour, RED)) return "RED";
    if (coloursAreEqual(currentColour, ORANGE)) return "ORANGE";
    if (coloursAreEqual(currentColour, YELLOW)) return "YELLOW";
    if (coloursAreEqual(currentColour, GREEN)) return "GREEN";
    if (coloursAreEqual(currentColour, BLUE)) return "BLUE";
    if (coloursAreEqual(currentColour, PURPLE)) return "PURPLE";
    if (coloursAreEqual(currentColour, WHITE)) return "WHITE";
    if (coloursAreEqual(currentColour, BLACK)) return "BLACK";
    
    return "UNKNOWN"; 
}

void handleToolbarClick(int column, Color& selectedColour, bool& paintBucketActive, Vector2 mousePosition, vector<vector<Color>>& gridColours)
{
    if (column == 0)
    {
        //exportToPNG(gridColours);
    }
    if (column == 1)
    {
        paintBucketActive = false;
    }
    else if (column == 2)
    {
        paintBucketActive = true;
    }
    else if (column >= 3 && column <= 10)
    {
        //selectedColour = setColour(mousePosition);
    }
}

void paintBucket(int initialCellColumn, int initialCellRow, Color newColour, vector<vector<Color>>& gridColours, int columns, int rows)
{
    if (initialCellColumn < 0 || initialCellRow < 0 || initialCellColumn >= columns || initialCellRow >= rows)
    {
        return;
    }

    Color targetColour = gridColours[initialCellRow][initialCellColumn];
    if (coloursAreEqual(targetColour, newColour))
    {
        return;
    }

    stack<pair<int, int>> toFill;
    toFill.push({initialCellColumn, initialCellRow});

    while (!toFill.empty())
    {
        pair<int, int> cellCoordinate = toFill.top();
        int column = cellCoordinate.first;
        int row = cellCoordinate.second;
        toFill.pop();

        if (column < 0 || row < 1 || column >= columns || row >= rows)
        {
            continue;
        }
        if (!coloursAreEqual(gridColours[row][column], targetColour))
        {
            continue;
        }

        gridColours[row][column] = newColour;

        toFill.push({ column + 1, row});
        toFill.push({ column - 1, row});
        toFill.push({ column, row + 1});
        toFill.push({ column, row - 1});
    }
}

Color setColour(Vector2 mousePosition, int cellSize)
{
    int cellIndex = (mousePosition.x / cellSize);

    switch (cellIndex)
    {
        case 3: return RED;
        case 4: return ORANGE;
        case 5: return YELLOW;
        case 6: return GREEN;
        case 7: return BLUE;
        case 8: return PURPLE;
        case 9: return BLACK;
        case 10: return WHITE;
        default: return BLACK;
    }   
}