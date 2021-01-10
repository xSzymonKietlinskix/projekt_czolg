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

#define W 20 //wysokosc i szerokosc mapy zeby szybciej zmienic jak cos
#define S 20



struct Macierz //obraz swiata w formie macierzy
{   
    int xo; //pocztakowe polozenie czolgu na osi x
    int yo; //pocztakowe polozenie czolgu na osi y
    char zwrot; //pocztakowy zwrot czolgu
    float tab[W][S];
    int nastepny; //podloze ktore jest jedno pole przed czolgiem
};

#define forward "http://edi.iem.pw.edu.pl:30000/worlds/api/v1/worlds/move/qwerty_15" //zeby szybciej bylo w dalszej czesci projektu
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

char *buffor; //buffor bedzie uzywany do przechwytywania chunck response, jest zmienna globalna. W przyszlosci go nie bedzie

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

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        /* Sprawdzamy czy wystapił jakis błąd? */
        if (res != CURLE_OK)
            fprintf(stderr, "Błąd! curl_easy_perform() niepowodzenie: %s\n", curl_easy_strerror(res));
        else
        {
            //printf("%s \n", chunk.response); //printf jest wylaczony bo denerwowal w konosli przy robieniu mapy
            buffor = NULL;
            buffor = chunk.response;
        }
        
        /* zawsze po sobie sprzątaj */ //chwilowo tego nie zwalniam
        //free(chunk.response);
        curl_easy_cleanup(curl);
    }
}

int check_info (int n) { //poniższe funkcje wykonoją make request dla odpowiedniej czynności
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

struct Macierz wczytaj_poczatek() //funkcja wczytujaca macierz z początkowym połozeniem, na razie nie odgrywa istonej roli, lecz gdy bede minimalizowac liczbe krokow moze sie przydac
{
    struct Macierz M; 
    for(int i = 0; i < W; i++) //zerowanie macierzy co by krzaków nie było
        for(int j = 0; j < S; j++)
            M.tab[i][j]=0;
    
    const cJSON *dana = NULL; //trzy zmienne pomocnicze do przechowywania danych
    const cJSON *dane = NULL;
    const cJSON *status = NULL;

    int i = 0; //zmienna pomocnicza przyda się do czytania Json-a

    check_info(0); //robi request o info poczatkowe

    char * const monitor = buffor; //wczytuje Json file z make request
    cJSON *poczatek_json = cJSON_Parse(monitor);
    if (poczatek_json == NULL) //sprawdza czy pusty nie jest
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        goto end;
    }

    status = cJSON_GetObjectItemCaseSensitive(poczatek_json, "status");
    if (cJSON_IsString(status) && (status->valuestring != NULL)) //w tym wypadku sluzy jak sprawdzenie poprawnosci polczenia
    {
        printf("Checking if exist \"%s\"\n", status->valuestring);
    }

    dane = cJSON_GetObjectItemCaseSensitive(poczatek_json, "payload");
    
    cJSON_ArrayForEach(dana, dane)
    {
        if (!cJSON_IsNumber(dana)) //sprawdzam czy dana nie jest numerem zeby nie zrobic segmentation fault przy uzywaniu jakis stringow
        {
            if(strcmp (dana->valuestring, "grass") == 0) {
            //printf("typ podłoża to trawa \n");
            //M.tab[M.xo][M.yo] = 1; //na razie nie jest to uzywane
             }
            else if(strcmp (dana->valuestring, "sand") == 0) {
            //M.tab[M.xo][M.yo] = 2; 
            //printf("typ podłoża to piasek \n");
            }
            else if(strcmp (dana->valuestring, "S") == 0)
            M.zwrot = 'S';
            else if(strcmp (dana->valuestring, "W") == 0)
            M.zwrot = 'W';
            else if(strcmp (dana->valuestring, "E") == 0)
            M.zwrot = 'E';
            else if(strcmp (dana->valuestring, "N") == 0)
            M.zwrot = 'N';
        }
        else
        {
            if(i == 1){
            M.xo = dana->valuedouble; 
            //printf("to jest położenie x %f \n", dana->valuedouble);
            }
            else if(i == 2){
            M.yo = dana->valuedouble;
            //printf("to jest położenie y %f \n", dana->valuedouble);
            }
        }
        i++;
        }
    
end:
    cJSON_Delete(poczatek_json);
    buffor = NULL;
    free(buffor);
    return M;
}

struct Macierz pierwszy_bot() //odpowiada za czytanie JSON-a z komendy explore i zapisywanie podloza do macierzy
{   
    struct Macierz M = wczytaj_poczatek(); //wczytuje polozenie pocztakowe
   
    const cJSON *payload = NULL; //zmienne pomocnicze do przechowywania danych
    const cJSON *list = NULL;;
    const cJSON *type = NULL;
    lets_explore(0); //wlacza ekspoloracje
    char * const monitor = buffor; //wczytuje Json file z make request
    cJSON *explore_json = cJSON_Parse(monitor);
    if (explore_json == NULL) //sprawdza istnienie Jsona
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        goto end;
    }

    payload = cJSON_GetObjectItemCaseSensitive(explore_json, "payload"); //pozyskuje koljene informacje
    list = cJSON_GetObjectItemCaseSensitive(payload, "list");

    int i = 0; //3 zmienne pomocnicze
    int a = 0;
    int b = 0;
    cJSON_ArrayForEach(type, list)
    {
        cJSON *x = cJSON_GetObjectItemCaseSensitive(type, "x");
        cJSON *y = cJSON_GetObjectItemCaseSensitive(type, "y");
        cJSON *field = cJSON_GetObjectItemCaseSensitive(type, "type");

        b = x->valueint; //przechywtuje na chwile biezace polozenie czolgu
        a = y->valueint;

        if(strcmp(field->valuestring, "grass") == 0){
            M.tab[a][b] = 1;
            if(i == 1) //chodzi o to zeby tylko gdy mowimy o polu centralnie przed czolgiem to dzialalo
            M.nastepny = 1;
        }
        else if(strcmp(field->valuestring, "sand") == 0){
            M.tab[a][b] = 2;
            if(i == 1)
            M.nastepny = 2;
        }
        else if(strcmp(field->valuestring, "wall") == 0){
            M.tab[a][b] = 8;
            if(i == 1)
            M.nastepny = 8;
        }
        a = 0;
        b = 0;
        i++;
    }
    end:
    cJSON_Delete(explore_json);
    free(buffor);
    return M;
}

struct Macierz drugi_bot() //odpowiada za poruszanie sie czolgu
{
    struct Macierz M = pierwszy_bot(); //wczytuje macierz z pierwszego bota
    struct Macierz B; //macierz pomocnicza
    B.nastepny = M.nastepny;
    for(int i = 0; i < W; i++) //wyzerowoana zeby bledow nie bylo
        for(int j = 0; j < S; j++)
            B.tab[i][j]=0;
    int i = 0; //licnzik powtorzen
    while(i < 40){ //na razie dobierane na oko bo nie ma jeszczse zadnego mechanizmu minimalizujacego te kroki
    move(0); //krok do przodu
    struct Macierz M = pierwszy_bot();
    if(M.nastepny == 8) //jezeli przed nami sciana to obraca w lewo. Tutaj na pewno tworzą się niepotrzbne kroki. DO poprawy
    turn_left(0);
    for(int i = 0; i < W; i++)
        for(int j = 0; j < S; j++) //mialem problem z zerowaniem sie macierzy dltaego przepisuje macierz M do B pomiajajc puste pola
            if(M.tab[i][j] != 0)
                B.tab[i][j] = M.tab[i][j];
    M.zwrot = B.zwrot;
    M.nastepny = B.nastepny; // na wszeli wypadek przenosze wszytskie info z jedenj do drugiej. Moze sie kiedys przyda
    M.xo = B.xo;
    M.yo = B.yo;
    i++;
    }
    return B;
}
    



void wypisz(struct Macierz m) //wypisuje macierz
{
   struct Macierz B; //byl problem z odbiciem lustrzanym stąd druga macierz naprawcza
   for(int i = 0; i < W; i++)
    {
        for(int j = 0; j < S; j++){
        m.tab[i][j] = B.tab[W - 1 - i][S - 1 - j];
        }
    }
    int i,j;
    //printf("["); //jakies tam wizulane kwestie to nie teraz
    for(i = 0; i < W; i++)
    {
        for(j = 0; j < S; j++){
        if(B.tab[W - 1 - i][j] != 0) //nie wypisuje 0 bo nieczytlnie wtedy jest
        printf("%5.0f", B.tab[W - 1 - i][j]);
        }
        if (i < W-1)
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int n; //zmienna pomocnicza do wypisywania lub nie info o polaczeniu
    if (argv[1] == NULL) {
        //wypisz(wczytaj_poczatek());
        //wypisz(pierwszy_bot());
        wypisz(drugi_bot()); //na kamien milowy 1. Poprawne dzialnie fukncji po wpisaniu ./a.out i to tyle
    }
    else {
    for (int i = 1; argv[i] != NULL; i++)
    {
        if(i == 1)
            n = 0;
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
    }}
    
    return 0;
}
