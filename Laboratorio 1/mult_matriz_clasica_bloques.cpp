#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sys/time.h>
#include <fstream>

typedef int T;

using namespace std;

void print(vector<vector<T>> & m){
    int row=m.size();
    int col=m[0].size();
    for (int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            cout<<m[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
    
}

void llenar(vector<vector<T>> &m){
    int row=m.size();
    int col=m[0].size();
    for (int i=0;i<row;i++)
        for(int j=0;j<col;j++)
            m[i][j]=j;
}
vector<vector<T>>mult_clasica(vector<vector<T>>m1,vector<vector<T>>m2){
    
    int row1=m1.size();
    int col1=m1[0].size();
    int col2=m2[0].size();
    
    vector<vector<T>> m_rpta(row1,vector<T>(col2));
    for (int i=0;i<row1;i++)
        for(int j=0;j<col2;j++)
            for(int k=0;k<col1;k++)
                    m_rpta[i][j]+=m1[i][k]*m2[k][j];    
    return m_rpta;
            
}
vector<vector<T>>mult_bloque (vector<vector<T>>m1,vector<vector<T>>m2){
    
    int n=m1.size(); 
    int block_size=n/10;
    vector<vector<T>> m_rpta(n,vector<T>(n));
    for(int i=0; i<n; i+=block_size )
        for(int j=0; j<n; j+=block_size)
            for(int k=0; k<n; k+=block_size )
                for(int y=i; y<i+block_size; y++ )
                    for(int x=j; x<j+block_size; x++ )
                        for (int z=k; z<k+block_size; z++ )
                            m_rpta[y][x] += m1[y][z] * m2[z][x]; 
    return m_rpta;
}


int main(){
int MAX,row1,col1,row2,col2;
MAX=500;
row1=col1=row2=col2=MAX;
if(col1!=row2) return 0;
vector<vector<T>> A(row1,vector<T>(col1));
vector<vector<T>> B(row2, vector<T>(col2));
vector<vector<T>> m_clasica(row1, vector<T>(col2));
vector<vector<T>> m_bloque(row1, vector<T>(col2));
llenar(A);
llenar(B);

//PRIMER ALGORITMO
struct timeval inicio, fin;
    double ttime1,ttime2;
/*
gettimeofday(&inicio, NULL);
m_clasica=mult_clasica(A,B);
gettimeofday(&fin, NULL);
ttime1 = (fin.tv_sec - inicio.tv_sec)*1000 + (fin.tv_usec - inicio.tv_usec)/1000;  
printf("tiempo multiplicacion clasica: %.10f s\n", ttime1/1000);
*/
//SEGUNDO ALGORITMO
gettimeofday(&inicio, NULL);
m_bloque=mult_bloque(A,B);
gettimeofday(&fin, NULL);
ttime2 = (fin.tv_sec - inicio.tv_sec)*1000 + (fin.tv_usec - inicio.tv_usec)/1000;  
printf("tiempo multiplicacion bloque: %.10f s\n", ttime2/1000);
 
return 0;
}