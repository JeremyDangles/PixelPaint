#include "canvas.h"
#include "UI.h"

#include <iostream>
#include <raylib.h>
#include <vector>
#include <stack>

using namespace std;

void clearCanvas(Canvas& currentCanvas);
bool coloursAreEqual(Color firstColour, Color secondColour);
void exportToPNG(Canvas currentCanvas, pair<int, int> currentCell);
string GetColorName(Color currentColour);
void handleUserInput(UI currentUI, Canvas& currentCanvas, pair <int, int> currentCell, Vector2 mousePosition);
void paintBucket(Canvas& currentCanvas, pair<int, int> currentCell, Color newColour);

Color UIHoverColour = WHITE;
Color gridColour = RED;
Color selectedColour = BLACK;
Color transparentColour = {0, 0, 0, 0};
Color backgroundColour = {32, 32, 32, 255};
Color UIColour = {24, 24, 24, 255};
Color UIGridColour = {43, 43, 43, 255};

int screenWidth =  512;
int screenHeight = 512;
int cellSize = 16;

int UIWidth = 32;
int UIHeight = screenHeight;
int UIGridSize = screenHeight / 32;

bool pencilSelected = true;
bool paintBucketSelected = false;
bool downloadSelected = false;
bool clearCanvasSelected = false;

bool showButtonFeedback = false;
float flashStartTime = 0.0f;
const float BUTTON_DISPLAY_TIME = 0.15;

int main() 
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "PixelPaint :)");

    int columns = screenWidth / cellSize; 
    int rows = screenHeight / cellSize;

    vector<vector<Color>> gridColours(rows, vector<Color>(columns));

    int canvasWidth = 256;
    int canvasHeight = 256;

    Canvas currentCanvas(canvasWidth, canvasHeight, LIGHTGRAY, cellSize);
    currentCanvas.createGrid();

    UI currentUI(UIWidth, UIHeight, UIGridSize, UIColour, UIGridColour, UIHoverColour);

    Texture2D pencilIconIdle = currentUI.loadButton("pencilIcon_Idle.png", cellSize);
    Texture2D paintBucketIconIdle = currentUI.loadButton("paintBucketIcon_Idle.png", cellSize);
    Texture2D downloadIconIdle = currentUI.loadButton("downloadIcon_Idle.png", cellSize);
    Texture2D clearIconIdle = currentUI.loadButton("clearIcon_Idle.png", cellSize);

    Texture2D pencilIconActive = currentUI.loadButton("pencilIcon_Active.png", cellSize);
    Texture2D paintBucketIconActive = currentUI.loadButton("paintBucketIcon_Active.png", cellSize);
    Texture2D downloadIconActive = currentUI.loadButton("downloadIcon_Active.png", cellSize);
    Texture2D clearIconActive = currentUI.loadButton("clearIcon_Active.png", cellSize);

    while(!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(backgroundColour);

            int xOffset = screenWidth / 4;
            int yOffset = screenHeight / 4;

            currentUI.draw();
            currentUI.drawGrid();

            Vector2 mousePosition = GetMousePosition();
            pair <int, int> currentCell = currentCanvas.getCellCoordinates(mousePosition, xOffset, yOffset);
            handleUserInput(currentUI, currentCanvas, currentCell, mousePosition);

            if (pencilSelected)
            {
                currentUI.drawButton(pencilIconActive, 8, cellSize);
            }
            else
            {
                currentUI.drawButton(pencilIconIdle, 8, cellSize);
            }

            if (paintBucketSelected)
            {
                currentUI.drawButton(paintBucketIconActive, 9, cellSize);
            }
            else
            {
                currentUI.drawButton(paintBucketIconIdle, 9, cellSize);
            }

            if (clearCanvasSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
            {
                currentUI.drawButton(clearIconActive, 14, cellSize);
            }
            else
            {
                currentUI.drawButton(clearIconIdle, 14, cellSize);
                clearCanvasSelected = false;
            }

            if (downloadSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
            {
                currentUI.drawButton(downloadIconActive, 15, cellSize);
            }
            else
            {
                currentUI.drawButton(downloadIconIdle, 15, cellSize);
                downloadSelected = false;
            }
            
            currentCanvas.drawCanvas(xOffset, yOffset);
            currentCanvas.updateCanvasGridColours(xOffset, yOffset);

            DrawFPS(screenWidth - 150, screenHeight - 50);

        EndDrawing();
    }

    UnloadTexture(pencilIconIdle);
    UnloadTexture(paintBucketIconIdle);
    UnloadTexture(downloadIconIdle);
    UnloadTexture(clearIconIdle);

    UnloadTexture(pencilIconActive);
    UnloadTexture(paintBucketIconActive);
    UnloadTexture(downloadIconActive);
    UnloadTexture(clearIconActive);

    CloseWindow();
    return 0;
}

void handleUserInput(UI currentUI, Canvas& currentCanvas, pair <int, int> currentCell, Vector2 mousePosition)
{
    currentUI.drawHoverColour(currentUI.getButton(mousePosition));
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (currentCanvas.isWithinCanvas(currentCell))
            {
                if (paintBucketSelected)
                {
                    paintBucket(currentCanvas, currentCell, selectedColour);
                }
                else
                {
                    currentCanvas.setCellColour(currentCell, selectedColour);
                }
            }
        }

        vector<Color> colourButtons = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, WHITE, BLACK };
        int buttonIndex = currentUI.getButton(mousePosition);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            if (buttonIndex >= 0 && buttonIndex < colourButtons.size()) // CHOOSE COLOUR 
            {
                selectedColour = colourButtons[buttonIndex];
            }
            else if (buttonIndex == 8) // PENCIL TOOL
            {
                pencilSelected = true;
                paintBucketSelected = false;
            }
            else if (buttonIndex == 9) // PAINT BUCKET TOOL
            {
                pencilSelected = false;
                paintBucketSelected = true;
            }
            else if (buttonIndex ==14) // CLEAR
            {
                clearCanvas(currentCanvas);
                clearCanvasSelected = true;
                flashStartTime = GetTime();
            }
            else if (buttonIndex == 15) //EXPORT
            {                
                exportToPNG(currentCanvas, currentCell);
                downloadSelected = true;    
                flashStartTime = GetTime();
            }
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            if (currentCanvas.isWithinCanvas(currentCell))
            {
                currentCanvas.setCellColour(currentCell, transparentColour);
            }
        }
}

void clearCanvas(Canvas& currentCanvas)
{
    int rows = currentCanvas.getHeight() / cellSize;
    int columns = currentCanvas.getWidth() / cellSize;

    for (int row = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++)
        {
            currentCanvas.setCellColour({row, column}, transparentColour);
        }
    }
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
    showButtonFeedback = true;

    int columns = currentCanvas.getWidth() / cellSize;
    int rows = currentCanvas.getHeight() / cellSize;

    RenderTexture2D canvas = LoadRenderTexture(currentCanvas.getWidth(),  currentCanvas.getHeight());

    BeginTextureMode(canvas);
        ClearBackground(transparentColour);
        for (int row = 0; row < rows; row++)
        {
            for (int column = 0; column < columns; column++)
            {
                pair <int, int> cell = {row, column};
                bool isInsideCanvas = currentCanvas.isWithinCanvas(cell);
                Color colour = currentCanvas.getCellColour(cell, isInsideCanvas);
                DrawRectangle(column * cellSize, row * cellSize, cellSize, cellSize, colour);
            }
        }
    EndTextureMode();

    Image image = LoadImageFromTexture(canvas.texture);
    ImageFlipVertical(&image);
    ExportImage(image, "Masterpiece.png");
    
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

void paintBucket(Canvas& currentCanvas, pair<int, int> currentCell, Color newColour)
{
    if (!currentCanvas.isWithinCanvas(currentCell))
    {
        return;
    }

    Color initialColour = currentCanvas.getCellColour(currentCell, currentCanvas.isWithinCanvas(currentCell));
    if (coloursAreEqual(initialColour, newColour))
    {
        return;
    }

    stack<pair<int, int>> toFill;
    toFill.push(currentCell);

    while (!toFill.empty())
    {
        pair<int, int> cell = toFill.top();
        toFill.pop();

        if (!currentCanvas.isWithinCanvas(cell))
        {
            continue;
        }

        Color currentColour = currentCanvas.getCellColour(cell, currentCanvas.isWithinCanvas(cell));
        if (!coloursAreEqual(currentColour, initialColour))
        {
            continue;
        }

       currentCanvas.setCellColour(cell, newColour);

        toFill.push({ cell.first + 1, cell.second });
        toFill.push({ cell.first - 1, cell.second });
        toFill.push({ cell.first, cell.second + 1 });
        toFill.push({ cell.first, cell.second - 1} );
    }
}