
#if _WIN32
#include <Windows.h>
#define FREEGLUT_LIB_PRAGMAS 0
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#endif

// Entete OpenGL 
#define GLEW_STATIC 1
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
int pas = 50;
float current_control_r, current_control_g, current_control_b, control_r, control_g, control_b, curve_r, curve_g, curve_b, current_curve_r, current_curve_g, current_curve_b;
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
			for (double t = 0; t <= 1; t += (1.0/pas))
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
		for (double t = 0; t <= 1; t += (1.0 / pas))
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
	deCateljau();
	glutPostRedisplay();
	glFlush();
}

void onClick(int button, int state, int x, int y)
{
	if (state)
	{
		if (button == 0)
		{
			currentCurve->push_back(Point(x, y));
			renderDeCasteljau();
		}
		else if (button == 1)
		{
			curves.clear();
			currentCurve = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();
			glFlush();
		}
		if (button == 2)
		{
			curves.push_back(std::move(currentCurve));
			currentCurve = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
			renderDeCasteljau();
		}
	}
}

void onKeyPress(unsigned char key, int x, int y)
{
	if (key == '+')
		pas++;
	if (key == '-' && pas > 1)
		pas--;
	if (key == 'x' && (*currentCurve).size() > 0) // C0
	{
		std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
		Point p = (*currentCurve).back();
		(*tmp).push_back(Point(p.x, p.y));
		curves.push_back(std::move(currentCurve));
		currentCurve = std::move(tmp);
	}
	if (key == 'c' && (*currentCurve).size() > 1) // C1
	{
		std::unique_ptr<std::vector<Point>> tmp = std::unique_ptr<std::vector<Point>>(new std::vector<Point>);
		Point p = (*currentCurve).back();
		Point p2 = (*currentCurve).at((*currentCurve).size() - 2);
		(*tmp).push_back(Point(p.x, p.y));
		(*tmp).push_back(Point(p.x + (p.x - p2.x), p.y + (p.y - p2.y)));
		curves.push_back(std::move(currentCurve));
		currentCurve = std::move(tmp);
	}
	if (key == 'v' && (*currentCurve).size() > 2) // C2
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
	}
	if (key == 'd' && (*currentCurve).size() > 1) // C1
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
	}
	if (key == 'f' && (*currentCurve).size() > 2) // C2
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
	}
	if (key == 'r' && (*currentCurve).size() > 0) // C0
	{
		(*currentCurve).pop_back();
	}
	renderDeCasteljau();
}

void renderLite() {}


int main(int argc, char* argv[])
{
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
	glutDisplayFunc(renderLite);
	glutMainLoop();
	return 0;
}