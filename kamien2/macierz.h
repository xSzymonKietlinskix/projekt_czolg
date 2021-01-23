typedef struct Macierz_ //obraz swiata w formie macierzy
{
    int xo;     //pocztakowe polozenie czolgu na osi x
    int yo;     //pocztakowe polozenie czolgu na osi y
    float **tab;
    int nastepny; //podloze ktore jest jedno pole przed czolgiem
    int akt_roz_x;
    int akt_roz_y;
} Macierz;


Macierz utworz(int x, int y);
void zwolnij(Macierz *M);
Macierz przepisz_macierz(Macierz D, Macierz M);
Macierz realokuj(Macierz *ms, int ile_razy);
Macierz zeruj_macierz(Macierz B);