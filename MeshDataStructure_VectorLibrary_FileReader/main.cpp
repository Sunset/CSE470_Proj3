#include <stdlib.h>
#include <stdio.h>
#include "glut.h"

#include <fstream>
using namespace std;

#include "loaders.h"
#include "mesh.h"

#define MIN2(a,b) (((a) < (b))?(a):(b))
#define MAX2(a,b) (((a) > (b))?(a):(b))

// window parameters
GLint winId;
int position_x		= 0;
int position_y		= 0;
int width			= 800;
int height			= 800;

// controling parameters
int mouse_button;
int mouse_x		= 0;
int mouse_y		= 0;
float scale		= 1.0;
float x_angle	= 0.0;
float y_angle	= 0.0;

// perspective projection parameters
GLfloat fieldofview = 45.0;
GLfloat aspect      = 1.0;
GLfloat nearPlane   = 0.1;
GLfloat farPlane    = 100.0;

// lighting parameters
GLfloat AmbientLight[]		= { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat DiffuseLight[]		= { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat SpecularLight[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPos[]			= {-50.0f,50.0f,100.0f,1.0f};

// scene parameters
Vec3f bbxMin, bbxMax, bbxCenter;

// mesh model
Mesh* model = 0;

// models
static const char* modelfile[] = {
	"../Models/Required/al.obj",
	"../Models/Required/cars01_obj/Car_01.obj",
	"../Models/Required/cars14_obj/Car_14.obj",
};

/* 
 * Facilities for drawing models. The functions below show:
 * - How to use ILM(Industrial Light & Magic)'s math library
 * - Mesh data structure
 * - How to draw the mesh in immediate mode
 */

/* 
 * Compute the bounding box of the model
 */

void calc_bbox(Vec3f& bbxmin, Vec3f& bbxmax)
{
	vector<Vec3f>& v = model->m_v;

	if(v.empty()) return;		

	bbxmax = bbxmin= v[0];

	for(unsigned int i = 1;i < v.size();i++)
	{	
		bbxmin.x = MIN2(bbxmin.x, v[i].x);
		bbxmin.y = MIN2(bbxmin.y, v[i].y);
		bbxmin.z = MIN2(bbxmin.z, v[i].z);

		bbxmax.x = MAX2(bbxmax.x, v[i].x);
		bbxmax.y = MAX2(bbxmax.y, v[i].y);
		bbxmax.z = MAX2(bbxmax.z, v[i].z);
	}
}

/* 
 * Load a model from a file
 */

void load_model(const char* name)
{
	if(model) delete model;
	model = ObjLoader::load(name);
	calc_bbox(bbxMin, bbxMax);
	bbxCenter = (bbxMin+bbxMax)*0.5f;
}

/* 
 * Draw the model in immediate mode
 */

void draw_model()
{
	// data references for convenience purpose
	vector<int>& vi = model->m_vi;
	vector<int>& ni = model->m_ni;
	vector<int>& ti = model->m_ti;
	vector<Vec3f>& v = model->m_v;
	vector<Vec3f>& n = model->m_n;
	vector<Vec2f>& t = model->m_texc;

	// drawing in immediate mode
	glBegin(GL_TRIANGLES);

	for(unsigned int i = 0;i < vi.size();i++)
	{
		// normals and texture coordiates is not used.
		// to achieve lighting, per vertex normal has to be attached
		// *** Note: normals are sometimes absent from the data.You have to create
		// *** them yourself

		/*
		if(!n.empty() && !ni.empty())
			glNormal3fv(&n[ni[i]].x);

		if(!t.empty() && !ti.empty())
			glTexCoord2fv(&t[ti[i]].x);
		*/

		// assign some funny color to each vertex
		Vec3f offset = (v[vi[i]] - bbxCenter);;
		glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));

		glVertex3fv(&v[vi[i]].x);
	}

	glEnd();
}

// ================================================================================
// Main program functions
void renderBitmapString(float x, float y, float z, char *string)
{
   char *c;
   
   glRasterPos3f(x, y, z);   // fonts position
   for(c=string; *c != '\0'; c++)
     glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

void shut()
{
	if(model) delete model;
}

void modelMenu (int menuId)
{
	load_model(modelfile[menuId]);

	// adjust the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	farPlane = nearPlane+4.0f*(bbxMax-bbxMin).length();
	gluPerspective(fieldofview, aspect, nearPlane, farPlane);

	// adjust the viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float cz = 2.0f*(bbxMax.z - bbxMin.z);

	gluLookAt(0.0f, 0.0f, cz, 
		0.0f, 0.0f, cz - 1.0f, 
		0.0f, 1.0f, 0.0f);

	glutPostRedisplay();
}

void menu(int menuId)
{
	if(menuId == 0)
	{
		glutDestroyWindow(winId);
		shut();
		exit(0);
	}

	glutPostRedisplay();
}

void glutMenus (void)
{	
	GLint subMenu_models = glutCreateMenu(modelMenu);
	glutAddMenuEntry("Al", 0);
	glutAddMenuEntry("Car01", 1);
	glutAddMenuEntry("Car14", 2);

	glutCreateMenu(menu);
	glutAddSubMenu("Models", subMenu_models);
	glutAddMenuEntry("Quit", 0);	

	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void init()
{	
	// use black to clean background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// always re-normalize normal (due to scale transform)
	glEnable(GL_NORMALIZE);

	// lighting (disabled)
	// To achieve lighting effect
	//	- enable lighting
	//	- setup lighting parameters
	//	- setup materials
	//	- attach normals to each vertex
	
	glDisable(GL_LIGHTING);
	 
	/*glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glEnable(GL_LIGHT0);*/	

	// smooth shading
	glShadeModel(GL_SMOOTH);

	// load the first model by default
	load_model(modelfile[0]);
	
	// setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspect = (double)width/(double)height;
	farPlane = nearPlane+4.0f*(bbxMax-bbxMin).length();
	gluPerspective(fieldofview, aspect, nearPlane, farPlane);

	// setup viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float cz = 2.0f*(bbxMax.z - bbxMin.z);

	gluLookAt(0.0f, 0.0f, cz, 
		0.0f, 0.0f, cz - 1.0f, 
		0.0f, 1.0f, 0.0f);

	// set up menus
	glutMenus();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(-bbxCenter.x, -bbxCenter.y, -bbxCenter.z);

	draw_model();

	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,width, 0, height);	

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, height-13.0f, 0.0f, "Use [Mouse Left Key] to rotate");	
	renderBitmapString(0.0, height-26.0f, 0.0f, "Use [Mouse Right Key] to scale");
	renderBitmapString(0.0, height-39.0f, 0.0f, "Press [Mouse Middle key] to summon out menu");
		

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}


void mouse(int button, int state, int x, int y)
{
	// button -- GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON.
	// state  -- GLUT_UP, GLUT_DOWN

	mouse_x = x;
	mouse_y = y;

	mouse_button = button;
}


void reshape(int x, int y)
{
    width  = x;
	height = y;

	if (height == 0)		// not divided by zero
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0,0,width,height);

	aspect = (float)width/(float)height;

	gluPerspective(fieldofview, aspect, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);

}

void motion(int x, int y)
{
	if(mouse_button == GLUT_LEFT_BUTTON)
	{
		// compute the angle (0..360) around x axis, and y-axis
		y_angle += (float(x - mouse_x)/width)*360.0;
		x_angle += (float(y - mouse_y)/height)*360.0;

	}

	if(mouse_button == GLUT_RIGHT_BUTTON)
	{
		// scale
		scale += (y-mouse_y)/100.0;
			
		if(scale < 0.1) scale = 0.1;     // too small
		if(scale > 7)	scale = 7;		 // too big

	}

	mouse_x	= x;		// update current mouse position
	mouse_y	= y;
	glutPostRedisplay();

}

void main(int argc, char* argv[])
{
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(position_x,position_y);
	glutInitWindowSize(width,height);
	winId = glutCreateWindow("Mesh Viewer");
	
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);		// display function

	init();

	glutMainLoop();

	shut();
}






