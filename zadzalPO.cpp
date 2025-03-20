#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

class Wyrazenie
{
public:
    virtual ~Wyrazenie() {}

    virtual string obliczBezWartosciowania() const = 0;

    virtual string obliczZWartosciowaniem(const vector<pair<char, string>> &wartosciowanie) const = 0;

    virtual void wypisz(ostream &os) const = 0;

    friend ostream &operator<<(ostream &os, const Wyrazenie &w);
};

ostream &operator<<(ostream &os, const Wyrazenie &w){
    w.wypisz(os);
    return os;
}

//funkcje pomocnicze
string znajdzWartoscZmienna(char nazwaZmiennej,
                            const vector<pair<char, string>> &wartosciowanie){
    for (auto &para : wartosciowanie){
        if (para.first == nazwaZmiennej){
            return para.second;
        }
    }
    throw runtime_error(
        string("Zmienna :'") + nazwaZmiennej + "' niezdefiniowana w wartosciowaniu.");
}
string naDuzeLitery(const string &s){
    string wynik;
    for (char c : s){
        if (c >= 'a' && c <= 'z'){
            wynik.push_back(c - ('a' - 'A'));
        }
        else{
            wynik.push_back(c);
        }
    }
    return wynik;
}
string naMaleLitery(const string &s){
    string wynik;
    for (char c : s){
        if (c >= 'A' && c <= 'Z'){
            wynik.push_back(c + ('a' - 'A'));
        }
        else{
            wynik.push_back(c);
        }
    }
    return wynik;
}





class StaleWyrazenie : public Wyrazenie{
public:
    StaleWyrazenie(const string &val) : wartosc(val) {}

    StaleWyrazenie(const StaleWyrazenie &) = delete;
    StaleWyrazenie &operator=(const StaleWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        return wartosc;
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &) const override{
        return wartosc;
    }

    void wypisz(ostream &os) const override{
        os << "\"" << wartosc << "\"";
    }

private:
    string wartosc;
};

class ZmiennaWyrazenie : public Wyrazenie
{
public:
    ZmiennaWyrazenie(char c) : nazwaZmiennej(c) {}

    ZmiennaWyrazenie(const ZmiennaWyrazenie &) = delete;
    ZmiennaWyrazenie &operator=(const ZmiennaWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        //cout << " brak wyniku ";
        throw runtime_error(
            string("Zmienna '") + nazwaZmiennej + "' nie ma wartosci bez wartosciowania."
        );
    }

    string obliczZWartosciowaniem(const vector<pair<char, string>> &wartosciowanie) const override{
        return znajdzWartoscZmienna(nazwaZmiennej, wartosciowanie);
    }

    void wypisz(ostream &os) const override{
        os << nazwaZmiennej;
    }

private:
    char nazwaZmiennej;
};

class DoDuzychLiterWyrazenie : public Wyrazenie
{
public:
    DoDuzychLiterWyrazenie(Wyrazenie *e) : podrzedne(e) {}
    ~DoDuzychLiterWyrazenie() { delete podrzedne; }

    DoDuzychLiterWyrazenie(const DoDuzychLiterWyrazenie &) = delete;
    DoDuzychLiterWyrazenie &operator=(const DoDuzychLiterWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        return naDuzeLitery(podrzedne->obliczBezWartosciowania());
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        return naDuzeLitery(podrzedne->obliczZWartosciowaniem(w));
    }

    void wypisz(ostream &os) const override{
        os << "^(" << *podrzedne << ")";
    }

private:
    Wyrazenie *podrzedne;
};

class DoMalychLiterWyrazenie : public Wyrazenie{
public:
    DoMalychLiterWyrazenie(Wyrazenie *e) : podrzedne(e) {}
    ~DoMalychLiterWyrazenie() { delete podrzedne; }

    DoMalychLiterWyrazenie(const DoMalychLiterWyrazenie &) = delete;
    DoMalychLiterWyrazenie &operator=(const DoMalychLiterWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        return naMaleLitery(podrzedne->obliczBezWartosciowania());
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        return naMaleLitery(podrzedne->obliczZWartosciowaniem(w));
    }

    void wypisz(ostream &os) const override{
        os << "_(" << *podrzedne << ")";
    }

private:
    Wyrazenie *podrzedne;
};

class DlugoscWyrazenie : public Wyrazenie{
public:
    DlugoscWyrazenie(Wyrazenie *e) : podrzedne(e) {}
    ~DlugoscWyrazenie() { delete podrzedne; }

    DlugoscWyrazenie(const DlugoscWyrazenie &) = delete;
    DlugoscWyrazenie &operator=(const DlugoscWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        string val = podrzedne->obliczBezWartosciowania();
        return to_string(val.size());
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        string val = podrzedne->obliczZWartosciowaniem(w);
        return to_string(val.size());
    }

    void wypisz(ostream &os) const override{
        os << "#(" << *podrzedne << ")";
    }

private:
    Wyrazenie *podrzedne;
};

class DwuargumentoweWyrazenie : public Wyrazenie{
protected:
    Wyrazenie *lewe;
    Wyrazenie *prawe;

public:
    DwuargumentoweWyrazenie(Wyrazenie *l, Wyrazenie *r) : lewe(l), prawe(r) {}
    virtual ~DwuargumentoweWyrazenie(){
        delete lewe;
        delete prawe;
    }

    DwuargumentoweWyrazenie(const DwuargumentoweWyrazenie &) = delete;
    DwuargumentoweWyrazenie &operator=(const DwuargumentoweWyrazenie &) = delete;
};

class PolaczoneWyrazenie : public DwuargumentoweWyrazenie{
public:
    PolaczoneWyrazenie(Wyrazenie *l, Wyrazenie *r)
        : DwuargumentoweWyrazenie(l, r) {}

    PolaczoneWyrazenie(const PolaczoneWyrazenie &) = delete;
    PolaczoneWyrazenie &operator=(const PolaczoneWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        return lewe->obliczBezWartosciowania() + prawe->obliczBezWartosciowania();
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        return lewe->obliczZWartosciowaniem(w) + prawe->obliczZWartosciowaniem(w);
    }

    void wypisz(ostream &os) const override{
        os << "(" << *lewe << " & " << *prawe << ")";
    }
};

class MaskowanieWyrazenie : public DwuargumentoweWyrazenie{
public:
    MaskowanieWyrazenie(Wyrazenie *l, Wyrazenie *r)
        : DwuargumentoweWyrazenie(l, r) {}

    MaskowanieWyrazenie(const MaskowanieWyrazenie &) = delete;
    MaskowanieWyrazenie &operator=(const MaskowanieWyrazenie &) = delete;

    string obliczBezWartosciowania() const override{
        return maskuj(lewe->obliczBezWartosciowania(), prawe->obliczBezWartosciowania());
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        return maskuj(lewe->obliczZWartosciowaniem(w), prawe->obliczZWartosciowaniem(w));
    }

    static string maskuj(const string &s1, const string &s2){
        if (s2.empty()){
            return "";
        }
        string powtorzony;
        while (powtorzony.size() < s1.size()){
            powtorzony += s2;
        }
        powtorzony.resize(s1.size());

        string wynik;
        for (size_t i = 0; i < s1.size(); i++){
            if (powtorzony[i] == '*'){
                wynik.push_back(s1[i]);
            }
        }
        return wynik;
    }

    void wypisz(ostream &os) const override{
        os << "(" << *lewe << " * " << *prawe << ")";
    }
};

class PrzeplotWyrazenie : public DwuargumentoweWyrazenie{
public:
    PrzeplotWyrazenie(Wyrazenie *l, Wyrazenie *r)
        : DwuargumentoweWyrazenie(l, r) {}

    PrzeplotWyrazenie(const PrzeplotWyrazenie &) = delete;
    PrzeplotWyrazenie &operator=(const PrzeplotWyrazenie &) = delete;

    static string przeplot(const string &s1, const string &s2){
        string wynik;
        size_t maxLen = (s1.size() > s2.size() ? s1.size() : s2.size());
        for (size_t i = 0; i < maxLen; i++){
            if (i < s1.size()){
                wynik.push_back(s1[i]);
            }
            if (i < s2.size()){
                wynik.push_back(s2[i]);
            }    
        }
        return wynik;
    }

    string obliczBezWartosciowania() const override{
        return przeplot(lewe->obliczBezWartosciowania(), prawe->obliczBezWartosciowania());
    }
    string obliczZWartosciowaniem(const vector<pair<char, string>> &w) const override{
        return przeplot(lewe->obliczZWartosciowaniem(w), prawe->obliczZWartosciowaniem(w));
    }

    void wypisz(ostream &os) const override{
        os << "(" << *lewe << " @ " << *prawe << ")";
    }
};

//funkcja pomocnicza do liczenia z wartościowaniem i bez oraz do łapania bledow
string oblicz(const Wyrazenie* w, bool zWartosciowaniem, const vector<pair<char, string>>& wartosciowanie = {}){
    try {
        if (zWartosciowaniem) {
            return w->obliczZWartosciowaniem(wartosciowanie);
        } else {
            return w->obliczBezWartosciowania();
        }
    }
    catch (const runtime_error& ex) {
        return string("brak wyniku --> (") + ex.what() + ")";
    }
}



int main() {
    cout << "Start programu\n\n";

    //wartosciowanie
    vector<pair<char, string>> wartosciowanie1 {
        {'b', "C++"},
        {'x', "Python"}
    };
    
    //wyr1: b @ x
    Wyrazenie* wyr1 = new PrzeplotWyrazenie(
        new ZmiennaWyrazenie('b'),
        new ZmiennaWyrazenie('x')
    );

    cout << "Wyrazenie 1: " << *wyr1 << "\n";
    cout << "Wynik z wartosciowaniem: "
         << oblicz(wyr1, true, wartosciowanie1) << "\n";
    cout << "Wynik bez wartosciowania: "
         << oblicz(wyr1, false) << "\n\n";

    //wy2: ^("c++") & _("PyThOn")
    Wyrazenie* wyr2 = new PolaczoneWyrazenie(
        new DoDuzychLiterWyrazenie( new StaleWyrazenie("c++") ),
        new DoMalychLiterWyrazenie( new StaleWyrazenie("PyThOn") )
    );

    cout << "Wyrazenie 2: " << *wyr2 << "\n";
    cout << "Wynik z wartosciowaniem: "
         << oblicz(wyr2, true, wartosciowanie1) << "\n";
    cout << "Wynik bez wartosciowania: "
         << oblicz(wyr2, false) << "\n\n";

    // wyr3: (b * "*") 
    Wyrazenie* wyr3 = new MaskowanieWyrazenie(
        new ZmiennaWyrazenie('b'),
        new StaleWyrazenie("a*")
    );

    cout << "Wyrazenie 3: " << *wyr3 << "\n";
    cout << "Wynik z wartosciowaniem: "
         << oblicz(wyr3, true, wartosciowanie1) << "\n";
    cout << "Wynik bez wartosciowania: "
         << oblicz(wyr3, false) << "\n\n";

    //wyr4: #(x)
    Wyrazenie* wyr4 = new DlugoscWyrazenie(new ZmiennaWyrazenie('x'));
    cout << "Wyrazenie 4: " << *wyr4 << "\n";
    cout << "Wynik z wartosciowaniem: "
         << oblicz(wyr4, true, wartosciowanie1) << "\n";
    cout << "Wynik bez wartosciowania: "
         << oblicz(wyr4, false) << "\n\n";


    delete wyr1;
    delete wyr2;
    delete wyr3;
    delete wyr4;

    cout << "Koniec programu\n";
    return 0;
}
