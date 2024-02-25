#include "the1.h"

//You may write your own helper functions here

//You may write your own helper functions here

int partition(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, int size){
    //if(size < 2) return 0;
	int target = arr[size-1], j = 0, i = 0;
	for(; i < size - 1  ; i++){
		if(arr[i] >= target){
			int tmp = arr[j];
			max_dist = (max_dist >= j - i) ? max_dist: j - i;
			avg_dist = (avg_dist*swap + (i-j)) / (++swap);
			arr[j++] = arr[i];
			arr[i] = tmp;
		}
	}
	if(j < size ){
		arr[size - 1] = arr[j];
		max_dist = (max_dist >= size - 1 - j) ? max_dist: size - 1 -j;
		avg_dist = (avg_dist*swap + size - 1 - j) / (++swap);
		arr[j] = target;
	}
	return j;
}


int  hpartition(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, int size){
    //if(size < 2) return 0;
	int target = arr[(size - 1) / 2], j = size, i = -1;
	while(true){
		do{
			j--;
		}while(j >= 0 && arr[j] < target) ;
		do{
			i++;
		}while(i < size && arr[i] > target);

		if(i  < j && i < size && j>= 0 ){
			max_dist = (max_dist >= (j - i)) ? max_dist: (j - i);
			avg_dist = (avg_dist*swap + (j-i)) / (++swap );
			int tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp; 
		}
		else return j;
	}
	return 0;
}

int quickSort(unsigned short* arr, long& swap, double& avg_dist, double& max_dist, bool hoare, bool median_of_3, int size){
    //Your code here

	

	if(!hoare){
        if( size > 2 && median_of_3){
            int first = arr[0], mid = arr[(size-1)/2], last = arr[size-1];
            if((first < mid && mid < last) || (last < mid && mid < first)){ // mid is the middle
                int tmp = arr[size-1];
                max_dist = (max_dist >= (size - 1 - (size - 1)/2) ) ? max_dist: (size - 1 - (size - 1)/2);
                avg_dist = (avg_dist*swap + (size - 1 - (size - 1 ) / 2 )) / (++swap);
                arr[size - 1] = mid;
                arr[(size-1)/2] = tmp;
            }
            else if((mid < first && first < last) || (last <  first && first <  mid)){ // first is the middle
                int tmp = arr[size-1];
                max_dist = (max_dist >= (size - 1) ) ? max_dist: (size - 1);
                avg_dist = (avg_dist*swap + (size -  1)) / (++swap);
                arr[size - 1] = first;
                arr[0] = tmp;
            }
	    }

		if(size > 1){
			int i =  partition(arr,swap,avg_dist,max_dist,size);
			int f = quickSort(arr,swap,avg_dist, max_dist , hoare, median_of_3, i );  
		    int s = quickSort(arr + i + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - i - 1) ;
	    	return f + s + 1;
		}
	}
	else{
        if( size > 2 && median_of_3){
            int first = arr[0], mid = arr[(size-1)/2], last = arr[size-1];
            if((first < last && last < mid) || (mid < last && last < first)){ // last is the middle
                max_dist = (max_dist >= (size - 1 - (size - 1)/2) ) ? max_dist: (size - 1 - (size - 1)/2);
                avg_dist = (avg_dist*swap + (size - 1 - (size - 1 ) / 2 )) / (++swap);
                arr[(size - 1)/2] = last;
                arr[size-1] = mid;
            }
            else if((mid < first && first < last) || (last <  first && first <  mid)){ // first is the middle
                max_dist = (max_dist >= (size - 1)/2 ) ? max_dist: (size - 1)/2;
                avg_dist = (avg_dist*swap + (size -  1)/2 ) / (++swap);
                arr[(size - 1)/2] = first;
                arr[0] = mid;
            }
	    }
		if(size > 1){
			int i = hpartition(arr,swap,avg_dist,max_dist,size);
			int f = quickSort(arr,swap,avg_dist, max_dist , hoare, median_of_3, i + 1), s = quickSort(arr + i + 1, swap, avg_dist, max_dist, hoare, median_of_3, size - i - 1 ) ;
			return f + s + 1;
		}
	}
    return 1;
}
