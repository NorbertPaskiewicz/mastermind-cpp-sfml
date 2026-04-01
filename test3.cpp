#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

class GraMastermind {
private:
    string tajnyKod;
    int dlugoscKodu;

    string losujKod() {
        string kod = "";
        for(int i = 0; i < dlugoscKodu; i++) {
            int losowaCyfra = rand() % 6 + 1; 
            kod += to_string(losowaCyfra);
        }
        return kod;
    }

public:
    GraMastermind() {
        srand(time(NULL)); 
        dlugoscKodu = 4;
        tajnyKod = losujKod();
    }

    void wypiszSekret() {
        cout << "[DEBUG] Tajny kod to: " << tajnyKod << endl;
    }

    string pobierzSekret() {
        return tajnyKod;
    }

    void ocenProbe(string probaGracza, int &dobreMiejsca, int &dobreKolory) {
        dobreMiejsca = 0; dobreKolory = 0;  
        string kopiaSekretu = tajnyKod;
        string kopiaProby = probaGracza;

        for(int i = 0; i < dlugoscKodu; i++) {
            if(kopiaProby[i] == kopiaSekretu[i]) {
                dobreMiejsca++;
                kopiaSekretu[i] = 'S'; 
                kopiaProby[i] = 'G';   
            }
        }

        for(int i = 0; i < dlugoscKodu; i++) {
            for(int j = 0; j < dlugoscKodu; j++) {
                if(kopiaProby[i] == kopiaSekretu[j]) {
                    dobreKolory++;
                    kopiaSekretu[j] = 'S'; 
                    break; 
                }
            }
        }
    }
};

int main() {
    sf::RenderWindow okno(sf::VideoMode(450, 700), "Mastermind");

    GraMastermind gra;
    gra.wypiszSekret(); 

    sf::Color kolory[7] = {
        sf::Color(150, 150, 150), sf::Color::Red, sf::Color::Green, 
        sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan
    };

    int plansza[10][4] = {0};
    int wyniki[10][2] = {0}; 

    sf::CircleShape otwor(20.f); 
    sf::CircleShape podpowiedz(8.f); 

    sf::RectangleShape guzikReset(sf::Vector2f(60.f, 40.f));
    guzikReset.setFillColor(sf::Color(200, 50, 50)); 
    guzikReset.setPosition(360.f, 620.f); 

    int wybranyKolor = 1; 
    int aktualnyRzad = 0; 
    bool koniecGry = false; 

    while (okno.isOpen()) {
        sf::Event zdarzenie;
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == sf::Event::Closed) okno.close();

            if (zdarzenie.type == sf::Event::MouseButtonPressed) {
                if (zdarzenie.mouseButton.button == sf::Mouse::Left) {
                    int myszX = zdarzenie.mouseButton.x;
                    int myszY = zdarzenie.mouseButton.y;

                    // OBSŁUGA GUZIKA RESET
                    if (myszX > 360 && myszX < 420 && myszY > 620 && myszY < 660) {
                        cout << "\nRESETOWANIE GRY!!!!!!!!!!!!" << endl;
                        // 1. Zerujemy planszę i wyniki
                        for (int r = 0; r < 10; r++) {
                            for (int k = 0; k < 4; k++) plansza[r][k] = 0;
                            wyniki[r][0] = 0;
                            wyniki[r][1] = 0;
                        }
                        // 2. Zerujemy stan gry
                        aktualnyRzad = 0;
                        koniecGry = false;
                        wybranyKolor = 1;
                        gra = GraMastermind();
                        gra.wypiszSekret();
                    }

                    // Obsługa palety i planszy działa tylko gdy gra trwa
                    if (!koniecGry) {
                        // Wybór koloru z palety
                        if (myszY > 600 && myszY < 680) {
                            for (int i = 1; i <= 6; i++) {
                                float srodekX = 35.f + (i - 1) * 55.f + 20.f; 
                                if (myszX > srodekX - 20 && myszX < srodekX + 20) wybranyKolor = i;
                            }
                        }

                        // Malowanie po planszy
                        float rzadY = 30.f + aktualnyRzad * 55.f; 
                        if (myszY > rzadY && myszY < rzadY + 40 && aktualnyRzad < 10) { 
                            for (int kolumna = 0; kolumna < 4; kolumna++) {
                                float srodekX = 100.f + kolumna * 55.f + 20.f;
                                if (myszX > srodekX - 20 && myszX < srodekX + 20) plansza[aktualnyRzad][kolumna] = wybranyKolor; 
                            }
                        }
                    }
                }
            }

            // Obsługa ENTERA też tylko w trakcie gry
            if (!koniecGry && zdarzenie.type == sf::Event::KeyPressed) {
                if (zdarzenie.key.code == sf::Keyboard::Enter && aktualnyRzad < 10) {
                    bool rzadPelny = true;
                    for(int i = 0; i < 4; i++) {
                        if(plansza[aktualnyRzad][i] == 0) rzadPelny = false;
                    }

                    if(rzadPelny) {
                        string strzal = "";
                        for(int i = 0; i < 4; i++) {
                            strzal += to_string(plansza[aktualnyRzad][i]);
                        }

                        int idealne = 0, tylkoKolor = 0;
                        gra.ocenProbe(strzal, idealne, tylkoKolor);
                        wyniki[aktualnyRzad][0] = idealne;
                        wyniki[aktualnyRzad][1] = tylkoKolor;

                        aktualnyRzad++;
                        cout << "Zatwierdzono rzad! Czarne: " << idealne << ", Biale: " << tylkoKolor << endl;

                        if (idealne == 4) {
                            cout << "\n*** GRATULACJE! WYGRYWASZ! ***" << endl;
                            koniecGry = true; 
                        } else if (aktualnyRzad == 10) {
                            cout << "\n*** PRZEGRANA! Koniec prob. Kod to: " << gra.pobierzSekret() << " ***" << endl;
                            koniecGry = true; 
                        }

                    } else {
                        cout << "Hej! Wypelnij caly rzad wszystkimi 4 kolorami przed wcisnieciem ENTER!" << endl;
                    }
                }
            }
        }

        okno.clear(sf::Color(50, 50, 50)); 

        // Rysowanie planszy głównej
        for (int rzad = 0; rzad < 10; rzad++) {
            for (int kolumna = 0; kolumna < 4; kolumna++) {
                int numerKoloru = plansza[rzad][kolumna];
                otwor.setFillColor(kolory[numerKoloru]);
                
                float pozycjaX = 100.f + kolumna * 55.f;
                float pozycjaY = 30.f + rzad * 55.f;
                
                otwor.setPosition(pozycjaX, pozycjaY);
                okno.draw(otwor); 
            }

            //rysowanie podpowiedzi
            if (rzad < aktualnyRzad) {
                int ileCzarnych = wyniki[rzad][0];
                int ileBialych = wyniki[rzad][1];

                for(int p = 0; p < 4; p++) {
                    if (ileCzarnych > 0) {
                        podpowiedz.setFillColor(sf::Color::Black);
                        ileCzarnych--;
                    } else if (ileBialych > 0) {
                        podpowiedz.setFillColor(sf::Color::White);
                        ileBialych--;
                    } else {
                        podpowiedz.setFillColor(sf::Color(100, 100, 100)); 
                    }

                    float podpowiedzX = 350.f + (p % 2) * 20.f; 
                    float podpowiedzY = 35.f + rzad * 55.f + (p / 2) * 20.f; 
                    
                    podpowiedz.setPosition(podpowiedzX, podpowiedzY);
                    okno.draw(podpowiedz);
                }
            }
        }

        // Rysowanie palety
        for (int i = 1; i <= 6; i++) {
            otwor.setFillColor(kolory[i]);
            float pozycjaX = 35.f + (i - 1) * 55.f;
            otwor.setPosition(pozycjaX, 620.f);
            okno.draw(otwor);
        }

        // Rysowanie guzika reset
        okno.draw(guzikReset);

        okno.display();
    }

    return 0;
}