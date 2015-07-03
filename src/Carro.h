/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM


    Classe para criação e configuração carro.
**/

#include <gl/glut.h> //gl utility toolkit
#include <gl/gl.h>  //gl utility
#include <math.h>
#include "Ponto3D.h"
#include "Vetor.h"




class Carro{
public:
int indexPosicaoCarro;
Ponto3D posicaoDoCarroNaPista;
float velocidade;
float deslocamentoLateral;
float r,g,b;


Carro()
{
    indexPosicaoCarro=0;
    posicaoDoCarroNaPista=Ponto3D(0,0,0);
    velocidade=0;
    deslocamentoLateral=0;
}


void setCor(float red, float green, float blue){
    r = red;
    g = green;
    b = blue;
}

void setVelocidade(int v){
    velocidade = v;
}


void draw()
{
     glColor3f(r,g,b);
    glBegin(GL_QUADS);			// Face posterior
        glNormal3f(0.0, 0.0, -1.0);	// Normal da face
        glVertex3f(5.0, 5.0, -10.0);
        glVertex3f(-5.0, 5.0, -10.0);
        glVertex3f(-5.0, -5.0, -10.0);
        glVertex3f(5.0, -5.0, -10.0);
    glEnd();


    glBegin(GL_QUADS);			// Face frontal
        glNormal3f(0.0, 0.0, 1.0); 	// Normal da face
        glVertex3f(5.0, 5.0, 10.0);
        glVertex3f(-5.0, 5.0, 10.0);
        glVertex3f(-5.0, -5.0, 10.0);
        glVertex3f(5.0, -5.0, 10.0);
    glEnd();

    glBegin(GL_QUADS);			// Face lateral esquerda
        glNormal3f(1.0, 0.0, 0.0); 	// Normal da face
        glVertex3f(5.0, 5.0, -10.0);
        glVertex3f(5.0, 5.0, 10.0);
        glVertex3f(5.0, -5.0, 10.0);
        glVertex3f(5.0, -5.0, -10.0);
    glEnd();
    glBegin(GL_QUADS);			// Face lateral direita
        glNormal3f(-1.0, 0.0, 0.0);	// Normal da face
        glVertex3f(-5.0, 5.0, -10.0);
        glVertex3f(-5.0, 5.0, 10.0);
        glVertex3f(-5.0, -5.0, 10.0);
        glVertex3f(-5.0, -5.0, -10.0);
    glEnd();
    glBegin(GL_QUADS);			// Face superior
        glNormal3f(0.0, 1.0, 0.0);  	// Normal da face
          glVertex3f(-5.0, 5.0, -10.0);
        glVertex3f(-5.0, 5.0, 10.0);
        glVertex3f(5.0, 5.0, 10.0);
        glVertex3f(5.0, 5.0, -10.0);
    glEnd();
    glBegin(GL_QUADS);			// Face inferior
        glNormal3f(0.0, -1.0, 0.0); 	// Normal da face
         glVertex3f(-5.0, -5.0, -10.0);
        glVertex3f(-5.0, -5.0, 10.0);
        glVertex3f(5.0, -5.0, 10.0);
        glVertex3f(5.0, -5.0, -10.0);
    glEnd();
}
};

