#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <math.h>
#include <random>
#include <iostream>

double xSquared(int samples, int taskid);

#define MASTER 0        /* task ID of master task */

int main(int argc, char* argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);
    std::cout << "1" << std::endl;

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

    std::cout << "MPI task " << taskid << " has started..." << std::endl;

    if (taskid == MASTER) {
        homesum = xSquared(samplesPerTask + (N - samplesPerTask * numtasks), taskid);
    }
    else {
        homesum = xSquared(samplesPerTask, taskid);
    }

    rc = MPI_Reduce(&homesum, &totsum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    if (taskid == MASTER) {
        std::cout << "The estimate for integral " << P << " is " << totsum / N << std::endl;
        std::cout << "Bye!" << std::endl;
    }

    MPI_Finalize();
    return 0;
}

double xSquared(int samples, int taskid)
{
    std::default_random_engine gen;
    gen.seed(taskid);
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double sum, x;
    int score, n;
    double intpart;

    sum = 0;
    /* "throw darts at board" */
    for (n = 0; n < samples; n++) {
        /* generate random numbers for x and y coordinates */
        x = dis(gen);
        x = modf(x, &intpart);
        sum += x * x;
    }
    return(sum);
}


