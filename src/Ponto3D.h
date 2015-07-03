/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM


    Definição da classe Ponto 3D.
**/



#ifndef PONTO3D_H
#define PONTO3D_H
#include "Vetor.h"


class Ponto3D
{

    public:
        float ponto [4];
        float x,y,z;
        Ponto3D(float x1, float y1, float z1);
        Ponto3D();
        virtual ~Ponto3D();
		Ponto3D operator+=(const Ponto3D& ponto);
		Ponto3D operator+(const Ponto3D& ponto);
		Ponto3D operator-(const Ponto3D& ponto);
		Ponto3D operator-(const Vetor& ponto);
		Ponto3D operator+(const Vetor& ponto);
		Ponto3D operator*(const Ponto3D& ponto);
		Ponto3D& operator=(const Ponto3D& ponto);
		Ponto3D operator*(float num);

    protected:
    private:
};

#endif // PONTO3D_H
