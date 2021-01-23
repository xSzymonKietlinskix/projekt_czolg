#include "zapis.h"

void wypisz(Macierz m)
{
    Macierz B = utworz(m.akt_roz_x, m.akt_roz_y);

    zeruj_macierz(B);
    for (int i = 0; i < B.akt_roz_x; i++)
    {
        for (int j = 0; j < B.akt_roz_y; j++)
        {
            B.tab[B.akt_roz_y - 1 - i][j] = m.tab[i][j];
        }
    }
    int i, j;
    for (i = 0; i < B.akt_roz_x; i++)
    {
        for (j = 0; j < B.akt_roz_x; j++)
        {
            if (B.tab[i][j] != 0)
                printf("%3.0f", B.tab[i][j]);
        }
        if (i < B.akt_roz_x - 1)
            printf("\n");
    }
    printf("\n");
}

void wypisz_do_pliku(FILE *fun, Macierz m)
{
    Macierz B = utworz(m.akt_roz_x, m.akt_roz_y);

    zeruj_macierz(B);
    for (int i = 0; i < B.akt_roz_x; i++)
    {
        for (int j = 0; j < B.akt_roz_y; j++)
        {
            B.tab[B.akt_roz_y - 1 - i][j] = m.tab[i][j];
        }
    }
    int i, j;
    for (i = 0; i < B.akt_roz_x; i++)
    {
        for (j = 0; j < B.akt_roz_x; j++)
        {
            if (B.tab[i][j] != 0)
                fprintf(fun, "%3.0f", B.tab[i][j]);
        }
        if (i < B.akt_roz_x - 1)
            fprintf(fun, "\n");
    }
    fprintf(fun, "\n");
    zwolnij(&B);
    zwolnij(&m);
}
