//=============================================================================
// Sample Application for GLEW, and cwc Application/Window class using freeglut
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include "mesh.h"
//-----------------------------------------------------------------------------
Mesh* plane= new Mesh;
extern "C" unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);

vector<float>forwardDirection;
vector<float>centerPos;
vector<float>cameraPos;
float h,v=0.0;

float cameraAngle=0.0f;
GLUquadric* quadratic=gluNewQuadric();

vector<float> zdir;

bool isCheckered = false;
bool isStriped = false;
//silver parameters
GLfloat SilverAmbientMaterial[]	= {0.19225f, 0.19225f, 0.19225f,  1.0f};
GLfloat SilverDiffuseMaterial[]	= {0.50754f, 0.50754f, 0.50754f,  1.0f};
GLfloat SilverSpecularMaterial[]	= { 0.508273f, 0.508273f, 0.508273f, 1.0f};
GLfloat SilverShininessMaterial[]	= {51.2};
//Gold material
GLfloat GoldAmbientMaterial[]	= {0.24725, 0.1995, 0.0745, 1.0};
GLfloat GoldDiffuseMaterial[]	= {0.75164, 0.60648, 0.22648, 1.0};
GLfloat GoldSpecularMaterial[]	= {0.628281, 0.555802, 0.366065, 1.0};
GLfloat GoldShininessMaterial[]	= {51.2};

//green plastic
GLfloat GPAmbientMaterial[]	= {0.0,1.0,0.0,0.5};
GLfloat GPDiffuseMaterial[]	= {0.1,0.35,0.1};
GLfloat GPSpecularMaterial[]	= {0.45,0.55,0.45};
GLfloat GPShininessMaterial[]	= {1.0};
class myWindow : public cwc::glutWindow
{

protected:
   cwc::glShaderManager SM;
   cwc::glShader *shader;
   cwc::glShader *strips;
   cwc::glShader *checkered;
   cwc::glShader *bump;
protected: 
	int scene;

	//Ross: Some variables I created for texturing
	unsigned char *image;
	unsigned char *doge;
	unsigned char *kirby;
	int imageWidth;
	int imageHeight;
	int bitDepth;
	GLuint texture;

	GLuint kirbyTexture;
public:

	myWindow(){}
void GPMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, GPAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, GPDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,GPSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,GPShininessMaterial);
}
void silverMaterial()
{
	//material effect
	glMaterialfv(GL_FRONT,GL_AMBIENT, SilverAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, SilverDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,SilverSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,SilverShininessMaterial);
}
void GoldMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, GoldAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, GoldDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,GoldSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,GoldShininessMaterial);
}
void draw_otherPartsOfHead()
	{
		//nose
	glPushMatrix();
	glTranslatef(0.3f,0.0f,0.0f);
	glColor3f(1.0f,1.0f,1.0f);
	glScalef(2.0f,1.0f,1.0f);
	gluSphere(quadratic,0.1,200,200);
	glPopMatrix();

	//eyeballs
	glPushMatrix();
	glTranslatef(0.25f,0.2f,0.14f);
	gluSphere(quadratic,0.05,200,200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.25f,0.2f,-0.14f);
	gluSphere(quadratic,0.05,200,200);
	glPopMatrix();
	
	//ears
	glPushMatrix();
	glTranslatef(0.05,0.2,0.2);
	glScalef(1.0,2.0,2.0);
	gluSphere(quadratic,0.08,200,200);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.05,0.2,-0.2);
	glScalef(1.0,2.0,2.0);
	gluSphere(quadratic,0.08,200,200);
	glPopMatrix();
	}
void draw_Head()
{
	//head
	gluSphere(quadratic,0.35,200,200);
}
void draw_leg()
{glRotatef(90,0,1,0);
	if(isStriped==true)
	{
		if (strips) strips->begin(); 
			glColor3f(0.5,0.5,0.5);
			gluSphere(quadratic,0.2,200,200);	//upperleg
			glTranslatef(0.0f,-0.3f,-0.0f);
			gluSphere(quadratic,0.2,200,200);	//lower leg
		if (strips) strips->end();
	}
	else
	{
		glColor3f(0.5,0.5,0.5);
			gluSphere(quadratic,0.2,200,200);	//upperleg
			glTranslatef(0.0f,-0.3f,-0.0f);
			gluSphere(quadratic,0.2,200,200);	//lower leg
	}
}
void draw_tail()
{
	glRotatef(90,0,1,0);
	glColor3f(0.24f,0.24f,0.24f);
	gluSphere(quadratic,0.2,200,200);
}
void draw_torso()
{		
	
	glBindTexture(GL_TEXTURE_2D, this->texture);
	
	//drawing torso
	glColor3f(0.0f,1.0f,1.0f);
	gluSphere(quadratic,0.5,200,200);
	glColor3f(0.0f,1.0f,0.0f);
	gluQuadricTexture(quadratic,true);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.0,2.0,1.0);
	glRotatef(90,0.0,0.0,1.0);
//	glRotatef(-15,0.0,1.0,0.0);
//	glRotatef(24,0.8,0.0,0.0);

	//glTranslatef(-0.1,-0.1,0.0);
	glMatrixMode(GL_MODELVIEW);

	if (shader) shader->begin();
	//drawing leg
	
	glPushMatrix();
	glTranslatef(0.3f ,-0.45f, -0.3f);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.3f ,-0.45f, 0.3f);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3f ,-0.45f, -0.3f);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3f ,-0.45f, 0.3f);
	draw_leg();
	glPopMatrix();

	//draw head
	glPushMatrix();
	glRotatef(90,0,1,0);
	glTranslatef(0.4f,0.4f,0.0f);
	draw_Head();
	draw_otherPartsOfHead();
	glPopMatrix();
	
	//draw tail
	glPushMatrix();
	glRotatef(90,0,1,0);
	glTranslatef(-0.4f,0.3f,0.0f);
	draw_tail();
	glPopMatrix();
	if (shader) shader->end();
}
void draw_doggie()
{
	draw_torso();
}
void planeCreation(){
    //verticies 
    plane->m_v.push_back(V3f(-5,-2,5)); 
    plane->m_v.push_back(V3f(5,-2,5));    
    plane->m_v.push_back(V3f(5,-2,-5)); 
    plane->m_v.push_back(V3f(-5,-2,-5)); 
  
    plane->textureCoordinates.push_back(V2f(0,1)); 
    plane->textureCoordinates.push_back(V2f(1,1)); 
    plane->textureCoordinates.push_back(V2f(1,0)); 
    plane->textureCoordinates.push_back(V2f(0,0));
}
void createGround() 
{    
    glBegin(GL_TRIANGLES); 
  
    for(unsigned int i = 0;i < plane->m_v.size();i++) 
    { 
        glNormal3f(0,1,0); 
        glColor3f(0.0,0.0,1.0); 
        glVertex3fv(&plane->m_v[i].x); 
        glTexCoord2fv(&plane->textureCoordinates[i].x); 
    } 
    //printf("%s","Hello"); 
    glEnd(); 
}
virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Ross:  This is where I draw things.  I have three different scene examples.
		switch(scene)
		{
			case 1:
				Scene1();
				break;
			case 2:
				Scene2();
				break;
			case 3:
				Scene3();
				break;
		}
		
		glutSwapBuffers();
	}

void Scene1(void) 
    { 
        glMatrixMode(GL_PROJECTION); 
        glLoadIdentity(); 
      
       gluPerspective(45,1,1,100); 
       glMatrixMode(GL_MODELVIEW); 
       glLoadIdentity(); 
       gluLookAt(cameraPos.at(0),cameraPos.at(1),cameraPos.at(2),  
           centerPos.at(0),centerPos.at(1),centerPos.at(2), 
                   0.0f,1.0f,0.0f); 
         
            glTranslatef(0.0f,0.0f,-1.0f); 
            if (shader) shader->begin(); 
			if(isCheckered==true)
			{
				if(checkered)checkered->begin();
				planeCreation();
				createGround();
				if(checkered)checkered->end();
			}
			else
			{

				planeCreation();
				createGround();

			}
			if (shader) shader->end(); 
			glRotatef(90,0.0f,1.0f,0.0f); 
            draw_doggie(); 

    }

void Scene2(void)
	{
		glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	
       gluPerspective(45,1,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(cameraPos.at(0),cameraPos.at(1),cameraPos.at(2), 
		   centerPos.at(0),centerPos.at(1),centerPos.at(2),
			       0.0f,1.0f,0.0f);
	   glEnable(GL_BLEND);
			
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);

			GoldMaterial();
			glTranslatef(0.0f,0.0f,-2.0f);
			glutSolidTorus(0.4,0.9,40,40);
			silverMaterial();
			glTranslatef(0.0f,0.0f,2.0f);

			draw_doggie();
		glDisable(GL_BLEND);
		if (shader) shader->begin();
			if(isCheckered==true)
			{
				if(checkered)checkered->begin();
				planeCreation();
				createGround();
				if(checkered)checkered->end();
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, this->texture);
				planeCreation();
				createGround();
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		if (shader) shader->end();
	}

void Scene3(void)
	{
		glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	
       gluPerspective(45,1,1,100);
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluLookAt(cameraPos.at(0),cameraPos.at(1),cameraPos.at(2), 
		   centerPos.at(0),centerPos.at(1),centerPos.at(2),
			       0.0f,1.0f,0.0f);
	   if(bump) bump->begin();
		glBindTexture(GL_TEXTURE_2D,this->kirbyTexture);
		gluSphere(quadratic,0.3,20,20);
		glBindTexture(GL_TEXTURE_2D, 0);
		if(bump) bump->end();
		if (shader) shader->begin();
		if(isCheckered==true)
			{
				if(checkered)checkered->begin();
				planeCreation();
				createGround();
				if(checkered)checkered->end();
			}
		else
			{
				glBindTexture(GL_TEXTURE_2D, this->texture);
				planeCreation();
				createGround();
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		if (shader) shader->end();
			glTranslatef(-1.0f,0.0f,0.0f);
			glRotatef(90.0f,0.0f,1.0f,0.0f);
			
			gluCylinder(quadratic,0.2,0,0.5,20,20);
		
	}


virtual void OnIdle() {}

	// When OnInit is called, a render context (in this case GLUT-Window) 
	// is already available!
virtual void OnInit()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		forwardDirection.push_back(0.0f);
		forwardDirection.push_back(0.0f);
		forwardDirection.push_back(0.0f);
		cameraPos.push_back(0.0f);
		cameraPos.push_back(0.0f);
		cameraPos.push_back(4.0f);
		centerPos.push_back(0.0f);
		centerPos.push_back(0.0f);
		centerPos.push_back(-1.0f);
		zdir.push_back(1.0f);
		zdir.push_back(0.0f);
		zdir.push_back(0.0f);
		//Ross: Here is where I declare some variables you need for textures (not scene, that's for changing the scene)
		this->scene = 1;
		this->imageWidth = 128;
		this->imageHeight = 128;
		this->bitDepth = 24;

		//Ross:  This is where I set up the lights
		DemoLight();

		//Ross:  This is where I set up the textures
		DemoTexture();

		drawKirby();
		//Ross: The GLSL loaders initialization is already handled by the CWC files linked.
		//If you change the name of your shader you would need to modify here
		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
		strips = SM.loadfromFile("vertexshader.txt","StripeFragment.txt");
		checkered = SM.loadfromFile("vertexshader.txt","CheckerFragment.txt");
		bump = SM.loadfromFile("vertexshader.txt","fragmentShaderBump.txt");
		
		  if (shader==0) 
			  std::cout << "Error Loading, compiling or linking shader\n";
			if (strips==0) 
			  std::cout << "Error Loading, compiling or linking shader\n";
			if (checkered==0) 
			  std::cout << "Error Loading, compiling or linking shader\n";
			if (bump==0) 
			  std::cout << "Error Loading, compiling or linking shader\n";
	}

	//Ross: Callbacks for most everything you need
virtual void OnResize(int w, int h) {}
virtual void OnClose(void){}
virtual void OnMouseDown(int button, int x, int y) {}    
virtual void OnMouseUp(int button, int x, int y) {}
virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}

//Ross: This should be obvious, but these are the keyboard listeners
virtual void OnKeyDown(int nKey, char cAscii)
{       
	if (cAscii == 27) // 0x1b = ESC
	{
		this->Close(); // Close Window!
	} 

	if(cAscii == '1')
		this->scene = 1;
	if(cAscii == '2')
		this->scene = 2;
	if(cAscii == '3')
		this->scene = 3;

	glutPostRedisplay();
};

virtual void OnKeyUp(int nKey, char cAscii)
{

	if (cAscii == 'f')
	{
		SetFullscreen(true);
	}
	else if (cAscii == 'w')
	{
		SetFullscreen(false);
	}
	else if(cAscii=='a')	//up
	{
		cameraPos.at(1)+=1.0;
		centerPos.at(1)+=1.0;
	}
	else if(cAscii=='s')	//down
	{
			
		cameraPos.at(1)-=1.0;
		centerPos.at(1)-=1.0;
	}
	else if(nKey==GLUT_KEY_DOWN)	//back
	{
		forwardDirection.at(0)=(centerPos.at(0)-cameraPos.at(0))/10.0;
		forwardDirection.at(1)=(centerPos.at(1)-cameraPos.at(1))/10.0;
		forwardDirection.at(2)=(centerPos.at(2)-cameraPos.at(2))/10.0;
		
		centerPos.at(0)=centerPos.at(0)-forwardDirection.at(0);
		centerPos.at(1)=centerPos.at(1)-forwardDirection.at(1);
		centerPos.at(2)=centerPos.at(2)-forwardDirection.at(2);

		cameraPos.at(0)=cameraPos.at(0)-forwardDirection.at(0);
		cameraPos.at(1)=cameraPos.at(1)-forwardDirection.at(1);
		cameraPos.at(2)=cameraPos.at(2)-forwardDirection.at(2);
	}
	else if(nKey==GLUT_KEY_UP)	//forward
	{
		forwardDirection.at(0)=(centerPos.at(0)-cameraPos.at(0))/10.0;
		forwardDirection.at(1)=(centerPos.at(1)-cameraPos.at(1))/10.0;
		forwardDirection.at(2)=(centerPos.at(2)-cameraPos.at(2))/10.0;
		
		centerPos.at(0)=centerPos.at(0)+forwardDirection.at(0);
		centerPos.at(1)=centerPos.at(1)+forwardDirection.at(1);
		centerPos.at(2)=centerPos.at(2)+forwardDirection.at(2);

		cameraPos.at(0)=cameraPos.at(0)+forwardDirection.at(0);
		cameraPos.at(1)=cameraPos.at(1)+forwardDirection.at(1);
		cameraPos.at(2)=cameraPos.at(2)+forwardDirection.at(2);
	}
	else if(nKey==GLUT_KEY_RIGHT)	//rotate right
	{
		cameraAngle+=0.2;
		centerPos.at(0)=sin(cameraAngle)*5;
		centerPos.at(2)=-cos(cameraAngle)*5;
	}
	else if(nKey==GLUT_KEY_LEFT)	//rotate left
	{
		cameraAngle-=0.2;
		centerPos.at(0)=sin(cameraAngle)*5;
		centerPos.at(2)=-cos(cameraAngle)*5;
	}
	else if(cAscii=='p')
	{
		if(isCheckered==false)
			isCheckered=true;
		else
			isCheckered=false;
	}
	else if(cAscii=='o')
	{
		if(isStriped==false)
		{
			isStriped=true;
		}
		else
			isStriped=false;
	}
	glutPostRedisplay();
};

void drawKirby(void)
{
	/* Load in a PNG image */
	int loadCorrectly = 0;
	//Use the stbi_image file to load an image
	//image = stbi_load("Frodo.bmp", &this->imageWidth, &this->imageHeight, &this->bitDepth, 0);
	kirby = stbi_load("kirby.bmp",&this->imageWidth,&this->imageHeight,&this->bitDepth,0);
	glGenTextures(1, &this->kirbyTexture); //generate the texture with the loaded data

	glBindTexture(GL_TEXTURE_2D, kirbyTexture); //bind texture to its array

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, kirby);
	glEnable (GL_TEXTURE_2D);
}
//Ross: This is where I set up all the textures
void DemoTexture(void)
{
	/* Load in a PNG image */
	int loadCorrectly = 0;
	//Use the stbi_image file to load an image
	//image = stbi_load("Frodo.bmp", &this->imageWidth, &this->imageHeight, &this->bitDepth, 0);
	doge = stbi_load("Doge.bmp",&this->imageWidth,&this->imageHeight,&this->bitDepth,0);
	glGenTextures(1, &this->texture); //generate the texture with the loaded data

	//The first thing that must take place in the process of uploading the texture is a call to glBindTexture. 
	//What glBindTexture does is it tells OpenGL which texture "id" we will be working with. 
	//A texture "id" is just a number that you will use to access your textures. Here is a sample call.
	glBindTexture(GL_TEXTURE_2D, texture); //bind texture to its array


	//The glPixelStorei call tells OpenGL how the data that is going to be uploaded is aligned. 
	//This call tells OpenGL that the pixel data which is going to be passed to it is aligned in byte order, 
	//this means that the data has one byte for each component, one for red, green and blue
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	//The glTexEnvf call sets environment variables for the current texture. 
	//What this does is tell OpenGL how the texture will act when it is rendered into a scene.
	//What this does is sets the active texture to GL_MODULATE. 
	//The GL_MODULATE attribute allows you to apply effects such as lighting and coloring to your texture. 
	//If you do not want lighting and coloring to effect your texture and you would like to display the texture unchanged when 
	//coloring is applied replace GL_MODULATE with GL_DECAL.
	//This is not needed if you use shaders!!
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//The glTexImage2D call is our goal. 
	//This call will upload the texture to the video memory where it will be ready for us to use in our programs. 
	//I am going to explain this call parameter by parameter since it is so important to what we are doing.
	//target - The target of this call, it will always be GL_TEXTURE_2D.
	//level - The level of detail number, this should be left at 0 for our purposes. 
		//Once you become more adept at OpenGL texture mapping this parameter will be something that you might change.
	//internalformat - Internal components parameter. 
		//This tells OpenGL how many color components to represent internally from the texture that is uploaded. 
		//There are many symbolic constants for this parameter but the one which is most widely used is GL_RGB; this constant is equal to 3.
	//width & height - The width and height of the image data. These must be integers that are equal to 2n+2(border) for some integer n.
		//What this basically means is that the texture width and height must be a power of two (2,4,8,16,32,63,128,256,512, etc).
	//border - Image border, must be 0 or 1. I always use 0 in my code since I do not use image borders.
	//format - Format of the pixel data that will be uploaded. There are many constants which are accepted but GL_RGB is the value that is widely used.
	//type - Type of data that will be uploaded. Again there are several symbolic constants but the one which I use is GL_UNSIGNED_BYTE.
	//pixels - Pointer to the image data. 
		//This is the image data that will be uploaded to the video memory. 
		//Note that after your call to glTexImage2D you can free this memory since the texture is already uploaded into video memory.
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, doge);
	glEnable (GL_TEXTURE_2D);
}

//Ross: This is where I set up the lights
void DemoLight(void)
   {
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
     
     // Light model parameters:
     // -------------------------------------------
     
     GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
     glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
     glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
     // -------------------------------------------
     // Spotlight Attenuation
     
     GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
     GLint spot_exponent = 30;
     GLint spot_cutoff = 180;
     
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
     glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
     glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
     GLfloat Kc = 1.0;
     GLfloat Kl = 0.0;
     GLfloat Kq = 0.0;
     
     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
     // ------------------------------------------- 
     // Lighting parameters:

     GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
     GLfloat light_Ka[]  = {1.0f, 0.5f, 0.5f, 1.0f};
     GLfloat light_Kd[]  = {0.0f, 0.0f, 1.0f, 1.0f};
     GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

     // -------------------------------------------
     // Material parameters:

     GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
     GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
     GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
     GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
     GLfloat material_Se = 20.0f;

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
   }
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	//Ross: Here is a simple output for checking that things are working
	virtual void OnInit() {std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------

