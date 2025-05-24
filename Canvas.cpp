#include "canvas.h"

Canvas::Canvas(int width, int height, Color colour, int gridSize)
{
    canvasWidth = width;
    canvasHeight = height;
    this->gridSize = gridSize;
    canvasColour = colour;
}

void Canvas::drawCanvas(float xOffset, float yOffset) const
{
    DrawRectangle(static_cast<int>(xOffset + 5), static_cast<int>(yOffset + 5), canvasWidth, canvasHeight, {0, 0, 0, 50});
    DrawRectangle(static_cast<int>(xOffset), static_cast<int>(yOffset), canvasWidth, canvasHeight, canvasColour);
} 

void Canvas::updateCanvasGridColours(int xOffset, int yOffset) const
{
    int rows = (int)canvasGrid.size();
    int columns =  (int)canvasGrid[0].size();

    for (int row = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++)
        {
            Color cellColour = canvasGrid[row][column];
            DrawRectangle(xOffset + column * gridSize, yOffset + row * gridSize, gridSize, gridSize, cellColour);
        }
    }
}

void Canvas::createGrid()
{
    int columns = canvasWidth / gridSize;
    int rows = canvasHeight / gridSize;

    Color transparent = { 0, 0, 0, 0 };

    canvasGrid.resize(rows, vector<Color>(columns, transparent));
}

vector<vector<Color>> Canvas::getCanvasGrid()
{
    return canvasGrid;
}

pair<int, int> Canvas::getCellCoordinates(Vector2 position, int xOffset, int yOffset)
{
    int column = (position.x - xOffset) / gridSize;
    int row = (position.y - yOffset) / gridSize;

    if (row >= 0 && row < canvasGrid.size() && column >= 0 && column < canvasGrid[0].size())
    {
        return { row, column };
    }
    return { -1, -1 };
}

void Canvas::drawGrid(int xOffset, int yOffset) const
{
    for (int x = 0; x <= canvasWidth; x += gridSize)
    {
        DrawLine((x + xOffset), yOffset, (x + xOffset), (yOffset + canvasHeight), RED);
    }

    for (int y = 0; y <= canvasHeight; y += gridSize)
    {
        DrawLine(xOffset, (y + yOffset), (xOffset + canvasWidth), (y + yOffset), GREEN);
    }
}

Vector2 Canvas::getCanvasCentrePosition() const
{
    float canvasCentrePositionX = canvasWidth / 2;
    float canvasCentrePositionY = canvasHeight / 2;

    Vector2 canvasCentrePosition = { canvasCentrePositionX, canvasCentrePositionY };
    return canvasCentrePosition;
}

int Canvas::getHeight() const
{
    return canvasHeight;
}

int Canvas::getWidth() const
{
    return canvasWidth;
}

Color Canvas::getCellColour(pair<int, int> cellCoordinates, bool isWithinCanvas) const
{
    if (isWithinCanvas)
    {
        return canvasGrid[cellCoordinates.first][cellCoordinates.second];
    }
    return canvasColour;
}

void Canvas::setCellColour(pair<int, int> cellCoordinates, Color targetColour)
{
    canvasGrid[cellCoordinates.first][cellCoordinates.second] = targetColour;
}

bool Canvas::isWithinCanvas(pair<int, int> cellCoordinates)
{
    return (cellCoordinates.first >= 0 && cellCoordinates.first < canvasGrid.size() && 
            cellCoordinates.second >= 0 && cellCoordinates.second < canvasGrid[0].size());
}
