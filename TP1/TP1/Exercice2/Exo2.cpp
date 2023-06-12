/*
* Programme qui retourne le nombre de mois n�cessaires afin de rembourser un pr�t
* en plus de retourner le total des int�r�ts accumul�s par le pr�teur. Le programme
* prend en entr�e la somme emprunt�e, le montant rembours� � chaque mois, ainsi que 
* le taux d'int�r�t annuel.
* \file   Exo2.cpp
* \author Erreur-404 et Mo-LK
* \date   18 janvier 2022
* Cr�� le 18 janvier 2022
*/
#include <iostream>
#include <cmath>

using namespace std;

bool entreeEstValide(double& somme, double& tauxAnnuel, double& montantRembourse);
bool paramEstValide(double entree, double minimum, double maximum);
int obtenirNombreDeMois(double somme, double remboursementMensuel, double tauxMensuel, double& interets);

int main()
{
	double somme, montantRembourse, tauxAnnuel;
	cout << "Veuillez entrer, dans l'ordre, la somme pr�t�e, le montant rembours�"
		<< "chaque mois, ainsi que le taux d'int�r�t annuel: ";
	cin >> somme >> montantRembourse >> tauxAnnuel;

	while (!entreeEstValide(somme, montantRembourse, tauxAnnuel)) { };

	double tauxMensuel = tauxAnnuel / 1200;

	double interetsAccumules = 0;
	int nombreDeMois = obtenirNombreDeMois(somme, montantRembourse, tauxMensuel, interetsAccumules);

	cout << nombreDeMois << " mois sont n�cessaires afin de rembourser la dette." << endl;
	cout << "Le pr�teur accumulera " << interetsAccumules << "$ au terme du pr�t.";
}

bool entreeEstValide(double& somme, double& tauxAnnuel, double& montantRembourse)
{
	bool sommeValide = paramEstValide(somme, 0.0, INFINITY);
	bool tauxAnnuelValide = paramEstValide(tauxAnnuel, 0.0, 100.0);
	bool montantRembourseValide = paramEstValide(montantRembourse, 0.0, INFINITY);

	if (sommeValide && tauxAnnuelValide && montantRembourseValide) {
		return true;
	}

	if (!sommeValide) {
		cout << "Veuillez entrer une somme positive: ";
		cin >> somme;
	}

	if (!montantRembourseValide) {
		cout << "Veuillez entrer un montant rembours� positif: ";
		cin >> montantRembourse;
	}

	if (!tauxAnnuelValide) {
		cout << "Veuillez entrer un taux d'int�r�t se trouvant entre 0% et 100%: ";
		cin >> tauxAnnuel;
	}

	return false;
}

bool paramEstValide(double entree, double minimum, double maximum)
{
	if ((minimum < entree) && (entree < maximum)) {
		return true;
	}
	else {
		return false;
	}
}

int obtenirNombreDeMois(double somme, double remboursementMensuel, 
						double tauxMensuel, double& interets)
{
	int nombreDeMois = 0;
	while (somme > 0) {
		nombreDeMois++;
		somme += somme * tauxMensuel;
		interets += somme * tauxMensuel;
		somme -= remboursementMensuel;
	}
	return nombreDeMois;
}