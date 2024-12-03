#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <math.h>

void srandom(unsigned seed);
double xSquared(int samples);

#define MASTER 0        /* task ID of master task */

int main(int argc, char* argv[])
{
    printf("1\n");
    //MPI_Status status;
    printf("2\n");
    MPI_Init(NULL, NULL);
    printf("3\n");

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


