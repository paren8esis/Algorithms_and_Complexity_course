/* Copyright 2015 paren8esis*/

/* This program is free software: you can redistribute it and/or modify*/
/*    it under the terms of the GNU General Public License as published by*/
/*    the Free Software Foundation, either version 3 of the License, or*/
/*    (at your option) any later version.*/

/*    This program is distributed in the hope that it will be useful,*/
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the*/
/*    GNU General Public License for more details.*/

/*    You should have received a copy of the GNU General Public License*/
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_N 100000

long long int streams[MAX_N*2][2];

/* Function to be used by qsort().
    It compares the first column of given array. */
int time_ascending (const void * pa, const void * pb) {
    const long long int (*a)[2] = pa;
    const long long int (*b)[2] = pb;
    if ( (*a)[0] < (*b)[0] ) return -1;
    if ( (*a)[0] > (*b)[0] ) return +1;
    return 0;
}

int main(int argc, char *argv[]) {
    long long int N, K, i, s, f, prevTotal, total, num, prev;

    /* Read the first line: N and K */
    if(scanf("%lld %lld", &N, &K) != 2) {
        printf("\nError while reading file.\n");
        exit(1);
    }

    /* Read the rest of the lines: si fi */
    for (i = 0; i < 2*N; i+=2) {
        scanf("%lld %lld", &s, &f);
        streams[i][0] = s;
        streams[i][1] = 0;   // 0 for start of stream
        streams[i+1][0] = f;
        streams[i+1][1] = 1;   // 1 for end of stream
    }

    /* Sorting streams array by the first column in ascending order */
    qsort(streams, 2*N, sizeof(streams[0]), time_ascending);    

    prevTotal = 0;   // Total amount of chocolate poured in previous step
    total = 0;   // Total amount of chocolate poured in current step
    num = 0;   // Number of streams currently pouring into the tank
    prev = 0;   // Previous step

    for (i = 0; i < 2*N; i++) {
        if (streams[i][1] == 0) {  // found start of stream
            prevTotal = total;
            total += ((streams[i][0] - prev)*num) + 1;
            num++;
        } else {  // found end of stream
            prevTotal = total;
            total += ((streams[i][0] - prev)*num);
            num--;
        }
        
        if (total > K)  
            break;

        prev = streams[i][0];

        if (total == K)
            break;
    }

    /* Now i points to the first line of the streams array where 
        chocolate poured >= K */

    /* If the total amount of chocolate poured in step i exceeds 
    the target K, we have to find the exact moment when 
    chocolate poured = K.
    The moment we are looking for is equal to prev (aka previous step) + 
    the time needed for num streams to pour (K - prevTotal) amount of
    chocolate into the tank. */
    if (total > K) {
        if (streams[i][1] == 1) num++;
        else num--;
        prev = (int)ceil((double)prev + (((double)K - (double)prevTotal) / (double)num));
    }

    printf("%lld\n", prev); 
    
    return 0;
}
