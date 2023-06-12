/*
* Programme qui approxime la valeur de pi � l'aide de la m�thode de 
* Monte Carlo.
* \file   Exo4.cpp
* \author Erreur-404 et Mo-LK
* \date   21 janvier 2022
* Cr�� le 21 janvier 2022
*/
#include <iostream>
#include <iomanip>
#include <random>

using namespace std;

const int randomMaxValue = 100000;

double randomDecimal(int minimum, int maximum);

int main()
{
	int nbIterations;
	cout << "Entrez le nombre d'it�rations souhait�es: ";
	cin >> nbIterations;
	int pointsDansCercle = 0;
	for (int i = 0; i < nbIterations; i++) {
		double x = randomDecimal(-1, 1);
		double y = randomDecimal(-1, 1);
		if ((pow(x, 2) + pow(y, 2)) < 1)
			pointsDansCercle++;
	}
	double pi = 4.0 * pointsDansCercle / nbIterations;
	cout << "La valeur de pi est " << setprecision(7) << pi << endl;
}

double randomDecimal(int minimum, int maximum)
{
	default_random_engine random(random_device{}());
	int range = abs(maximum - minimum);
	double randomValue = double(random() % (range * randomMaxValue));
	randomValue += (minimum * randomMaxValue);
	randomValue /= randomMaxValue;
	return randomValue;
}