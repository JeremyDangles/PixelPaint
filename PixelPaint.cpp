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
string GetColourName(Color currentColour);
void handleUserInput(UI currentUI, Canvas& currentCanvas, pair <int, int> currentCell, Vector2 mousePosition);
void paintBucket(Canvas& currentCanvas, pair<int, int> currentCell, Color newColour);

void DrawHSVColourPicker(Rectangle rect, float valueSlider);
Color GetColourFromHSVPicker(Rectangle rect, Vector2 mousePos, float valueSlider);
void handleUINavigation(UI currentUI, Texture2D pencilIcon, Texture2D paintBucketIcon, Texture2D colourPickerIcon, 
                        Texture2D colourPickerIconCurrentColour, Texture2D clearIcon, Texture2D downloadIcon, Texture2D eraserIcon);

float valueSlider(Rectangle rectangle);


Color UIHoverColour = WHITE;
Color gridColour = RED;
Color selectedColour = WHITE;
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

int UINumberOfButtons = 8;
int UIButtonSize = 32;
int yOffsetUI = UIButtonSize * 4;

bool pencilSelected = true;
bool paintBucketSelected = false;
bool colourPickerSelected = false;
bool downloadSelected = false;
bool clearCanvasSelected = false;
bool eraserSelected = false;

bool showButtonFeedback = false;
float flashStartTime = 0.0f;
const float BUTTON_DISPLAY_TIME = 0.15;

float currentValueSlider = 1.0f;

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

    UI currentUI(UINumberOfButtons, UIButtonSize, UIColour, UIGridColour, UIHoverColour);

    Texture2D pencilIcon = currentUI.loadButton("pencilIcon2.png");
    Texture2D paintBucketIcon = currentUI.loadButton("paintBucketIcon.png");
    Texture2D downloadIcon = currentUI.loadButton("downloadIcon.png");
    Texture2D clearIcon = currentUI.loadButton("clearIcon.png");
    Texture2D colourPickerIcon = currentUI.loadButton("colourPickerIcon.png");
    Texture2D colourPickerIconCurrentColour = currentUI.loadButton("colourPickerIconCurrentColour.png");
    Texture2D eraserIcon = currentUI.loadButton("eraserIcon.png");

    while(!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(backgroundColour);

            int xOffset = screenWidth / 4;
            int yOffset = screenHeight / 4;

            currentUI.draw(0, yOffsetUI);
            currentUI.drawGrid(0, yOffsetUI);

            Vector2 mousePosition = GetMousePosition();
            pair <int, int> currentCell = currentCanvas.getCellCoordinates(mousePosition, xOffset, yOffset);
            handleUserInput(currentUI, currentCanvas, currentCell, mousePosition);

            currentCanvas.drawCanvas(xOffset, yOffset);
            currentCanvas.updateCanvasGridColours(xOffset, yOffset);

            handleUINavigation(currentUI, pencilIcon, paintBucketIcon, colourPickerIcon, colourPickerIconCurrentColour, clearIcon, downloadIcon, eraserIcon);
            
            DrawFPS(screenWidth - 150, screenHeight - 50);

        EndDrawing();
    }

    UnloadTexture(pencilIcon);
    UnloadTexture(paintBucketIcon);
    UnloadTexture(downloadIcon);
    UnloadTexture(clearIcon);
    UnloadTexture(colourPickerIcon);
    UnloadTexture(colourPickerIconCurrentColour);
    UnloadTexture(eraserIcon);

    CloseWindow();
    return 0;
}

void handleUINavigation(UI currentUI, Texture2D pencilIcon, Texture2D paintBucketIcon, Texture2D colourPickerIcon, 
                        Texture2D colourPickerIconCurrentColour, Texture2D clearIcon, Texture2D downloadIcon, Texture2D eraserIcon)
{
    if (pencilSelected)
    {
        currentUI.drawButton(pencilIcon, 4, WHITE);
    }
    else
    {
        currentUI.drawButton(pencilIcon, 4, GRAY);
    }

    if (eraserSelected)
    {
        currentUI.drawButton(eraserIcon, 5, WHITE);
    }
    else
    {
        currentUI.drawButton(eraserIcon, 5, GRAY);
    }

    if (paintBucketSelected)
    {
        currentUI.drawButton(paintBucketIcon, 6, WHITE);
    }
    else
    {
        currentUI.drawButton(paintBucketIcon, 6, GRAY);
    }

    if (colourPickerSelected)
    {
        currentUI.drawButton(colourPickerIcon, 7, WHITE);
        currentUI.drawButton(colourPickerIconCurrentColour, 7, selectedColour);
        Rectangle rect = { 0, ((float)screenHeight - 104), 304, 104 };
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, UIColour);
        
        Rectangle colourPickerGraph = {1, ((float)screenHeight - 102), 300, 100};
        DrawHSVColourPicker(colourPickerGraph, valueSlider(colourPickerGraph));
        valueSlider(colourPickerGraph);

        Color hoveredColour = GetColourFromHSVPicker(colourPickerGraph, GetMousePosition(), valueSlider(colourPickerGraph));
        if (!coloursAreEqual(hoveredColour, selectedColour) && !coloursAreEqual(hoveredColour, transparentColour))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                selectedColour = hoveredColour;
            }
        }
    }
    else
    {
        currentUI.drawButton(colourPickerIcon, 7, DARKGRAY);
        currentUI.drawButton(colourPickerIconCurrentColour, 7, selectedColour);
    }

    if (clearCanvasSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
    {
        currentUI.drawButton(clearIcon, 10, WHITE);
    }
    else
    {
        currentUI.drawButton(clearIcon, 10, DARKGRAY);
        clearCanvasSelected = false;
    }

    if (downloadSelected && ((GetTime() - flashStartTime) < BUTTON_DISPLAY_TIME))
    {
        currentUI.drawButton(downloadIcon, 11, WHITE);
    }
    else
    {
        currentUI.drawButton(downloadIcon, 11, DARKGRAY);
        downloadSelected = false;
    }

}

void handleUserInput(UI currentUI, Canvas& currentCanvas, pair <int, int> currentCell, Vector2 mousePosition)
{
    int buttonIndex = currentUI.getButton(mousePosition, 0, 0);

    currentUI.drawHoverColour(buttonIndex);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        if (currentCanvas.isWithinCanvas(currentCell))
        {
            if (paintBucketSelected)
            {
                paintBucket(currentCanvas, currentCell, selectedColour);
            }
            else if (eraserSelected)
            {
                currentCanvas.setCellColour(currentCell, transparentColour);
            }
            else
            {
                currentCanvas.setCellColour(currentCell, selectedColour);
            }
        }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        if (buttonIndex == 4) // PENCIL TOOL
        {
            pencilSelected = true;
            paintBucketSelected = false;
            colourPickerSelected = false;
            eraserSelected = false;
        }
        else if (buttonIndex == 5) // ERASER TOOL
        {
            pencilSelected = false;
            paintBucketSelected = false;
            colourPickerSelected = false;
            eraserSelected = true;
        }
        else if (buttonIndex == 6) // PAINT BUCKET TOOL
        {
            pencilSelected = false;
            paintBucketSelected = true;
            colourPickerSelected = false;
            eraserSelected = false;
        }
        else if (buttonIndex == 7) // COLOUR PICKER
        {
            if (!colourPickerSelected)
            {
                colourPickerSelected = true;
                pencilSelected = false;
            }
            else
            {
                colourPickerSelected = false;
                pencilSelected = true;
            }
            paintBucketSelected = false;
            eraserSelected = false;
        }
        else if (buttonIndex == 10) // CLEAR
        {
            clearCanvas(currentCanvas);
            clearCanvasSelected = true;
            flashStartTime = GetTime();
        }
        else if (buttonIndex == 11) //EXPORT
        {                
            exportToPNG(currentCanvas, currentCell);
            downloadSelected = true;    
            flashStartTime = GetTime();
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

string GetColourName(Color currentColour) 
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

void DrawHSVColourPicker(Rectangle rect, float valueSlider) 
{
    for (int x = 0; x < rect.width; ++x) {
        for (int y = 0; y < rect.height; ++y) {
            float hue = (float)x / rect.width * 360.0f;
            float saturation = 1.0f - (float)y / rect.height;
            Color color = ColorFromHSV(hue, saturation, valueSlider);
            DrawPixel((int)rect.x + x, (int)rect.y + y, color);
        }
    }
}

Color GetColourFromHSVPicker(Rectangle rect, Vector2 mousePosition, float valueSlider)
{
    if (CheckCollisionPointRec(mousePosition, rect)) 
    {
        float localX = mousePosition.x - rect.x;
        float localY = mousePosition.y - rect.y;

        float hue = (localX / rect.width) * 360.0f;
        float saturation = 1.0f - (localY / rect.height);
        float value = valueSlider;

        return ColorFromHSV(hue, saturation, value);
    }
    return selectedColour;
}



float valueSlider(Rectangle rectangle)
{
    int min = 0;
    int max = 1;

    int borderOffset = 2;
    int backgroundX = rectangle.width + borderOffset;
    int backgroundY = screenHeight - rectangle.height - (borderOffset * 2);
    int backGroundWidth = UIButtonSize;
    int backGroundHeight = rectangle.height + (borderOffset * 2);

    int grooveX = (rectangle.width + 2) + (UIButtonSize / 2) - (UIButtonSize / 8);
    int grooveY = (screenHeight - rectangle.height) + (UIButtonSize / 4);
    int grooveWidth = (UIButtonSize / 4);
    int grooveHeight = (rectangle.height - UIButtonSize / 2);

    int knobWidth = 16;
    int knobHeight = 16;

    DrawRectangle(backgroundX, backgroundY, backGroundWidth, backGroundHeight, UIColour);
    DrawRectangle(grooveX, grooveY, grooveWidth, grooveHeight, BLACK);

    Vector2 mousePosition = GetMousePosition();
    if (CheckCollisionPointRec(mousePosition, Rectangle{ (float)grooveX - ((float)knobWidth / 4), (float)grooveY, (float)knobWidth, (float)grooveHeight }))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            float knobY = mousePosition.y;

            if (knobY < grooveY)
            {
                knobY = grooveY;
            }
            if (knobY > grooveY + grooveHeight)
            {
                knobY = grooveY + grooveHeight;
            }

            currentValueSlider = 1.0f - ((knobY - grooveY) / (float)grooveHeight);
        }
    }
    float knobY = grooveY + (1.0f - currentValueSlider) * grooveHeight - knobHeight / 2;
    float knobX = grooveX + grooveWidth / 2 - knobWidth / 2;

    DrawRectangle((int)knobX, (int)knobY, knobWidth, knobHeight, GRAY);

    return currentValueSlider;
}





