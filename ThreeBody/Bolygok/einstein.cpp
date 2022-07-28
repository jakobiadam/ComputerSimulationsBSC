#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

#include "vector.hpp"
#include "odeint.hpp"
using namespace cpl;

const double pi = 4 * atan(1.0);
const double GmPlusM = 4 * pi * pi; // G * (m + M), itt valójában m-et elhanyagoljuk M mellett, mert M >> m.
const static double alpha = 1.1 * pow(10,-8);

static bool switch_t_with_y = false;    //  to interpolate to y = 0
static Vector x;

//  Derivative vector for Newton's law of gravitation
Vector f(const Vector& x) {
    double r_x = x[1], r_y = x[2], v_x = x[3], v_y = x[4];
    double rSquared = r_x*r_x + r_y*r_y;
    double rCubed = rSquared * sqrt(rSquared);
    Vector f(5);
    f[0] = 1;
    f[1] = v_x;
    f[2] = v_y;
    f[3] = - GmPlusM * r_x / rCubed * (1 + alpha / rSquared); // elhanyagoltuk m járulékát M-hez képest, a képlet helyesen -G * M * r_x / rCubed
    f[4] = - GmPlusM * r_y / rCubed * (1 + alpha / rSquared); // Einstein korrekciója
    if (switch_t_with_y) {
        //  use y as independent variable
        for (int i = 0; i < 5; i++)
            f[i] /= v_y;
    }
    return f;
}

//  Change independent variable from t to y and step back to y = 0
void interpolate_crossing(Vector& x, int& crossing) {
    ++crossing;
    switch_t_with_y = true;
    RK4Step(x, -x[2], f);
    //cout << " crossing " << crossing << "\t t = " << x[0]
    //     << "\t x = " << x[1]   << "\t y = " << x[2] << endl;
    switch_t_with_y = false;
}

int main(int argc, char *argv[]) {
    double r_ap, eccentricity, a, T, v0, periods, dt, accuracy;
	string path_file_static, path_file_adapt;
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 8) {
			throw -1;
		}
		else {
	    r_ap = atof(argv[1]);
		eccentricity = atof(argv[2]);
		periods = atof(argv[3]);
		dt = atof(argv[4]);
		accuracy = atof(argv[5]);
		path_file_static = argv[6];
		path_file_adapt = argv[7];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 7 argumentum kellene, melyek rendre: \n r_ap \n eccentricity \n periods \n dt \n accuracy \n path_file_static \n path_file_adapt"<< endl;
		return -1;
	}
	
    a = r_ap / (1 + eccentricity);
    T = pow(a, 1.5);
    v0 = sqrt(GmPlusM * (2 / r_ap - 1 / a));
	
	cout << "a=" << a << "T=" << T << endl;
	
    Vector x0(5);
    x0[0] = 0;  x0[1] = r_ap;  x0[2] = 0;  x0[3] = 0;  x0[4] = v0;

    ofstream dataFile(path_file_static);
    x = x0;
    int steps = 0, crossing = 0;
    cout << "\n Integrating with fixed step size" << endl;
    do {
        for (int i = 0; i < 5; i++)
            dataFile << x[i] << '\t';
		dataFile << sqrt(x[1] * x[1] + x[2] * x[2]) << '\n';
        double y = x[2];
        RK4Step(x, dt, f);
        ++steps;
        if (y * x[2] < 0) {
            interpolate_crossing(x, crossing);
		}
    } while (x[0] < periods * T);
    cout << " number of fixed size steps = " << steps << endl;
    cout << " data in file fixed.data" << endl;
    dataFile.close();

    dataFile.open(path_file_adapt);
    x = x0;
    steps = crossing = 0;
    double dt_max = 0, dt_min = 100;
    cout << "\n Integrating with adaptive step size" << endl;
    do {
        for (int i = 0; i < 5; i++)
            dataFile << x[i] << '\t';
		dataFile << sqrt(x[1] * x[1] + x[2] * x[2]) << '\t';
        double t_save = x[0];
        double y = x[2];
        adaptiveRK4Step(x, dt, accuracy, f);
		dataFile << dt << '\n';
        double step_size = x[0] - t_save;
        ++steps;
        if (step_size < dt_min) dt_min = step_size;
        if (step_size > dt_max) dt_max = step_size;
        if (y * x[2] < 0) {
            interpolate_crossing(x, crossing);
		}
    } while (x[0] < periods * T);
    cout << " number of adaptive steps = " << steps << endl;
    cout << " step size: min = " << dt_min << "  max = " << dt_max << endl;
    cout << " data in file adaptive.data" << endl;
    dataFile.close();
}


