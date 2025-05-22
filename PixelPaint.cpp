#include <iostream>
#include <raylib.h>
#include <vector>
#include <stack>

using namespace std;

bool coloursAreEqual(Color firstColour, Color secondColour);
void drawGrid(int screenWidth, int screenHeight, int cellSize, Color gridColour);
void drawUIGrid(int screenWidth, int screenHeight, int cellSize, Color UIGridColour, Color UIColour);
void exportToPNG(int screenWidth, int screenHeight, int cellSize, vector<vector<Color>>& gridColours);
void handleLeftMouseClick(vector<vector<Color>>& gridColours, int columns, int rows, int cellSize, Color initialColour, Color& selectedColour, bool& paintBucketActive, int screenWidth, int screenHeight);
void handleRightMouseClick(vector<vector<Color>>& gridColours, int columns, int rows, int cellSize, Color initialColour, Color selectedColour);
void mouseHoverUI(int cellSize);
void paintBucket(int initialCellColumn, int initialCellRow, Color newColour, vector<vector<Color>>& gridColours, int columns, int rows);
Color setColour(Vector2 mousePosition, int cellSize);
void toggleCellColour(Color& cellColour, Color initialColour, Color selectedColour);
void handleToolbarClick(int column, Color& selectedColour, bool& paintBucketActive, Vector2 mousePosition, int cellSize, int screenWidth, int screenHeight, vector<vector<Color>>& gridColours);


Color transparentColour = {0, 0, 0, 0};


int main() 
{
    const int screenWidth =  480;
    const int screenHeight = 480;
    const int cellSize = 30;

    Color backgroundColour = WHITE;
    Color initialColour = WHITE;
    Color gridColour = LIGHTGRAY;
    Color selectedColour = BLACK;
    Color UIGridColour = BLACK;
    Color UIColour = DARKGRAY;
    

    SetConfigFlags(FLAG_MSAA_4X_HINT );
    InitWindow(screenWidth, screenHeight, "Window");
    //SetTargetFPS(60);

    int columns = screenWidth / cellSize;
    int rows = screenHeight / cellSize;
    bool paintBucketActive = false;
    //int brushSize = 1;

    vector<vector<Color>> gridColours(rows, vector<Color>(columns, initialColour));

    while(!WindowShouldClose())
    {
        handleLeftMouseClick(gridColours, columns, rows, cellSize, initialColour, selectedColour, paintBucketActive, screenWidth, screenHeight);
        handleRightMouseClick(gridColours, columns, rows, cellSize, initialColour, selectedColour);

        BeginDrawing();
            ClearBackground(backgroundColour);

            for (int row = 0; row < rows; row++)
            {
                for (int column = 0; column < columns; column++)
                {
                    DrawRectangle(column * cellSize, row * cellSize, cellSize, cellSize, gridColours[row][column]);
                }
            }
            drawUIGrid(screenWidth, screenHeight, cellSize, UIGridColour, UIColour);
            mouseHoverUI(cellSize);
            //drawGrid(screenWidth, screenHeight, cellSize, gridColour);

            //DrawFPS(20, screenHeight - 20);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void drawUIGrid(int screenWidth, int screenHeight, int cellSize, Color UIGridColour, Color UIColour)
{
    DrawRectangle(0, 0, screenWidth, cellSize, GRAY); // BACKGROUND
    DrawText("Pencil", cellSize * 1.2, cellSize / 2, 10, BLACK); //PENCIL TEXT
    DrawText("Bucket", (cellSize * 1.1) * 2, cellSize / 2, 10, BLACK); //BUCKET TEXT
    DrawRectangle(cellSize * 3, 0, cellSize, cellSize, RED); // RED
    DrawRectangle(cellSize * 4, 0, cellSize, cellSize, ORANGE); // ORANGE
    DrawRectangle(cellSize * 5, 0, cellSize, cellSize, YELLOW); // YELLOW
    DrawRectangle(cellSize * 6, 0, cellSize, cellSize, GREEN); // GREEN
    DrawRectangle(cellSize * 7, 0, cellSize, cellSize, BLUE); // BLUE
    DrawRectangle(cellSize * 8, 0, cellSize, cellSize, PURPLE); // PURPLE
    DrawRectangle(cellSize * 9, 0, cellSize, cellSize, BLACK); // BLACK
    DrawRectangle(cellSize * 10, 0, cellSize, cellSize, WHITE); // WHITE

    for (int x = 0; x <= screenWidth; x += cellSize)
    {
        DrawLine(x, 0, x, cellSize, UIColour);
    }
    DrawLine(0, cellSize, screenWidth, cellSize, UIGridColour);
}

void mouseHoverUI(int cellSize)
{
    Vector2 currentMousePosition = { (float)GetMouseX(), (float)GetMouseY() };

    if (currentMousePosition.y >= 0 && currentMousePosition.y <= cellSize)
    {
        int hoveredCellIndex = (int)(currentMousePosition.x / cellSize);
        int highlightX = hoveredCellIndex * cellSize;
        DrawRectangleLines(highlightX, 0, cellSize, cellSize, WHITE);
    }  
}

void drawGrid(int screenWidth, int screenHeight, int cellSize, Color gridColour)
{
    for (int x = 0; x <= screenWidth; x += cellSize)
    {
        DrawLine(x, 0, x, screenHeight, gridColour);
    }
    for (int y = 0; y <= screenHeight; y += cellSize)
    {
        DrawLine(0, y, screenWidth, y, gridColour);
    }
}

void handleLeftMouseClick(vector<vector<Color>>& gridColours, int columns, int rows, int cellSize, Color initialColour, Color& selectedColour, bool& paintBucketActive, int screenWidth, int screenHeight)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePosition = GetMousePosition();
        int column = mousePosition.x / cellSize;
        int row = mousePosition.y / cellSize;

        if (row == 0)
        {
            handleToolbarClick(column, selectedColour, paintBucketActive, mousePosition, cellSize, screenWidth, screenHeight, gridColours);
        }
        else if (row >= 1 && row < rows && column >= 0 && column < columns)
        {
            if (paintBucketActive)
            {
                paintBucket(column, row, selectedColour, gridColours, columns, rows);
            }
            else
            {
                gridColours[row][column] = selectedColour;
            }
        }
    }
}

void handleRightMouseClick(vector<vector<Color>>& gridColours, int columns, int rows, int cellSize, Color initialColour, Color selectedColour)
{
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        Vector2 mousePosition = GetMousePosition();
        int column = mousePosition.x / cellSize;
        int row = mousePosition.y / cellSize;

        if (row >= 1 && row < rows && column >= 0 && column < columns)
        {
            gridColours[row][column] = initialColour;
        }
    }
}

void toggleCellColour(Color& cellColour, Color initialColour, Color selectedColour)
{
    if (coloursAreEqual(cellColour, initialColour))
    {
        cellColour =  selectedColour;
    }
    else
    {
        cellColour = initialColour;
    }
}

bool coloursAreEqual(Color firstColour, Color secondColour)
{
    return ((firstColour.r == secondColour.r) &&
            (firstColour.g == secondColour.g) &&
            (firstColour.b == secondColour.b) &&
            (firstColour.a == secondColour.a));
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

void handleToolbarClick(int column, Color& selectedColour, bool& paintBucketActive, Vector2 mousePosition, int cellSize, int screenWidth, int screenHeight, vector<vector<Color>>& gridColours)
{
    if (column == 0)
    {
        exportToPNG(screenWidth, screenHeight, cellSize, gridColours);
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
        selectedColour = setColour(mousePosition, cellSize);
    }
}

void exportToPNG(int screenWidth, int screenHeight, int cellSize, vector<vector<Color>>& gridColours)
{
    int columns = screenWidth / cellSize;
    int rows = screenHeight / cellSize;

    RenderTexture2D canvas = LoadRenderTexture(screenWidth,  (screenHeight - cellSize));

    BeginTextureMode(canvas);
        ClearBackground(transparentColour);
        for (int row = 1; row < rows; row++)
        {
            for (int column = 0; column < columns; column++)
            {
                DrawRectangle(column * cellSize, (row - 1) * cellSize, cellSize, cellSize, gridColours[row][column]);
            }
        }
    EndTextureMode();

    Image image = LoadImageFromTexture(canvas.texture);
    ImageFlipVertical(&image);
    ExportImage(image, "PixelPaint.png");
    
    UnloadImage(image);
    UnloadRenderTexture(canvas);
}