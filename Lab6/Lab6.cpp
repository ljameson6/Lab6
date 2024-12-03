/**********************************************************************
 * FILE: mpi_pi_reduce.c
 * OTHER FILES: dboard.c
 * DESCRIPTION:
 *   MPI pi Calculation Example - C Version
 *   Collective Communication example:
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.  All processes contribute to the calculation, with the
 *   master averaging the values for pi. This version uses mpc_reduce to
 *   collect results
 * AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95)
 * LAST REVISED: 06/13/13 Blaise Barney
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <math.h>

void srandom(unsigned seed);
double xSquared(int samples);

#define DARTS 50000     /* number of throws at dartboard */
#define ROUNDS 10      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

int main(int argc, char* argv[])
{
    //Parse input args
    std::map<std::string, std::string> mp;
    mp["-P"] = "1";
    mp["-N"] = "1000000";

    for (int i = 0; i < argc - 1; i++) {
        if (mp.find(argv[i]) != mp.end()) {
            mp[argv[i]] = argv[i + 1];
        }
    }

    int P = abs(std::stoi(mp["-P"]));
    int N = abs(std::stoi(mp["-N"]));

    double	homesum,         /* value of pi calculated by current task */
        totsum,	        /* sum of tasks' pi values */
        pi,	            /* average of pi after "darts" is thrown */
        avesum;	        /* average pi value for all iterations */

    int	taskid,	        /* task ID - also used as seed number */
        numtasks,       /* number of tasks */
        rc,             /* return code */
        i;


    MPI_Status status;

    MPI_Init(&argc, &argv);

    /* Obtain number of tasks and task ID */
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    int samplesPerTask = N / numtasks;

    printf("MPI task %d has started...\n", taskid);

    /* Set seed for random number generator equal to task ID */
    srandom(taskid);


    avesum = 0;

    if (taskid == MASTER) {
        homesum = xSquared(samplesPerTask + (N - samplesPerTask * numtasks));
    }
    else {
        homesum = xSquared(samplesPerTask);
    }

    rc = MPI_Reduce(&homesum, &totsum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    if (taskid == MASTER) {
        printf("The estimate for integral %d is %g\nBye!\n", P, totsum / N);
    }

    MPI_Finalize();
    return 0;
}

double xSquared(int samples)
{
    long random(void);
    double sum, x;
    int score, n;
    double intpart;

    sum = 0;
    /* "throw darts at board" */
    for (n = 0; n < samples; n++) {
        /* generate random numbers for x and y coordinates */
        x = (double)random();
        x = modf(x, &intpart);
        sum += x * x;
    }
    return(sum);
}


