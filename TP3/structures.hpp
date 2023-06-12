#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <iostream>
#include <memory>
#include <functional>

#include <cppitertools/range.hpp>
#include "gsl/span"

using namespace std;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() : capacite_(0), nElements_(0), elements_(nullptr) {}

	int obtenirCapacite() const;
	int obtenirNElements() const;
	Film** obtenirElements() const;

	void allouerElements(int taille);
	void desallouerElements();

	void ajouterFilm(Film* ptrFilm);
	void enleverFilm(Film* ptrFilm);

	void afficher() const;
	Film* chercherFilm(const function<bool(Film*)>& critere) const;

	Film& operator[](int index) const;

private:
	int capacite_, nElements_;
	Film** elements_; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
};

template <typename T>
class Liste {
public:
	Liste() : capacite(0), nElements(0) {}
	Liste(const Liste& liste) :
		capacite(liste.capacite), nElements(liste.nElements)
	{
		elements = make_unique<shared_ptr<T>[]>(liste.capacite);
		for (int i : iter::range(liste.nElements))
			elements[i] = liste.elements[i];
	}

	void ajouterElement(const T& elem);
	void modifierCapacite(int nouvelleCapacite);
	void modifierNElements(int nouveauNElements);
	int obtenirNElements() const;

	Liste<T>& operator=(const Liste<T>& liste);
	shared_ptr<T>& operator[](int index);

	unique_ptr<shared_ptr<T>[]> elements;
private:
	int capacite, nElements;
};

using ListeActeurs = Liste<Acteur>;

struct Film
{
	Film() = default;
	Film(const Film& film) :
		titre(film.titre), realisateur(film.realisateur),
		anneeSortie(film.anneeSortie), recette(film.recette),
		acteurs(film.acteurs)
	{ }

	std::string titre = "";
	std::string realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0;
	int recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs = {};

	friend ostream& operator<<(ostream& stream, const Film& film);
};

struct Acteur
{
	std::string nom = "";
	int anneeNaissance = 0;
	char sexe = '\0';
	//ListeFilms joueDans = {};
};

int ListeFilms::obtenirCapacite() const
{
	return capacite_;
}

int ListeFilms::obtenirNElements() const
{
	return nElements_;
}

Film** ListeFilms::obtenirElements() const
{
	return elements_;
}

void ListeFilms::allouerElements(int taille)
{
	elements_ = new Film * [taille];
}

void ListeFilms::desallouerElements()
{
	delete[] elements_;
}

void ListeFilms::ajouterFilm(Film* ptrFilm)
{
	if (nElements_ == capacite_) {
		capacite_ = std::max(1, capacite_ * 2);
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
	// Utilise des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const std::string ligneDeSeparation = "\n\033[33m========================================\033[0m\n";
	std::cout << ligneDeSeparation;
	for (const Film* film : gsl::span(elements_, nElements_)) {
		cout << *film;
		std::cout << ligneDeSeparation;
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

Film& ListeFilms::operator[](int index) const
{
	return *elements_[index];
}

template <typename T>
void Liste<T>::ajouterElement(const T& elem)
{
	if (nElements == capacite) {
		capacite = std::max(1, capacite * 2);
		unique_ptr<shared_ptr<T>[]> tempElements = make_unique<shared_ptr<T>[]>(capacite);
		for (int i : iter::range(nElements)) {
			tempElements[i] = elements[i];
		}
		elements = move(tempElements);
	}
	elements[nElements++] = make_shared<T>(elem);
}

template <typename T>
void Liste<T>::modifierCapacite(int nouvelleCapacite)
{
	capacite = nouvelleCapacite;
}

template <typename T>
void Liste<T>::modifierNElements(int nouveauNElements)
{
	nElements = nouveauNElements;
}

template <typename T>
int Liste<T>::obtenirNElements() const
{
	return nElements;
}

template <typename T>
Liste<T>& Liste<T>::operator=(const Liste<T>& liste)
{
	capacite = liste.capacite;
	nElements = liste.nElements;
	elements = make_unique<shared_ptr<T>[]>(capacite);
	for (int i : iter::range(nElements))
		elements[i] = move(liste.elements[i]);
	return *this;
}

template <typename T>
shared_ptr<T>& Liste<T>::operator[](int index)
{
	return elements[index];
}

ostream& operator<<(ostream& stream, const Film& film)
{
	stream << "Titre : " << film.titre << endl
		   << "Realisateur : " << film.realisateur << endl
	  	   << "Annee Sortie : " << film.anneeSortie << endl
		   << "Recette : " << film.recette << " M$" << endl;
	for (shared_ptr<Acteur>& acteur : gsl::span(film.acteurs.elements.get(), film.acteurs.obtenirNElements())) {
		stream << "  " << acteur->nom 
			   << ", " << acteur->anneeNaissance 
			   << " "  << acteur->sexe << endl;
	}
	return stream;
}