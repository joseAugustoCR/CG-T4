/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM


    Classe para ponto 3D homogeneo.

**/

#include "Ponto3D.h"

Ponto3D::Ponto3D(float x1, float y1, float z1)
{
	// 4 coordenadas (x,y,z,h)
    ponto[0]=x1;
    ponto[1]=y1;
    ponto[2]=z1;
    ponto[3]=1;
    x= ponto[0];
    y=ponto[1];
    z=ponto[2];
}
Ponto3D::Ponto3D()
{
    ponto[0]=0;
    ponto[1]=0;
    ponto[2]=0;
    ponto[3]=1;
    x= ponto[0];
    y=ponto[1];
    z=ponto[2];
}

Ponto3D::~Ponto3D()
{
    //dtor
}

// Operadores para ponto homogeneo 3D:
Ponto3D Ponto3D::operator*(float num)
{
	Ponto3D pontoResultante(x * num, y * num, z*num);
    return pontoResultante;
}

Ponto3D Ponto3D::operator+=(const Ponto3D& ponto)
{
	x += ponto.x;
	y += ponto.y;
	z += ponto.z;
	return *this;
}

Ponto3D Ponto3D::operator+(const Ponto3D& ponto){
	Ponto3D pontoResultante(x + ponto.x, y + ponto.y, z + ponto.z);
	return pontoResultante;
}

Ponto3D Ponto3D::operator-(const Ponto3D& ponto){
	Ponto3D pontoResultante(x - ponto.x, y - ponto.y, z - ponto.z);
	return pontoResultante;
}

Ponto3D Ponto3D::operator-(const Vetor& vetor){
	Ponto3D pontoResultante(x - vetor.x, y - vetor.y, z - vetor.z);
	return pontoResultante;
}

Ponto3D Ponto3D::operator+(const Vetor& vetor){
	Ponto3D pontoResultante(x + vetor.x, y + vetor.y, z + vetor.z);
	return pontoResultante;
}



Ponto3D Ponto3D::operator*(const Ponto3D& ponto){
	Ponto3D pontoResultante(x * ponto.x, y * ponto.y, z*ponto.z);
	return pontoResultante;
}

Ponto3D& Ponto3D::operator=(const Ponto3D& ponto){
	x = ponto.x;
	y = ponto.y;
	z = ponto.z;
	return *this;
}
