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
void keyboard2(unsigned char key, int mousex, int mousey);

GLuint texture_names[2];
int load = 0;
int altitude[1024][512];
float znear=0.0, zfar=0.0;

int main(int argc, char *argv[]){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(WIDTH,HEIGTH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Desenha um cubo");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard2);
	init();
	glutMainLoop();
	return 0;
}

void printMatrix(GLfloat *m,int l, int c){
	int i;
	for(i=0;i<l*c;i++){
		printf("%f ",*m);
		m++;
		if((i+1)%4==0)
			printf("\n");
	}
	printf("\n");
}

void loadMatrix(){
	GLfloat m[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX,&m);
	printMatrix(m,4,4);
}


/*
##################################################################
*/
int get_int(char *header, unsigned int offset){
	return  (header[offset+3]<<24)+
			(header[offset+2]<<16)+
			(header[offset+1]<<8) +
			(header[offset+0]);
}

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

static void makeTexture(char *file_name)
{
	unsigned char *texture;
	int width, height;
	
	texture = load_bmp(file_name,&width,&height);
	if(load==1){
		unsigned char *t = texture;
		int i,j;
		for(j=0;j<512;j++){
			for(i=0;i<1024;i++){
				if(i==200)
					printf("%d ",*t);
				altitude[i][j]=(*t);
				//altitude[i][j/3]=20*sin(2*M_PI*(float)i/1023)+20*sin(2*M_PI*(float)j/(512*3));
				t+=3;
			}
		}
	}
	printf("\n");	
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, 0x80E0 , GL_UNSIGNED_BYTE, texture);
    free(texture);
}

/*
##################################################################
*/

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

/*
##################################################################
*/
void init(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	glClearColor(0.0,0.0,0.0,1.0);
	//glOrtho(0,WIDTH,0,HEIGTH,-1,1);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0,(float)WIDTH/(float)HEIGTH,100.0,1000.0);
	
	glMatrixMode(GL_MODELVIEW);
	glTranslated(0,0,-500);
	glRotated(-30,1,0,0);
	
	glGenTextures(2, texture_names);
	
	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	makeTexture("altimetria_1024x512.bmp");
	
	load++;
	glBindTexture(GL_TEXTURE_2D, texture_names[0]);
	makeTexture("altimetria_1024x512.bmp");
	
	int i,j;
	for(i=0;i<1024;i++){
		for(j=0;j<512;j+=1){
			if(i==200)
				printf("%d ",altitude[i][j]);
		}
	}
	printf("\n");
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	loadMatrix();
	drawGrid();
	glFlush();
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

void keyboard2(unsigned char key, int mousex, int mousey){
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

