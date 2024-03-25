## Tema 4 PCOM

### Cerinta:
Sa se implementeze un client care interactioneaza cu un REST API prin protocolul HTTP, integrand concepte precum JSON si JWT

### Rezolvare
* Pentru rezolvarea temei s-a folosit pe post de schelet laboratoul 9 iar pentru manipularea datelor de tip json s-a folosit bibleoteca parson
* Implementarea are loc in cea mai mare parte in fisierul client.c
* Programul poate sa primeasca urmatoarele input-uri de la tastatura:
    * #### login
    Primeste de la tastatura un username si o parola. In cazul in care se gasesc spatii se returneaza un mesaj de eroare. De asemenea se mai returneaza un mesaj de eroare daca username-ul este deja folosit
    * #### register
    Primeste de la tastatura un username si o parola. In cazul in care se gasesc spatii se returneaza un mesaj de eroare. In cazul in care logarea are succes clientul salveaza un cookie de autentificare care se actualizeaza si in cazul in care se foloseste din nou comanda login fara a se deloga inainte iar token-ul se reseteaza. Daca username-ul nu se gaseste se returneaza un mesaj de eroare.
    * #### enter_library
    Functia functioneaza daca s-a efectuat logarea mai intai. Aceasta salveaza un token pe baza caruia utilizatorul poate accesa bibleoteca de pe server.
    * #### get_books
    Functia functioneaza daca utilizatorul este logat si daca are acces la bibleoteca. Aceasta functie acceseaza toate cartile din bibleoteca utilizatorului.
    * #### get_book
    Functia functioneaza daca utilizatorul este logat si daca are acces la bibleoteca. Aceasta functie afiseaza datele despre o carte cu id-ul introdus de utilizator. In cazul in care cartea nu se gaseste sau id-ul este introdus gresit se afiseaza un mesaj de eroare
    * #### add_book
    Functia functioneaza daca utilizatorul este logat si daca are acces la bibleoteca. Aceasta functie adauga o noua carte in bibleoteca utilizatorului. Acesta introduce la tastatura numele, autorul, gennul literar, numarul de pagini, si cine a publicat cartea. In cazul in care datele sunt introduse gresit adaugarea esueaza.
    * #### delete_book
    Functia functioneaza daca utilizatorul este logat si daca are acces la bibleoteca. Aceasta functie sterge din bibleoteca o carte cu id-ul introdus de la tastatura. In cazul in care nu se gaseste se afiseaza un mesaj de eroare
    * #### logout
    Functia functioneaza daca utilizatorul este logat si are rolul de a deloga utilizatorul.
    * #### exit
    Functia opreste aplicatia
* Pentru a nu avea probleme cu conexiunea dintre server si client la fiecare comanda aceasta se porneste si se opreste la sfarsitul efectuarii acesteia