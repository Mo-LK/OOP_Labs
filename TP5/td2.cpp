/*
* Programme qui gère une liste de films et de livres contenant des informations sur ceux-ci, mais
* aussi sur les acteurs qui jouent dans les films.
* \file   tp5.cpp
* \author Erreur-404 et Mo-LK
* \date   27 mars 2022
* Créé le 22 mars 2022
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include <limits>

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"
#include "structures.hpp"      // Structures de données pour la collection de films en mémoire

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

const int INDEX_THE_HOBBIT_MOVIE = 4;
const int INDEX_THE_HOBBIT_BOOK = 9;
const int INDEX_ALIEN = 0;


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

	Film* theHobbitMovie = dynamic_cast<Film*>(biblio[INDEX_THE_HOBBIT_MOVIE].get());
	Livre* theHobbitBook = dynamic_cast<Livre*>(biblio[INDEX_THE_HOBBIT_BOOK].get());
	FilmLivre theHobbit(*theHobbitMovie, *theHobbitBook);
	biblio.push_back(make_unique<FilmLivre>(theHobbit));

	cout << "\n\n\n\033[032m===========================SECTION 0============================\033[0m\n";

	afficherListeItems<vector<unique_ptr<Item>>>(biblio, "Question 0");

	cout << "\n\n\n\033[032m===========================SECTION 1============================\033[0m\n";

	forward_list<Item*> forwardList1;
	forward_list<Item*>::iterator it = forwardList1.before_begin();
	// Nous utilisons un unsigned int ici pour corriger un warning. Celui-ci était 
	// présent puisque biblio.size() retourne un élément de type size_t
	for (unsigned int i = 0; i < biblio.size(); i++) {
		it = forwardList1.insert_after(it, biblio[i].get());
	}
	afficherListeItems<forward_list<Item*>>(forwardList1, "Question 1.1");


	forward_list<Item*> forwardList2;
	for (it = forwardList1.begin(); it != forwardList1.end(); it++) {
		forwardList2.push_front(*it);
	}
	afficherListeItems<forward_list<Item*>>(forwardList2, "Question 1.2");


	forward_list<Item*> forwardList3;
	forward_list<Item*>::iterator position = forwardList3.before_begin();
	for (it = forwardList1.begin(); it != forwardList1.end(); it++) {
		position = forwardList3.insert_after(position, *it);
	}
	afficherListeItems<forward_list<Item*>>(forwardList3, "Question 1.3");


	vector<Item*> vector4;
	int size = 0;
	for (it = forwardList1.begin(); it != forwardList1.end(); it++) {	// O(n)
		size++;															// O(1)
	}
	vector4.reserve(size);												// O(n)
	forwardList1.reverse();												// O(n)
	for (it = forwardList1.begin(); it != forwardList1.end(); it++) {	// O(n)
		vector4.push_back(*it);											// O(1)
	}
	// L'algorithme est O(n) * O(1) + O(n) + O(n) + O(n) * O(1) = O(4n) = O(n)
	afficherListeItems<vector<Item*>>(vector4, "Question 1.4");
	forwardList1.reverse(); // Ramène la liste dans l'état original pour utilisation plus tard


	cout << "\n\n\nQuestion 1.5\n\n\nActeurs du film Alien:" << endl;
	Film* filmPtr = dynamic_cast<Film*>(biblio[INDEX_ALIEN].get());
	Film film = *filmPtr;
	for (auto&& acteur : film.obtenirActeurs()) {
		cout << "\n\t" << acteur->nom << endl;
	}

	cout << "\n\n\n\033[032m===========================SECTION 2============================\033[0m\n";

	set<Item*, ComparaisonSet<Item>> setBiblio;
	for (auto&& item : biblio) {
		setBiblio.insert(item.get());
	}
	afficherListeItems<set<Item*, ComparaisonSet<Item>>>(setBiblio, "Question 2.1");


	cout << "\n\n\nQuestion 2.2\n\n\n";
	unordered_map<string, Item*> unorderedMapBiblio;
	for (auto&& item : biblio) {
		unorderedMapBiblio[item->obtenirTitre()] = item.get();
	}
	cout << *unorderedMapBiblio["The Hobbit"] << endl;

	cout << "\n\n\n\033[032m===========================SECTION 3============================\033[0m\n";


	vector<Item*> films;
	copy_if(forwardList1.begin(), forwardList1.end(), back_inserter(films), [](Item* item) {return dynamic_cast<Film*>(item); });
	afficherListeItems<vector<Item*>>(films, "Question 3.1");


	auto incrementerRecetteTotale = [](int init, Item* item) { 
										Film* film = dynamic_cast<Film*>(item); 
										return init + film->obtenirRecette();
									};

	int totalRecettes = accumulate(films.begin(), films.end(), 0, incrementerRecetteTotale);
	cout << "\n\n\nQuestion 3.2\n\nTotal des recettes: " << totalRecettes << " M$\n";

	return 0;
}