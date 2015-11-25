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

#define MAX_N 500000
#define MAX_M 10000000

long long int houses[MAX_N][3];
long long int xcosts[MAX_N][3];
long long int ycosts[MAX_N][3];

/* Function to be used by qsort().
    It compares the first column of given array. */
int coord_ascending (const void * pa, const void * pb) {
    const long long int (*a)[3] = pa;
    const long long int (*b)[3] = pb;
    if ( (*a)[0] < (*b)[0] ) return -1;
    if ( (*a)[0] > (*b)[0] ) return +1;
    return 0;
}

/* Function to be used by qsort().
    It compares the third column of given array. */
int cost_ascending (const void * pa, const void * pb) {
    const long long int (*a)[3] = pa;
    const long long int (*b)[3] = pb;
    if ( (*a)[2] < (*b)[2] ) return -1;
    if ( (*a)[2] > (*b)[2] ) return +1;
    return 0;
}

/* Binary search for a specific coordinate in given cost array */
long long int findCoord(long long int costs[][3], long long int coord, long long int N) {
    long long int left = 0, right = N-1, ind = (left + right)/2;

    while (coord != costs[ind][0]) {
        if (coord > costs[ind][0]) {
            left = ind + 1;;
         } else {
            right = ind - 1;
        }

        ind = (right + left)/2;
    }

    return costs[ind][2];
}

int main(int argc, char *argv[]) {
    long long int N, L, i, x, y;

    /* Read number of houses: N */
    if(scanf("%lld", &N) != 1) {
        printf("\nError while reading file.\n");
        exit(1);
    }

    /* Read the coordinates of the houses */
    for (i = 0; i < N; i++) {
        scanf("%lld %lld", &x, &y);
        houses[i][0] = x;
        houses[i][1] = y;
        houses[i][2] = 0;

        xcosts[i][0] = x;
        xcosts[i][1] = 0;       

        ycosts[i][0] = y;
        ycosts[i][1] = 0;
    }

    /* Sorting of the xcosts array by the x-coordinates in ascending order */
    qsort(xcosts, N, sizeof(xcosts[0]), coord_ascending);

    /* Sorting of the ycosts array by the y-coordinates in ascending order */
    qsort(ycosts, N, sizeof(ycosts[0]), coord_ascending);

    /* Computation of x-costs and y-costs from left to right (-->) */
    for (i = 1; i < N; i++) {
        xcosts[i][1] = (i * (xcosts[i][0] - xcosts[i-1][0])) + xcosts[i-1][1];

        ycosts[i][1] = (i * (ycosts[i][0] - ycosts[i-1][0])) + ycosts[i-1][1];
    }
    
    /* Computation of x-costs and y-costs from right to left (<--) */
    for (i = N-2; i >= 0; i--) {
        xcosts[i][2] = ((N - 1 - i) * (xcosts[i+1][0] - xcosts[i][0])) + xcosts[i+1][2];    
    
        ycosts[i][2] = ((N - 1 - i) * (ycosts[i+1][0] - ycosts[i][0])) + ycosts[i+1][2];
    }

    /* Computation of total x-costs and y-costs */
    for (i = 0; i < N; i++) {
        xcosts[i][2] = xcosts[i][1] + xcosts[i][2];
        ycosts[i][2] = ycosts[i][1] + ycosts[i][2];
    }

    /* Computation of total cost for every house */
    for (i = 0; i < N; i++) {
        houses[i][2] = findCoord(xcosts, houses[i][0], N) + findCoord(ycosts, houses[i][1], N);
    }

    /* Sorting of the houses array by the cost in ascending order */
    qsort(houses, N, sizeof(houses[2]), cost_ascending);

    printf("%lld\n", houses[0][2]);
    
    return 0;
}
