#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <iomanip>
#include <vector>

#include <cppitertools/range.hpp>
#include "gsl/span"

using namespace std;

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}


class Affichable
{
public:
	virtual ~Affichable() = default;

	friend ostream& operator<<(ostream& stream, const Affichable& affichable);
	virtual string afficher() const = 0;
};

class Item : public Affichable
{
public:
	Item(string titre = "", int anneeSortie = 0) : titre_(titre), anneeSortie_(anneeSortie) {}
	virtual ~Item() = default;


	string afficher() const override;

protected:
	string titre_;
	int anneeSortie_;
};


class Livre : virtual public Item
{
public:
	Livre(ifstream& fichier);
	Livre(const Livre& livre) :
		Item(livre.titre_, livre.anneeSortie_),
		auteur_(livre.auteur_), nbCopies_(livre.nbCopies_),
		nbPages_(livre.nbPages_)
	{}
	virtual ~Livre() = default;

	string afficher() const override;

protected:
	string auteur_;
	int nbCopies_;
	int nbPages_;
};


class Film; struct Acteur;


class ListeFilms {
public:
	ListeFilms() : capacite_(0), nElements_(0), elements_(nullptr) {}
	ListeFilms(string nomFichier);
	~ListeFilms();

	inline int obtenirNElements() const		{ return nElements_; };
	inline Film** obtenirElements() const	{ return elements_; };

	inline void allouerElements(int taille) { elements_ = new Film* [taille]; };
	void ajouterFilm(Film* ptrFilm);
	void enleverFilm(Film* ptrFilm);
	void afficher() const;

	Film* chercherFilm(const function<bool(Film*)>& critere) const;
	shared_ptr<Acteur> trouverActeur(const string& nom) const;
	inline Film& operator[](int index) const { return *elements_[index]; };

private:
	int capacite_, nElements_;
	Film** elements_;
};


template <typename T>
class Liste {
public:
	Liste() : capacite_(0), nElements_(0) {}
	Liste(istream& fichier);
	Liste(const Liste& liste);

	void ajouterElement(const T& elem);
	inline void modifierCapacite(int nouvelle) { capacite_ = nouvelle; };
	inline void modifierNElements(int nouveau) { nElements_ = nouveau; };
	inline int obtenirNElements() const        { return nElements_; };

	Liste<T>& operator=(const Liste<T>& liste);
	inline shared_ptr<T>& operator[](int index) { return elements[index]; };

	unique_ptr<shared_ptr<T>[]> elements;
private:
	int capacite_, nElements_;
};


using ListeActeurs = Liste<Acteur>;

shared_ptr<Acteur> lireActeur(istream& fichier, const ListeFilms& listeFilms);


class Film: virtual public Item
{
public:
	Film() = default;
	Film(istream& fichier, ListeFilms& listeFilms);
	Film(const Film& film) :
		Item(film.titre_, film.anneeSortie_),
		realisateur_(film.realisateur_), recette_(film.recette_),
		acteurs_(film.acteurs_) {}
	virtual ~Film() = default;

	inline const string& obtenirTitre() const			{ return titre_; };
	inline const int& obtenirAnneeSortie() const		{ return anneeSortie_; };
	inline const Liste<Acteur>& obtenirActeurs() const	{ return acteurs_; };
	string obtenirNomActeurs() const;
	
	friend ostream& operator<<(ostream& stream, const Film& film);
	string afficher() const override;

protected:
	string realisateur_ = "";
	int recette_ = 0;
	ListeActeurs acteurs_ = {};
};


struct Acteur
{
	string nom = "";
	int anneeNaissance = 0;
	char sexe = '\0';
};

class FilmLivre : public Film, public Livre
{
public:
	FilmLivre(const Film& film, const Livre& livre) :
		Item(film.obtenirTitre(), film.obtenirAnneeSortie()), 
		Film(film), Livre(livre)
	{}
	virtual ~FilmLivre() = default;
	string afficher() const override;
};

//---------------------------------------------------------------------------\\
//                                                                           \\
//            Définition des méthodes de la classe Affichable                \\
//                                                                           \\
//---------------------------------------------------------------------------\\

ostream& operator<<(ostream& stream, const Affichable& affichable)
{
	return stream << affichable.afficher() << endl;
}

//---------------------------------------------------------------------------\\
//                                                                           \\
//              Définition des méthodes de la classe Item                    \\
//                                                                           \\
//---------------------------------------------------------------------------\\

string Item::afficher() const
{
	static const string ligneDeSeparation = "\n\033[33m========================================\033[0m\n";
	return (ligneDeSeparation + 
			"\nTitre: " + titre_ + 
			"\nAnnee Sortie: " + to_string(anneeSortie_));
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//              Définition des méthodes de la classe Livre                   \\
//                                                                           \\
//---------------------------------------------------------------------------\\


Livre::Livre(ifstream& fichier)
{
	fichier >> quoted(titre_) >> anneeSortie_
            >> quoted(auteur_) >> nbCopies_
            >> nbPages_;
}

string Livre::afficher() const
{
	return (Item::afficher() + 
			"\nAuteur: " + auteur_ +
			"\nMillions de copies vendues: " + to_string(nbCopies_) + " M" +
			"\nNombre de pages: " + to_string(nbPages_));
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//              Définition des méthodes de la classe ListeFilms              \\
//                                                                           \\
//---------------------------------------------------------------------------\\


ListeFilms::ListeFilms(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	capacite_ = 0;
	nElements_ = 0;
	elements_ = nullptr;

	int nElements = lireUint16(fichier);
	for ([[maybe_unused]] int i : iter::range(nElements)) {
		Film* ptrFilm = new Film(fichier, *this);
		ajouterFilm(ptrFilm);
	}
}

ListeFilms::~ListeFilms()
{
	for (Film*& ptrFilm : gsl::span(elements_, nElements_)) {
		delete ptrFilm;
	}
	delete[] elements_;
}

void ListeFilms::ajouterFilm(Film* ptrFilm)
{
	if (nElements_ == capacite_) {
		capacite_ = max(1, capacite_ * 2);
		Film** tempElements = new Film * [capacite_];
		for (int i : iter::range(nElements_)) {
			tempElements[i] = elements_[i];
		}
		delete[] elements_;
		elements_ = tempElements;
	}
	elements_[nElements_++] = ptrFilm;
}

void ListeFilms::enleverFilm(Film* ptrFilm)
{
	for (Film*& film : gsl::span(elements_, nElements_)) {
		if (film == ptrFilm) {
			film = elements_[--nElements_];
			elements_[nElements_] = nullptr;
		}
	}
}

void ListeFilms::afficher() const
{
	static const string ligneDeSeparation = "\n\033[33m========================================\033[0m\n";
	cout << ligneDeSeparation;
	for (const Film* film : gsl::span(elements_, nElements_)) {
		cout << *film;
		cout << ligneDeSeparation;
	}
}

Film* ListeFilms::chercherFilm(const function<bool(Film*)>& critere) const
{
	for (Film* ptrFilm : gsl::span(elements_, nElements_)) {
		if (critere(ptrFilm))
			return ptrFilm;
	}
	return nullptr;
}

shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nom) const
{
	for (Film* ptrFilm : gsl::span(elements_, nElements_)) {

		// Ces lignes me permettent de diminuer la longueur de la ligne 319, 
		// qui s'étendait sur plus de 130 caractères
		ListeActeurs acteurs = ptrFilm->obtenirActeurs();
		shared_ptr<Acteur>* ptrActeurs = acteurs.elements.get();
		int nElements = acteurs.obtenirNElements();
		
		for (shared_ptr<Acteur> acteur : gsl::span(ptrActeurs, nElements)) {
			if (acteur->nom == nom) {
				return acteur;
			}
		}
	}
	return nullptr;
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//               Définition des méthodes de la classe Liste<T>               \\
//                                                                           \\
//---------------------------------------------------------------------------\\


template <typename T>
Liste<T>::Liste(istream& fichier)
{
	capacite_ = lireUint8(fichier);
	nElements_ = capacite_;
	elements = make_unique<shared_ptr<Acteur>[]>(capacite_);
}

template <typename T>
Liste<T>::Liste(const Liste& liste) :
	capacite_(liste.capacite_), nElements_(liste.nElements_)
{
	elements = make_unique<shared_ptr<T>[]>(liste.capacite_);
	for (int i : iter::range(liste.nElements_))
		elements[i] = liste.elements[i];
}

template <typename T>
void Liste<T>::ajouterElement(const T& elem)
{
	if (nElements_ == capacite_) {
		capacite_ = max(1, capacite_ * 2);
		unique_ptr<shared_ptr<T>[]> tempElements = make_unique<shared_ptr<T>[]>(capacite_);
		for (int i : iter::range(nElements_)) {
			tempElements[i] = elements[i];
		}
		elements = move(tempElements);
	}
	elements[nElements_++] = make_shared<T>(elem);
}

template <typename T>
Liste<T>& Liste<T>::operator=(const Liste<T>& liste)
{
	capacite_ = liste.capacite_;
	nElements_ = liste.nElements_;
	elements = make_unique<shared_ptr<T>[]>(capacite_);
	for (int i : iter::range(nElements_))
		elements[i] = move(liste.elements[i]);
	return *this;
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//               Définition des méthodes de la classe Film                   \\
//                                                                           \\
//---------------------------------------------------------------------------\\


Film::Film(istream& fichier, ListeFilms& listeFilms)
{
	titre_ = lireString(fichier);
	realisateur_ = lireString(fichier);
	anneeSortie_ = lireUint16(fichier);
	recette_ = lireUint16(fichier);
	acteurs_ = ListeActeurs(fichier);
	for (int i : iter::range(acteurs_.obtenirNElements())) {
		acteurs_.elements[i] = move(lireActeur(fichier, listeFilms));
	}
}

string Film::obtenirNomActeurs() const
{
	string nomActeurs = "Acteurs du film:\n\t";
	for (int i : iter::range(acteurs_.obtenirNElements())) {		
		nomActeurs += acteurs_.elements[i].get()->nom + "\n\t";
	}
	return nomActeurs;
}

ostream& operator<<(ostream& stream, const Film& film)
{
	stream << "Titre : " << film.titre_ << endl
		   << "Realisateur : " << film.realisateur_ << endl
	  	   << "Annee Sortie : " << film.anneeSortie_ << endl
		   << "Recette : " << film.recette_ << " M$" << endl;

	// Ces lignes me permettent de diminuer la ligne 420, 
	// qui s'étendait sur plus de 112 caractères
	ListeActeurs acteurs = film.obtenirActeurs();
	shared_ptr<Acteur>* ptrActeurs = acteurs.elements.get();
	int nElements = acteurs.obtenirNElements();

	for (shared_ptr<Acteur>& acteur : gsl::span(ptrActeurs, nElements)) {
		stream << "  " << acteur->nom 
			   << ", " << acteur->anneeNaissance 
			   << " "  << acteur->sexe << endl;
	}
	return stream;
}

string Film::afficher() const
{
	string nomActeurs = obtenirNomActeurs();
	return (Item::afficher() + 
			"\nRealisateur: " + realisateur_ +
			"\nRecette: " + to_string(recette_) +
			" M$\n" + nomActeurs);
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//            Définition des méthodes de la classe FilmLivre                 \\
//                                                                           \\
//---------------------------------------------------------------------------\\

string FilmLivre::afficher() const
{
	string delimiteurLivre = "\n\n\033[1m\033[4mInformations Relatives au Livre\033[0m";
	string delimiteurFilm = "\n\n\033[1m\033[4mInformations Relatives au Film\033[0m";

	string infosLivre = "\nAuteur: " + auteur_ +
						"\nMillions de copies vendues: " + to_string(nbCopies_) +
						"\nNombre de pages: " + to_string(nbPages_);
	string infosFilm =	"\nRealisateur: " + realisateur_ +
						"\nRecette: " + to_string(recette_) +
						" M$\nActeurs du film: " + obtenirNomActeurs() + "\n";
	
	return Item::afficher() + 
			delimiteurLivre + infosLivre +
			delimiteurFilm + infosFilm;
}


//---------------------------------------------------------------------------\\
//                                                                           \\
//                    Définition de fonctions autres                         \\
//                                                                           \\
//---------------------------------------------------------------------------\\


shared_ptr<Acteur> lireActeur(istream& fichier, const ListeFilms& listeFilms)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	shared_ptr<Acteur> ptrActeur = listeFilms.trouverActeur(acteur.nom);
	if (ptrActeur == nullptr) {
		cout << "Cree " << acteur.nom << endl;
		return make_shared<Acteur>(acteur);
	}
	return ptrActeur;
}

void lireLivre(string nomFichier, vector<unique_ptr<Item>>& bibliotheque)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	for ([[maybe_unused]] int i : iter::range(5)) { // Le nombre de ligne a été hardcodé par autorisation du chargé puisqu'il y a une ligne vide à la fin du fichier .txt
		Livre livre(fichier);
		bibliotheque.push_back(make_unique<Livre>(livre));
	}
}

void afficherListeItems(const vector<unique_ptr<Item>>& liste)
{
	for (auto&& item : liste) {
		cout << *item;
	}
}