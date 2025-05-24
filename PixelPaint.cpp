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

void DrawHSVColorPicker(Rectangle rect);
void handleUINavigation(UI currentUI, Texture2D pencilIcon, Texture2D paintBucketIcon, Texture2D colourPickerIcon, 
                        Texture2D colourPickerIconCurrentColour, Texture2D clearIcon, Texture2D downloadIcon);


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
bool colourPickerSelected = false;
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

    Texture2D pencilIcon = currentUI.loadButton("pencilIcon.png", cellSize);
    Texture2D paintBucketIcon = currentUI.loadButton("paintBucketIcon.png", cellSize);
    Texture2D downloadIcon = currentUI.loadButton("downloadIcon.png", cellSize);
    Texture2D clearIcon = currentUI.loadButton("clearIcon.png", cellSize);
    Texture2D colourPickerIcon = currentUI.loadButton("colourPickerIcon.png", cellSize);
    Texture2D colourPickerIconCurrentColour = currentUI.loadButton("colourPickerIconCurrentColour.png", cellSize);

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

            currentCanvas.drawCanvas(xOffset, yOffset);
            currentCanvas.updateCanvasGridColours(xOffset, yOffset);

            handleUINavigation(currentUI, pencilIcon, paintBucketIcon, colourPickerIcon, colourPickerIconCurrentColour, clearIcon, downloadIcon);
            


            DrawFPS(screenWidth - 150, screenHeight - 50);

        EndDrawing();
    }

    UnloadTexture(pencilIcon);
    UnloadTexture(paintBucketIcon);
    UnloadTexture(downloadIcon);
    UnloadTexture(clearIcon);
    UnloadTexture(colourPickerIcon);
    UnloadTexture(colourPickerIconCurrentColour);

    CloseWindow();
    return 0;
}

void handleUINavigation(UI currentUI, Texture2D pencilIcon, Texture2D paintBucketIcon, Texture2D colourPickerIcon, 
                        Texture2D colourPickerIconCurrentColour, Texture2D clearIcon, Texture2D downloadIcon)
{
    if (pencilSelected)
    {
        currentUI.drawButton(pencilIcon, 8, cellSize, WHITE);
    }
    else
    {
        currentUI.drawButton(pencilIcon, 8, cellSize, GRAY);
    }

    if (paintBucketSelected)
    {
        currentUI.drawButton(paintBucketIcon, 9, cellSize, WHITE);
    }
    else
    {
        currentUI.drawButton(paintBucketIcon, 9, cellSize, GRAY);
    }

    if (colourPickerSelected)
    {
        currentUI.drawButton(colourPickerIcon, 10, cellSize, WHITE);
        currentUI.drawButton(colourPickerIconCurrentColour, 10, cellSize, selectedColour);
        DrawRectangleLines(((float)cellSize * 2) + 1, ((float)screenHeight - 100) - 3, 204, 104, UIGridColour);
        DrawHSVColorPicker({((float)cellSize * 2) + 3, ((float)screenHeight - 100) - 1, 200, 100} );
    }
    else
    {
        currentUI.drawButton(colourPickerIcon, 10, cellSize, DARKGRAY);
        currentUI.drawButton(colourPickerIconCurrentColour, 10, cellSize, selectedColour);
    }

    if (clearCanvasSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
    {
        currentUI.drawButton(clearIcon, 14, cellSize, WHITE);
    }
    else
    {
        currentUI.drawButton(clearIcon, 14, cellSize, DARKGRAY);
        clearCanvasSelected = false;
    }

    if (downloadSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
    {
        currentUI.drawButton(downloadIcon, 15, cellSize, WHITE);
    }
    else
    {
        currentUI.drawButton(downloadIcon, 15, cellSize, DARKGRAY);
        downloadSelected = false;
    }

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
                colourPickerSelected = false;
            }
            else if (buttonIndex == 9) // PAINT BUCKET TOOL
            {
                pencilSelected = false;
                paintBucketSelected = true;
                colourPickerSelected = false;
            }
            else if (buttonIndex == 10) // COLOUR PICKER
            {
                if (!colourPickerSelected)
                {
                    colourPickerSelected = true;
                }
                else
                {
                    colourPickerSelected = false;
                    pencilSelected = true;
                }
                pencilSelected = false;
                paintBucketSelected = false;

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

void DrawHSVColorPicker(Rectangle rect) 
{
    for (int x = 0; x < rect.width; ++x) {
        for (int y = 0; y < rect.height; ++y) {
            float hue = (float)x / rect.width * 360.0f;
            float saturation = 1.0f - (float)y / rect.height;
            Color color = ColorFromHSV(hue, saturation, 1.0f);
            DrawPixel((int)rect.x + x, (int)rect.y + y, color);
        }
    }
}




