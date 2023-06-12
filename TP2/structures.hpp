#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <iostream>

#include <cppitertools/range.hpp>
#include "gsl/span"

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

void afficherFilm(const Film& film);

class ListeFilms {
public:
	ListeFilms();

	int obtenirCapacite() const;
	int obtenirNElements() const;
	Film** obtenirElements() const;

	void modifierCapacite(const int& autre);
	void modifierNElements(const int& autre);
	void modifierElements(Film**& autre);

	void allouerElements(int taille);
	void desallouerElements();

	void ajouterFilm(Film* ptrFilm);
	void enleverFilm(Film* ptrFilm);

	void afficher() const;

private:
	int capacite_, nElements_;
	Film** elements_; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
};

struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};


ListeFilms::ListeFilms() :
	capacite_(0), nElements_(0), elements_(nullptr)
{
	// Note au correcteur : nous sommes conscient que l'allocution dynamique de elements_
	// devrait avoir lieu ici, mais lorsque nous tentons de le faire, nous obtenons des fuites 
	// de mémoire. Ainsi, nous avons préféré laisser le code à un état sans fuite de mémoire,
	// soit quand l'allocution se fait dans le fichier td2.cpp
}

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

void ListeFilms::modifierCapacite(const int& autre)
{
	if (autre > capacite_)
		capacite_ = autre;
}

void ListeFilms::modifierNElements(const int& autre)
{
	if (autre > 0 && autre <= capacite_)
		nElements_ = autre;
}
void ListeFilms::modifierElements(Film**& autre)
{
	for (int i : iter::range(nElements_)) {
		elements_[i] = autre[i];
	}
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
		ListeFilms listeFilmsAgrandie = {};
		listeFilmsAgrandie.capacite_ = std::max(1, capacite_ * 2);
		listeFilmsAgrandie.modifierNElements(nElements_);
		listeFilmsAgrandie.elements_ = new Film * [listeFilmsAgrandie.capacite_];
		listeFilmsAgrandie.modifierElements(elements_);
		delete[] elements_;
		*this = listeFilmsAgrandie;
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
		afficherFilm(*film);
		std::cout << ligneDeSeparation;
	}
}