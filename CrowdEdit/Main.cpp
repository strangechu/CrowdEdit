#include <GL/glut.h>
#include <cmath>
#include <vector>
#include "../RVO/RVO.h"

// angle of rotation for the camera direction
float angle = 0.0;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// position of the camera
float x = 0.0f, y = 0.0f, z = 240.0f;

float width = 0.0f;
float height = 0.0f;

RVO::RVOSimulator *sim;

std::vector<RVO::Vector2> goals;

void drawAgent(RVO::Vector2 pos) {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(pos.x(), pos.y(), 0.0f);
	glutSolidSphere(1.f, 10, 10);
	glPopMatrix();
}

void addAgent(RVO::Vector2 pos, RVO::Vector2 goal) {
	sim->addAgent(pos);
	goals.push_back(goal);
}

void renderScene(void) {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(x, y, z,
		x, y, z - 100.0f,
		0.0f, 1.0f, 0.0f);

	// Draw floor
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, -100.0f, 0.0f);
	glVertex3f(-100.0f, 100.0f, 0.0f);
	glVertex3f(100.0f, 100.0f, 0.0f);
	glVertex3f(100.0f, -100.0f, 0.0f);
	glEnd();

	for (size_t i = 0; i < sim->getNumAgents(); i++) {
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);
		goalVector = RVO::normalize(goalVector) * 10.0f;
		sim->setAgentPrefVelocity(i, goalVector);
	}

	sim->doStep();

	for (size_t i = 0; i < sim->getNumAgents(); i++) {
		drawAgent(sim->getAgentPosition(i));
	}

	glutSwapBuffers();
}

RVO::Vector2 mouse2DPick(int mouse_x, int mouse_y) {
	float x = ((2.0f * mouse_x) / width - 1.0f) * 100.0f;
	float y = (1.0f - (2.0f * mouse_y) / height) * 100.0f;
	return RVO::Vector2(x, y);
}

void mainLoop(void) {
	renderScene();
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

	width = w;
	height = h;
}

void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_LEFT:
		x -= 10.0f;
		break;
	case GLUT_KEY_RIGHT:
		x += 10.0f;
		break;
	case GLUT_KEY_UP:
		y += 10.0f;
		break;
	case GLUT_KEY_DOWN:
		y -= 10.0f;
		break;
	}
}

void processMouse(int button, int state, int x, int y) {
	RVO::Vector2 pos = mouse2DPick(x, y);
	switch (button) {
	case GLUT_LEFT_BUTTON:
		addAgent(pos, -pos);
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	sim = new RVO::RVOSimulator();

	sim->setTimeStep(0.25f);

	sim->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);
	
	// add agents
	for (int i = 0; i < 200; i++) {
		RVO::Vector2 start = 100.0f * RVO::Vector2(std::cos(i * 2.0f * 3.14 / 20.0f), std::sin(i * 2.0f * 3.14 / 20.0f));
		addAgent(start, -start);
	}


	// init GLUT and create window

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 640);
	glutCreateWindow("CrowdEdit");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(mainLoop);
	//glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(processMouse);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}