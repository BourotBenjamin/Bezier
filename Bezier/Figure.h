
#if _WIN32
#include <Windows.h>
#define FREEGLUT_LIB_PRAGMAS 0
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#endif

// Entete OpenGL 
#define GLEW_STATIC 1
#define PI 3.14159265
#include <GL/glew.h>
#include "GL/freeglut.h"

#include "Point.h"
#include <vector>
#include <array>
#include <stdlib.h>
#include <iostream>
#include <memory>

double x, y;
std::unique_ptr<std::vector<Point>> currentCurve;
std::vector<std::unique_ptr<std::vector<Point>>> curves;
int step= 50.0;
float current_control_r, current_control_g, current_control_b, control_r, control_g, control_b, curve_r, curve_g, curve_b, current_curve_r, current_curve_g, current_curve_b;
Point* currentPoint = nullptr;
int minCurrentX, minCurrentY, mouseX, mouseY;
float a, b;
float matrix[6];
bool showControlPoints;

void getCasteljauPoint(int r, int i, double t, double* x, double* y, std::vector<Point>& points);
void getCasteljauPointIter(int r, int i, double t, std::vector<Point>& points);
void deCateljau();
void renderCurves();
void clearWindow();
void onClick(int button, int state, int x, int y);
void lowerSteps();
void higherSteps();
void c0Continuity();
void c1Continuity();
void c2Continuity();
void c1ContinuityAsym(int x, int y);
void c2ContinuityAsym(int x, int y);
void newCurve();
void removeLastPoint();
void onKeyPress(unsigned char key, int x, int y);
void renderLite();
void setColor(int index, float* red, float* green, float* blue);
void controlColorMenu(int index);
void curveColorMenu(int index);
void currentControlColorMenu(int index);
void currentCurveColorMenu(int index);
void clearMenu(int index);
void stepsMenu(int index);
void continuityMenu(int index);
void applyMatrix(std::vector<Point>& curve);
void transformMenu(int index);
void menuStateChange(int status, int x, int y);
void selectPoint();
void changeParamSpace();
void mainMenu(int index);
void createMenu();
int main(int argc, char* argv[]);