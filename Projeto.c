#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 1024
#define HEIGTH 512

#define HEADER_SIZE 54 //TAMANDO DO CABECALHO
#define COMPRESSION_OFFSET 0x1e //ENDERECO NO CABECALHO BMP QUE DEFINE A COMPRESSAO
#define BPP_OFFSET 0x1c //QUANTIDADE DE BITS POR PIXEL
#define IMAGE_START_OFFSET 0x0a //DIZ ONDE A IMAGEM COMECA
#define IMAGE_WIDTH_OFFSET 0x12 //LARGURA DA IMAGEM
#define IMAGE_HEIGHT_OFFSET 0x16 //ALTURA DA IMAGEM

enum {TRANSLATE, SCALE, ROTATE} transforMode;

void init();
void display();
void keyboard(unsigned char key, int x, int y);
void keyboard2(int key, int mousex, int mousey);
void mouse(int button,int state, int mousex, int mousey);

GLuint texture_names[2];
int load = 0;
int altitude[1024][512];
float znear=0.0, zfar=0.0;

int main(int argc, char *argv[]){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(WIDTH,HEIGTH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Marte");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard2);
	init();
	glutMainLoop();
	return 0;
}
/*Função utilizada em load_bmp */
int get_int(char *header, unsigned int offset){
	return  (header[offset+3]<<24)+
			(header[offset+2]<<16)+
			(header[offset+1]<<8) +
			(header[offset+0]);
}
/*Função para carregar a imagem*/
unsigned char* load_bmp(char *file_name, int *width, int *height){
	void *data = NULL;
	FILE *fp = fopen(file_name,"rb");
	if(fp==NULL){
		printf("falha ao abrir arquivo");
		goto out;
	}
	char header[HEADER_SIZE];
	if(fread(header,HEADER_SIZE,1,fp)!=1){
		printf("falha ao carregar cabecalho\n");
		goto out;
	}	
	if(header[0]!='B' || header[1]!='M'){
		printf("BM\n");
		goto out;
	}
	if(get_int(header,COMPRESSION_OFFSET)!=0 || 
		get_int(header,BPP_OFFSET)!=24 ){
		printf("Cmpressao invalida\n");	
		goto out;
	}		
	*width = get_int(header, IMAGE_WIDTH_OFFSET);
	*height = get_int(header, IMAGE_HEIGHT_OFFSET);
	int image_size = 3*(*width)*(*height);
	data = malloc(image_size);
	fread(data,image_size,1,fp);
	out:
		fclose(fp);
		return data;
}
/* Função para carregar a textura*/
static void makeTexture(char *file_name){
	unsigned char *texture;
	int width, height;
	texture = load_bmp(file_name,&width,&height);
	if(load==1){ //Carrega a altitude
		unsigned char *t = texture;
		int i,j;
		for(j=0;j<512;j++){ 
			for(i=0;i<1024;i++){
					altitude[i][j]=(*t)/20;
				t+=3;
			}
		}
	}	
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, 0x80E0 , GL_UNSIGNED_BYTE, texture);
    free(texture);
}

/*void drawWater(float diameter, int rdiv, int hdiv){
	int i;
	float delta =2*M_PI/rdiv;
	float delta_h=diameter/hdiv;
	float radius =diameter/2.0;
	float d = diameter/2;
	//Rings
	int j; 
	for(j=0; j<hdiv; j++){
		float heigth = j*delta_h;
		float heigth_ = (j+1)*delta_h;
		float r = sqrt(radius*radius - (radius-heigth)*(radius-heigth));
		float r_ = sqrt(radius*radius - (radius-heigth_)*(radius-heigth_));
		for(i=0; i<rdiv;i++){
			float theta =delta*i;
			glBegin(GL_QUADS);
				glColor3f(35,35,142);
				glVertex3f(cos(theta)*r, heigth-d, sin(theta)*r);
				glVertex3f(cos(theta)*r_, heigth_-d, sin(theta)*r_);
				glVertex3f(cos(theta +delta)*r_, heigth_-d, sin(theta+delta)*r_);
				glVertex3f(cos(theta+delta)*r, heigth-d, sin(theta+delta)*r);
			glEnd();
		}
	}
}*/

void scale(float *v, float factor){
	v[0]=v[0]*factor;
	v[1]=v[1]*factor;
	v[2]=v[2]*factor;
}

void normalize(float *p){
	float m = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
	if(m==0) return;
	p[0]=p[0]/m;
	p[1]=p[1]/m;
	p[2]=p[2]/m;
}

void getCoords(float *v, float heigth, float theta, float diameter){
	float radius = diameter/2.0;
	float r = sqrt(radius*radius - (radius-heigth)*(radius-heigth));
	v[0] = cos(theta)*r;
	v[1] = heigth-diameter/2;
	v[2] = sin(theta)*r;
}

void drawSphere(float diameter, int rdiv, int hdiv){
	int i;
	float delta =2*M_PI/rdiv;
	float delta_h=diameter/hdiv;
	float radius =diameter/2.0;
	int j; 
	glBindTexture(GL_TEXTURE_2D,texture_names[1]);
	for(j=0; j<hdiv; j++){
		float heigth = j*delta_h;
		float heigth_ = (j+1)*delta_h;
		float r = sqrt(radius*radius - (radius-heigth)*(radius-heigth));
		float r_ = sqrt(radius*radius - (radius-heigth_)*(radius-heigth_));
		for(i=0; i<rdiv;i++){
			float theta =delta*i;
			float d = 0.0;
			glBegin(GL_QUADS);
				glTexCoord2f(theta/(2*M_PI),heigth/diameter);
				float p[3];
				getCoords(p,heigth,theta,diameter);
				normalize(p);
			//	d = altitude[i*1023/(rdiv)][j*511/(hdiv)];
			/*	if(d<2){
					d=2;
					glColor3f(0.0,0.0,1.0);
				}
				else{
					glColor3f(1.0,1.0,1.0);
				}*/
				scale(p,diameter+d);
				glVertex3f(p[0],p[1],p[2]);			
				glTexCoord2f(theta/(2*M_PI),heigth_/diameter);
				getCoords(p,heigth_,theta,diameter);
				normalize(p);
				
			//	d = altitude[i*1023/(rdiv)][(j+1)*511/(hdiv)];
				/*if(d<2){
					d=2;
					glColor3f(0.0,0.0,1.0);
				}
				else{
					glColor3f(1.0,1.0,1.0);
				}*/
				scale(p,diameter+d);
				glVertex3f(p[0],p[1],p[2]);
				
				glTexCoord2f((theta+delta)/(2*M_PI),heigth_/diameter);
				getCoords(p,heigth_, theta +delta, diameter);
				normalize(p);
			//	d = altitude[(i+1)*1023/(rdiv)][(j+1)*511/(hdiv)];
				/*if(d<2){
					d=2;
					glColor3f(0.0,0.0,1.0);
				}
				else{
					glColor3f(1.0,1.0,1.0);
				}*/
				scale(p,diameter+d);
				glVertex3f(p[0],p[1],p[2]);
				glTexCoord2f((theta+delta)/(2*M_PI),heigth/diameter);
				getCoords(p,heigth, theta + delta, diameter);
				normalize(p);
			//	d = altitude[(i+1)*1023/(rdiv)][j*511/(hdiv)];
			/*	if(d<2){
					d=2;
					glColor3f(0.0,0.0,1.0);
				}
				else{
					glColor3f(1.0,1.0,1.0);
				}*/
				scale(p,diameter+d);
				glVertex3f(p[0],p[1],p[2]);
			glEnd();
		}
	}
	
}

void drawGrid(){
	int i,j;
	float Lx=WIDTH, Ly=HEIGTH;
	int Nx = 1024;
	int Ny = 512;
	int p = 1;
	float Dx=Lx/Nx, Dy=Ly/Ny;
	glBindTexture(GL_TEXTURE_2D,texture_names[1]);
	glBegin(GL_QUADS);
	for(i=0;i<=Nx-1;i+=p){
		for(j=0;j<=Ny-1;j+=p){
			glTexCoord2f(i*Dx/Lx,j*Dy/Ly);
			glVertex3f(i*Dx-Lx/2,j*Dy-Ly/2,altitude[i][j]);
			glTexCoord2f(i*Dx/Lx,(j+p)*Dy/Ly);
			glVertex3f(i*Dx-Lx/2,(j+p)*Dy-Ly/2,altitude[i][j+p]);
			glTexCoord2f((i+p)*Dx/Lx,(j+p)*Dy/Ly);
			glVertex3f((i+p)*Dx-Lx/2,(j+p)*Dy-Ly/2,altitude[i+p][j+p]);
			glTexCoord2f((i+p)*Dx/Lx,j*Dy/Ly);
			glVertex3f((i+p)*Dx-Lx/2,j*Dy-Ly/2,altitude[i+p][j]);
		}
	}
	glEnd();
}

void init(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0,0.0,0.0,1.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0,(float)WIDTH/(float)HEIGTH,1.0,1000.0);
	glMatrixMode(GL_MODELVIEW);
	glTranslated(0,0,-850);
	glRotated(-30,1,0,0);
	glGenTextures(2, texture_names);
	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	makeTexture("textura_da_superficie_1024x512_.bmp");
	load++;
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	makeTexture("altimetria_1024x512_.bmp");
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawSphere(300,300,300);
	glFlush();
	glRotated(1.0,0,0,1);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(0);
			break;
		case 't':
		case 'T':
			transforMode = TRANSLATE;
			break;
		case 'r':
		case 'R':
			transforMode = ROTATE;
			break;
		case 's':
		case 'S':
			transforMode = SCALE;
			break;
	}
}

void keyboard2(int key, int mousex, int mousey){
	int x=0,y=0,z=0;
	switch(key){
		case GLUT_KEY_PAGE_DOWN:
			z=-5;
			break;
		case GLUT_KEY_PAGE_UP:
			z=+5;
			break;	
		case GLUT_KEY_LEFT:
			x-=5;
			break;
		case GLUT_KEY_RIGHT:
			x+=5;
			break;
		case GLUT_KEY_UP:
			y+=5;
			break;
		case GLUT_KEY_DOWN:
			y-=5;
			break;		
	}
	
	glMatrixMode(GL_MODELVIEW);
	switch(transforMode){
		case SCALE:
			glScalef(1.0+x/10.0,1.0+y/10.0,1.0+z/10.0);
			break;
		case TRANSLATE:
			glTranslated(x,y,z);
			break;
		case ROTATE:
			glRotated(1.0,x,y,z);
			break;
	}
	glutPostRedisplay();
}
