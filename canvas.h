#ifndef CANVAS_H
#define CANVAS_H

#include <raylib.h>
#include <utility>
#include <vector>

using namespace std;

class Canvas
{
    private:
        int canvasWidth = 0;
        int canvasHeight = 0;
        int gridSize = 0;
        Color canvasColour = {0, 0, 0, 0};
        vector<vector<Color>> canvasGrid;

    public:
        Canvas(int width, int height, Color colour, int gridSize);

        void drawCanvas(float xOffset, float yOffset) const;
        void updateCanvasGridColours(int xOffset, int yOffset) const;
        void createGrid();
        vector<vector<Color>> getCanvasGrid();
        pair<int, int> getCellCoordinates(Vector2 position, int xOffset, int yOffset);
        void drawGrid(int xOffset, int yOffset) const;
        Vector2 getCanvasCentrePosition() const;
        int getHeight() const;
        int getWidth() const;
        Color getCellColour(pair<int, int> cellCoordinates, bool isWithinCanvas) const;
        void setCellColour(pair<int, int> cellCoordinates, Color targetColour);
        bool isWithinCanvas(pair<int, int> cellCoordinates);
};

#endif // CANVAS_H