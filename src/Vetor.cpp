/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM

    
    Implementacao da classe vetor 3D
**/

#include "Vetor.h"
#include <cmath>
#include <iostream>
#include "Ponto3D.h"

using namespace std;

Vetor::Vetor ()
{
    x = 0;
    y = 0;
    z = 0;
}

Vetor::Vetor(const Ponto3D& a, const Ponto3D& b)
{
    x = b.x - a.x;
    y = b.y - a.y;
    z = b.z - a.z;
}

Vetor::Vetor(float x1, float y1, float z1)
{
    x = x1;
    y = y1;
    z = z1;
}

void Vetor::setVetor(const Ponto3D& a, const Ponto3D& b)
{
    x = b.x - a.x;
    y = b.y - a.y;
    z = b.z - a.z;
}


Vetor::~Vetor()
{
    //dtor
}

void Vetor:: normaliza() // normaliza o tamanho do vetor
{
    float modulo = sqrt(pow(x,2)+pow(y,2)+ pow(z,2));
    x = x / modulo;
    y = y / modulo;
    z = z / modulo;
}

Vetor Vetor::operator*(float num)
{
	Vetor vetorResultante(x * num, y * num, z*num);
    return vetorResultante;
}

Vetor Vetor::operator+(const Vetor& vetor){
	Vetor vetorResultante(x + vetor.x, y + vetor.y, z + vetor.z);
	return vetorResultante;
}




