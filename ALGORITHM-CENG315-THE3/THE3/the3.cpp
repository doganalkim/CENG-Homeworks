#include "the3.h"
//#include <iostream>
// do not add extra libraries, but you can define helper functions below.
std::string a, b, res;
int lena , lenb, counter = 0, matchp, gapp, mismatchp;
std::vector<std::vector<int>> dp;

int rec(int i, int j,std::string &str){
    //std:: cout << "i:" << i << " j:" << j <<  " call_count:" << call_count   << std::endl;
    counter++;
    if(counter >= 1000000 ){
        res = "STACK LIMIT REACHED";
        return INT_MIN;
    }
    if(i < 0 && j < 0){
        str = "";
        return 0;
    }
    if(i < 0){
        for(int k = 0; k <= j ; k++) str += "_";
        return (j+1)*-gapp;
    }
    if(j < 0){
        for(int k = 0; k <= i ; k++) str += ".";
        return (i+1)*-gapp;
    }
    std::string var1 = "", var2 = "", var3 = "";;
    int takeboth = INT_MIN,takefirst = INT_MIN, takesecond = INT_MIN;
    int m = (a[i] == b[j] ? matchp : - mismatchp );
    takeboth = rec( i-1, j-1 ,var1) + m;
    takefirst = rec( i-1, j,  var2 ) - gapp;
    takesecond = rec( i, j-1 ,var3) - gapp;
    if(counter >= 1000000 ){
        res = "STACK LIMIT REACHED";
        return INT_MIN;
    }
    if( takeboth >= takefirst && takeboth >= takesecond){
        if(a[i] == b[j]){
            var1 += a[i];
        }
        else{
            var1 += '!';
        }
        str = var1;
        return takeboth;
    }
    else if(takefirst >= takeboth && takefirst >= takesecond){
        var2 += '.';
        str = var2;
        return takefirst;
    }
    else if (takesecond >= takeboth && takesecond >= takefirst){
        var3 += '_';
        str = var3;
        return takesecond;
    }
    return INT_MIN;
}

/* 
PART 1
you are expected to call recursive_alignment (as the name suggests) recursively to find an alignment. 
initial call_count value given to you will be 0.
you should check if call_count >= 1000000, if so, set possible_alignment string to "STACK LIMIT REACHED", return INT_MIN (or anything - it will not be checked).
*/
int recursive_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment, int call_count){
    int highest_alignment_score;
    matchp = match; mismatchp = mismatch; gapp = gap;
    lena = sequence_A.length();  lenb = sequence_B.length();
    //std::cout << lena << std::endl;
    a = sequence_A; b = sequence_B;
    int cc = 0;
    highest_alignment_score = rec(lena - 1 ,lenb - 1,possible_alignment);
    if(res == "STACK LIMIT REACHED") possible_alignment = res;
    return highest_alignment_score;    
}
/* 
PART 2
you are expected to create a dynamic programming table to find the highest alignment score. 
then you will need to reconstruct a possible alignment string from the table.
*/
int dp_table_alignment(std::string sequence_A, std::string sequence_B, int gap, int mismatch, int match, std::string &possible_alignment){
    int highest_alignment_score;
    a = sequence_A; b = sequence_B;
    lena = sequence_A.length();  lenb = sequence_B.length();
    dp.resize(lena,std::vector<int>(lenb,INT_MIN));
    //for(int i = 0; i < lena ; i++) for(int j  = 0; j < lenb ; j++) dp[i][j] = INT_MIN;
    if(a[0] == b[0]) dp[0][0] = match;
    else dp[0][0] = (-2*gap > -mismatch ? -2*gap : -mismatch);
    for(int i = 0; i < lena ; i++){
        for(int j = 0; j < lenb ; j++){
            if(!i && !j) continue;
            if(!i){
                if(a[i] == b[j]){
                    dp[i][j] = -gap*(j) + match;
                }
                else dp[i][j] = dp[i][j-1] - gap;
                continue;
            }
            if(!j){
                if(a[i] == b[j]){
                    dp[i][j] = -gap*(i) + match;
                }
                else dp[i][j] = dp[i-1][j] - gap;
                continue;
            }
            int m = (a[i] == b[j] ? match : -mismatch);
            int first = dp[i-1][j-1] + m;
            int second = dp[i-1][j] - gap;
            int third = dp[i][j-1] - gap;
            if(first >= second && first >= third) dp[i][j] = first;
            else if(second >= first && second >= third) dp[i][j] = second;
            else if(third >= second && third >= first) dp[i][j] = third;
        }
    }
    /*
    cout << endl ;
    for(int i = 0; i < lena ; i++){
        for(int j = 0; j < lenb ; j++){
            cout << dp[i][j] << " ";
        }
        cout << endl;
    }*/
    int i = lena-1, j = lenb-1;
    int val = 0;
    std::string s = "";
    bool var = false;
    while( i > 0 && j > 0){
        //std::cout << "s:" << s << " dp[i][j]=" << dp[i][j] << " dp[i-1][j]=" << dp[i-1][j] << " dp[i][j-1]=" << dp[i][j-1] << " dp[i-1][j-1]=" << dp[i-1][j-1] << std::endl;
        if(dp[i-1][j-1] >= dp[i-1][j] && dp[i-1][j-1] >= dp[i][j-1] && dp[i][j] == dp[i-1][j-1] + match ){
            s = a[i] + s;
            i--; j--;
        }
        else if(dp[i-1][j] >= dp[i-1][j-1] && dp[i-1][j] >= dp[i][j-1] ){
            s = '.' + s;
            i--;
        }
        else if(dp[i][j-1] >= dp[i-1][j-1] && dp[i][j-1] >= dp[i-1][j]  ) {
            s = '_' + s;
            j--;
        }
        else if(dp[i-1][j-1] >= dp[i-1][j] && dp[i-1][j-1] >= dp[i][j-1] && dp[i][j] == dp[i-1][j-1] - mismatch){
            s = '!' + s;
            i--; j--;
        }
        else{
            //cout <<"dp[i][j]:" << dp[i][j]  <<  " dp[i-1][j-1]:" << dp[i-1][j-1] << " dp[i][j-1]:" << dp[i][j-1] <<  " dp[i-1][j]:" << dp[i-1][j] << endl;
            break;
            /*
            s = a;
            for(int m = 0; m < lenb-lena; m++) s = '_' + s;
            break;*/
        }
    }
    //std::cout << "i:" << i << " j:" << j <<  " s:" << s <<  std::endl;
    if(!i){
        while(j >= 0){
            if(!var && a[0] == b[j]){
                var = true;
                 s = b[j] + s;
            }
            else s = '_' + s;
            j--;
        }
    }
    else if(!j){
        var = false;
        while(i >= 0){
            if(!var && a[i] == b[0]){
                var = true;
                 s = a[i] + s;
            }
            else s = '.' + s;
            i--;
        }
    }
    /*
    cout << endl ;
    for(int i = 0; i < lena ; i++){
        for(int j = 0; j < lenb ; j++){
            cout << dp[i][j] << " ";
        }
        cout << endl;
    }*/
    //cout << "s:" << s << endl;
    highest_alignment_score = dp[lena-1][lenb-1];
    possible_alignment = s;
    return highest_alignment_score;    
}
