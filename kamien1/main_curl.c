#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../../cJSON/cJSON.h"

typedef struct _Memory
{
    char *response;
    size_t size;
} Memory;

#define W 12 //wysokosc i szerokosc mapy zeby szybciej zmienic jak cos
#define S 9

struct Macierz //obraz swiata w formie macierzy
{
    int r;
    int c;
    float tab[W][S];
};

#define forward "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/move/qwerty_15"
#define left "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/rotate/qwerty_15/left"
#define right "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/rotate/qwerty_15/right"
#define explore "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/explore/qwerty_15"
#define info "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/info/qwerty_15"

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
{
    /* to jest rzeczywista liczba bajtów przekazanych przez curl */
    size_t realsize = size * nmemb;

    /* jawnie konwertujemy (void*) na naszą strukturę, bo wiemy, że będziemy ją tutaj otrzymywać */
    Memory *mem = (Memory *) userp;

    char *ptr = NULL;

    /* Sprawdzamy czy pierwszy raz wywołujemy funkcję i trzeba zaalokować pamięć po raz pierwszy,
    czy trzeba zrobić reallokację (która kopiuje automatycznie starą zawartość w nowe miejsce) */
    if (mem->response != NULL)
        ptr = realloc(mem->response, mem->size + realsize + 1);
    else
        ptr = malloc(mem->size + realsize + 1);

    if (ptr == NULL)
        return 0; /* brak pamięci! */

    /* teraz zapamiętujemy nowy wskaźnik i doklejamy na końcu dane przekazane przez curl w 
       obszarze pamięci wskazywanym przez data */
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0; // to na końcu dodajemy bo to w końcu string, i zawsze powinien się skończyć!

    return realsize;
}

char *buffor;

char * make_request(char *url, int n)
{
CURL *curl;
CURLcode res;
Memory chunk;
chunk.size = 0;
chunk.response = NULL;
curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        if(n == 1) //zeby nie wypisywac za kazdym razem tylko za pierwszym tej daty serwera itp
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        /* to jest funkcja 'callback', która będzie wywołana przez curl gdy odczyta on kawałek danych
       ta funkcja musi mieć wywołanie zgodne z wymaganiami, które możesz sprawdzić tutaj:
       https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        /* to jest adress struktury, który będzie przekazywany do naszej funkcji 'callback',
       do tej struktury nasz funkcja 'callback' będzie dopisywać wynik */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        /* Wykonaj zapytanie 'synchronicznie', to znaczy następna linijka kodu nie wykona się
       dopóki nie nadejdzie odpowiedź z serwera. */
        res = curl_easy_perform(curl);

        /* Sprawdzamy czy wystapił jakis błąd? */
        if (res != CURLE_OK)
            fprintf(stderr, "Błąd! curl_easy_perform() niepowodzenie: %s\n", curl_easy_strerror(res));
        else
        {
            printf("%s \n", chunk.response);
            buffor = NULL;
            buffor = chunk.response;
        }
        
        /* zawsze po sobie sprzątaj */
        free(chunk.response);
        curl_easy_cleanup(curl);
    }
}

int check_info (int n) {
    char *token = info;
    make_request(token,n);
    return 0;
}

int move(int n) {
    char *token = forward;
    make_request(token,n);
    return 0;
}

int turn_left(int n) {
    char *token = left;
    make_request(token,n);
    return 0;
}

int turn_right(int n) {
    char *token = right;
    make_request(token,n);
    return 0;
}

int lets_explore(int n) {
    char *token = explore;
    make_request(token, n);
    return 0;
}

void wypisz(struct Macierz m)
{
    int i,j;
    printf("[ ");
    for(i = 0; i < m.r; i++)
    {
        for(j = 0; j < m.c; j++)
        printf("%5.1f", m.tab[i][j]);
        if (i < m.r-1)
        printf("\n");
    }
    printf(" ]\n");
}

int main(int argc, char **argv)
{
    int n; //zmienna pomocnicza do wypisywania lub nie info o polaczeniu
    for (int i = 1; argv[i] != NULL; i++)
    {
        if(i == 1)
            n = 1;
        else
            n = 0;
        if( strcmp (argv[i], "M") == 0)
            move(n);
        else if( strcmp (argv[i], "L") == 0)
            turn_left(n);
        else if( strcmp (argv[i], "R") == 0)
            turn_right(n);
        else if( strcmp (argv[i], "E") == 0)
            lets_explore(n);
        else if( strcmp (argv[i], "I") == 0)
            check_info(n);
    }
    
    printf("to jest test %s \n",buffor);
    return 0;
}