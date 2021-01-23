all:
	cc main.c macierz.c lacznosc.c zapis.c mozg.c -lcurl ../../cJSON/cjson.o -o "bot_nieinteligenty"

wypisz: all
	./bot_nieinteligenty wypisz
reset: all
	./bot_nieinteligenty reset
do_pliku: all
	./bot_nieinteligenty do_pliku test.txt
wyczysc:
	rm bot_nieinteligenty test.txt

