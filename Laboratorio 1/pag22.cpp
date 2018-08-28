#include<iostream>
#include <time.h>

#define MAX 10000
using namespace std;

double A[MAX][MAX],x[MAX],y[MAX];

int main()
{
	clock_t ini1,ini2,fin1,fin2;
	double time1,time2;

	//primer bucle
	ini1=clock();
	for (int i = 0; i < MAX; i++)
	{
        for (int j = 0; j < MAX; j++)
        {
            y[i] += A[i][j]*x[j];
        }
	}
    fin1=clock();

    time1=((double) (fin1 - ini1)) * 1000 / CLOCKS_PER_SEC;
    cout<<"Tiempo primer bucle: "<<time1<<endl;
    
    //segundo bucle
    ini2=clock();
    for (int j = 0; j < MAX; j++)
    {
        for (int i = 0; i < MAX; i++)
        {
            y[i] += A[i][j]*x[j];
        }
    }
    fin2=clock();
    time2=((double) (fin2 - ini2)) * 1000 / CLOCKS_PER_SEC;
    cout<<"Tiempo segundo bucle: "<<time2<<endl;

	return 0;
}