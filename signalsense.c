
#include <stdio.h>

#define ROADS 4
#define CYCLES 5

char *roadNames[ROADS] = {
    "North", "South", "East", "West"
};

#define TOTAL_GREEN 80
#define FIXED_GREEN 20
#define SERVICE_RATE 0.5

// Structure to store simulation results
typedef struct
{
    int totalServed;
    int totalRemaining;
    int totalArrivals;

    int served[ROADS];
    int remaining[ROADS];

    double averageWaiting[ROADS];
    double overallWaiting;

} SimulationResult;


// Calculate adaptive green timings
void calculateAdaptiveGreen(int queue[], double green[])
{
    int totalQueue = 0;

    for (int i = 0; i < ROADS; i++)
        totalQueue += queue[i];

    if (totalQueue == 0)
    {
        for (int i = 0; i < ROADS; i++)
            green[i] = FIXED_GREEN;
    }
    else
    {
        for (int i = 0; i < ROADS; i++)
        {
            green[i] = (double)queue[i] /
                       totalQueue * TOTAL_GREEN;
        }
    }
}


// Simulate traffic signals
SimulationResult simulate(int arrivals[], int adaptive)
{
    SimulationResult result = {0};

    int queue[ROADS] = {0};
    double green[ROADS];
    double waitingArea[ROADS] = {0};

    printf("\n====================================\n");

    if (adaptive)
        printf(" ADAPTIVE TRAFFIC SIGNAL SIMULATION\n");
    else
        printf(" FIXED-TIME TRAFFIC SIGNAL SIMULATION\n");

    printf("====================================\n");

    for (int cycle = 1; cycle <= CYCLES; cycle++)
    {
        // Add newly arriving vehicles
        for (int i = 0; i < ROADS; i++)
        {
            queue[i] += arrivals[i];
            result.totalArrivals += arrivals[i];
        }

        // Calculate green time
        if (adaptive)
        {
            calculateAdaptiveGreen(queue, green);
        }
        else
        {
            for (int i = 0; i < ROADS; i++)
                green[i] = FIXED_GREEN;
        }

        printf("\nCycle %d\n", cycle);
        printf("------------------------------------\n");

        printf("%-10s %-10s %-10s %-10s\n",
               "Road", "Green(s)", "Served", "Queue");

        for (int i = 0; i < ROADS; i++)
        {
            int queueBefore = queue[i];

            // Calculate service capacity
            int capacity =
                (int)(green[i] * SERVICE_RATE);

            // Vehicles served cannot exceed queue
            int served = (capacity < queue[i])
                         ? capacity : queue[i];

            queue[i] -= served;

            result.served[i] += served;
            result.totalServed += served;

            // Estimate accumulated waiting time
            waitingArea[i] +=
                ((queueBefore + queue[i]) / 2.0)
                * TOTAL_GREEN;

            printf("%-10s %-10.2f %-10d %-10d\n",
                   roadNames[i],
                   green[i],
                   served,
                   queue[i]);
        }
    }

    // Calculate final statistics
    for (int i = 0; i < ROADS; i++)
    {
        result.remaining[i] = queue[i];

        result.totalRemaining += queue[i];

        int roadArrivals = arrivals[i] * CYCLES;

        if (roadArrivals > 0)
        {
            result.averageWaiting[i] =
                waitingArea[i] / roadArrivals;
        }

        result.overallWaiting += waitingArea[i];
    }

    if (result.totalArrivals > 0)
    {
        result.overallWaiting /=
            result.totalArrivals;
    }

    printf("\n========== SIMULATION SUMMARY ==========\n");

    printf("Total vehicles arrived : %d\n",
           result.totalArrivals);

    printf("Total vehicles served  : %d\n",
           result.totalServed);

    printf("Vehicles remaining     : %d\n",
           result.totalRemaining);

    printf("Estimated average waiting time: %.2f seconds\n",
           result.overallWaiting);

    return result;
}


// Display queue statistics
// FIX: arrivals[] is passed as a parameter
void displayStatistics(int arrivals[],
                       SimulationResult result,
                       char title[])
{
    printf("\n========== %s ==========\n", title);

    printf("%-10s %-12s %-15s %-15s\n",
           "Road", "Arrivals", "Final Queue", "Avg Wait(s)");

    for (int i = 0; i < ROADS; i++)
    {
        printf("%-10s %-12d %-15d %-15.2f\n",
               roadNames[i],
               arrivals[i] * CYCLES,
               result.remaining[i],
               result.averageWaiting[i]);
    }
}


// Compare fixed and adaptive results
void compareResults(SimulationResult fixed,
                    SimulationResult adaptive)
{
    printf("\n===============================================\n");
    printf("        FIXED VS ADAPTIVE COMPARISON\n");
    printf("===============================================\n");

    printf("%-25s %-12s %-12s\n",
           "Parameter", "Fixed", "Adaptive");

    printf("------------------------------------------------\n");

    printf("%-25s %-12d %-12d\n",
           "Vehicles Served",
           fixed.totalServed,
           adaptive.totalServed);

    printf("%-25s %-12d %-12d\n",
           "Remaining Queue",
           fixed.totalRemaining,
           adaptive.totalRemaining);

    printf("%-25s %-12.2f %-12.2f\n",
           "Average Wait (sec)",
           fixed.overallWaiting,
           adaptive.overallWaiting);

    printf("===============================================\n");

    printf("\nPERFORMANCE ANALYSIS\n");

    if (adaptive.overallWaiting < fixed.overallWaiting)
    {
        printf("Adaptive system has lower estimated waiting time.\n");
    }
    else if (adaptive.overallWaiting > fixed.overallWaiting)
    {
        printf("Fixed system has lower estimated waiting time.\n");
    }
    else
    {
        printf("Both systems have equal estimated waiting time.\n");
    }

    if (adaptive.totalServed > fixed.totalServed)
    {
        printf("Adaptive system served more vehicles.\n");
    }
    else if (adaptive.totalServed < fixed.totalServed)
    {
        printf("Fixed system served more vehicles.\n");
    }
    else
    {
        printf("Both systems served the same number of vehicles.\n");
    }
}


// Main function
int main()
{
    int arrivals[ROADS];

    SimulationResult fixedResult;
    SimulationResult adaptiveResult;

    printf("====================================\n");
    printf("       SIGNALSENSE PROJECT\n");
    printf(" Adaptive Traffic Signal Simulation\n");
    printf("====================================\n");

    printf("\nEnter vehicle arrivals per cycle:\n");

    for (int i = 0; i < ROADS; i++)
    {
        printf("%s Road: ", roadNames[i]);

        if (scanf("%d", &arrivals[i]) != 1 ||
            arrivals[i] < 0)
        {
            printf("Invalid input!\n");
            return 1;
        }
    }

    // Run fixed-time simulation
    fixedResult = simulate(arrivals, 0);

    // Run adaptive simulation
    adaptiveResult = simulate(arrivals, 1);

    // Display statistics
    displayStatistics(arrivals, fixedResult,
                      "FIXED-TIME STATISTICS");

    displayStatistics(arrivals, adaptiveResult,
                      "ADAPTIVE STATISTICS");

    // Compare both simulations
    compareResults(fixedResult, adaptiveResult);

    printf("\n====================================\n");
    printf("       SIMULATION COMPLETED\n");
    printf("====================================\n");

    return 0;
}
