#include<iostream>

using namespace std;

void printArray(vector<double> arr, int length) {
        for (int i=0;i<length;++i) {
                cout << arr[i] << endl;
        }
}

int main() {
        
        vector<double> array = {10,11,1,2,2,3,4,5,6,10};

        printArray(array,10);
        
        return 0;
        
}       

