#ifndef deklaracje
#define deklaracje

#include "macierz.h"
#include "lacznosc.h"
#include "zapis.h"
#include "mozg.h"

#endif

int main(int argc, char **argv)
{
    if (strcmp(argv[1], "reset") == 0)
    {
        make_request(reset, 1);
    }
    else if (strcmp(argv[1], "do_pliku") == 0)
    {
        FILE *fun = fopen(argv[2], "w+");
        char *token = infor;
        wypisz_do_pliku(fun, drugi_bot(make_request(token, 0)));
    }
    else if (strcmp(argv[1], "wypisz") == 0)
    {
        char *token = infor;
        wypisz(drugi_bot(make_request(token, 0)));
    }
    return 0;
}
