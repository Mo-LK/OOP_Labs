/*
* Programme qui calcul la hauteur d'une balle � son ni�me rebond
* lanc�e d'une hauteur x.
* \file   Exo3.cpp
* \author Erreur-404 et Mo-LK
* \date   18 janvier 2022
* Cr�� le 18 janvier 2022
*/

#include <iostream>
#include <cmath>

using namespace std;

const int gConstant = 9.81;

double getInitialVelocity(double initialHeight);
double getFinalVelocity(double initialVelocity, double coefficientRebound);
double getFinalHeight(double finalVelocity);
bool isValidInput(double& initialHeight, int& nbRebounds, double& coefficientRebound);
bool isValidParam(double valeur, double valeurMinimum, double valeurMaximum);

int main() {
	double initialHeight;
	int nbRebounds;
	double coefficientRebound;

	cout << "Enter the ball's initial height, followed by the number of rebounds after which the maximum height will be taken and the coefficient of rebound:";
	cin >> initialHeight >> nbRebounds >> coefficientRebound;

	while (!isValidInput(initialHeight, nbRebounds, coefficientRebound)) { };

	double maximumHeight = 0;
	for (int i = 0; i < nbRebounds; i++) {
		double initialVelocity = getInitialVelocity(initialHeight);
		double finalVelocity = getFinalVelocity(initialVelocity, coefficientRebound);
		maximumHeight = getFinalHeight(finalVelocity);

		initialHeight = maximumHeight;
	}
	cout << "The ball's maximum height after " << nbRebounds << " rebounds is " << maximumHeight << "m." << endl;
	return 0;
}


double getInitialVelocity(double initialHeight) {
	return pow(2.0 * gConstant * initialHeight, 0.5);
}


double getFinalVelocity(double initialVelocity, double coefficientRebound) {
	return coefficientRebound * initialVelocity;
}


double getFinalHeight(double finalVelocity) {
	return pow(finalVelocity, 2) / (2.0 * gConstant);
}


bool isValidInput(double& initialHeight, int& nbRebounds, double& coefficientRebound)
{
	bool isValidHeight = isValidParam(initialHeight, 0.0, INFINITY);
	bool isValidNbRebounds = isValidParam(nbRebounds, 0.0, INFINITY);
	bool isValidCoefficientRebound = isValidParam(coefficientRebound, 0.0, 1.0);

	if (isValidHeight && isValidNbRebounds && isValidCoefficientRebound) {
		return true;
	}

	if (!isValidHeight) {
		cout << "Enter a positive height:";
		cin >> initialHeight;
	}

	if (!isValidNbRebounds) {
		cout << "Enter a positive number of rebounds:";
		cin >> nbRebounds;
	}

	if (!isValidCoefficientRebound) {
		cout << "Enter a coefficient of rebound whose value is between 0 and 1:";
		cin >> coefficientRebound;
	}

	return false;
}


bool isValidParam(double value, double minimumValue, double maximumValue) {
	if (value < minimumValue || value > maximumValue)
		return false;
	else
		return true;
}