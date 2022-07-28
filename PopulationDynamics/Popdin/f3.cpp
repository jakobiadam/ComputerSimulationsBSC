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
Lotka-Volterra-modell
nR_deriv = a * nR - b * nF * nR;
nF_deriv = c * nR * nF - d * nF;
*/

Vector x(3);

//statikus változók, melyeket argumentumként nyerünk.
double a, b, c, d, nR_start, nF_start, T, dt, accuracy; // itt dt a kezdeti lépéshossz, utána az ARK4 megváltoztatja kedve szerint.
string file_path;

//  Derivative vector
Vector f(const Vector& x) {
    Vector f(3);
    f[0] = 1;
    f[1] = a * x[1] - b * x[2] * x[1];
	f[2] = c * x[1] * x[2] - d * x[2];
    return f;
}

int main(int argc, char *argv[]) {
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 11) {
			throw -1;
		}
		else {
	    a = atof(argv[1]);
		b = atof(argv[2]);
		c = atof(argv[3]);
		d = atof(argv[4]);
		nR_start = atof(argv[5]);
		nF_start = atof(argv[6]);
		T = atof(argv[7]);
		dt = atof(argv[8]);
		accuracy = atof(argv[9]);
		file_path = argv[10];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 12 argumentum kellene, melyek rendre: \n a \n b \n c \n d \n nR_0 \n nF_0 \n T \n dt \n accuracy \n file_path"<< endl;
		return -1;
	}
	
    x[0] = 0;  x[1] = nR_start; x[2] = nF_start;

    ofstream dataFile(file_path);
	auto start = chrono::steady_clock::now();
	
    do {
        dataFile << x[0] << '\t';
		dataFile << x[1] << '\t';
		dataFile << x[2] << '\n';
        adaptiveRK4Step(x, dt, accuracy, f);
    } while (x[0] < T);
    cout << " data in file" << endl;
    dataFile.close();
	
	auto end = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    cout << "static elapsed time: " << elapsed_seconds.count() << "s\n";
		
}

