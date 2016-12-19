/* PAGERANK - Giacomo Randazzo
 *
 * ToDo: 
 *       -scrivi for in main
 *       -scrivi test migliori
 *
 */

#include <stdio.h>
#include <float.h>
#include <math.h>

#define DIM 6

//define matrix
float links[DIM][DIM] = {
    {0, 1, 1, 0, 1 ,0}, 
    {1, 1, 0, 0, 0, 1}, 
    {0, 1, 0, 0, 0, 0}, 
    {1, 1, 0, 1, 0, 0}, 
    {0, 1, 0, 1, 0, 1}, 
    {1, 1, 1, 0, 1, 1}};

//define initial vector
float initial_vec[DIM] = {1, 1, 1, 1, 1, 1};

//resulting vector
float vec[DIM];

//dumping factor
float d=0.85;

//function declaration
void normalize(int sel);
void dot();
int checkEquality();

int main() {
    //variable declaration
    int i, j, done=0;
    float somma=0;

    //normalize matrix and initial vec
    normalize(0);
    normalize(2);

    //multiply until equals
    //for (i=0;i<DIM;i++) {
    //    vec[i] = initial_vec[i];
    //}

    while (!done) {
        dot();
        normalize(1);
        done = checkEquality();
        printf("Done: %d\n", done);
        if (!done) 
            for (i=0;i<DIM;i++) initial_vec[i]=vec[i];
    

        //print result
        printf("Resulting vector: \n");
        printf("[ ");
        for (i=0;i<DIM;i++) {
            somma+=vec[i];
            if (i<DIM-1) {
                printf("%.7f, ", vec[i]);
            } else printf("%.7f", vec[i]);
        }
        printf(" ]\n\n");
    }

    return 0;
}

//vector matrix product between initial_vec and links,
//results saved in vec
void dot() {
    int i, j;
    float somma=0;
    for (j=0;j<DIM;j++) { //colonna matrice
        for (i=0;i<DIM;i++) {
            somma += initial_vec[i]*links[i][j];
        }
        vec[j] = (1-d) + d*somma; //prendi in considerazione dumping factor
        somma = 0;
    }
}

//normalize rows      sel: [0 initial_vec, 1 vec, 2 links]
void normalize(int sel) {
    float somma=0;
    int i, j;

    switch (sel) {
        case 0:
            for(i=0;i<DIM;i++) {
                    somma += initial_vec[i];
                }
            if (somma!=0) {
                for(i=0;i<DIM;i++) {
                    initial_vec[i] = initial_vec[i]/somma;
                }
            }
            break;
        case 1:
            for(i=0;i<DIM;i++) {
                somma += vec[i];
            }
            if (somma!=0) {
                for(i=0;i<DIM;i++) {
                    vec[i] = vec[i]/somma;
                }
            }
            break;
        case 2:
            for(i=0;i<DIM;i++) {
                for(j=0;j<DIM;j++) {
                    somma += links[i][j];
                }
                if (somma!=0) {
                    for (j=0;j<DIM;j++) {
                        links[i][j] = links[i][j]/somma;
                    }
                    somma = 0;
                }
            }
            break;
        default:
            printf("Incorrect case.");
    }
    somma=0;
}

//check if vectors are similar
int checkEquality() {
    int equals=1, i;

    for (i=0;i<DIM;i++) {
        if(fabs(initial_vec[i]-vec[i]) > FLT_EPSILON)
            equals=0;
    }

    return equals;
}
