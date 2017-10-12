#include <GL/glut.h>
#include <math.h>
#include "../RVO/RVO.h"

// angle of rotation for the camera direction
float angle = 0.0;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// position of the camera
float x = 0.0f, y = 0.0f, z = 200.0f;

RVO::RVOSimulator *sim;

void drawAgent(RVO::Vector2 pos) {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(pos.x(), pos.y(), 0.0f);
	glutSolidSphere(1.f, 10, 10);
	glPopMatrix();
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

	RVO::Vector2 goal = RVO::Vector2(100.0f, 100.0f);

	RVO::Vector2 goalVector = goal - sim->getAgentPosition(0);

	goalVector = RVO::normalize(goalVector);
	sim->setAgentPrefVelocity(0, goalVector);

	sim->doStep();

	for (size_t i = 0; i < sim->getNumAgents(); i++) {
		drawAgent(sim->getAgentPosition(i));
	}

	/*
	for (int i = -10; i <= 10; i++) {
		for (int j = -10; j <= 10; j++) {
			glPushMatrix();
			glColor3f(1.0f, 0.0f, 0.0f);
			glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(i * 10.0f, j * 10.0f, 0.0f);
			glutSolidSphere(1.f, 10, 10);
			glPopMatrix();
		}
	}
	*/


	glutSwapBuffers();
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

int main(int argc, char *argv[])
{
	sim = new RVO::RVOSimulator();

	sim->setTimeStep(0.25f);

	sim->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);
	sim->addAgent(RVO::Vector2(0.0f, 0.0f));


	// init GLUT and create window

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("CrowdEdit");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(mainLoop);
	//glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}