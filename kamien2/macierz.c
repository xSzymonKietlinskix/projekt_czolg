#include "macierz.h"
#include "lacznosc.h"

Macierz utworz(int x, int y)
{
    Macierz *ms;
    ms = (Macierz *)malloc(sizeof(Macierz));
    ms->akt_roz_x = x;
    ms->akt_roz_y = y;
    ms->tab = (float **)malloc(sizeof(float *) * ms->akt_roz_x);

    for (int i = 0; i < ms->akt_roz_x; i++)
        ms->tab[i] = (float *)malloc(sizeof(float) * ms->akt_roz_y);
    
    return *ms;
}

Macierz realokuj(Macierz *ms, int ile_razy)
{
    ms->akt_roz_x = ms->akt_roz_x * ile_razy;
    ms->akt_roz_y = ms->akt_roz_y * ile_razy;
    ms->tab = (float **)realloc(ms->tab, sizeof(float *) * ms->akt_roz_x * ile_razy);

    for (int i = 0; i < ms->akt_roz_x; i++)
        ms->tab[i] = (float *)realloc(ms->tab[i], sizeof(float) * ms->akt_roz_y * ile_razy);
    
    return *ms;
}

Macierz przepisz_macierz(Macierz D, Macierz M)
{
    for (int i = 0; i < M.akt_roz_x; i++)
        for (int j = 0; j < M.akt_roz_y; j++)
            if (M.tab[i][j] != 0)
                D.tab[i][j] = M.tab[i][j];
    return D;
}

Macierz zeruj_macierz(Macierz B)
{
    for (int i = 0; i < B.akt_roz_x; i++)
        for (int j = 0; j < B.akt_roz_y; j++)
            B.tab[i][j] = 0;
    return B;
}

void zwolnij(Macierz *M)
{

    for (int i = 0; i < M->akt_roz_x; i++)
        free(M->tab[i]);
    free(M->tab);
}


