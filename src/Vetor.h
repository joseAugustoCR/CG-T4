/**
    TRABALHO 4 DE COMPUTACAO GRÁFICA: AUTORAMA USANDO BPLINE 3D
    JOSÉ AUGUSTO COMIOTTO ROTTINI - 201120279
    ENGENHARIA DE COMPUTAÇÃO - UFSM
    

    Classe para vetor 3D
**/

#include "Ponto3D.h"

#ifndef VETOR_H
#define VETOR_H

class Ponto3D;


class Vetor
{

    public:
        float x;
        float y;
        float z;
        Vetor();

        Vetor (float x1, float y1, float z1);
          Vetor (const Ponto3D& a, const Ponto3D& b);
        Vetor operator*(float num);
        Vetor operator+(const Vetor& vetor);
        void setVetor (const Ponto3D& a, const Ponto3D& b);
        void normaliza();
        virtual ~Vetor();
    protected:
    private:
};

#endif // VETOR_H
