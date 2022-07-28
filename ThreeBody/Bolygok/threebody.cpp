#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

#include "vector.hpp"
#include "odeint.hpp"
using namespace cpl;

//const double G = 6.6743 * pow(10,-11); // m^3 / (kg * s^2)  ==> át kell váltani AU^3 / (m_bolygo * year^2)
//static const double m_bolygo = 6 * pow(10,24) // kb a Föld tömege kg-ban, referenciatömeg
//static const double AU = 1.496 * pow(10,11) // egy csillagászati egység méterben
//static const double year = 31557600 // egy év másodpercekben

//a fent kikommentelt egységek lesznek a rendszerem mértékegységei
static const double G = 0.000119116; // AU^3 / (m_bolygo * year^2)
static double m1, m2, m3;
//  Derivative vector for Newton's law of gravitation
Vector f(const Vector& x) {
    double r1_x = x[1], r1_y = x[2], v1_x = x[3], v1_y = x[4], r2_x = x[5], r2_y = x[6], v2_x = x[7], v2_y = x[8], r3_x = x[9], r3_y = x[10], v3_x = x[11], v3_y = x[12];
    Vector f(13);
    f[0] = 1;
    f[1] = v1_x;
    f[2] = v1_y;
    f[3] = -G * (m2 * (r1_x - r2_x) / (pow(pow(r1_x-r2_x,2) + pow(r1_y-r2_y,2),1.5)) + m3 * (r1_x - r3_x) / (pow(pow(r1_x-r3_x,2) + pow(r1_y-r3_y,2),1.5)));
    f[4] = -G * (m2 * (r1_y - r2_y) / (pow(pow(r1_x-r2_x,2) + pow(r1_y-r2_y,2),1.5)) + m3 * (r1_y - r3_y) / (pow(pow(r1_x-r3_x,2) + pow(r1_y-r3_y,2),1.5)));
    f[5] = v2_x;
    f[6] = v2_y;
    f[7] = -G * (m1 * (r2_x - r1_x) / (pow(pow(r2_x-r1_x,2) + pow(r2_y-r1_y,2),1.5)) + m3 * (r2_x - r3_x) / (pow(pow(r2_x-r3_x,2) + pow(r2_y-r3_y,2),1.5)));
    f[8] = -G * (m1 * (r2_y - r1_y) / (pow(pow(r2_x-r1_x,2) + pow(r2_y-r1_y,2),1.5)) + m3 * (r2_y - r3_y) / (pow(pow(r2_x-r3_x,2) + pow(r2_y-r3_y,2),1.5)));
    f[9] = v3_x;
    f[10] = v3_y;
    f[11] = -G * (m1 * (r3_x - r1_x) / (pow(pow(r3_x-r1_x,2) + pow(r3_y-r1_y,2),1.5)) + m2 * (r3_x - r2_x) / (pow(pow(r3_x-r2_x,2) + pow(r3_y-r2_y,2),1.5)));
    f[12] = -G * (m1 * (r3_y - r1_y) / (pow(pow(r3_x-r1_x,2) + pow(r3_y-r1_y,2),1.5)) + m2 * (r3_y - r2_y) / (pow(pow(r3_x-r2_x,2) + pow(r3_y-r2_y,2),1.5)));
	
	cout << f[8] << endl;
	cout << f[12] << endl;
	
    return f;
}

int main(int argc, char *argv[]) {
    double r1_x, r1_y, r2_x, r2_y, r3_x, r3_y, v1_x, v1_y, v2_x, v2_y, v3_x, v3_y, T, dt, accuracy;
	string path_file_static, path_file_adapt;
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 21) {
			throw -1;
		}
		else {
		m1 =  atof(argv[1]);
		r1_x = atof(argv[2]);
		r1_y = atof(argv[3]);
		v1_x = atof(argv[4]);
		v1_y = atof(argv[5]);
		m2 =  atof(argv[6]);
		r2_x = atof(argv[7]);
		r2_y = atof(argv[8]);
		v2_x = atof(argv[9]);
		v2_y = atof(argv[10]);
		m3 =  atof(argv[11]);
		r3_x = atof(argv[12]);
		r3_y = atof(argv[13]);
		v3_x = atof(argv[14]);
		v3_y = atof(argv[15]);
		T = atof(argv[16]);
		dt = atof(argv[17]);
		accuracy = atof(argv[18]);
		
		
		path_file_static = argv[19];
		path_file_adapt = argv[20];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 7 argumentum kellene, melyek rendre: \n r_ap \n eccentricity \n periods \n dt \n accuracy \n path_file_static \n path_file_adapt"<< endl;
		return -1;
	}
	
    Vector x(13);
    x[0] = 0;  x[1] = r1_x;  x[2] = r1_y;  x[3] = v1_x;  x[4] = v1_y;  x[5] = r2_x;  x[6] = r2_y;  x[7] = v2_x;  x[8] = v2_y;  x[9] = r3_x;  x[10] = r3_y;  x[11] = v3_x;  x[12] = v3_y;

    ofstream dataFile(path_file_adapt);

    do {
        for (int i = 0; i < 13; i++) {
            dataFile << x[i] << '\t';
		}
        RK4Step(x, dt, f);
		dataFile <<  '\n';
    } while (x[0] < T);

    dataFile.close();

}


