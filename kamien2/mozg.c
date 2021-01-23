#include "macierz.h"
#include "mozg.h"
#include "lacznosc.h"

AkPol znajdz_rozmiar()
{
    AkPol M;
    const cJSON *payload = NULL;
    const cJSON *list = NULL;
    const cJSON *type = NULL;

    char *const dane = make_request(explore, 0);
    cJSON *explore_json = cJSON_Parse(dane);

    payload = cJSON_GetObjectItemCaseSensitive(explore_json, "payload");
    list = cJSON_GetObjectItemCaseSensitive(payload, "list");

    cJSON_ArrayForEach(type, list)
    {
        cJSON *x = cJSON_GetObjectItemCaseSensitive(type, "x");
        cJSON *y = cJSON_GetObjectItemCaseSensitive(type, "y");
        cJSON *field = cJSON_GetObjectItemCaseSensitive(type, "type");

        M.akt_x = x->valueint;
        M.akt_y = y->valueint;
    }
    return M;
}

Macierz pierwszy_bot(char *buffor, Macierz M) //odpowiada za czytanie JSON-a z komendy explore i zapisywanie podloza do macierzy
{
    zeruj_macierz(M);
    const cJSON *payload = NULL;
    const cJSON *list = NULL;
    const cJSON *type = NULL;

    char *const dane = make_request(explore, 0);
    cJSON *explore_json = cJSON_Parse(dane);

    payload = cJSON_GetObjectItemCaseSensitive(explore_json, "payload");
    list = cJSON_GetObjectItemCaseSensitive(payload, "list");

    int pole_przed_czolgiem = 0;
    int a = 0;
    int b = 0;
    cJSON_ArrayForEach(type, list)
    {
        cJSON *x = cJSON_GetObjectItemCaseSensitive(type, "x");
        cJSON *y = cJSON_GetObjectItemCaseSensitive(type, "y");
        cJSON *field = cJSON_GetObjectItemCaseSensitive(type, "type");

        b = x->valueint;
        a = y->valueint;

        if (strcmp(field->valuestring, "grass") == 0)
        {
            M.tab[a][b] = 1;
            if (pole_przed_czolgiem == 1)
                M.nastepny = 1;
        }
        else if (strcmp(field->valuestring, "sand") == 0)
        {
            M.tab[a][b] = 2;
            if (pole_przed_czolgiem == 1)
                M.nastepny = 2;
        }
        else if (strcmp(field->valuestring, "wall") == 0)
        {
            M.tab[a][b] = 8;
            if (pole_przed_czolgiem == 1)
                M.nastepny = 8;
        }
        a = 0;
        b = 0;
        pole_przed_czolgiem++;
    }

end:
    cJSON_Delete(explore_json);
    return M;
}

Macierz drugi_bot(char *buffor) //odpowiada za poruszanie sie czolgu
{
    int i = 0; //licznik pętli
    int ile_powiekszen = 1; //licznik powiekszen macierzy
    AkPol F = znajdz_rozmiar();
    while (F.akt_x >= 5 * ile_powiekszen || F.akt_y >= 5 * ile_powiekszen)
    {
        ile_powiekszen++;
    }
    int c = 5 * ile_powiekszen;
    int d = 5 * ile_powiekszen;

    Macierz C = utworz(c, d);
    Macierz M = pierwszy_bot(buffor, C);
    Macierz B = utworz(c, d);
    zeruj_macierz(B);
    
    while (i < 50 * (ile_powiekszen))
    {
        AkPol C = znajdz_rozmiar();
        printf("aktualne polozenie  (%d %d)\n", C.akt_x, C.akt_y);
        if (C.akt_x >= 5 * ile_powiekszen || C.akt_y >= 5 * ile_powiekszen)
        {
            Macierz P = utworz(5 * ile_powiekszen, 5 * ile_powiekszen); //buffor
            zeruj_macierz(P);
            P = przepisz_macierz(P, M);
            ile_powiekszen++;
            realokuj(&B, ile_powiekszen);
            zeruj_macierz(B);
            B = przepisz_macierz(B, P);
        }
        int a = 5 * ile_powiekszen;
        int b = 5 * ile_powiekszen;

        Macierz D = utworz(a, b);
        printf("aktualny rozmiar świata %d x %d \n", D.akt_roz_x, D.akt_roz_y);
        Macierz M = pierwszy_bot(buffor, D);
        
        if (M.nastepny != 8)
        {
            make_request(forward, 0);
            make_request(left, 0);
            make_request(forward, 0);
        }
        else if (M.nastepny == 8)
        {
            make_request(right, 0);
        }
        B = przepisz_macierz(B, M);
        i++;
    }

    zwolnij(&C);
    return B;
}