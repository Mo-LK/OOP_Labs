/**
* Programme qui indique si le nombre entré est entier. Dans le cas contraire,
* retourne le premier diviseur du nombre en question.
* \file   Exo1.cpp
* \author Erreur-404 et Mo-LK
* \date   18 janvier 2022
* Créé le 18 janvier 2022
*/
#include <iostream>

using namespace std;

int obtenirPlusPetitDiviseur(int nombre)
{
	if (nombre % 2 == 0)
		return 2;
	else {
		for (int i = 3; i <= sqrt(nombre); i += 2) {
			if (nombre % i == 0)
				return i;
		}
	}
	return 0;
}

int main() {
	while (true) {
		int reponse;
		cout << "Entrez un nombre entier: ";
		cin >> reponse;

		int diviseur = obtenirPlusPetitDiviseur(reponse);

		if (diviseur == 0)
			cout << "Ce nombre est premier." << endl;
		else
			cout << "Ce nombre n'est pas premier car il est divisible par " << diviseur << "." << endl;
	}
	return 0;
}
