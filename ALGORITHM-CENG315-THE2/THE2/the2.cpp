#include <iostream>

using namespace std;

void cs(string *arr, int size,  int index, int & res){
    string *b = new string[size]; 
    int *c = new int [27];
    
    for(int i = 0; i < 27 ; i++) c[i] = 0;
    
    for(int i = 0; i < size ; i++){
        res++;
        if(index >= arr[i].length()){
            c[0]++;
        }
        else c[arr[i][index] - 'A' + 1]++;
    }
    
    for(int i = 1; i < 27 ; i++){
        res++;
        c[i] = c[i-1] + c[i];
    }
    
    
    for(int i = size-1; i >  -1  ; i--){
        res++;
        if(index >= arr[i].length()){
            b[c[0] - 1 ] = arr[i];
            c[0]--;
            continue;
        }
        else{
            b[c[ arr[i][index] - 'A' + 1  ] - 1 ] = arr[i];
            c[ arr[i][index] - 'A' + 1]--;   
        }
    }
    for(int i = 0; i <  size ; i++){
        res++;
        arr[i] = b[i];
    }
}

void cs1(string *arr, int size,  int index, int & res){
    string *b = new string[size]; 
    int *c = new int [27];
    
    for(int i = 0; i < 27 ; i++) c[i] = 0;
    
    for(int i = 0; i < size ; i++){
        res++;
        if(index >= arr[i].length()){
            c[26]++;
        }
        else c[26 -  arr[i][index] + 'A' - 1]++;
    }
    
    for(int i = 1; i < 27 ; i++){
        res++;
        c[i] = c[i-1] + c[i];
    }
    
    
    for(int i = size-1; i >  -1  ; i--){
        res++;
        if(index >= arr[i].length()){
            b[c[26] - 1 ] = arr[i];
            c[26]--;
            continue;
        }
        else{
            b[c[ 26 - arr[i][index] + 'A' - 1  ] - 1 ] = arr[i];
            c[ 26 - arr[i][index] + 'A' - 1]--;   
        }
    }
    for(int i = 0; i <  size ; i++){
        res++;
        arr[i] = b[i];
    }
}


int radix_string_sort(std::string* arr, int size, bool ascending){
    int max_len = 0;
    int res = 0;
    for(int i = 0; i < size ; i++){
        if( arr[i].length() > max_len ) max_len = arr[i].length();
    }
    for(int i = max_len-1 ; i > -1 ; i--){
        if(ascending) cs(arr,size,i,res);
        else cs1(arr,size,i,res);
        
    }
    
    return res;
}


int main(){
    string arr0[5] = {"OGUZ", "UMAY", "ASLI", "EMRE", "EREN"};
    string arr1[6] = {"PAPATYA", "LALE", "MENEKSE", "AKSAMSEFASI", "BEGONVIL", "KARANFIL"};
    int x = radix_string_sort(arr0,5,true);
    cout << "result: " << x << endl;
    return 0;
}