#include <iostream>
#include <cmath>


class Wyrazenie {
public:
    virtual void wypisz() =0;
    virtual Wyrazenie* pochodna() =0;
    virtual double oblicz_wartosc(double x) =0;
    double calka_numeryczna(double lewy, double prawy,int dokl){
        double wynik = 0;
        double przedzial = (prawy - lewy) / dokl;
        for (int i = 0; i < dokl; ++i){
            double x = lewy + i/dokl * prawy;
            double y = lewy + (i+1)/dokl * prawy;
            wynik += (x + y)/2 * przedzial;
        }
        return wynik;
    };
    virtual Wyrazenie* kopiuj() = 0;
    virtual ~Wyrazenie() {};
};


class Stala : public Wyrazenie {
    double value;
public:
    Stala(double _value=0): value(_value) {};
    ~Stala() = default;
    void wypisz() override {std::cout <<value;}
    Wyrazenie* kopiuj() override {return new Stala(value);}
    Wyrazenie* pochodna() override { return new Stala; }
    double oblicz_wartosc(double x) override { return value; }
};


class Zmienna : public Wyrazenie {
public :
    Zmienna() = default;
    ~Zmienna() = default;
    Wyrazenie* pochodna() override {return new Stala(1);}
    double oblicz_wartosc(double x) override {return x;}
    void wypisz() override {std::cout << "x";}
    Wyrazenie* kopiuj() override {return new Zmienna();}
};


class Operator : public Wyrazenie {
protected:
    Wyrazenie* lewy;
    Wyrazenie* prawy;
public:
    Operator(Wyrazenie* _lewy, Wyrazenie* _prawy) : lewy(_lewy), prawy(_prawy) {};
    ~Operator() { delete lewy; delete prawy; }
    //void wypisz() override {} //decyzja projektowa
};

class Razy : public Operator {
public:
    Razy(Wyrazenie* _lewy, Wyrazenie* _prawy) : Operator(_lewy, _prawy) {}
    ~Razy() = default;
    void wypisz() override {
        lewy->wypisz();
        std::cout << " * ";
        prawy->wypisz();
    };
    Wyrazenie* pochodna() override;
    double oblicz_wartosc(double x) override {return lewy->oblicz_wartosc(x) * prawy->oblicz_wartosc(x);};
    Wyrazenie* kopiuj() override {return new Razy(lewy->kopiuj(), prawy->kopiuj());};
};


class Suma : public Operator {
public:
    Suma(Wyrazenie* _lewy, Wyrazenie* _prawy) : Operator(_lewy, _prawy) {}
    ~Suma() = default;
    void wypisz() override {
        lewy->wypisz();
        std::cout << " + ";
        prawy->wypisz();
    };
    Wyrazenie* pochodna() override { return new Suma(lewy->pochodna(), prawy->pochodna()); };
    double oblicz_wartosc(double x) override {
        return lewy->oblicz_wartosc(x) + prawy->oblicz_wartosc(x);
    };
    Wyrazenie* kopiuj() override {return new Suma(lewy->kopiuj(), prawy->kopiuj());};
};

Wyrazenie* Razy::pochodna() {return new Suma(new Razy(lewy->kopiuj(), prawy->pochodna()), new Razy(lewy->pochodna(), prawy->kopiuj()));};


class Funkcja : public Wyrazenie {
public:
    Funkcja() = delete;
    Funkcja(Wyrazenie* _arg) : arg(_arg) {};
    ~Funkcja() {delete arg;};
protected:
    Wyrazenie* arg;
};


class Cos: public Funkcja{
public:
    Cos(Wyrazenie* _arg): Funkcja(_arg) {};
    ~Cos() = default;
    double oblicz_wartosc(double x) override {return cos(arg->oblicz_wartosc(x));}
    Wyrazenie* pochodna() override;
    Wyrazenie* kopiuj() override {return new Cos(arg->kopiuj());};
    void wypisz() override {
        std::cout << "cos(";
        arg->wypisz();
        std::cout << ")";
    }
};


class Sin: public Funkcja {
public:
    Sin(Wyrazenie* _arg): Funkcja(_arg) {};
    ~Sin() = default;
    double oblicz_wartosc(double x) override {return sin(arg->oblicz_wartosc(x));}
    Wyrazenie* pochodna() override {return new Razy(new Cos(arg->kopiuj()), arg->pochodna());};
    Wyrazenie* kopiuj() override {return new Sin(arg->kopiuj());};
    void wypisz() override {
        std::cout << "sin(";
        arg->wypisz();
        std::cout << ")";
    }
};

Wyrazenie* Cos::pochodna() {return new Razy(new Stala(-1), new Razy(new Sin(arg->kopiuj()), arg->pochodna()));};


int main() {
    Wyrazenie* w1 = new Sin(new Zmienna());
    w1->wypisz();
    Wyrazenie* w2 = w1->pochodna();
    std::cout <<"\n";
    w2->wypisz();
}