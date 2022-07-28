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
n1_deriv = r1 * n1 * (1 - (n1 + alpha * n2) / k1);
n2_deriv = r2 * n2 * (1 - (n2 + beta * n1) / k2);

*/

Vector x(3);

//statikus változók, melyeket argumentumként nyerünk.
double r1, r2, n1_start, n2_start, alpha, beta, k1, k2, T, dt, accuracy; // itt dt a kezdeti lépéshossz, utána az ARK4 megváltoztatja kedve szerint.
string file_path;

//  Derivative vector for f1
Vector f(const Vector& x) {
    Vector f(3);
    f[0] = 1;
    f[1] = r1 * x[1] * (1 - (x[1] + alpha * x[2]) / k1);
	f[2] = r2 * x[2] * (1 - (x[2] + beta * x[1]) / k2);
    return f;
}

int main(int argc, char *argv[]) {
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 13) {
			throw -1;
		}
		else {
	    r1 = atof(argv[1]);
		r2 = atof(argv[2]);
		n1_start = atof(argv[3]);
		n2_start = atof(argv[4]);
		alpha = atof(argv[5]);
		beta = atof(argv[6]);
		k1 = atof(argv[7]);
		k2 = atof(argv[8]);
		T = atof(argv[9]);
		dt = atof(argv[10]);
		accuracy = atof(argv[11]);
		file_path = argv[12];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 12 argumentum kellene, melyek rendre: \n r1 \n r2 \n n1_0 \n n2_0 \n alpha \n beta \n k1 \n k2 \n T \n dt \n accuracy \n file_path"<< endl;
		return -1;
	}
	
    x[0] = 0;  x[1] = n1_start; x[2] = n2_start;

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

