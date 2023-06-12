/*
* Programme qui lit les mots associ�s de leur genre et de leur d�finition dans un fichier et 
* affiche le plus long mots.
* \file   Exo6.cpp
* \author Erreur-404 et Mo-LK
* \date   21 janvier 2022
* Cr�� le 21 janvier 2022
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int nbMots = 4;

struct Mot
{
	string terme;
	string genre;
	string definition;
};

Mot genererMot(string ligne);
Mot obtenirPlusLongMot(Mot dictionnaire[]);

int main()
{
	Mot dictionnaire[nbMots];
	fstream donnees;
	string ligne;
	string nomFichier = "../dictionnaire.txt";

	donnees.open(nomFichier, ios::in);
	for (int i = 0; i < nbMots; i++) {
		getline(donnees, ligne);
		dictionnaire[i] = genererMot(ligne);
	}
	donnees.close();
	
	Mot longMot = obtenirPlusLongMot(dictionnaire);
	
	cout << longMot.terme << " (" << longMot.genre << ") : " << longMot.definition << endl;
}

Mot genererMot(string ligne)
{
	Mot nouveauMot;

	int posPremierTab = ligne.find('\t');
	int posDeuxiemeTab = ligne.find('\t', posPremierTab + 1);
	int longueurGenre = posDeuxiemeTab - posPremierTab - 1;
	
	string terme = ligne.substr(0, posPremierTab);
	string genre = ligne.substr(posPremierTab + 1, longueurGenre);
	string definition = ligne.substr(posDeuxiemeTab + 1);
	
	nouveauMot.terme = terme;
	nouveauMot.genre = genre;
	nouveauMot.definition = definition;

	return nouveauMot;
}

Mot obtenirPlusLongMot(Mot dictionnaire[])
{
	Mot plusLongMot = dictionnaire[0];
	int longueurPlusLongMot = plusLongMot.terme.size();

	for (int i = 0; i < nbMots; i++) {
		int longueurMotCourant = dictionnaire[i].terme.size();

		if (longueurMotCourant > longueurPlusLongMot) {
			plusLongMot = dictionnaire[i];
			longueurPlusLongMot = plusLongMot.terme.size();
		}
	}
	return plusLongMot;
}