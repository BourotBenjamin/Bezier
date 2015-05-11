
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
int step= 50;
float current_control_r, current_control_g, current_control_b, control_r, control_g, control_b, curve_r, curve_g, curve_b, current_curve_r, current_curve_g, current_curve_b;
Point* currentPoint = nullptr;
int minCurrentX, minCurrentY, mouseX, mouseY;
float matrix[6];
bool showControlPoints;

void getCasteljauPoint(int r, int i, double t, double* x, double* y, std::vector<Point>& points) {

	double x_1, x_2, y_1, y_2;
	if (r == 0)
	{
		*x = points.at(i).x;
		*y = points.at(i).y;
		return;
	}
	getCasteljauPoint(r - 1, i, t, &x_1, &y_1, points);
	getCasteljauPoint(r - 1, i + 1, t, &x_2, &y_2, points);
	*x = (1.0 - t) * x_1 + t * x_2;
	*y = (1.0 - t) * y_1 + t * y_2;
}


void getCasteljauPointIter(int r, int i, double t, std::vector<Point>& points) {
	double* xTab = (double*)malloc(sizeof(double)*r*r);
	double* yTab = (double*)malloc(sizeof(double)*r*r);
	for (int j = 0; j < r; j++)
	{
		xTab[j] = points.at(j).x;
		yTab[j] = points.at(j).y;
	}
	for (int i = 1; i < r; i++)
	{
		for (int j = 0; j < r-i; j++)
		{
			yTab[j + i*r] = t*yTab[(i - 1)*r + (j + 1)] + (1 - t)*yTab[(i - 1)*r + j];
			xTab[j + i*r] = t*xTab[(i - 1)*r + (j + 1)] + (1 - t)*xTab[(i - 1)*r + j];
		}
	}
	for (int i = 0; i < r; i++)
	{
		glVertex2d(xTab[r*(r-1)], yTab[r*(r-1)]);
	}
	free(yTab);
	free(xTab);
}

void deCateljau()
{
	glColor3f(curve_r, curve_g, curve_b);
	for each(auto& points in curves)
	{
		int psize = (*points).size();
		if (psize > 0)
		{
			glBegin(GL_LINE_STRIP);
			for (double t = 0; t <= 1; t += (1.0/step))
			{
				getCasteljauPointIter(psize, 0, t, (*points));
			}
			glVertex2d((*points).back().x, (*points).back().y);
			glEnd();
		}
	}
	glColor3f(current_curve_r, current_curve_g, current_curve_b);
	int psize = (*currentCurve).size();
	if (psize > 0)
	{
		glBegin(GL_LINE_STRIP);
		for (double t = 0; t <= 1; t += (1.0 / step))
		{
			getCasteljauPointIter(psize, 0, t, (*currentCurve));
		}
		glVertex2d((*currentCurve).back().x, (*currentCurve).back().y);
		glEnd();
	}
}


void renderDeCasteljau()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(1);
	if (showControlPoints)
	{
		glColor3f(control_r, control_g, control_b);
		for each (auto& points in curves)
		{
			glBegin(GL_LINE_STRIP);
			for each (Point p in (*points))
			{
				glVertex2i(p.x, p.y);
			}
			glEnd();
		}
		glColor3f(current_control_r, current_control_g, current_control_b);
		glBegin(GL_LINE_STRIP);
		for each (Point p in (*currentCurve))
		{
			glVertex2i(p.x, p.y);
		}
		glEnd();
	}
	deCateljau();
	glutPostRedisplay();
	glFlush();
}

void clearWindow()
{
	curves.clear();
	currentCurve = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
	glFlush();
}

void onClick(int button, int state, int x, int y)
{
	if (state)
	{
		if (button == 0)
		{
			if (currentPoint != nullptr)
			{
				(*currentPoint).x = x;
				(*currentPoint).y = y;
				currentPoint = nullptr;
				renderDeCasteljau();
			}
			else
			{
				currentCurve->push_back(Point(x, y));
				renderDeCasteljau();
			}
			if (minCurrentX == -1 || minCurrentX > x)
				minCurrentX = x;
			if (minCurrentY == -1 || minCurrentY > y)
				minCurrentY = y;
		}
		else if (button == 1)
		{
			clearWindow();
		}
		else if (button == 2)
		{
			mouseX = x;
			mouseY = y;
			std::cout << mouseX << std::endl;
			std::cout << mouseY << std::endl;
		}
	}
}

void lowerSteps()
{
	if (step > 1)
		step--;
	renderDeCasteljau();
}
void higherSteps()
{
	step++;
	renderDeCasteljau();
}

void c0Continuity()
{
	std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	Point p = (*currentCurve).back();
	(*tmp).push_back(Point(p.x, p.y));
	curves.push_back(std::move(currentCurve));
	currentCurve = std::move(tmp);
	renderDeCasteljau();
}

void c1Continuity()
{
	std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	Point p = (*currentCurve).back();
	Point p2 = (*currentCurve).at((*currentCurve).size() - 2);
	(*tmp).push_back(Point(p.x, p.y));
	(*tmp).push_back(Point(p.x + (p.x - p2.x), p.y + (p.y - p2.y)));
	curves.push_back(std::move(currentCurve));
	currentCurve = std::move(tmp);
	renderDeCasteljau();
}

void c2Continuity()
{
	std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	Point p = (*currentCurve).back();
	Point p2 = (*currentCurve).at((*currentCurve).size() - 2);
	Point p3 = (*currentCurve).at((*currentCurve).size() - 3);
	Point temp = Point(p2.x + (p2.x - p3.x), p2.y + (p2.y - p3.y));
	Point newP = Point(p.x + (p.x - p2.x), p.y + (p.y - p2.y));
	(*tmp).push_back(Point(p.x, p.y));
	(*tmp).push_back(Point(newP.x, newP.y));
	(*tmp).push_back(Point(newP.x + (newP.x - temp.x), newP.y + (newP.y - temp.y)));
	curves.push_back(std::move(currentCurve));
	currentCurve = std::move(tmp);
	renderDeCasteljau();
}

void c1ContinuityAsym(int x, int y)
{
	std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	Point p = (*currentCurve).back();
	Point p2 = (*currentCurve).at((*currentCurve).size() - 2);
	float distPoint = sqrt((p.x - p2.x)*(p.x - p2.x) + (p.y - p2.y)*(p.y - p2.y));
	float distMouse = sqrt((p.x - x)*(p.x - x) + (p.y - y)*(p.y - y));
	float coeff = distMouse / distPoint;
	(*tmp).push_back(Point(p.x, p.y));
	(*tmp).push_back(Point(p.x + coeff*(p.x - p2.x), p.y + coeff*(p.y - p2.y)));
	curves.push_back(std::move(currentCurve));
	currentCurve = std::move(tmp);
	std::cout << coeff << std::endl;
	renderDeCasteljau();
}

void c2ContinuityAsym(int x, int y)
{
	std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	Point p = (*currentCurve).back();
	Point p2 = (*currentCurve).at((*currentCurve).size() - 2);
	Point p3 = (*currentCurve).at((*currentCurve).size() - 3);
	float distPoint = sqrt((p.x - p2.x)*(p.x - p2.x) + (p.y - p2.y)*(p.y - p2.y));
	float distMouse = sqrt((p.x - x)*(p.x - x) + (p.y - y)*(p.y - y));
	float coeff = distMouse / distPoint;
	Point temp = Point(p2.x + coeff*(p2.x - p3.x), p2.y + coeff*(p2.y - p3.y));
	Point newP = Point(p.x + coeff*(p.x - p2.x), p.y + coeff*(p.y - p2.y));
	(*tmp).push_back(Point(p.x, p.y));
	(*tmp).push_back(Point(newP.x, newP.y));
	(*tmp).push_back(Point(newP.x + coeff*(newP.x - temp.x), newP.y + coeff*(newP.y - temp.y)));
	curves.push_back(std::move(currentCurve));
	currentCurve = std::move(tmp);
	std::cout << coeff << std::endl;
	renderDeCasteljau();
}

void newCurve()
{
	minCurrentX = minCurrentY = -1;
	curves.push_back(std::move(currentCurve));
	currentCurve = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	renderDeCasteljau();
}

void removeLastPoint()
{
	if ((*currentCurve).size() > 0)
	{
		(*currentCurve).pop_back();
	}
	renderDeCasteljau();
}

void onKeyPress(unsigned char key, int x, int y)
{
	if (key == '+')
		higherSteps();
	else if (key == '-')
		lowerSteps();
	else if (key == 'x' && (*currentCurve).size() > 0) // C0
	{
		c0Continuity();
	}
	else if (key == 'c' && (*currentCurve).size() > 1) // C1
	{
		c1Continuity();
	}
	else if (key == 'v' && (*currentCurve).size() > 2) // C2
	{
		c2Continuity();
	}
	else if (key == 'd' && (*currentCurve).size() > 1) // C1
	{
		c1ContinuityAsym(x, y);
	}
	else if (key == 'f' && (*currentCurve).size() > 2) // C2
	{
		c2ContinuityAsym(x, y);
	}
	else if (key == 'r')
	{
		removeLastPoint();
	}
	else if (key == 'n')
	{
		newCurve();
	}
	else if (key == 'h')
	{
		showControlPoints = !showControlPoints;
		renderDeCasteljau();
	}
}


void renderLite() {}



void setColor(int index, float* red, float* green, float* blue)
{
	switch (index)
	{
	case 0:
		(*red) = 1.0f;
		(*green) = 0.0f;
		(*blue) = 0.0f;
		break;
	case 1:
		(*red) = 0.0f;
		(*green) = 1.0f;
		(*blue) = 0.0f;
		break;
	case 2:
		(*red) = 0.0f;
		(*green) = 0.0f;
		(*blue) = 1.0f;
		break;
	case 3:
		(*red) = 1.0f;
		(*green) = 1.0f;
		(*blue) = 0.0f;
		break;
	case 4:
		(*red) = 1.0f;
		(*green) = 0.0f;
		(*blue) = 1.0f;
		break;
	case 5:
		(*red) = 0.0f;
		(*green) = 1.0f;
		(*blue) = 1.0f;
		break;
	case 6:
		(*red) = 1.0f;
		(*green) = 1.0f;
		(*blue) = 1.0f;
		break;
	case 7:
		(*red) = 1.0f;
		(*green) = 0.5f;
		(*blue) = 0.0f;
		break;
	}
	renderDeCasteljau();
}

void controlColorMenu(int index)
{
	setColor(index, &control_r, &control_g, &control_b);
}
void curveColorMenu(int index)
{
	setColor(index, &curve_r, &curve_g, &curve_b);
}
void currentControlColorMenu(int index)
{
	setColor(index, &current_control_r, &current_control_g, &current_control_b);
}
void currentCurveColorMenu(int index)
{
	setColor(index, &current_curve_r, &current_curve_g, &current_curve_b);
}
void clearMenu(int index)
{
	switch (index)
	{
	case 0:
		clearWindow();
		break;
	case 1:
		removeLastPoint();
		break;
	}
}

void stepsMenu(int index)
{
	switch (index)
	{
	case 0:
		lowerSteps();
		break;
	case 1:
		higherSteps();
		break;
	}
}

void continuityMenu(int index)
{
	switch (index)
	{
	case 0:
		c0Continuity();
		break;
	case 1:
		c1Continuity();
		break;
	case 2:
		c2Continuity();
		break;
	case 3:
		c1ContinuityAsym(mouseX, mouseY);
		break;
	case 4:
		std::cout << mouseX << std::endl;
		std::cout << mouseY << std::endl;
		c2ContinuityAsym(mouseX, mouseY);
		break;
	}
}

void applyMatrix(std::vector<Point>& curve)
{
	auto point = curve.begin();
	auto end = curve.end();
	while (point != end)
	{
		int tmpx = (*point).x - minCurrentX;
		int tmpy = (*point).y - minCurrentY;
		(*point).x = minCurrentX + tmpx * matrix[0] + tmpy * matrix[1] + matrix[2];
		(*point).y = minCurrentY + tmpx * matrix[3] + tmpy * matrix[4] + matrix[5];
		point++;
	}
}

void transformMenu(int index)
{
	switch (index)
	{
	case 0:
		int angle;
		std::cout << "Angle :  " << std::endl;
		std::cin >> angle;
		matrix[0] = std::cos(angle  * PI / 180.0);
		matrix[1] = -std::sin(angle * PI / 180.0);
		matrix[2] = 0;
		matrix[3] = std::sin(angle * PI / 180.0);
		matrix[4] = std::cos(angle * PI / 180.0);
		matrix[5] = 0;
		applyMatrix((*currentCurve));
		renderDeCasteljau();
		break;
	case 1:
		float scaleX;
		std::cout << "Scaling X :  " << std::endl;
		std::cin >> scaleX;
		float scaleY;
		std::cout << "Scaling Y :  " << std::endl;
		std::cin >> scaleY;
		matrix[0] = scaleX;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 0;
		matrix[4] = scaleY;
		matrix[5] = 0;
		applyMatrix((*currentCurve));
		renderDeCasteljau();
		break;
	case 2:
		float transX;
		std::cout << "Translate X :  " << std::endl;
		std::cin >> transX;
		float transY;
		std::cout << "Translate Y :  " << std::endl;
		std::cin >> transY;
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = transX;
		matrix[3] = 0;
		matrix[4] = 1;
		matrix[5] = transY;
		applyMatrix((*currentCurve));
		renderDeCasteljau();
		break;
	}
}

void menuStateChange(int status, int x, int y)
{
	if (status == 1)
	{
		mouseX = x;
		mouseY = y;
	}
}


void selectPoint()
{
	int curveNumber;
	std::cout << "Numero de la courbe entre 0 et " << curves.size() << std::endl;
	std::cin >> curveNumber;
	if (curveNumber > curves.size())
	{
		std::cout << "Valuer invalide" << std::endl;
	}
	else if (curveNumber == curves.size())
	{
		int pointNumber;
		std::cout << "Numero de la courbe entre 0 et " << (*currentCurve).size() - 1  << std::endl;
		std::cin >> pointNumber;
		if (pointNumber >= 0 && pointNumber < (*currentCurve).size())
		{
			currentPoint = &(*currentCurve).at(pointNumber);
			std::cout << "Vous pouvez repositionner le point" << std::endl;
		}
		else
		{
			std::cout << "Valuer invalide" << std::endl;
		}
	}
	else
	{
		int pointNumber;
		std::cout << "Numero de la courbe entre 0 et " << (*curves.at(curveNumber)).size() << std::endl;
		std::cin >> pointNumber;
		if (pointNumber >= 0 && pointNumber < (*curves.at(curveNumber)).size())
		{
			currentPoint = &(*curves.at(curveNumber)).at(pointNumber);
			std::cout << "Vous pouvez repositionner le point" << std::endl;
		}
		else
		{
			std::cout << "Valuer invalide" << std::endl;
		}
	}
}

void mainMenu(int index)
{
	switch (index)
	{
	case 0:
		newCurve();
		break;
	case 1:
		selectPoint();
		break;
	}
}

void createMenu()
{
	int menuIndex = glutCreateMenu(mainMenu);
	//couleur menu
	int colorSubmenu = glutCreateMenu(nullptr);
	int controlColorSubmenu = glutCreateMenu(controlColorMenu);
	glutAddMenuEntry("Rouge", 0);
	glutAddMenuEntry("Vert", 1);
	glutAddMenuEntry("Bleu", 2);
	glutAddMenuEntry("Jaune", 3);
	glutAddMenuEntry("Violet", 4);
	glutAddMenuEntry("Cyan", 5);
	glutAddMenuEntry("Blanc", 6);
	glutAddMenuEntry("Orange", 7);
	int curveColorSubmenu = glutCreateMenu(curveColorMenu);
	glutAddMenuEntry("Rouge", 0);
	glutAddMenuEntry("Vert", 1);
	glutAddMenuEntry("Bleu", 2);
	glutAddMenuEntry("Jaune", 3);
	glutAddMenuEntry("Violet", 4);
	glutAddMenuEntry("Cyan", 5);
	glutAddMenuEntry("Blanc", 6);
	glutAddMenuEntry("Orange", 7);
	int currentControlColorSubmenu = glutCreateMenu(currentControlColorMenu);
	glutAddMenuEntry("Rouge", 0);
	glutAddMenuEntry("Vert", 1);
	glutAddMenuEntry("Bleu", 2);
	glutAddMenuEntry("Jaune", 3);
	glutAddMenuEntry("Violet", 4);
	glutAddMenuEntry("Cyan", 5);
	glutAddMenuEntry("Blanc", 6);
	glutAddMenuEntry("Orange", 7);
	int currentCurveColorSubmenu = glutCreateMenu(currentCurveColorMenu);
	glutAddMenuEntry("Rouge", 0);
	glutAddMenuEntry("Vert", 1);
	glutAddMenuEntry("Bleu", 2);
	glutAddMenuEntry("Jaune", 3);
	glutAddMenuEntry("Violet", 4);
	glutAddMenuEntry("Cyan", 5);
	glutAddMenuEntry("Blanc", 6);
	glutAddMenuEntry("Orange", 7);
	//clear menu
	int clearSubMenu = glutCreateMenu(clearMenu);
	glutAddMenuEntry("Effacer tout", 0);
	glutAddMenuEntry("Effacer le dernier point", 1);
	int stepsSubMenu = glutCreateMenu(stepsMenu);
	glutAddMenuEntry("Diminuer le pas", 0);
	glutAddMenuEntry("Augmenter le pas", 1);
	int tranformSubMenu = glutCreateMenu(transformMenu);
	glutAddMenuEntry("Rotation", 0);
	glutAddMenuEntry("Scaling", 1);
	glutAddMenuEntry("Translation", 2);
	int continuitysSubMenu = glutCreateMenu(continuityMenu);
	glutAddMenuEntry("C0", 0);
	glutAddMenuEntry("C1 (r1 == r2)", 1);
	glutAddMenuEntry("C2 (r1 == r2)", 2);
	glutAddMenuEntry("C1 (r1 != r2)", 3);
	glutAddMenuEntry("C2 (r1 != r2)", 4);
	//menu principal
	glutSetMenu(colorSubmenu);
	glutAddSubMenu("Control points", controlColorSubmenu);
	glutAddSubMenu("Curve points", curveColorSubmenu);
	glutAddSubMenu("Current control points", currentControlColorSubmenu);
	glutAddSubMenu("Current curve points", currentCurveColorSubmenu);
	glutSetMenu(menuIndex);
	glutAddMenuEntry("Create new curve", 0);
	glutAddMenuEntry("Select point", 1);
	glutAddSubMenu("Transformation", tranformSubMenu);
	glutAddSubMenu("Couleurs", colorSubmenu);
	glutAddSubMenu("Raccord", continuitysSubMenu);
	glutAddSubMenu("Pas", stepsSubMenu);
	glutAddSubMenu("Effacer", clearSubMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMenuStatusFunc(menuStateChange);
}

int main(int argc, char* argv[])
{
	minCurrentX = minCurrentY = -1;
	showControlPoints = true;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowSize(1600, 900);
	glutCreateWindow("Window");
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0, 0.0, 1.0);
#ifdef FREEGLUT
	// Note: glutSetOption n'est disponible qu'avec freeGLUT
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
	currentCurve = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
	glutMouseFunc(onClick);
	glutKeyboardFunc(onKeyPress);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
	glFlush();
	curve_b = 1.0f;
	control_r = 1.0f;
	current_control_g = 1.0f;
	current_curve_r = 1.0f;
	current_curve_g = 1.0f;
	createMenu();
	glutDisplayFunc(renderLite);
	glutMainLoop();
	return 0;
}