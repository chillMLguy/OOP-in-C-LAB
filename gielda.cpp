#include <iostream>
#include <vector>
#include <string>
#include <random>

// -----------------------------------------------------------
//   Klasa Oferta
// -----------------------------------------------------------
class Sprzedajacy; // deklaracja w przód

class Oferta {
public:
    Oferta(const std::string& nazwaTowaru, double cena, int liczbaSztuk, Sprzedajacy* sprzed)
        : m_nazwaTowaru(nazwaTowaru),
          m_cena(cena),
          m_liczbaSztuk(liczbaSztuk),
          m_sprzedajacy(sprzed)
    {
    }

    // Nie chcemy kopiować Ofert, więc wyłączamy konstruktor kopiujący i operator=
    Oferta(const Oferta&) = delete;
    Oferta& operator=(const Oferta&) = delete;

    // brak wywołania jakichś wyjątków – w razie błędu tylko wypisujemy
    void sprzedajSztuki(int ile) {
        if (ile <= 0 || ile > m_liczbaSztuk) {
            std::cerr << "[Oferta] Błędna liczba sztuk do sprzedaży!\n";
            return;
        }
        m_liczbaSztuk -= ile;
    }

    const std::string& getNazwaTowaru() const { return m_nazwaTowaru; }
    double getCena() const                    { return m_cena; }
    int getLiczbaSztuk() const               { return m_liczbaSztuk; }
    Sprzedajacy* getSprzedajacy() const      { return m_sprzedajacy; }

private:
    std::string   m_nazwaTowaru;
    double        m_cena;
    int           m_liczbaSztuk;
    Sprzedajacy*  m_sprzedajacy;
};

// -----------------------------------------------------------
//   Klasa Sprzedajacy
// -----------------------------------------------------------
class Sprzedajacy {
public:
    explicit Sprzedajacy(const std::string& unikatowyId)
        : m_id(unikatowyId)
    {
    }

    // Wyłączamy kopiowanie (nie jest potrzebne):
    Sprzedajacy(const Sprzedajacy&) = delete;
    Sprzedajacy& operator=(const Sprzedajacy&) = delete;

    // Giełda będzie zwalniać pamięć, więc w destruktorze nic nie robimy
    ~Sprzedajacy() {
    }

    const std::string& getId() const {
        return m_id;
    }

    Oferta* wystawOferte(const std::string& nazwaTowaru, double cena, int liczbaSztuk) {
        Oferta* nowa = new Oferta(nazwaTowaru, cena, liczbaSztuk, this);
        m_mojeOferty.push_back(nowa);
        return nowa;
    }

private:
    std::string        m_id;
    std::vector<Oferta*> m_mojeOferty; // (opcjonalnie) Sprzedawca pamięta swoje oferty
};

// -----------------------------------------------------------
//   Deklaracja klasy Gielda (użyta w Kupujacy)
// -----------------------------------------------------------
class Gielda;

// -----------------------------------------------------------
//   Klasa bazowa Kupujacy (ABSTRAKCYJNA, z metodą wirtualną)
// -----------------------------------------------------------
class Kupujacy {
public:
    Kupujacy(const std::string& id, double budzet)
        : m_id(id), m_budzet(budzet)
    {
    }

    Kupujacy(const Kupujacy&) = delete;
    Kupujacy& operator=(const Kupujacy&) = delete;

    virtual ~Kupujacy() {
    }

    const std::string& getId() const   { return m_id; }
    double getBudzet() const          { return m_budzet; }

    // Zmniejsza budżet. Jeśli kwota > m_budzet, tylko wypisze błąd.
    void zmniejszBudzet(double kwota) {
        if (kwota > m_budzet) {
            std::cerr << "[Kupujacy] Budżet za mały, nie można kupić!\n";
            return;
        }
        m_budzet -= kwota;
    }

    // Metoda czysto wirtualna (implementacje w podklasach - różne strategie).
    virtual void kup(const std::string& nazwaTowaru, Gielda* g) = 0;

protected:
    std::string m_id;
    double      m_budzet;
};

// -----------------------------------------------------------
//   Podklasy Kupujacy (różne strategie)
// -----------------------------------------------------------

// Kupuje 1 szt. z najtańszej dostępnej oferty
class KupujacyEkonomiczny : public Kupujacy {
public:
    KupujacyEkonomiczny(const std::string& id, double budzet)
        : Kupujacy(id, budzet)
    {}

    void kup(const std::string& nazwaTowaru, Gielda* g) override;
};

// Kupuje 1 szt. z najdroższej dostępnej oferty
class KupujacyWybredny : public Kupujacy {
public:
    KupujacyWybredny(const std::string& id, double budzet)
        : Kupujacy(id, budzet)
    {}

    void kup(const std::string& nazwaTowaru, Gielda* g) override;
};

// Kupuje po 1 sztuce z każdej oferty, zaczynając od najtańszej
class KupujacyDetalista : public Kupujacy {
public:
    KupujacyDetalista(const std::string& id, double budzet)
        : Kupujacy(id, budzet)
    {}

    void kup(const std::string& nazwaTowaru, Gielda* g) override;
};

// -----------------------------------------------------------
//   Klasa Gielda
// -----------------------------------------------------------
class Gielda {
public:
    Gielda() {
        // Inicjalizujemy generator liczb pseudolosowych
        std::random_device rd;
        m_gen.seed(rd());
    }

    ~Gielda() {
        // Zwalniamy wszystko to, co 'posiadamy'
        for (std::size_t i = 0; i < m_oferty.size(); i++) {
            delete m_oferty[i];
        }
        for (std::size_t i = 0; i < m_sprzedajacy.size(); i++) {
            delete m_sprzedajacy[i];
        }
        for (std::size_t i = 0; i < m_kupujacy.size(); i++) {
            delete m_kupujacy[i];
        }
    }

    // uniemożliwiamy kopiowanie Gieldy
    Gielda(const Gielda&) = delete;  
    Gielda(const Gielda &other)
    {
        // Uwaga: przypisujemy wektory wskaźników wprost
        // Oba obiekty Gielda będą pokazywały na TE SAME obiekty Sprzedajacy, Kupujacy, Oferta!
        m_sprzedajacy = other.m_sprzedajacy;
        m_kupujacy    = other.m_kupujacy;
        m_oferty      = other.m_oferty;

        // Możemy skopiować też stan generatora:
        m_gen = other.m_gen;
    }
    


    Gielda& operator=(const Gielda&) = delete;
    Gielda& operator=(const Gielda &other)
    {
        if (this != &other) {
            // Najpierw zwalniamy "własne" obiekty,
            // bo zaraz nadpiszemy wskaźniki.
            for (auto* s : m_sprzedajacy) delete s;
            for (auto* k : m_kupujacy)    delete k;
            for (auto* o : m_oferty)     delete o;

            // Teraz kopiujemy wskaźniki.
            // Uwaga: to spowoduje "współdzielenie" obiektów między dwiema giełdami!
            m_sprzedajacy = other.m_sprzedajacy;
            m_kupujacy    = other.m_kupujacy;
            m_oferty      = other.m_oferty;
            m_gen         = other.m_gen;
        }
        return *this;
    }


    void zarejestrujSprzedajacego(Sprzedajacy* s) {
        m_sprzedajacy.push_back(s);
    }

    void zarejestrujKupujacego(Kupujacy* k) {
        m_kupujacy.push_back(k);
    }

    void dodajOferte(Oferta* o) {
        m_oferty.push_back(o);
    }

    // Zwraca wszystkie oferty na dany towar, w których jest > 0 sztuk
    std::vector<Oferta*> znajdzOferty(const std::string& nazwaTowaru) {
        std::vector<Oferta*> wynik;
        for (std::size_t i = 0; i < m_oferty.size(); i++) {
            Oferta* of = m_oferty[i];
            if (of->getNazwaTowaru() == nazwaTowaru && of->getLiczbaSztuk() > 0) {
                wynik.push_back(of);
            }
        }
        return wynik;
    }

    // Kupujący kupuje towar -> wywołujemy polimorficzną metodę kup(...)
    void kupTowar(Kupujacy* k, const std::string& nazwaTowaru) {
        k->kup(nazwaTowaru, this);
    }

    // Prosty wypis stanu giełdy
    void wypiszStan() {
        std::cout << "\n=== STAN GIELDY ===\n";
        for (std::size_t i = 0; i < m_oferty.size(); i++) {
            Oferta* of = m_oferty[i];
            std::cout << "Towar: " << of->getNazwaTowaru()
                      << ", cena: " << of->getCena()
                      << ", sztuk: " << of->getLiczbaSztuk()
                      << ", sprzedajacy: " << of->getSprzedajacy()->getId()
                      << "\n";
        }
        std::cout << "====================\n";
    }

    // Potrzebny np. w strategiach losowych (nie używamy w tym przykładzie)
    std::mt19937& getGen() { return m_gen; }

private:
    std::vector<Sprzedajacy*> m_sprzedajacy;
    std::vector<Kupujacy*>    m_kupujacy;
    std::vector<Oferta*>      m_oferty;
    std::mt19937 m_gen;
};

// -----------------------------------------------------------
//   Implementacje metod kup(...) w strategiach Kupujacy
// -----------------------------------------------------------

void KupujacyEkonomiczny::kup(const std::string& nazwaTowaru, Gielda* g)
{
    std::vector<Oferta*> oferty = g->znajdzOferty(nazwaTowaru);
    if (oferty.empty()) {
        std::cout << "[Ekonomiczny:" << getId()
                  << "] Brak ofert na " << nazwaTowaru << "\n";
        return;
    }
    // Ręczne szukanie najtańszej (bez <algorithm>)
    Oferta* najtansza = oferty[0];
    for (std::size_t i = 1; i < oferty.size(); i++) {
        if (oferty[i]->getCena() < najtansza->getCena()) {
            najtansza = oferty[i];
        }
    }
    double cena = najtansza->getCena();
    if (cena <= getBudzet() && najtansza->getLiczbaSztuk() > 0) {
        zmniejszBudzet(cena);
        najtansza->sprzedajSztuki(1);
        std::cout << "[Ekonomiczny:" << getId()
                  << "] Kupił 1 szt. '" << nazwaTowaru
                  << "' za " << cena << "\n";
    } else {
        std::cout << "[Ekonomiczny:" << getId()
                  << "] Za drogo, nie kupił.\n";
    }
}

void KupujacyWybredny::kup(const std::string& nazwaTowaru, Gielda* g)
{
    std::vector<Oferta*> oferty = g->znajdzOferty(nazwaTowaru);
    if (oferty.empty()) {
        std::cout << "[Wybredny:" << getId()
                  << "] Brak ofert na " << nazwaTowaru << "\n";
        return;
    }
    // Ręczne szukanie najdroższej
    Oferta* najdrozsza = oferty[0];
    for (std::size_t i = 1; i < oferty.size(); i++) {
        if (oferty[i]->getCena() > najdrozsza->getCena()) {
            najdrozsza = oferty[i];
        }
    }
    double cena = najdrozsza->getCena();
    if (cena <= getBudzet() && najdrozsza->getLiczbaSztuk() > 0) {
        zmniejszBudzet(cena);
        najdrozsza->sprzedajSztuki(1);
        std::cout << "[Wybredny:" << getId()
                  << "] Kupił 1 szt. '" << nazwaTowaru
                  << "' za " << cena << "\n";
    } else {
        std::cout << "[Wybredny:" << getId()
                  << "] Nie stać mnie na najdroższe!\n";
    }
}

void KupujacyDetalista::kup(const std::string& nazwaTowaru, Gielda* g)
{
    std::vector<Oferta*> oferty = g->znajdzOferty(nazwaTowaru);
    if (oferty.empty()) {
        std::cout << "[Detalista:" << getId()
                  << "] Brak ofert na " << nazwaTowaru << "\n";
        return;
    }
    // Sortujemy rosnąco po cenie (bez <algorithm>): proste bąbelkowanie
    for (std::size_t i = 0; i < oferty.size(); i++) {
        for (std::size_t j = 0; j + 1 < oferty.size() - i; j++) {
            if (oferty[j]->getCena() > oferty[j+1]->getCena()) {
                Oferta* temp = oferty[j];
                oferty[j] = oferty[j+1];
                oferty[j+1] = temp;
            }
        }
    }
    // Kupujemy po 1 sztuce od najtańszej do najdroższej, dopóki starcza budżet
    bool kupilCos = false;
    for (std::size_t i = 0; i < oferty.size(); i++) {
        Oferta* of = oferty[i];
        double cena = of->getCena();
        if (cena <= getBudzet() && of->getLiczbaSztuk() > 0) {
            zmniejszBudzet(cena);
            of->sprzedajSztuki(1);
            std::cout << "[Detalista:" << getId()
                      << "] Kupił 1 szt. '" << nazwaTowaru
                      << "' za " << cena << "\n";
            kupilCos = true;
        }
    }
    if (!kupilCos) {
        std::cout << "[Detalista:" << getId()
                  << "] Nic nie kupiłem (za drogo).\n";
    }
}

// -----------------------------------------------------------
//   Funkcja main - przykładowe użycie
// -----------------------------------------------------------
int main()
{
    // 1) Tworzymy giełdę
    Gielda* g = new Gielda();

    // 2) Dodajemy sprzedających
    Sprzedajacy* s1 = new Sprzedajacy("Sprzedawca_1");
    Sprzedajacy* s2 = new Sprzedajacy("Sprzedawca_2");
    g->zarejestrujSprzedajacego(s1);
    g->zarejestrujSprzedajacego(s2);

    // 3) Sprzedający wystawiają oferty
    Oferta* o1 = s1->wystawOferte("Krysztaly", 10.0, 5);
    Oferta* o2 = s1->wystawOferte("Krysztaly", 12.5, 2);
    Oferta* o3 = s2->wystawOferte("Krysztaly", 9.0, 10);

    // Dodajemy je do giełdy
    g->dodajOferte(o1);
    g->dodajOferte(o2);
    g->dodajOferte(o3);

    // 4) Dodajemy kupujących o różnych strategiach
    Kupujacy* kEko = new KupujacyEkonomiczny("Klient_Eko", 50.0);
    Kupujacy* kWyb = new KupujacyWybredny("Klient_Wyb", 40.0);
    Kupujacy* kDet = new KupujacyDetalista("Klient_Det", 25.0);

    g->zarejestrujKupujacego(kEko);
    g->zarejestrujKupujacego(kWyb);
    g->zarejestrujKupujacego(kDet);

    // 5) Kupujący kupują towar "Krysztaly"
    std::cout << "\n=== ZAKUPY ===\n";
    g->kupTowar(kEko, "Krysztaly");
    g->kupTowar(kWyb, "Krysztaly");
    g->kupTowar(kDet, "Krysztaly");

    // 6) Wypisujemy stan giełdy po zakupach
    g->wypiszStan();

    // 7) Usuwamy Giełdę (zwolni sprzedających, kupujących, oferty)
    delete g;

    return 0;
}
