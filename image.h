#ifndef IMAGE_H
#define IMAGE_H

#endif // IMAGE_H

#include <Imagine/Graphics.h>
#include <algorithm>
#include <iostream>
#include "../quadtree/quadtree.h"

using namespace Imagine;
using namespace std;

int puiss(int a, int n){
    int b = 1;
    for(int i = 1; i<=n; i++){
        b *= a;
    }
    return b;
}

//Retourne la taille du carré de dimension 2nx2n supérieur le plus proche
int taille_im(int width, int height){
    int max;
    if(width>height){max=width;}
    else{max=height;}
    int n=0;
    while(max!=0){
        max = max/2;
        n++;
    }
    return puiss(2, n);
}

QuadTree<byte>* arbre(byte* tab, int width, int height, int departx, int departy, int dist){
    if(dist == 1){
        assert(departx*width+departy<width*height);
        QuadLeaf<byte> *A;
        A = new QuadLeaf<byte>(tab[departx+departy*width]);
        return A;
    }
    else{
        int dist2 = int(dist/2);
        QuadTree<byte> *Q;
        QuadTree<byte> *q1 = arbre(tab, width, height, departx, departy, dist2);
        QuadTree<byte> *q2 = arbre(tab, width, height, departx + dist2, departy, dist2);
        QuadTree<byte> *q3 = arbre(tab, width, height, departx + dist2, departy + dist2, dist2);
        QuadTree<byte> *q4 = arbre(tab, width, height, departx, departy + dist2, dist2);

        if(q1->isLeaf() && q2->isLeaf() && q3->isLeaf() && q4->isLeaf() && (q1->value()==q2->value()) && (q1->value()==q3->value()) && (q1->value()==q4->value())){
            Q = new QuadLeaf<byte>(q1->value());
        }
        else{
            Q = new QuadNode<byte>(q1, q2, q3, q4);
        }

        return Q;
    }
}

QuadTree<byte>* compression( byte* tab, int width, int height){

    int taille = taille_im(width, height);
    //On plonge l'image dans un carré de taille 2nx2n
    if(width != taille || height != taille){
        byte* tab2 = new byte[taille*taille];
        for(int i=0; i<taille; i++){
            for(int j=0; j<taille; j++){
                if(i<width && j<height){
                    tab2[i+taille*j]=tab[i+width*j];
                }
                else{tab2[i+taille*j]=byte(255);}
            }
        }
        return arbre(tab2, taille, taille, 0, 0, taille);
    }
    else{
        return arbre(tab, width, height, 0, 0, width);
    }

}


byte* parcours(QuadTree<byte>* A,int width, int departx, int departy, int dist, byte* tab){

    if(A -> isNode()){
        int dist2 = int(dist/2);

        parcours(A->son(NW), width, departx, departy, dist2, tab);
        parcours(A->son(NE), width, departx + dist2, departy, dist2, tab);
        parcours(A->son(SE), width, departx + dist2, departy + dist2, dist2, tab);
        parcours(A->son(SW), width, departx, departy + dist2, dist2, tab);
    }
    else{
        byte b = A->value();
        for(int i = departx; i<departx+dist; i++){
            for(int j = departy; j<departy+dist; j++){
                tab[i+j*width] = b;
            }
        }
        putGreyImage(IntPoint2(0,0), tab, width, width);
        return tab;
    }
}

byte* decompression(QuadTree<byte>* A, int width, int height){
    int taille = taille_im(width, height);
    byte *tab = new byte[taille*taille];
    openWindow(taille,taille);
    return parcours(A, taille, 0, 0, taille, tab);
}



bool voisins(byte a, byte b, byte c, byte d, int seuil){
    return(abs(int(a)-int(b))<seuil && abs(int(a)-int(c))<seuil && abs(int(a)-int(d))<seuil && abs(int(b)-int(c))<seuil && abs(int(b)-int(d))<seuil && abs(int(c)-int(d))<seuil);
}


QuadTree<byte>* arbre_gris(byte* tab, int width, int height, int departx, int departy, int dist, int seuil){
    if(dist == 1){
        assert(departx*width+departy<width*height);
        QuadLeaf<byte> *A;
        A = new QuadLeaf<byte>(tab[departx+departy*width]);
        return A;
    }
    else{
        int dist2 = int(dist/2);
        QuadTree<byte> *Q;
        QuadTree<byte> *q1 = arbre_gris(tab, width, height, departx, departy, dist2, seuil);
        QuadTree<byte> *q2 = arbre_gris(tab, width, height, departx + dist2, departy, dist2, seuil);
        QuadTree<byte> *q3 = arbre_gris(tab, width, height, departx + dist2, departy + dist2, dist2, seuil);
        QuadTree<byte> *q4 = arbre_gris(tab, width, height, departx, departy + dist2, dist2, seuil);

        if(q1->isLeaf() && q2->isLeaf() && q3->isLeaf() && q4->isLeaf() && voisins(q1->value(),q2->value(),q3->value(),q4->value(), seuil)){
            int moyenne = int((int(q1->value())+int(q2->value())+int(q3->value())+int(q4->value()))/4);
            Q = new QuadLeaf<byte>(byte(int(moyenne)));
        }
        else{
            Q = new QuadNode<byte>(q1, q2, q3, q4);
        }

        return Q;
    }
}


QuadTree<byte>* compression_gris( byte* tab, int width, int height, int seuil){
    int taille = taille_im(width, height);
    //On plonge l'image dans un carré de taille 2nx2n
    if(width != taille || height != taille){
        byte* tab2 = new byte[taille*taille];
        for(int i=0; i<taille; i++){
            for(int j=0; j<taille; j++){
                if(i<width && j<height){
                    tab2[i+taille*j]=tab[i+width*j];
                }
                else{tab2[i+taille*j]=byte(255);}
            }
        }
        return arbre_gris(tab2, taille, taille, 0, 0, taille, seuil);
    }
    else{
        return arbre_gris(tab, width, height, 0, 0, width, seuil);
    }

}


//Mesure de la compression

int profondeur(QuadTree<byte>* A){
    if(A->isNode()){
        return 1 + profondeur(A->son(NW)) + profondeur(A->son(NE)) + profondeur(A->son(SW)) + profondeur(A->son(SE));
    }
    else{return 0;}
}

void taux_compression(byte* image, int width, int height){
    int taille = width*height;
    QuadTree<byte> *A = compression(image, width, height);
    int noeuds = profondeur(A);
    cout<<"Taille initiale de l'image : "<<taille<<" pixels"<<endl;
    cout<<"La taille de l'image compressée est environ égale au nombre de noeuds c'est à dire "<<noeuds<<endl;
    cout<<"Mesure du taux de compression :"<<endl;
    cout<<"Taille initiale de l'image / taille de l'image compressée  = "<<taille/noeuds<<endl;
    cout<<"Soit un taux de compression d'environ : "<<taille/noeuds-100<<"%"<<endl;
}
