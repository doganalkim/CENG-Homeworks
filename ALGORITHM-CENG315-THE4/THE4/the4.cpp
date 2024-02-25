#include "the4.h"
//#include <iostream>
// do not add extra libraries here


std::vector<std::vector<int>> dp;
//int x, y;

int max(int a,int b){
    if(a >= b) return a;
    return b;
}

int helper(int i, int j, bool** arr){ // MEMOIZATION
    //std::cout << "i:" << i << " j:" << j << std::endl;
    if(!i || !j) return 0;
    if(arr[i][j]) return dp[i][j] = i*j;
    if(dp[i][j] != -1) return dp[i][j];
    int res = 0;
    for(int k = 1 ; k <= i/2 ; k++  ){
        res = max(res,helper(k,j,arr) + helper(i-k,j,arr));
    }
    for(int k = 1; k <= j/2 ; k++){
        res = max(res,helper(i,k,arr) + helper(i,j-k,arr));
    }
    return dp[i][j] = res;
}

int divide_land(int X, int Y, bool** possible_plots){
    //x = X; y = Y;
    dp.resize(X+1,std::vector<int> (Y+1,-1));
    //int val = helper(X,Y,possible_plots);
    int res,val;
    for(int i = 0; i <= X ; i++){ // iterative
        for(int j = 0; j <= Y ; j++){
            res = 0;
            if(possible_plots[i][j]){
                dp[i][j] = i*j; continue;
            }
            for(int k = 1; k <= i/2; k++){
                res = max(res,dp[k][j] + dp[i-k][j]);
            }
            for(int k = 1; k <= j/2 ; k++){
                res = max(res, dp[i][k] + dp[i][j-k]);
            }
            dp[i][j] = res;
        }
    }
    return X*Y - dp[X][Y];
    return X*Y-val;
    return X*Y;
}
