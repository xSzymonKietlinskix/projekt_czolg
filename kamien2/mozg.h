#define forward "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/move/qwerty_15"
#define left "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/rotate/qwerty_15/left"
#define right "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/rotate/qwerty_15/right"
#define explore "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/explore/qwerty_15"
#define infor "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/info/qwerty_15"
#define reset "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/reset/qwerty_15"

#ifndef deklaracje
#define deklaracje
#include <stdlib.h>
#include <stdio.h>
#include "../../cJSON/cJSON.h"
#endif
typedef struct AkPol_
{
    int akt_x;
    int akt_y;
} AkPol;

Macierz pierwszy_bot(char *buffor, Macierz M);
Macierz drugi_bot(char *buffor);

AkPol znajdz_rozmiar();