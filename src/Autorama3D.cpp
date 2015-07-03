/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM

    REQUISITOS MÍNIMOS: TODOS
        - Pista é circuito fechado em 3D com elevacao variada
        - Carro 3D
        - Controle da velocidade e direcao do carro
        - Visao interna ou externa do carro
        - Visualizacao dos grafos de controle (Grafos e vetor direcao)
        - Iluminacao
        - Pista com largura


    EXTRAS:
        - Geracao da pista utilizando o mouse
        - Iluminacao especular
        - Carro pode se deslocar lateralmente na pista
        - Carros "inimigos"

**/

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <assert.h>

#include "Ponto3D.h"
#include "Carro.h"
#include "Vetor.h"

#define STEP 0.002  // Step para criar a curva (Determina a quantidade de pontos)


// Largura e altura da janela
GLfloat windowWidth=900.f;
GLfloat windowHeight=700.f;

// Parametros de visualizacao
GLfloat fAspect;
float abertura = 60.0;

// Variaveis Globais:
float tempo=0;
int posicaoAtual=0;

// Flags:
bool up = false;
bool down = false;
bool left = false;
bool right = false;
bool editRoad = true;
bool pista_salva = false;
bool viewControlGraphs = false;
bool insideView = false;

// Vetores para salvar pontos relevantes a curva
std::vector<Ponto3D> pontosDeControle;
std::vector<Ponto3D> subPontosDeControle;
std::vector<Ponto3D> curva;
std::vector<Vetor> tangentes;
std::vector<Vetor> normais;
std::vector<Ponto3D> rotaInimigo1;
std::vector<Ponto3D> rotaInimigo2;

// Carros:
Carro meuCarro;
Carro inimigo1;
Carro inimigo2;


/** RETORNA AS FUNCOES DE BASE BSPLINE **/
float basisFunction(int i, float t){
	if (i==3){
		return ( (0.167f)*pow(t,3) );
	}
	else if(i==2){
		return ( (0.167f)*((-3*pow(t,3))+(3*pow(t,2))+(3*t)+1) );
	}
	else if (i==1){
		return ( (0.167f)*((3*pow(t,3))-(6*pow(t,2))+4) );
	}
	else if (i==0){
		return ( (0.167f)*pow(1-t,3) );
	}
}

/** CALCULA A BSPLINE **/
Ponto3D bSpline(std::vector<Ponto3D>& pontosDeControle, float t)
{
	Ponto3D bspline(0,0,0);

    // Calcula a BSpline para um vetor de 4 pontos de controle em um tempo "t"
	for (int i=0;i<4;i++){
		bspline+= pontosDeControle[i]*basisFunction(i,t);
	}
	return bspline; // Retorna o ponto da curva resultante
}


/** CONFIGURA A ILUMINACAO **/
void configuraIluminacao()
{
    // Cor de fundo azul para o ceu:
    glClearColor(0.5f, 0.5f, 0.9f, 1.0f);

    // Ambiente + difusa + especular
    // Cor e posicao das luzes:
    GLfloat luzAmbiente[4]={0.3f,0.3f,0.3f,1.0f};
    GLfloat luzDifusa[4]={0.4f,0.3f,0.4f,1.0f};
    GLfloat luzEspecular[4]={0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat posicaoLuz[4]={100.0f, 500.0f, 300.0f, 1.0};

    // Caracteristicas do material:
    GLfloat especularidade[4]={1.0f,1.0f,1.0f,1.0f};
    GLfloat especMaterial[] = {10.0f};

    // Gouraud
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, especularidade);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,especMaterial);

    // Ativa o uso da luz ambiente
    // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    // Define os parâmetros da luz de número 0
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

    glPolygonMode(GL_FRONT, GL_FILL);

    // Habilita a definição da cor do material a partir da cor corrente (color tracking)
    glEnable(GL_COLOR_MATERIAL);
    // Habilita iluminacao:
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}



/** RESHAPE PARA QUANDO TAMANHO DA JANELA EH ALTERADO **/
void reshapeWindow(GLsizei w, GLsizei h)
{
	windowWidth=w;
	windowHeight=h;

	// Ajusta a visualizacao para o novo tamanho de janela

	if (editRoad==true){    //  Configura para visualizacao 2D
		glViewport (0, 0, (GLsizei) w, (GLsizei) h);

		// Projecao ortografica
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity ();
	}
	else{               // 3D
		if ( h == 0 )
            h = 1;

		glViewport(0, 0, w, h);
		fAspect = (GLfloat)w/(GLfloat)h;

        // Projecao perspectiva
		glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(abertura,fAspect,1,10000);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
	}

    glutPostRedisplay();
}



/** DESENHA UM CIRCULO **/
void circleFill( int x, int y,int z, int raio, int div )
{
	float ang, x1, y1;
	float inc = 6.28318530717958/div;
	glBegin(GL_POLYGON);
	glNormal3d(x,y,z);
	for(ang=0; ang<6.27; ang+=inc)
	{
		x1 = (cos(ang)*raio);
		y1 = (sin(ang)*raio);

		glVertex3d(x1+x, y1+y, z);
	}
	glEnd();
}


/** DESENHA OS GRAFOS DE CONTROLE PARA A VISUALIZACAO 2D (CRIACAO DA PISTA) **/
void viewControlGraphs2D(){

	glColor3f(1.0f, 1.0f, 1.0f);

    // Percorre os pontos de controle desenhando os grafos
    // Como ainda esta em 2D, usa as coordenadas x e z para desenhar
	for (std::vector<Ponto3D>::iterator it = pontosDeControle.begin(); it != pontosDeControle.end(); it++)
	{
        // Traca as linhas entre os pontos de controle
       if ((it + 1) != pontosDeControle.end()){
           glBegin(GL_LINES);
                glVertex2f(it->x,it->z);
                glVertex2f((it+1)->x,(it+1)->z);
            glEnd();
        }
        else{                   // Se for o ultimo ponto, conecta ao primeiro
            glBegin(GL_LINES);
                glVertex2f(it->x,it->z);
                glVertex2f((it-(pontosDeControle.size()-1))->x,(it-(pontosDeControle.size()-1))->z);
            glEnd();
        }
    }
}


/** DESENHA OS GRAFOS DE CONTROLE PARA A VISUALIZACAO 3D (AUTORAMA INICIADO) **/
void viewControlGraphs3D(){

	glColor4f(1.0f, 1.0f, 1.0f,1.0f);

    // Percorre os pontos de controle desenhando os grafos
	for (std::vector<Ponto3D>::iterator it = pontosDeControle.begin(); it != pontosDeControle.end(); it++)
	{
        // Desenha os pontos de controle:
        circleFill((int)(it->x), (int)(it->y), (int)(it->z), 4, 19);

        // Traca as linhas entre os pontos de controle
       if ((it + 1) != pontosDeControle.end()){
           glBegin(GL_LINES);
                glVertex3f(it->x,it->y, it->z);
                glVertex3f((it+1)->x,(it+1)->y,(it+1)->z);
            glEnd();
        }
        else{                   // Se for o ultimo ponto, conecta ao primeiro
            glBegin(GL_LINES);
                glVertex3f(it->x,it->y,it->z);
                glVertex3f((it-(pontosDeControle.size()-1))->x,(it-(pontosDeControle.size()-1))->y,(it-(pontosDeControle.size()-1))->z);
            glEnd();
        }
    }
}


/** DESENHA A PISTA EM 2D (DURANTE A CRIACAO DA PISTA) **/
void drawRoad2D(){

    // Desenha os pontos de controle
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (std::vector<Ponto3D>::iterator it = pontosDeControle.begin(); it != pontosDeControle.end(); it++)
	{
        circleFill((int)(it->x), (int)(it->z),0, 4, 19);
    }

    // Se tem mais de 3 pontos de controle, percorre os pontos separando os segmentos
    // e desenhando a curva
	if (pontosDeControle.size()>3){
		for (int i=0;i<pontosDeControle.size();i++)
		{
            if (pontosDeControle.size()-i == 3){  // Antepenultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
            }
            else if (pontosDeControle.size()-i == 2){ // Penultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
                subPontosDeControle.push_back(pontosDeControle[1]);
            }
            else if (pontosDeControle.size()-i == 1){ // Ultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
                subPontosDeControle.push_back(pontosDeControle[1]);
                subPontosDeControle.push_back(pontosDeControle[2]);
            }
            else{
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), i+4));
            }

        assert(subPontosDeControle.size()==4);

        for(float t=0; t <=1; t += STEP) {
        	Ponto3D p = bSpline(subPontosDeControle,t);
              glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
              circleFill(p.x,p.z,0,8,20);   // Desenha a curva usando circulos
           }
        }
     }
  }


/** SALVA OS PONTOS DE FORMAM A CURVA EM UM VETOR **/
void salvaPontosDaCurva(){

  	curva.clear();

    // Faz a curva mais uma vez e salva os pontos no vetor:
  	if (pontosDeControle.size()>3){

  		for (int i=0; i<pontosDeControle.size();i++)
  		{
            if (pontosDeControle.size()-i == 3){  // Antepenultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
            }
            else if (pontosDeControle.size()-i == 2){ // Penultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
                subPontosDeControle.push_back(pontosDeControle[1]);
            }
            else if (pontosDeControle.size()-i == 1){ // Ultimo path
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), pontosDeControle.size() ));
                subPontosDeControle.push_back(pontosDeControle[0]);
                subPontosDeControle.push_back(pontosDeControle[1]);
                subPontosDeControle.push_back(pontosDeControle[2]);
            }
            else{
                subPontosDeControle.assign(std::next(pontosDeControle.begin(), i),std::next(pontosDeControle.begin(), i+4));
            }

            assert(subPontosDeControle.size()==4);

            for(float t=0; t <=1; t += STEP) {
                Ponto3D p = bSpline(subPontosDeControle,t);
                curva.push_back(p); // Salva os pontos
            }
        }
    }
}


/** CALCULA A TANGENTE E NORMAL PARA CADA PONTO DA CURVA E SALVA EM VETORES **/
void salvaTangentesENormais(){

	normais.clear();
	tangentes.clear();

	Vetor tangente;
	Vetor normal;

    // Percorre os pontos da curva
	for (int i=0; i<curva.size(); i++){

        if (i + 1 > curva.size() - 1){  // Se for o ultimo ponto
            tangente.setVetor(curva[i], curva[0]);  // tangente = curva[0] - curva[i]
        }
        else{
            tangente.setVetor(curva[i], curva[i+1]); // tangente = curva[i+1] - curva[i]
        }

        if (tangente.x==0){     // Evita futura divisao por zero para calcular o angulo
            tangente.x = 0.001;
        }

        tangente.normaliza();
        tangentes.push_back(tangente);

        // Calcula a normal de acordo com a tangente
        normal.x = tangente.z;
        normal.y = 0;
        normal.z = -tangente.x;

        normal.normaliza();
        normais.push_back(normal);
    }

    // Seta a posicao inicial do carro na pista no inicio da volta
    meuCarro.posicaoDoCarroNaPista = curva[0];
}


/** CRIA ROTAS PARA OS CARROS INIMIGOS PERCORREREM **/
void criaRotaDosInimigos()
{
    // Desloca a curva original usando as normais de modo q cada uma das rotas
    // ocupe um lado da pista
    for (int i=0; i<curva.size(); i++){
        rotaInimigo1.push_back( curva[i]+(normais[i]*25) );
        rotaInimigo2.push_back( curva[i]+(normais[i]*(-25)) );
    }

    pista_salva = true; // Ativa flag de confirmacao
}


/** DESENHA A CURVA EM 3D (PISTA JA CRIADA, AUTORAMA INICIADO) **/
void drawRoad3D(){

    // Desenha o chao (quadrado limitado no eixo Y)
    glColor4f(0.2f, 0.5f, 0.2f, 0.1f);
	glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-5000.0f, -1.0f, 5000.0f);
        glVertex3f(5000.0f, -1.0f, 5000.0f);
        glVertex3f(5000.0f, -1.0f, -5000.0f);
        glVertex3f(-5000.0f, -1.0f, -5000.0f);
	glEnd();


    // Percorre a curva desenhando a pista
	glColor4f(0.3f,0.3f,0.3f,0.5f);
	for (int i=0;i<curva.size()-1;i++){

        // Vertices do segmento de pista: (utiliza a normal para fazer a largura da pista usando um deslocamento)
		Ponto3D a (curva[i] + (normais[i]*(-45)));
		Ponto3D b (curva[i]+ (normais[i]*45));
		Ponto3D c (curva[i+1] + (normais[i+1]*45));
		Ponto3D d (curva[i+1] + (normais[i+1]*(-45)));

        // Calcula a normal do segmento de pista
        // Vetores entre os vertices a,b e c
		Vetor v1(a, b); // b-a
		Vetor v2(a, c); // c-a

        // Normal eh o produto vetorial entre v1 e v2
		Vetor normal(0,0,0);
		normal.x = ( (v1.y * v2.z) - (v1.z * v2.y) );
		normal.y = -((v2.z * v1.x) - (v2.x * v1.z) );
		normal.z = ( (v1.x * v2.y) - (v1.y * v2.x) ) ;
		normal.normaliza();

        // Desenha o segmento da pista em questao
		glBegin(GL_QUADS);
            glNormal3f(normal.x,-normal.y,normal.z);
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
            glVertex3f(d.x, d.y, d.z);
		glEnd();
	}


	// Desenha linha intermediaria da pista (linha amarela)
	glLineWidth(3.0f);
	glColor4f(1.0f,1.0f,0.0f,0.9f);
	glBegin(GL_LINE_STRIP);
	for (int i=0;i<curva.size();i++){   // Percorre a pista desenhando
		glVertex3f(curva[i].x, curva[i].y, curva[i].z);
	}
	glEnd();
}


/** AJUSTA A CAMERA QUE ESTA SENDO UTILIZADA (INTERNA OU EXTERNA) DE ACORDO COM A FLAG ACIONADA PELO TECLADO **/
void setCamera(int posicaoCarro){

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    if (insideView){
         // Usou-se a tangente para obter um "ponto futuro" do carro
         // Camera posisionada no carro, na parte da frente, dando visao de quem estaria dentro do carro:
         Ponto3D posicaoCamera = meuCarro.posicaoDoCarroNaPista + (tangentes[posicaoCarro]*15.0f);
         // Olhando para uma posicao mais a frente
         Ponto3D alvoVisao = curva[posicaoCarro] + ( (tangentes[posicaoCarro]*45.0f) + (normais[posicaoCarro]*meuCarro.deslocamentoLateral) );

         // Eleva-se um pouco em Y para nao focar apenas na pista
         gluLookAt(posicaoCamera.x, posicaoCamera.y+5, posicaoCamera.z,alvoVisao.x, alvoVisao.y, alvoVisao.z, 0,1,0);
    }
    else{
        // Define uma posicao padrao para a camera externa, olhando o carro
        gluLookAt(300,150,700, curva[posicaoCarro].x,curva[posicaoCarro].y,curva[posicaoCarro].z, 0,1,0);
    }
}


/** AJUSTA A VELOCIDADE DO CARRO DE ACORDO COM AS FLAGS ATIVADAS PELO TECLADO **/
void setSpeed(){

	if (up==true){              // Acelera
        if (meuCarro.velocidade==0){
            meuCarro.velocidade=1;
        }else{
            meuCarro.velocidade+=0.05;
        }
	}
	else if (down==true){       // Freia
		meuCarro.velocidade-=0.05;
	}
	else{                       // Se nao estiver acelerando nem freiando, diminui a velocidade gradativamente
        if (meuCarro.velocidade<1.1)
            meuCarro.velocidade-= 0.0002;
        else
            meuCarro.velocidade-=0.001;
	}

	if (meuCarro.velocidade<0){  // Velocidade minima
        meuCarro.velocidade=0;
	}
	if (meuCarro.velocidade>3){ // Velocidade maxima
        meuCarro.velocidade=3;
	}
}

/** AJUSTA O DESLOCAMENTO DO CARRO NA PISTA DE ACORDO COM AS FLAGS ATIVADAS PELO TECLADO **/
void setOffsetOnRoad(){
    // O valor do deslocamento leva em consideracao a velocidade do carro
	if (left==true){              // Esquerda
        meuCarro.deslocamentoLateral+=0.05*meuCarro.velocidade;
	}
	else if (right==true){       // Direita
        meuCarro.deslocamentoLateral-=0.05*meuCarro.velocidade;
	}

    // Define deslocamentos maximos na pista:
	if (meuCarro.deslocamentoLateral<-40){  // Deslocamento maximo esquerda
        meuCarro.deslocamentoLateral=-40;
	}
	if (meuCarro.deslocamentoLateral>40){ // Deslocamento maximo direita
        meuCarro.deslocamentoLateral=40;
	}

	// Atualiza a posicao dos carro na pista de acordo com a rota de cada um
	meuCarro.posicaoDoCarroNaPista = curva[meuCarro.indexPosicaoCarro] + (normais[meuCarro.indexPosicaoCarro]*meuCarro.deslocamentoLateral);
	inimigo1.posicaoDoCarroNaPista = rotaInimigo1[inimigo1.indexPosicaoCarro];
	inimigo2.posicaoDoCarroNaPista = rotaInimigo2[inimigo2.indexPosicaoCarro];
}


/** INICIA O AUTORAMA: MOVE CARRO PELA PISTA **/
void startAutorama(){

// Anima meu carro:
	if (viewControlGraphs){     // Desenha vetor direcao do carro se tecla C for pressionada

        // Caso esteja se deslocando lateralmente na pista, vira a tangente conforme o carro:
        float angTangente=0;
        if (left){
            angTangente = 25;
        }
        if (right){
            angTangente = -25;
        }

        glPushMatrix();
            glTranslatef(meuCarro.posicaoDoCarroNaPista.x, meuCarro.posicaoDoCarroNaPista.y+5, meuCarro.posicaoDoCarroNaPista.z);
            glRotatef(angTangente,0,1,0);
            glColor3d(1,1,1);
            // Desenha o vetor direcao:
            glBegin(GL_LINES);
                glVertex3f(50*tangentes[meuCarro.indexPosicaoCarro].x, 50*tangentes[meuCarro.indexPosicaoCarro].y,
                          50*tangentes[meuCarro.indexPosicaoCarro].z);
                glVertex3f(0,0,0);
            glEnd();
        glPopMatrix();
    }

    // Angulo de rotacao do carro na curva:
    double angY = atan(tangentes[meuCarro.indexPosicaoCarro].x/tangentes[meuCarro.indexPosicaoCarro].z)*180/3.1415;

    // Se o carro estiver se deslocando lateralmente na pista, irá rotacionar mais 25 graus:
    if (left){
        angY+=25;
    }
    if (right){
        angY-=25;
    }

    // Rotaciona o carro e translada para a posicao atual na curva
	glPushMatrix();
        glTranslatef(meuCarro.posicaoDoCarroNaPista.x, meuCarro.posicaoDoCarroNaPista.y+5, meuCarro.posicaoDoCarroNaPista.z); // Carro em cima da pista
        glRotatef(angY,0,1,0);    // rotacao horizontal do carrinho
        meuCarro.draw();            // Desenha o carro
	glPopMatrix();

	meuCarro.indexPosicaoCarro+= (int)meuCarro.velocidade; // Incrementa a posicao de acordo com a velocidade
	if (meuCarro.indexPosicaoCarro >= (curva.size()-1) ){     // Se terminou os pontos, inicia nova volta
		meuCarro.indexPosicaoCarro = 0;
	}


// Anima carros Inimigos:

	// Inimigo1:

	// Angulo de rotacao do carro na curva:
    double angY_inimigo1 = atan(tangentes[inimigo1.indexPosicaoCarro].x/tangentes[inimigo1.indexPosicaoCarro].z)*180/3.1415;

	glPushMatrix();
        glTranslatef(inimigo1.posicaoDoCarroNaPista.x, inimigo1.posicaoDoCarroNaPista.y+5, inimigo1.posicaoDoCarroNaPista.z); // Carro em cima da pista
        glRotatef(angY_inimigo1,0,1,0);    // rotacao horizontal do carrinho
        inimigo1.draw();            // Desenha o carro
	glPopMatrix();

	inimigo1.indexPosicaoCarro+= (int)inimigo1.velocidade;
	if (inimigo1.indexPosicaoCarro >= (rotaInimigo1.size()-1) ){     // Se terminou os pontos, inicia nova volta
		inimigo1.indexPosicaoCarro = 0;
	}


    // Inimigo2:

	// Angulo de rotacao do carro na curva:
    double angY_inimigo2 = atan(tangentes[inimigo2.indexPosicaoCarro].x/tangentes[inimigo2.indexPosicaoCarro].z)*180/3.1415;

    glPushMatrix();
        glTranslatef(inimigo2.posicaoDoCarroNaPista.x, inimigo2.posicaoDoCarroNaPista.y+5, inimigo2.posicaoDoCarroNaPista.z); // Carro em cima da pista
        glRotatef(angY_inimigo2,0,1,0);    // rotacao horizontal do carrinho
        inimigo2.draw();            // Desenha o carro
	glPopMatrix();

	inimigo2.indexPosicaoCarro+= (int)inimigo2.velocidade;
	if (inimigo2.indexPosicaoCarro >= (rotaInimigo2.size()-1) ){     // Se terminou os pontos, inicia nova volta
		inimigo2.indexPosicaoCarro = 0;
	}
}


/** CALLBACK DO TECLADO**/
void keyboard (unsigned char key, int x, int y){

    printf("%d\n", key);
	key = tolower(key);
	switch(key)
	{
		case 13:                        // Tecla Enter: entra no modo 3D
            if (editRoad){
              editRoad = false;
              configuraIluminacao();
              salvaPontosDaCurva();
              salvaTangentesENormais();
              criaRotaDosInimigos();
           }
           printf("\nENTER\n");
           break;

       case '+':                        //Tecla +: Aumenta a abertura (diminui zoom)
           abertura+=1;
           printf("ABERTURA = %2.f\n", abertura);
           break;

       case '-':                        //Tecla -: Diminui a abertura (aumenta o zoom)
           abertura-=1;
           printf("ABERTURA = %2.f\n", abertura);
           break;

       case 99:                         // Tecla C: Habilita grafos de controle
            if (viewControlGraphs)
                viewControlGraphs=false;
            else
                viewControlGraphs=true;
            break;

       case 118:                        // Tecla V: Altera modo de visualizacao (interno ou externo)
           if (insideView)
                insideView=false;
           else
                insideView=true;
    }

    reshapeWindow(windowWidth, windowHeight);   // Atualiza informacoes de abertura
    glutPostRedisplay();
 }


/** CALLBACK DO MOUSE **/
 void mouseCallback (int button, int state, int x, int y){
 	y = windowHeight-y;

    // Usa uma altura randomica para o ponto da pista:
    float z=rand()%300;

    if (button == GLUT_LEFT_BUTTON){
        if (state == GLUT_DOWN) {   // Clique do mouse

            if (editRoad==true){
              // Para salvar o ponto em 3D, inverte-se as coordenadas Y e Z.
              // Depois, em 3D, usa-se o eixo Y como a altura.
              pontosDeControle.push_back(Ponto3D(x,z, y));
            }
        }
    }
    glutPostRedisplay();
}



/** CALLBACK DE TECLAS ESPECIAIS (SETAS DO TECLADO) **/
void TeclasEspeciais(int key, int x, int y)
{
    // Aciona flags das setas para ajustar a velocidade e deslocamento na pista depois
    switch(key){
        case GLUT_KEY_UP:     // Seta para cima
            up = true;
            break;

        case GLUT_KEY_DOWN:  // Seta para baixo
            down = true;
            break;

        case GLUT_KEY_LEFT: // Seta esquerda
            left = true;
            break;

        case GLUT_KEY_RIGHT: // Seta direita
            right = true;
	}
	glutPostRedisplay();
}


/** CALLBACK PARA LIBERACAO DAS TECLAS ESPECIAIS **/
void TeclasEspeciaisUp(int key, int , int)
{
    // Desativa flags
    switch(key){
        case GLUT_KEY_UP:     // Seta para cima
            up = false;
            break;

        case GLUT_KEY_DOWN:  // Seta para baixo
            down = false;
            break;

        case GLUT_KEY_LEFT: // Seta esquerda
            left = false;
            break;

        case GLUT_KEY_RIGHT: // Seta direita
            right = false;
	}
	glutPostRedisplay();
}



/** MOSTRA UM MENU DE INSTRUCOES NO CONSOLE **/
void menu(void)
{
   printf("               AUTORAMA B-SPLINE 3D \n");
   printf("\n Trabalho 4 de Computacao Grafica");
   printf("\n Developed by Jose Augusto Comiotto Rottini");
   printf("\n Contact: guto_cr@outlook.com");

   printf("\n\n Instrucoes: ");
   printf("\n      Clique sobre o canvas para inserir os pontos de controle e desenhar");
   printf("\n      a pista. Ao terminar, pressione ENTER para entrar no modo 3D.");
   printf("\n      Entao, use as teclas para controlar o carro e as opcoes do jogo. ");

   printf("\n\n SETAS - Controle do carro");
   printf("\n + - Aumenta aberetura (diminui zoom)");
   printf("\n - - Diminui aberetura (aumenta zoom)");
   printf("\n V - Altera o modo de visualizacao (interno/externo");
   printf("\n C - Habilita/Desabilita os grafos de controle\n\n");
}


/** FUNCAO DE RENDERIZACAO **/
void display (void)
{
	if (editRoad){     // Edit road mode (2D):
        // Configuracoes 2D do openGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.7f, 0.2f, 0.7f);
		glPolygonMode(GL_FRONT, GL_FILL);

		drawRoad2D();   // Desenha pista em 2D durante a criacao da mesma

		if (viewControlGraphs){     // Mostra os grafos de controle em 2D nessa etapa
            viewControlGraphs2D();
		}
	}
	else{   // 3D: Apos a construcao da pista, inicia o autorama

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (pista_salva){
			drawRoad3D();   // Desenha a pista em 3D
			startAutorama();// Inicia o autorama

			if (viewControlGraphs){
                viewControlGraphs3D();  // Mostra grafos de controle em 3D
			}
		}
	}
	glutSwapBuffers();
    glutPostRedisplay();
}


/** UPDATE: ATUALIZA VALORES REFERENTES A POSICAO, CAMERA, VELOCIDADE E DESLOCAMENTO DOS CARROS **/
void update(){
    if (pista_salva){
        setSpeed();         // Ajusta a velocidade de acordo com teclas que foram pressionadas
        setOffsetOnRoad();  // Ajusta o deslocamento lateral na pista
        setCamera(meuCarro.indexPosicaoCarro);  // Ajusta a camera escolhida
    }
}



int main (int argc, char* argv[])
{
    // Configuracoes iniciais da janela:
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize (windowWidth, windowHeight);
	glutInitWindowPosition (450, 0);
	glutCreateWindow ("Teste de ViewPort" );

    // Associacao de funcoes padrao da glut:
	glutDisplayFunc(display);
	glutReshapeFunc(reshapeWindow);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseCallback);
	glutSpecialFunc(TeclasEspeciais);
	glutSpecialUpFunc(TeclasEspeciaisUp);

	glutIdleFunc(update);

   // Configura carros:
	meuCarro.setCor(1,0,0);
	inimigo1.setCor(0,0,1);
	inimigo1.setVelocidade(2);
	inimigo2.setCor(0,0,1);
	inimigo2.setVelocidade(3);

    // Mostra menu no console:
	menu();

	glutMainLoop();

	return 0;
}
