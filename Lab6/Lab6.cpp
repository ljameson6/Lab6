/*
Author: Lydia Jameson
Class: ECE6122 (A)
Last Date Modified: date
Description:
Estimate the value of two integrals
*/

#include "mpi.h"
#include <stdlib.h>
#include <map>
#include <string>
#include <math.h>
#include <random>
#include <iostream>

double xSquared(int samples, int taskid);
double e(int samples, int taskid);

#define MASTER 0        /* task ID of master task */

int main(int argc, char* argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);

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

    double	homesum, totsum;
    int	taskid, numtasks;

    /* Obtain number of tasks and task ID */
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    int samplesPerTask = N / numtasks;

    if (taskid == MASTER) {
        if (P == 2) {
            homesum = e(samplesPerTask + (N - samplesPerTask * numtasks), taskid);
        } else {
            homesum = xSquared(samplesPerTask + (N - samplesPerTask * numtasks), taskid);
        }
    }
    else {
        if (P == 2) {
            homesum = e(samplesPerTask, taskid);
        }
        else {
            homesum = xSquared(samplesPerTask, taskid);
        }
    }

    MPI_Reduce(&homesum, &totsum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

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

    sum = 0;
    
    for (int n = 0; n < samples; n++) {
        x = dis(gen);
        sum += x * x;
    }
    return(sum);
}

double e(int samples, int taskid)
{
    std::default_random_engine gen;
    gen.seed(taskid);
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double sum, x;

    sum = 0;
    
    for (int n = 0; n < samples; n++) {
        x = dis(gen);
        sum += exp(-1 * x * x);
    }
    return(sum);
}


