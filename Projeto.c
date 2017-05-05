#include<stdio.h>
#include<stdlib.h>
#include <GL/glu.h>
#include<GL/glut.h>
#include <math.h>

#define WIDTH 800
#define HEIGTH 600


#define HEADER_SIZE 54
#define COMPRESSION_OFFSET 0x1e
#define BPP_OFFSET 0X1c
//Capturar o arquivo desde onde ele começa
#define IMAGE_START_OFFSET 0x0a
//Determinar o tamanho da imagem
#define IMAGE_WIDTH_OFFSET 0x12
#define IMAGE_HEIGHT_OFFSET 0x16

void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void keyboard2(int key, int mouse_x, int mouse_y);

enum {TRANSLATE = 0, SCALE, ROTATE} transformMode;

GLuint texture_names[2];

int get_int(char* header, unsigned int offset){
	return (header[offset + 3]<<24)+
			(header[offset + 2]<<16)+
			(header[offset + 1]<<8)+
			(header[offset + 0]);
}

/* load the bmp file. */
unsigned char* load_bmp(char *file_name, int* width, int *height){
	void *data = NULL;
	
	FILE * fb = fopen(file_name, "rb");
	if(fb==NULL){
		goto out;
	}
	
	char header[HEADER_SIZE];
	if(fread(header, HEADER_SIZE, 1, fb)!=1){
		goto out;
	}
	
	if(header[0]!='B' || header[1]!='M'){
		goto out;
	}
	
	if(get_int(header, COMPRESSION_OFFSET)!=0 || get_int(header, BPP_OFFSET)!=24)
		goto out;
	
	*width = get_int(header, IMAGE_WIDTH_OFFSET);
	*height = get_int(header, IMAGE_HEIGHT_OFFSET);
	int image_size = 3*(*width)*(*height);
	data = malloc(image_size);
	fread(data, image_size, 1, fb);
	out:
		fclose(fb);
		return data;
}

static void
makeTexture(char *file_name){
	
	unsigned char *texture; 
	int width, height;
	
	texture = load_bmp(file_name, &width, &height);
	
	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
      0x80E0, GL_UNSIGNED_BYTE, texture);
      
      free(texture);
}

void init(void){
	glEnable(GL_DEPTH_TEST);
  	glEnable(GL_TEXTURE_2D);
  	
  	transformMode = TRANSLATE;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClearColor(0,0,0,1.0);
	gluPerspective(45.0f,(float)WIDTH/(float)HEIGTH,0.1f,100.0f);
	glTranslated(-1.0,0.0,-5.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glGenTextures(2, texture_names);
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
  	makeTexture("textura_da_superficie_1024x512.bmp");

  	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
  	makeTexture("altimetria_1024x512.bmp"); 
	
}

void drawGrid(int tex_id){
	
	glBindTexture(GL_TEXTURE_2D, texture_names[tex_id]);
	
	int N = 8; 	//Quantidade de linhas
	float DZ = 1; //Passo ou esperssura
	float DX = 1;
	int i, j;
	glBegin(GL_QUADS);
	for(i =0; i<N; i++){
		for(j=0; j<N; j++){
			float x = i*DX;
			float z = j*DZ;
			float y = 0;	
			float y_ = 0;
			glTexCoord2f(x/N, z/N); //Coordenada de Textura
			glVertex3f(x, y, z);
			glTexCoord2f(x/N, (z + DZ)/N); //Coordenada de Textura
			glVertex3f(x,y ,z +DZ);
			glTexCoord2f((x +DX)/N, (z + DZ)/N); //Coordenada de Textura
			glVertex3f(x +DX, y_, z + DZ);
			glTexCoord2f((x +DX)/N, z /N); //Coordenada de Textura
			glVertex3f(x +DX, y_, z);
		}
	}
	glEnd();
}

void display(){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScalef(.5,.5,.5);
	glRotatef(45, -1.0, 0.0, 0.0);
	drawGrid(0);
	glPopMatrix();
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  static float posx = 0.0, posz = -5.0, angle = 0.0;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,
                 (float)WIDTH / (float)HEIGTH,
                 1.0f, 100.0f);
  switch(key) {
    case 27:
      exit(0);
    break;
    case 't':
    case 'T':
      transformMode = TRANSLATE;
    break;
    case 'c':
    case 'C':
      transformMode = SCALE;
    break;
    case 'r':
    case 'R':
      transformMode = ROTATE;
    break;
    case 'w':
    case 'W':
      posz += 0.1*cos(M_PI*angle/180.0);
      posx += 0.1*sin(M_PI*angle/180.0);
    break;
    case 's':
    case 'S':
      posz -= 0.1*cos(M_PI*angle/180.0);
      posx -= 0.1*sin(M_PI*angle/180.0);
    break;
    case 'q':
    case 'Q':
      posz += -0.1*sin(M_PI*angle/180.0);
      posx += 0.1*cos(M_PI*angle/180.0);
    break;
    case 'e':
    case 'E':
      posz -= -0.1*sin(M_PI*angle/180.0);
      posx -= 0.1*cos(M_PI*angle/180.0);
    break;
    case 'a':
    case 'A':
      angle += 5;
    break;
    case 'd':
    case 'D':
      angle -= 5;
    break;
  }
  glRotatef(-angle, 0.0, 1.0, 0.0);
  glTranslatef(posx, 0.0, posz);

  glutPostRedisplay();
}

void keyboard2(int key, int mouse_x, int mouse_y)
{
  int x = 0, y = 0, z = 0;

  switch(key) {
    case GLUT_KEY_PAGE_UP:
      z++;
    break;
    case GLUT_KEY_PAGE_DOWN:
      z--;
    break;
    case GLUT_KEY_UP:
      y++;
    break;
    case GLUT_KEY_DOWN:
      y--;
    break;
    case GLUT_KEY_LEFT:
      x--;
    break;
    case GLUT_KEY_RIGHT:
      x++;
    break;
  }

  glMatrixMode(GL_MODELVIEW);
  switch(transformMode) {
    case TRANSLATE:
      glTranslatef(x, y, z);
    break;
    case ROTATE:
      glRotatef(5.0, x, y, z);
    break;
    case SCALE:
      glScalef(1.0 + x/10.0, 1.0 + y/10.0, 1.0 + z/10.0);
    break;
  }
  glutPostRedisplay();
}

int main(int argc,char **argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH,HEIGTH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Grid");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc (keyboard2);
	init();
	glutMainLoop();
	return 0;
}


