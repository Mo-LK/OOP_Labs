/*
* Programme qui gère une liste de films contenant des informations sur les films, mais
* aussi sur les acteurs qui jouent dans ces derniers.
* \file   tp2.cpp
* \author Erreur-404 et Mo-LK
* \date   13 février 2022
* Créé le 4 février 2022
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include <cppitertools/range.hpp>
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

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


//Une fonction pour trouver un Acteur par son nom dans une ListeFilms,
// qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  
// Devrait utiliser span.
Acteur* trouverActeur(const ListeFilms& listeFilms, const string& nom)
{
	Acteur* acteurCherche = nullptr;
	bool acteurEstTrouve = false;
	for (Film* ptrFilm : span(listeFilms.obtenirElements(), listeFilms.obtenirNElements())) {
		ListeActeurs acteurs = ptrFilm->acteurs;
		for (Acteur* acteur : span(acteurs.elements, acteurs.nElements)) {
			if (acteur->nom == nom) {
				acteurCherche = acteur;
				acteurEstTrouve = true;
				break;
			}
		}
		if (acteurEstTrouve)
			break;
	}
	return acteurCherche;
}

//Fonctions pour lire le fichier et créer/allouer une ListeFilms. 
// La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence 
// d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la 
// fonction ci-dessus).
Acteur* lireActeur(istream& fichier, const ListeFilms& listeFilms)
{
	Acteur acteur = {};
	acteur.nom			  = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe			  = lireUint8(fichier);
	Acteur* ptrActeur = trouverActeur(listeFilms, acteur.nom);
	if (ptrActeur == nullptr) {
		ptrActeur = new Acteur(acteur);
		ptrActeur->joueDans.allouerElements(ptrActeur->joueDans.obtenirNElements());
		cout << "Cree " << acteur.nom << endl;
	}
	return ptrActeur; //Retourne un pointeur soit vers un acteur existant ou un 
}			   // nouvel acteur ayant les bonnes informations, selon si l'acteur 
			   // existait déjà.  Pour fins de débogage, affiche les noms des 
			   // acteurs crées; on ne devrait pas voir le même nom d'acteur 
			   // affiché deux fois pour la création.


Film* lireFilm(istream& fichier, ListeFilms& listeFilms)
{
	Film* ptrFilm = new Film;
	ptrFilm->titre			   = lireString(fichier);
	ptrFilm->realisateur	   = lireString(fichier);
	ptrFilm->anneeSortie	   = lireUint16(fichier);
	ptrFilm->recette		   = lireUint16(fichier);
	ptrFilm->acteurs.nElements = lireUint8(fichier); //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	ptrFilm->acteurs.capacite = ptrFilm->acteurs.nElements;
	ptrFilm->acteurs.elements = new Acteur * [ptrFilm->acteurs.nElements];
	for (int i : range(ptrFilm->acteurs.nElements)) {
		Acteur* ptrActeur = lireActeur(fichier, listeFilms);
		ptrActeur->joueDans.ajouterFilm(ptrFilm);
		ptrFilm->acteurs.elements[i] = ptrActeur;
	}
	return ptrFilm;
}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	ListeFilms listeFilms = {};

	int nElements = lireUint16(fichier);
	listeFilms.allouerElements(listeFilms.obtenirNElements()); // Crée une liste de films vide.
	for ([[maybe_unused]] int i : range(nElements)) {
		Film* ptrFilm = lireFilm(fichier, listeFilms);
		listeFilms.ajouterFilm(ptrFilm); // Ajoute le film à la liste.
	}
	return listeFilms; // Retourne la liste de films.
}

void detruireActeur(Acteur*& ptrActeur)
{
	ptrActeur->joueDans.desallouerElements();
	delete ptrActeur;
}

//Une fonction pour détruire un film (relâche toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void detruireFilm(Film* ptrFilm)
{
	for (Acteur*& ptrActeur : span(ptrFilm->acteurs.elements, ptrFilm->acteurs.nElements)) {
		ptrActeur->joueDans.enleverFilm(ptrFilm);
		if (ptrActeur->joueDans.obtenirNElements() == 0) {
			detruireActeur(ptrActeur);
		}
	}
	delete[] ptrFilm->acteurs.elements;
	delete ptrFilm;
}

// Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
 void detruireListeFilms(ListeFilms& listeFilms)
 {
 	for (Film*& ptrFilm : span(listeFilms.obtenirElements(), listeFilms.obtenirNElements())) {
 		detruireFilm(ptrFilm);
 	}
 	listeFilms.desallouerElements();
 } 

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

// Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
void afficherFilm(const Film& film)
{
	cout << "Titre : " << film.titre << endl;
	cout << "Realisateur : " << film.realisateur << endl;
	cout << "Annee Sortie : " << film.anneeSortie << endl;
	cout << "Recette : " << film.recette << " M$" << endl;
	cout << "Acteurs" << endl;
	for (Acteur*& acteur : span(film.acteurs.elements, film.acteurs.nElements)) {
		afficherActeur(*acteur);
	}
}

void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	// Utilise notre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	Acteur* acteur = trouverActeur(listeFilms, nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		acteur->joueDans.afficher();
}

int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	// La ligne suivante lit le fichier binaire en allouant la mémoire nécessaire. Affiche les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans notre fonction lireActeur).
	ListeFilms listeFilms = creerListe("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	// Affiche le premier film de la liste.  Est bien Alien, comme prévu.
	afficherFilm(*listeFilms.obtenirElements()[0]);

	cout << ligneDeSeparation << "Les films sont:" << endl;
	// Affiche la liste des films.  Il devrait y en avoir 7.
	listeFilms.afficher();

	// Modifie l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	Acteur* ptrActeur = trouverActeur(listeFilms, "Benedict Cumberbatch");
	ptrActeur->anneeNaissance = 1976;

	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	// Affiche la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	afficherFilmographieActeur(listeFilms, ptrActeur->nom);

	// Détruit et enlève le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	detruireFilm(listeFilms.obtenirElements()[0]);
	listeFilms.enleverFilm(listeFilms.obtenirElements()[0]);

	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	// Affiche la liste des films.
	listeFilms.afficher();

	// Fait les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	int capacite = listeFilms.obtenirCapacite();  // Note au correcteur: cette ligne génère une erreur de build 
												  // puisque la variabel capacite n'est pas referencée, mais je
												  // n'utilisais cette ligne que pour couvrir le code en son entièreté,
												  // alors l'erreur est ignorable.
	listeFilms.modifierCapacite(listeFilms.obtenirCapacite() * 2);

	// Détruit tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	detruireListeFilms(listeFilms);
	return 0;
}