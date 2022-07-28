#include <cmath>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono> 
using namespace std;

#include "vector.hpp"
#include "odeint.hpp"
using namespace cpl;

/*
Reálisabb Lotka-Volterra-modell tápláléklánc

nH_deriv = a * (1 - nH / k) * nH - b * nP * nH / (1 + nH / sP);
nP_deriv = c * nH * nP / (1 + nH / sP) - d * nP * nF / (1 + nP / sF);
nF_deriv = e * nP * nF / (1 + nP / sF) - f * nF;
*/

Vector x(4);

//statikus változók, melyeket argumentumként nyerünk.
double a, b, c, d, e, f, g, nH_start, nP_start, nF_start, k, sP, sF, T, dt, accuracy; // itt dt a kezdeti lépéshossz, utána az ARK4 megváltoztatja kedve szerint.
string file_path;

//  Derivative vector
Vector derivate(const Vector& x) {
    Vector derivative(4);
    derivative[0] = 1;
    derivative[1] = a * (1 - x[1] / k) * x[1] - b * x[2] * x[1] / (1 + x[1] / sP); // A halakból, ha van kellő erőforrás, több születik, mint amennyi elpusztul. Ezt az a változó fejezi ki. A halak szaporodását k korlátozza.
	derivative[2] = c * x[1] * x[2] / (1 + x[1] / sP) - d * x[2] * x[3] / (1 + x[2] / sF) - e * x[2]; // A pingvinek szaporodását a halak mennyisége befolyásolja, pusztulásukat pedig a fókák és saját halálozási rátájuk.
	derivative[3] = f * x[2] * x[3] / (1 + x[2] / sF) - g * x[3]; // A fókák szaporodását a pingvinek mennyisége határozza meg, a halálozásukat pedig a fókákhoz hartozó halálozási ráta.
    return derivative;
}

int main(int argc, char *argv[]) {
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 18) {
			throw -1;
		}
		else {
	    a = atof(argv[1]);
		b = atof(argv[2]);
		c = atof(argv[3]);
		d = atof(argv[4]);
		e = atof(argv[5]);
		f = atof(argv[6]);
		g = atof(argv[7]);
		nH_start = atof(argv[8]);
		nP_start = atof(argv[9]);
		nF_start = atof(argv[10]);
		k = atof(argv[11]);
		sP = atof(argv[12]);
		sF = atof(argv[13]);
		T = atof(argv[14]);
		dt = atof(argv[15]);
		accuracy = atof(argv[16]);
		file_path = argv[17];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 12 argumentum kellene, melyek rendre: \n a \n b \n c \n d \n e \n f \n g \n nH_0 \n nP_0 \n nF_0 \n k \n sP \n sF \n T \n dt \n accuracy \n file_path"<< endl;
		return -1;
	}
	
    x[0] = 0;  x[1] = nH_start; x[2] = nP_start; x[3] = nF_start;

    ofstream dataFile(file_path);
	auto start = chrono::steady_clock::now();
	
    do {
        dataFile << x[0] << '\t';
		dataFile << x[1] << '\t';
		dataFile << x[2] << '\t';
		dataFile << x[3] << '\n';
		if (x[1] <= 0) {x[1] = 1;}
		if (x[2] <= 0) {x[2] = 1;}
		if (x[3] <= 0) {x[3] = 1;}
		
        adaptiveRK4Step(x, dt, accuracy, derivate);
    } while (x[0] < T);
    cout << " data in file" << endl;
    dataFile.close();
	
	auto end = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    cout << "static elapsed time: " << elapsed_seconds.count() << "s\n";
		
}

