#include<iostream>

using namespace std;

double scalar_mult_list(double scalar,const double &arr[],int length) 
{
	double arr_new[length] = {0.0};
	for (int i=0; i<length) 
	{
		arr_new[i] = arr[i] * scalar;
	}
	return arr_new;
}	

double list_p_list(const double &arr1[], const double &arr2[])
{
	double arr_new[length] = {0.0};
	for (int i=0; i<length; ++i) 
	{
		arr_new[i] = arr1[i] + arr2[i];
	}
}
