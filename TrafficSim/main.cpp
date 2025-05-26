#include <GL/freeglut.h>
#include <cmath>
#include <string>
#include <sstream>


//menu
bool showMenu = true;

//Daynight
bool isNight = false;

//congestion
int congestionLevel = 0;
std::string feedback = "Excellent";





// Manual traffic signal durations
const int GREEN_DURATION = 10;
const int RED_DURATION = 10;
const int YELLOW_DURATION = 3;

// Global variables for vehicle positions
float carPosX = -0.8f;
float busPosX = 0.2f;
float bikePosY = 0.3f;

// Stop positions 
const float carStopX = -0.15f;
const float busStopX = 0.005f;
const float bikeStopY = 0.00f;

// Traffic signal states and timers
enum SignalState { RED, GREEN, YELLOW };
SignalState horizontalSignal = GREEN;
SignalState verticalSignal = RED;

int horizontalTimer = GREEN_DURATION;
int verticalTimer = RED_DURATION;

// Signal positions
float hSignalX = 0.14f;
float hSignalY = -0.13f;
float vSignalX = -0.14f;
float vSignalY = 0.13f;










// Rectangle draw
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Draw Vehicle
void drawVehicle(float x, float y, float width, float height, float r, float g, float b) {
    drawRectangle(x, y, width, height, r, g, b);
}

// Draw Circle
void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.14159f / 20;
        float x = cx + cos(angle) * radius;
        float y = cy + sin(angle) * radius;
        glVertex2f(x, y);
    }
    glEnd();
}

//Draw Timer Text
void drawText(float x, float y, std::string text) {
    glColor3f(0, 0, 0);
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// Dashed line 
void drawDashedLine(float x1, float y1, float x2, float y2, float dashLength, float r, float g, float b) {
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    float dx = x2 - x1, dy = y2 - y1;
    float len = sqrt(dx * dx + dy * dy);
    float step = dashLength / len;
    for (float t = 0; t < 1.0f; t += step * 2) {
        float sx = x1 + dx * t;
        float sy = y1 + dy * t;
        float ex = x1 + dx * (t + step);
        float ey = y1 + dy * (t + step);
        glVertex2f(sx, sy);
        glVertex2f(ex, ey);
    }
    glEnd();

}









void drawBuildingBlock(float x, float y, float windowOffsetX, float windowOffsetY) {

    // Main building rectangle
    drawRectangle(x, y, 0.2f, 0.4f, 0.6f, 0.6f, 0.7f);

    // Windows - 3 rows x 2 cols
    float startX = x + windowOffsetX;
    float startY = y + windowOffsetY;
    float windowWidth = 0.03f;
    float windowHeight = 0.04f;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
            float wx = startX + col * 0.07f;
            float wy = startY - row * 0.07f;
            drawRectangle(wx, wy, windowWidth, windowHeight, 0.9f, 0.9f, 1.0f); // Light blue windows
        }
    }
}

void drawBuildingsRow(float startX, float startY, float dx, float dy, int count, float windowOffsetX, float windowOffsetY) {
    for (int i = 0; i < count; i++) {
        drawBuildingBlock(startX + i * dx, startY + i * dy, windowOffsetX, windowOffsetY);
    }
}








// Car with wheels and roof
void drawCar(float x, float y) {
    drawRectangle(x, y, 0.1f, 0.05f, 1.0f, 0.0f, 0.0f); // red body
    drawRectangle(x + 0.015f, y + 0.05f, 0.07f, 0.025f, 0.9f, 0.3f, 0.3f); // roof
    drawCircle(x + 0.02f, y - 0.01f, 0.015f, 0, 0, 0); // left wheel
    drawCircle(x + 0.08f, y - 0.01f, 0.015f, 0, 0, 0); // right wheel
}

// Bus with windows and wheels
void drawBus(float x, float y) {
    drawRectangle(x, y, 0.15f, 0.06f, 0.0f, 0.0f, 1.0f); // blue body
    drawRectangle(x + 0.01f, y + 0.03f, 0.03f, 0.02f, 1, 1, 1); // window 1
    drawRectangle(x + 0.05f, y + 0.03f, 0.03f, 0.02f, 1, 1, 1); // window 2
    drawRectangle(x + 0.09f, y + 0.03f, 0.03f, 0.02f, 1, 1, 1); // window 3
    drawCircle(x + 0.03f, y - 0.01f, 0.015f, 0, 0, 0); // left wheel
    drawCircle(x + 0.12f, y - 0.01f, 0.015f, 0, 0, 0); // right wheel
}

// Draw vcar with frame, seat, handlebars, and wheels
void drawVerticalCar(float x, float y) {
    // Body - taller than wide since vertical
    drawRectangle(x - 0.025f, y, 0.05f, 0.1f, 0.0f, 1.0f, 0.0f); // green body

    // Roof (smaller rectangle near top)
    drawRectangle(x - 0.02f, y + 0.08f, 0.04f, 0.02f, 0.2f, 0.8f, 0.2f);

    // Wheels - positioned on left and right sides near bottom and top
    drawCircle(x - 0.03f, y + 0.02f, 0.015f, 0, 0, 0); // left bottom wheel
    drawCircle(x + 0.03f, y + 0.02f, 0.015f, 0, 0, 0); // right bottom wheel

    drawCircle(x - 0.03f, y + 0.09f, 0.015f, 0, 0, 0); // left top wheel
    drawCircle(x + 0.03f, y + 0.09f, 0.015f, 0, 0, 0); // right top wheel
}












// Main display


void display() {

    if (showMenu) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Menu Background
        glColor3f(0.2f, 0.3f, 0.4f);
        drawRectangle(-1.0f, -1.0f, 2.0f, 2.0f, 0.2f, 0.3f, 0.4f);

        // Start Button
        drawRectangle(-0.2f, 0.6f, 0.4f, 0.1f, 0.0f, 0.6f, 0.0f);
        drawText(-0.05f, 0.63f, "Start");
        drawText(-0.18f, 0.52f, "Press Enter to Start");  // Text below Start button

        // Exit Button
        drawRectangle(-0.2f, 0.4f, 0.4f, 0.1f, 0.6f, 0.0f, 0.0f);
        drawText(-0.05f, 0.43f, "Exit");
        drawText(-0.17f, 0.32f, "Press Esc to Exit");    // Text below Exit button

        // Instructions Placeholder at the bottom
        drawText(-0.35f, -0.60f, "Instructions:");
        drawText(-0.35f, -0.70f, "Press 1 to Force Red Signal");
        drawText(-0.35f, -0.75f, "Press 2 to Force Yellow Signal");
        drawText(-0.35f, -0.80f, "Press 3 to Force Green Signal");

        glFlush();
        return;
    }

    glClearColor(isNight ? 0.1f : 0.8f, isNight ? 0.1f : 0.8f, isNight ? 0.2f : 0.8f, 1.0f);
    

    glClear(GL_COLOR_BUFFER_BIT);

    // Roads
    drawRectangle(-1.0f, -0.1f, 2.0f, 0.2f, 0.3f, 0.3f, 0.3f); // horizontal
    drawRectangle(-0.1f, -1.0f, 0.2f, 2.0f, 0.3f, 0.3f, 0.3f); // vertical

    // Lane Dividers
    drawDashedLine(-1.0f, 0.0f, 1.0f, 0.0f, 0.05f, 1, 1, 1);  // Horizontal
    drawDashedLine(0.0f, -1.0f, 0.0f, 1.0f, 0.05f, 1, 1, 1);  // Vertical

    // Stop lines
    drawRectangle(carStopX + 0.04f, -0.1f, 0.005f, 0.2f, 1, 1, 1); // Car stop line a bit to the left
    drawRectangle(busStopX - -0.1f, -0.1f, 0.005f, 0.2f, 1, 1, 1); // Bus stop line a bit to the left
    drawRectangle(-0.1f, bikeStopY + 0.13f, 0.2f, 0.005f, 1, 1, 1); 

    // Top row of buildings
    drawBuildingsRow(-0.9f, 0.1f, 0.35f, 0.0f, 2, 0.05f, 0.25f);

    // Bottom row of buildings
    drawBuildingsRow(-0.9f, -0.49f, 0.20f, 0.0f, 3, 0.035f, 0.30f);

    // Top right side buildings (vertical)
    drawBuildingsRow(0.38f, 0.1f, 0.0f, 0.5f, 1, 0.05f, 0.25f);

    // bottom Right side buildings (vertical)
    drawBuildingsRow(0.70f, -0.49f, 0.0f, 0.5f, 1, 0.05f, 0.30f);

    // bottom Right side buildings (vertical)
    drawBuildingsRow(0.2f, -0.49f, 0.0f, 0.5f, 1, 0.05f, 0.30f);




    // Vehicles
    drawCar(carPosX, 0.046f); // Car
   
    

    drawBus(busPosX, -0.05f); // Bus
    drawCircle(busPosX + 0.02f, -0.06f, 0.015f, 0, 0, 0); // Bus wheel front
    drawCircle(busPosX + 0.13f, -0.06f, 0.015f, 0, 0, 0); // Bus wheel rear

    drawVerticalCar(-0.05f, bikePosY);
    
    drawCircle(-2.1f, bikePosY + 0.02f, 0.015f, 0, 0, 0); // Bike wheel bottom
    drawCircle(-0.018f, bikePosY + 0.02f, 0.015f, 0, 0, 0);  // Bike wheel front

    // Signal poles and heads
    drawRectangle(hSignalX - 0.01f, hSignalY, 0.02f, 0.15f, 0.1f, 0.1f, 0.1f); // H pole
    drawRectangle(hSignalX - 0.03f, hSignalY + 0.15f, 0.06f, 0.06f, 0.1f, 0.1f, 0.1f); // H box

    drawRectangle(vSignalX - 0.01f, vSignalY, 0.02f, 0.15f, 0.1f, 0.1f, 0.1f); // V pole
    drawRectangle(vSignalX - 0.03f, vSignalY + 0.15f, 0.06f, 0.06f, 0.1f, 0.1f, 0.1f); // V box

    // Horizontal Signal Light
    if (horizontalSignal == RED) drawCircle(hSignalX, hSignalY + 0.18f, 0.015f, 1, 0, 0);
    else if (horizontalSignal == YELLOW) drawCircle(hSignalX, hSignalY + 0.18f, 0.015f, 1, 1, 0);
    else drawCircle(hSignalX, hSignalY + 0.18f, 0.015f, 0, 1, 0);

    // Vertical Signal Light
    if (verticalSignal == RED) drawCircle(vSignalX, vSignalY + 0.18f, 0.015f, 1, 0, 0);
    else if (verticalSignal == YELLOW) drawCircle(vSignalX, vSignalY + 0.18f, 0.015f, 1, 1, 0);
    else drawCircle(vSignalX, vSignalY + 0.18f, 0.015f, 0, 1, 0);

    // Countdown Text
    drawText(hSignalX - 0.02f, hSignalY + 0.24f, std::to_string(horizontalTimer));
    drawText(vSignalX - 0.02f, vSignalY + 0.24f, std::to_string(verticalTimer));
    // Display congestion
    drawText(-0.9f, -0.9f, ("Congestion Level: " + std::to_string(congestionLevel)).c_str());
    drawText(-0.9f, -0.95f, ("Status: " + feedback).c_str());


    glFlush();
}







//keyboard control
void keyboard(unsigned char key, int x, int y) {
    if (showMenu) {
        if (key == 13) {  // Enter key
            showMenu = false;
            glutPostRedisplay();
        }
        else if (key == 27) {  // Escape key
            exit(0);
        }
        return;
    }

    // Normal simulation controls
    switch (key) {
    case '1': // Force both signals to RED
        horizontalSignal = RED;
        verticalSignal = RED;
        break;
    case '2': // Force both signals to YELLOW
        horizontalSignal = YELLOW;
        verticalSignal = YELLOW;
        break;
    case '3': // Force both signals to GREEN
        horizontalSignal = GREEN;
        verticalSignal = GREEN;
        break;
    case '4': // Toggle day/night mode
        isNight = !isNight;
        break;
    }

    glutPostRedisplay();
}










// Timer updates
void timer(int value) {
    static int frameCount = 0;
    frameCount++;
    if (carPosX + 0.1f < carStopX) {
        // Car is before the stop line then always move
        carPosX += 0.005f;
    }
    else if (carPosX < carStopX) {
        // Car is between its front and stop line then obey signal
        if (horizontalSignal == GREEN)
            carPosX += 0.005f;
    }
    else {
        // Car has crossed the stop line then always move
        carPosX += 0.005f;
    }
    if (carPosX > 1.0f) carPosX = -1.0f;


    if (busPosX > busStopX + 0.15f) {
        // Bus is before the stop line
        busPosX -= 0.004f;
    }
    else if (busPosX > busStopX) {
        // Bus is near the stop line: obey signal
        if (horizontalSignal == GREEN)
            busPosX -= 0.004f;
    }
    else {
        // Bus has crossed the stop line
        busPosX -= 0.004f;
    }
    if (busPosX < -1.0f) busPosX = 1.0f;
    if (bikePosY > bikeStopY + 0.15f) {
        // vcar is before the stop line
        bikePosY -= 0.006f;
    }
    else if (bikePosY > bikeStopY) {
        // vcar is near the stop line: obey signal
        if (verticalSignal == GREEN)
            bikePosY -= 0.006f;
    }
    else {
        // vcar has crossed the stop line
        bikePosY -= 0.006f;
    }
    if (bikePosY < -1.0f) bikePosY = 1.0f;




    // Signal timer update every ~1 sec
    if (frameCount >= 60) {
        frameCount = 0;
        horizontalTimer--;
        verticalTimer--;

        // Horizontal signal state change
        if (horizontalTimer <= 0) {
            if (horizontalSignal == GREEN) {
                horizontalSignal = YELLOW;
                horizontalTimer = YELLOW_DURATION;
            }
            else if (horizontalSignal == YELLOW) {
                horizontalSignal = RED;
                horizontalTimer = RED_DURATION;
            }
            else {
                horizontalSignal = GREEN;
                horizontalTimer = GREEN_DURATION;
            }
        }

        // Vertical signal state change
        if (verticalTimer <= 0) {
            if (verticalSignal == RED) {
                verticalSignal = GREEN;
                verticalTimer = GREEN_DURATION;
            }
            else if (verticalSignal == GREEN) {
                verticalSignal = YELLOW;
                verticalTimer = YELLOW_DURATION;
            }
            else {
                verticalSignal = RED;
                verticalTimer = RED_DURATION;
            }
        }
    }
    int waitingCars = 0;

    float busFrontX = busPosX + 0.15f; // Assuming bus front is 0.15f ahead of busPosX, adjust if needed

    // Car stopped at red/yellow light
    if ((horizontalSignal != GREEN) && (carPosX + 0.1f >= carStopX - 0.01f && carPosX <= carStopX + 0.01f)) {
        waitingCars++;
    }

    // Bus stopped at red/yellow light
    if ((horizontalSignal != GREEN) && (busFrontX >= busStopX - 0.01f && busFrontX <= busStopX + 0.01f)) {
        waitingCars++;
    }

    // Bike stopped at red/yellow light
    if ((verticalSignal != GREEN) && (bikePosY >= bikeStopY - 0.01f && bikePosY <= bikeStopY + 0.01f)) {
        waitingCars++;
    }

    if (waitingCars > 0) {
        congestionLevel += waitingCars * 2;
    }
    else if (congestionLevel > 0) {
        congestionLevel -= 1; // slowly decrease congestion when no waiting cars
    }

    // Clamp congestionLevel within range
    if (congestionLevel > 200) congestionLevel = 200;
    if (congestionLevel < 0) congestionLevel = 0;

    // Set feedback text based on congestion level
    if (congestionLevel < 50) feedback = "Excellent";
    else if (congestionLevel < 120) feedback = "Moderate";
    else feedback = "Needs Improvement";



    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}










// Entry
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Smart Traffic Simulation");

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    

    return 0;
}
