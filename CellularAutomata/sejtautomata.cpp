#include <cmath>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <chrono> 
#include <random>
using namespace std;

//Globals
int M=40;
int N=40;
int mode;
string start_matrix;
int n_alive;
int epoch;
string savepath;

//Függvények a keret módosításához

void write_frame_nums(int ** arr,int num) {
    //Egy adott efész számmal (num) tölti fel a mátrix széleit
    arr[0] [0] = num;
    arr[0] [N] = num;
    arr[M] [0] = num;
    arr[M] [N] = num;
	for (int i=1;i<N;++i) {
        arr[0] [i] = num;
		arr[M][i] = num;
	}
    for (int i=1;i<M;++i) {
        arr[i] [0] = num;
        arr[i] [N] = num;
	}
}

int random_int_0_1() {
	random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(0, 1); // define the range
	return (int) distr(gen);
}

void write_frame_random(int ** arr,int rand_min,int rand_max) {
    //rand_min és rand_max közötti (zárt intervallum) véletlen
    //számokkal tölti fel a mátrix széleit.
    arr[0][0] = random_int_0_1();
    arr[0][N] = random_int_0_1();
    arr[M][0] = random_int_0_1();
    arr[M][N] = random_int_0_1();
    for (int i=1;i<N;++i) {
        arr[0][i] = random_int_0_1();
        arr[M][i] = random_int_0_1();
	}
    for (int i=1;i<M;++i) {
        arr[i][0] = random_int_0_1();
        arr[i][N] = random_int_0_1();
    }
}

void write_periodic_frame(int ** arr) {
    //A teljes mátrix széleit a belső mátrix alapján periodikus határfeltétellel egészíti ki.
    //csúcsok
    arr[0][0] = arr[M-1][N-1];
    arr[0][N] = arr[M-1][1];
    arr[M][0] = arr[1][N-1];
    arr[M][N] = arr[1][1];
    for (int i=1;i<N;++i) {
        arr[0][i] = arr[M-1][i]; //tető
        arr[M][i] = arr[1][i]; //alap
    }
	for (int i=1;i<M;++i) {
        arr[i][0] = arr[i][N-1]; //baloldal
        arr[i][N] = arr[i][1]; //jobboldal
    }
}

void write_frame(int ** arr) {
    /*Feltölti a mátrix (arr) széleit a mode változóban választott módon.
       0 : Minden szélső elem nulla értékű lesz.
       1 : Minden szélső elem egy értékű lesz.
       2 : A szélső elemek teljesítik a kétdimenziós periodikus peremfeltételt.
       3 : A szélső elemek külön-külön véletlenszerűen vagy nulla, vagy egy értékűek lesznek*/
    if (mode == 0) {
		return write_frame_nums(arr,0);
    } else if (mode == 1) {
        return write_frame_nums(arr,1);
    } else if (mode == 2) {
        return write_periodic_frame(arr);
    } else if(mode == 3) {
        return write_frame_random(arr,0,1);
    } else {
        cout << "Rossz paraméter lett megadva a write_frame függvénynek (lehetséges paraméterek:0,1,2,3)." << endl;
	}
}

//Mátrix kezdeti előállítása
int** generate_matrix() {
	//Ez a függvény egy (M+1) X (N+1)-es mátrixot hoz létre, mely az M X N-es mátrix, és a keretének feleltethető meg.
	int** arr = new int*[M+1]; //arr deklarációja
    for (int i=0;i<M+1;++i) {
        arr[i] = new int[N+1];
    }
	string s1 = "./start_matrix/";
	string s2 = start_matrix;
	string s3 = ".txt";
	string path = s1 + s2 + s3;
	ifstream f(path);
	for (int i=0;i<M+1;++i) {
        for (int j=0;j<N+1;++j) {
            f >> arr[i][j];
        }
    }
	f.close();
	
	write_frame(arr);
	
	return arr;
}

//Élő szomszédok kiszámítása
int sum_neighbours(int** arr,int m,int n) {
    //Megadja, hogy hány élő szomszédja van a mátrix egy elemének a megadott m-n indexen.
    int a = 0;
	for (int i=-1;i<2;++i) {
		for (int j=-1;j<2;++j) {
			if (i==0 && j==0) {
				continue;
			} else {
				a += arr[m+i][n+j];
			}
		}
	}
	return a;
}



int main(int argc, char *argv[]) {
	try { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		if (argc != 8) {
			throw -1;
		}
		else {
	    M = atoi(argv[1]);
		N = atoi(argv[2]);
		mode = atoi(argv[3]);
		start_matrix = argv[4];
		n_alive = atoi(argv[5]);
		epoch = atoi(argv[6]);
		savepath = argv[7];
		}
	} catch (...) {
		cout << "Rossz paraméterek, összesen 7 argumentum kellene, melyek rendre: \n M \n N \n mode \n start_matrix \n n_alive \n epoch \n savepath"<< endl;
		return -1;
	}
	
	int** arr = generate_matrix();
	
	/*for (int i=0;i<M+1;++i) {
        for (int j=0;j<N+1;++j) {
			cout << arr[i][j];
		}
		cout << endl;
	}
	return 0;*/
	
	string path;
	string i_s;
	for (int i=0;i<epoch;++i) {
		// átírom a külső ciklus indexét string változóba is a fájlok útvonalának megadásához.
		stringstream ss;
		ss << i;
		ss >> i_s;
		path = savepath + "/" + i_s + ".txt"; // egy időpont mátrixának útvonala
		
		//fájl megnyitás és az arr array fájlba írása és mentése
		ofstream f(path);
		for (int j=1;j<M;++j) {
			for (int k=1;k<N;++k) {
				f << arr[j][k] << " ";
			}
			f << "\n";
		}
		f.close();
		
		//új array deklarálása a következő lépés kiszámításához
		int** arr_new = new int*[M+1];
		for (int j=0;j<M+1;++j) {
			arr_new[j] = new int[N+1];
		}
		
		// a következő lépés kiszámítása
		for (int j=1;j<M;++j) {
			for (int k=1;k<N;++k) {
				int n_alive_now = sum_neighbours(arr,j,k); //élő szomszédok mennyiségének kiszámítása
				if (n_alive_now == n_alive) { // nincs változás
					arr_new[j][k] = arr[j][k];
				} else if (n_alive_now == n_alive + 1) { //élő lesz
					arr_new[j][k] = 1;
				} else {
					arr_new[j][k] = 0; //meghal
				}
			}
		}
		
		//előző időpont mátrixának belső sorainak felszabadítása a memóriából
		//for (int j=0;j<M+1;++j) {
		//	delete[] arr[j];
		//}
		arr = arr_new;
		write_frame(arr);
	}
	
	return 0;
	
}

