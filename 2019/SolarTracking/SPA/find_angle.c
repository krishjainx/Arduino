#include <stdio.h>
#include "spa.h"

int main (int argc, char *argv[])
{
    spa_data spa;
    int result;
    float min, sec;

    spa.year          = 2019;
    spa.month         = 6;
    spa.day           = 4;
    spa.hour          = 0;
    spa.minute        = 0;
    spa.second        = 0;
    spa.timezone      = -4.0;
    spa.delta_ut1     = 0;
    spa.delta_t       = 67;
    spa.longitude     = -83.8088;
    spa.latitude      = 39.9242;
    spa.elevation     = 1000;
    spa.pressure      = 820;
    spa.temperature   = 32;
    spa.slope         = 0;
    spa.azm_rotation  = 0; // 0 is north, 180 is south
    spa.atmos_refract = 0.5667;
    spa.function      = SPA_ALL;

    // Find the best slope for the solar panel
    //float bestSlope = 0;
    //float bestAngleOfIncidence = 180; //worst is opposite direction

    //FILE *output;
    //output = fopen("angles.txt","w+");

    float bestSlope = 0;
    float bestAngleOfIncidence = 180;

    //Iterate through all the slopes
    float degreeAccuracy = 1.0f;
    float degreeStart = -90.0f;
    float degreeEnd = 90.0f;
    for (float slope = degreeStart; slope <= degreeEnd; slope += degreeAccuracy){
        printf("Calculating for slope %f...",slope);

        spa.slope = slope;
        //Iterate through all the times of the day.
        //We record a curve of angle of incidence over the whole day for each angle.
        for (int hour = 0; hour < 24; hour++){
            for (int minute = 0; minute < 60; minute++){
                spa.hour = hour;
                spa.minute = minute;
                result = spa_calculate(&spa);
                if (result == 0){
                    //fprintf(output,"%f,",spa.incidence);
                    if (bestAngleOfIncidence > spa.incidence){
                        bestAngleOfIncidence = spa.incidence;
                        bestSlope = slope;
                    }
                } else {
                    printf("SPA Error Code: %d\n", result);
                }
            }
        }
        //fprintf(output,"0\n");
        printf("Done.\n");

        //At each slope and time, find the best angle of incidence at midday
        //result = spa_calculate(&spa);
        //if (result == 0){
            //if (spa.incidence < bestAngleOfIncidence){
            //    bestAngleOfIncidence = spa.incidence;
            //    bestSlope = slope;
        //    }
        //    printf("incidence = %f.\n",spa.incidence);
        //} else {
        //    printf("SPA Error Code: %d\n", result);
        //}
    }
    printf("\nBest slope: %f\n",bestSlope);
    printf("Best angle of incidence (At sun peak): %f\n\n",bestAngleOfIncidence);


    //fclose(output);
    //printf("Best slope was %f with angle of incidence of %f!\n",bestSlope,bestAngleOfIncidence);
    return 0;
}

/////////////////////////////////////////////
// The output of this program should be:
//
//Julian Day:    2452930.312847
//L:             2.401826e+01 degrees
//B:             -1.011219e-04 degrees
//R:             0.996542 AU
//H:             11.105902 degrees
//Delta Psi:     -3.998404e-03 degrees
//Delta Epsilon: 1.666568e-03 degrees
//Epsilon:       23.440465 degrees
//Zenith:        50.111622 degrees
//Azimuth:        194.340241 degrees
//Incidence:     25.187000 degrees
//Sunrise:       06:12:43 Local Time
//Sunset:        17:20:19 Local Time
//
/////////////////////////////////////////////
