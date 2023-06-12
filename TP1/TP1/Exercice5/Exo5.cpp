/*
* Programme qui prend en entr�e un tableau de 10 entiers et le r�arrange afin
* de pr�senter les nombres pairs d'abord
* \file   Exo6.cpp
* \author Erreur-404 et Mo-LK
* \date   21 janvier 2022
* Cr�� le 21 janvier 2022
*/
#include <iostream>
#include <cmath>

using namespace std;

const int sizeList = 10;

int main() 
{
	int counterEvenNumber = 0;
	int counterOddNumber = sizeList - 1;

	int list[sizeList] = {};
	int evenOddList[sizeList];
	for (int i = 0; i < ssize(list); i++) {
		cout << "Enter element " << i << " of the list:" << endl;
		cin >> list[i];
		if (list[i] % 2 == 0) {
			evenOddList[counterEvenNumber] = list[i];
			counterEvenNumber++;
		}
		else {
			evenOddList[counterOddNumber] = list[i];
			counterOddNumber--;
		}
	}
	
	cout << "\n\n";
	for (int value : evenOddList) {
		cout << value << " ";
	}

	return 0;
}