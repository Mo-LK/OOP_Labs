/*
* Programme qui gère une liste de films et de livres contenant des informations sur ceux-ci, mais
* aussi sur les acteurs qui jouent dans les films.
* \file   tp4.cpp
* \author Erreur-404 et Mo-LK
* \date   18 mars 2022
* Créé le 8 mars 2022
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include <limits>
#include <algorithm>

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"
#include "structures.hpp"      // Structures de données pour la collection de films en mémoire

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

int main()
{
	bibliotheque_cours::activerCouleursAnsi();

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	// La ligne suivante lit le fichier binaire en allouant la mémoire nécessaire. Affiche les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans notre fonction lireActeur).
	ListeFilms listeFilms("films.bin");

	vector<unique_ptr<Item>> biblio;
	for (Film*& ptrFilm : span(listeFilms.obtenirElements(), listeFilms.obtenirNElements())) {
		biblio.push_back(make_unique<Film>(*ptrFilm));
	}
	lireLivre("livres.txt"s, biblio);


	Film* theHobbitMovie = dynamic_cast<Film*>(biblio[4].get());
	Livre* theHobbitBook = dynamic_cast<Livre*>(biblio[9].get());
	FilmLivre theHobbit(*theHobbitMovie, *theHobbitBook);
	biblio.push_back(make_unique<FilmLivre>(theHobbit));

	afficherListeItems(biblio);

	return 0;
}