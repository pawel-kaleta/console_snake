#include<iostream>
#include<conio.h> // getch();
#include<cstdlib> // system("CLS");
#include<time.h> // srand(time(NULL));
#include<windows.h> // Sleep(); zmienne do zarządzania konsolą: COORD, CONSOLE_CURSOR_INFO
#include<queue> // queue <>;

using namespace std;

//skrajne wartości, jakie mogą przyjąć wymiary planszy
const int minimum = 5;
const int maximum = 40;

char pole[maximum][maximum]; //plansza: w - waz, p - pusto; j - jedzenie
bool gra = 1; //zmienna do sprawdzania, czy gramy dalej
int klawisz; //zmienna do zapisywania poleceń z klawiatury - sterowanie

//struktury zawierające informacje do zarządzania konsolą
COORD a;
CONSOLE_CURSOR_INFO cci;


struct waz
{
    int kierunek;
    queue < pair<int,int> > ogon; //współrzędne segmentów węża

    waz * dodaj(int x, int y, char pole[maximum][maximum]) //dodanie segmenu węża na danych współrzędnych
    {
        ogon.push(pair<int,int> (x,y));
        pole[x][y] = 'w';
    }

    waz * odejmij(char pole[maximum][maximum]) //zabranie ostatniego segmentu węża
    {
        pole[ogon.front().first][ogon.front().second] = 'p';
        ogon.pop();
    }

    waz * losujPole(int szer, int wys, char pole[maximum][maximum])
    {
        int x = rand() % szer;
        int y = rand() % wys;
        dodaj(x, y, pole); // tworzymy głowę na losowych pozycjach
        kierunek = rand() % 4; // losujemy kierunek ruchu węża
        for(int i=0; i<3; i++) // tworzymy 3 dodatkowe segmenty węża na starcie
        {
            switch(kierunek) // dla każdej z nich wyliczamy współrzędne w zależności od kierunku, w jakim wąż będzie się poruszał
            {
            case 0:
                x++;
                x%=szer;
                break;
            case 1:
                x--;
                x+=szer;
                x%=szer;
                break;
            case 2:
                y++;
                y%=wys;
                break;
            case 3:
                y--;
                y+=wys;
                y%=wys;
            }
            dodaj(x,y, pole);
        }
    }
};

struct ustawienia
{
    int szerokosc, wysokosc, szybkosc;

    ustawienia * wczytaj()
    {
        cout << "Podaj szerokosc planszy (min " << minimum << ", max " << maximum << ")"<< endl;
        cin >> szerokosc;
        if(szerokosc < minimum) szerokosc = minimum;
        else if(szerokosc > maximum) szerokosc = maximum;

        cout << "Podaj wysokosc planszy (min " << minimum << ", max " << maximum << ")" << endl;
        cin >> wysokosc;
        if(wysokosc < minimum) wysokosc = minimum;
        else if(wysokosc > maximum) wysokosc = maximum;

        cout << "Podaj szybkosc gry (min 1, max 10)" << endl;
        cin >> szybkosc;
        if(szybkosc < 0) szybkosc = 1;
        else if(szybkosc > 10) szybkosc = 10;

        cout << endl
        << "Wymiary planszy do gry: " << szerokosc << "x" << wysokosc << endl
        << "Szybkosc gry: " << szybkosc << endl;
    }
};

struct jedzenie
{
    int x,y;
    bool zjedzone = 1;

    jedzenie * losujPole( ustawienia &Ustawienia, char pole[maximum][maximum])
    {
        do // losuj pole dla jedzenia
        {
            x = rand() % Ustawienia.szerokosc;
            y = rand() % Ustawienia.wysokosc;
            zjedzone = 0;
        }
        while(pole[x][y] != 'p'); //dopóki nie natrafisz na wolne - nie będące wężem
        pole[x][y] = 'j';
    }
};

void rysujPlansze(waz &Waz, jedzenie &Jedzenie, ustawienia &Ustawienia) //dwa znaki na "kratkę"
{
    a.X = 0;
    a.Y = 0;
    SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), a); // ustawienie kursora na początku konsoli bez jej czyszczenia

    //górna ściana
    for(int i=0; i<=Ustawienia.szerokosc; i++)
        cout << (char) 219 << (char) 219;

    for(int i=0; i<Ustawienia.wysokosc; i++)
    {
        //lewa ściana
        cout << endl << (char) 219;

        //obszar gry
        for(int j=0; j<Ustawienia.szerokosc; j++)
            if(pole[j][i] == 'p') cout << "  ";
                else if(pole[j][i] == 'w') cout << "[]"; //segment węża
                        else cout << "o~"; //jedzenie - mysz

        //prawa ściana
        cout << (char) 219;
    }

    //dolna ściana
    cout << endl;
    for(int i=0; i<=Ustawienia.szerokosc; i++)
        cout << (char) 219 << (char) 219;
}

void ruch(waz &Waz, jedzenie &Jedzenie, ustawienia &Ustawienia, char pole[maximum][maximum], bool &gra)
{
    int x,y; //współrzędne ostatniego segmentu w kolejce, który jest głową węża
    x = Waz.ogon.back().first;
    y = Waz.ogon.back().second;
    int szer = Ustawienia.szerokosc;
    int wys = Ustawienia.wysokosc;
    switch(Waz.kierunek) //w zależności od kierunku ruchu węża generujemy współrzędne dla nowego segmentu
    {
    case 0:
        x++;
        x%=szer;
        break;
    case 1:
        x--;
        x+=szer;
        x%=szer;
        break;
    case 2:
        y++;
        y%=wys;
        break;
    case 3:
        y--;
        y+=wys;
        y%=wys;
        break;
    }

    if(pole[x][y]=='w') //sprawdzamy, czy nie zjadamy siebie samego
        gra = 0;

    if(x != Jedzenie.x) //jeśli nie zjadamy myszy pozbywamy się pierwszego elementu kolejki - ostatniego segmentu węża
        Waz.odejmij(pole);
    else
        if(y != Jedzenie.y)
            Waz.odejmij(pole);
        else
            Jedzenie.zjedzone=1;
    Waz.dodaj(x,y,pole); //dodajemy nowy segment węża na jego początku, czyli na końcu kolejki

}

void sterowanie (int &kierunek, int &klawisz)
{
    if(kbhit()) //jeśli wciśnięty jakiś klawisz
    {
		//klawisze funkcyjne mają przypisane po dwa sygnały z klawiatury
        getch(); //pierwszy do pominięcia - typ klawisza funkcyjnego, w naszym przypadku strzałki
        klawisz = getch(); //drugi do wczytania - oznacza konkretny klawisz z danego typu
        //w zależności od tego jaki klawisz był wciśniety zmieniamy kierunek ruchu węża
        //i sprawdzamy, czy przypadkiem nie próbujey zawrócić
        switch(klawisz)
        {
        case 72: //góra
            if(kierunek!=2)kierunek = 3;
            break;
        case 75: //lewo
            if(kierunek!=0)kierunek = 1;
            break;
        case 77: //prawo
            if(kierunek!=1)kierunek = 0;
            break;
        case 80: //dół
            if(kierunek!=3)kierunek = 2;
        }
    }
}

void gameOver (int wys, int szer)
{
    //wyśrodkowanie napisu
    for(int i=0; i<wys; i++)
        cout << endl;
    for(int i=0; i<szer-4; i++)
        cout << " ";

    string napis = "GAME OVER";

    for(int i=0; i<napis.length(); i++)
    {
        Sleep(100);
        cout << napis[i];
    }

    for(int i=0; i<wys; i++) //cd wyśrodkowania napisu
        cout << endl;
}

void konsola()
{
    system("CLS");
    GetConsoleCursorInfo(GetStdHandle( STD_OUTPUT_HANDLE ), & cci); //pobranie różnych info o kursorze
    cci.bVisible = false; //zmiana widoczności kursora
    SetConsoleCursorInfo( GetStdHandle( STD_OUTPUT_HANDLE ), & cci); //wprowadzenie zmian

}

int main()
{
    srand(time(NULL)); // losowania bazują na zawsze aktualnym czasie, a nie na początku programu

    ustawienia Ustawienia;
    Ustawienia.wczytaj();

    cout <<  "Sterowanie: strzalki" << endl << endl << "By rozpoczac nacisnij dowolny klawisz";
    getch();

    konsola(); //poprawa wizualizacji gry - brak mrygania, brak kursora

    //wyczyszczenie planszy po alokacji
    for(int i=0; i<Ustawienia.szerokosc; i++)
        for(int j=0; j<Ustawienia.wysokosc; j++)
            pole[i][j] = 'p';

    waz Waz; //stworzenie węża
    Waz.losujPole(Ustawienia.szerokosc, Ustawienia.wysokosc, pole); //wylosowanie pozycji startowej i kierunku startowego

    jedzenie Jedzenie; //utworzenie struktury dla jedzenia

	//główna pętla gry
    do
    {
        if(Jedzenie.zjedzone)
            Jedzenie.losujPole(Ustawienia,pole); //jeśli nie ma jedzenia, tworzymy nowe
        Sleep(10*(11-Ustawienia.szybkosc)); //przerwa techniczna
        sterowanie(Waz.kierunek, klawisz); //wczytywanie poleceń gracza i modyfikowanie kierunku ruchu węża zgodnie z nimi
        ruch(Waz, Jedzenie, Ustawienia, pole, gra); //przesunięcie węża i jeśli zjadł, to przedłużenie
        rysujPlansze(Waz, Jedzenie, Ustawienia); //wygenerowanie klatki
    }while(gra);

    //efekt wizualny mrygania po przegranej
    for(int i=0; i<4; i++)
    {
        system("CLS"); //wyczyszczenie konsoli
        Sleep(400);
        rysujPlansze(Waz, Jedzenie, Ustawienia);
        Sleep(400);
    }

    system("CLS"); //wyczyszczenie konsoli

    gameOver(Ustawienia.wysokosc/2, Ustawienia.szerokosc);

    cout << "Nacisnij dowolny klawisz, aby wyjsc";
    getch(); //wstrzymanie zamknięcia konsoli poprzez wczytywanie znaku z klawiatry

    return 0;
}

