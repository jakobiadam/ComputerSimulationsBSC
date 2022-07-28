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

Vector x1;
Vector x2;

//statikus változók, melyeket argumentumként nyerünk.
double r, x_start, T, dt, accuracy;
string file_path1, file_path2;

//  Derivative vector for f1
Vector f(const Vector& x) {
    Vector f(2);
    f[0] = 1;
    f[1] = r * x[1] * (1 - x[1]);
    return f;
}

//Euler algorithm
void EulerStep(Vector& x, double tau, Vector derivs(const Vector&))
{
	Vector k1 = tau * derivs(x);
	x += k1;
}

// Analitikus megoldás
double analFunc(const double t) {
	return 1 / (1 + (1/x_start - 1) * exp(- r * t));
}

int main(int argc, char *argv[]) {
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 8) {
			throw -1;
		}
		else {
	    r = atof(argv[1]);
		x_start = atof(argv[2]);
		T = atof(argv[3]);
		dt = atof(argv[4]);
		accuracy = atof(argv[5]);
		file_path1 = argv[6];
		file_path2 = argv[7];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 7 argumentum kellene, melyek rendre: \n r \n x0 \n T \n dt \n accuracy \n file_path1 \n file_path2"<< endl;
		return -1;
	}
	
    Vector x0(2);
    x0[0] = 0;  x0[1] = x_start;
	
	Vector x1, x2;
	x1=x0; x2 = x0;
	double x3 = x_start;

	// Euler és analitikus együtt egy fájlban
    ofstream dataFile1(file_path1);
	auto start1 = chrono::steady_clock::now();
	
    do {
        dataFile1 << x1[0] << '\t';
		dataFile1 << x1[1] << '\t';
		dataFile1 << x3 << '\n';
        EulerStep(x1, dt, f);
		x3 = analFunc(x1[0]);
    } while (x1[0] < T);
    cout << " data1 in file" << endl;
    dataFile1.close();
	
	auto end1 = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds1 = end1-start1;
    cout << "static elapsed time: " << elapsed_seconds1.count() << "s\n";
	
	// ARK4 külön fájlban (nem azonos a sorok száma az előzőekkel az adaptív lépéshosszok miatt.
	ofstream dataFile2(file_path2);
	auto start2 = chrono::steady_clock::now();
	
    do {
        dataFile2 << x2[0] << '\t';
		dataFile2 << x2[1] << '\n';
		adaptiveRK4Step(x2, dt, accuracy, f);
    } while (x2[0] < T);
    cout << " data2 in file" << endl;
    dataFile2.close();
	
	auto end2 = std::chrono::steady_clock::now();
    chrono::duration<double> elapsed_seconds2 = end2-start2;
    cout << "static elapsed time: " << elapsed_seconds2.count() << "s\n";
	
	
}

