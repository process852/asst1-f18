#include <algorithm>

// Generate random data
void initRandom(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // random input values
        values[i] = .001f + 2.998f * static_cast<float>(rand()) / RAND_MAX;
    }
}

// Generate data that gives high relative speedup
void initGood(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // Todo: Choose values
        // values[i] = 1.0f;
        values[i] = .001f + 2.998f * static_cast<float>(rand()) / RAND_MAX;
        if (values[i] >= 2.0f)
            values[i] = 2.0f;
        else if (values[i] < 0.5f)
            values[i] = 0.5f;
    }
}

// Generate data that gives low relative speedup
void initBad(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // Todo: Choose values
        // values[i] = 1.0f;
        float p = static_cast<float>(rand()) / RAND_MAX;
        if(p >0.5){
            values[i] = 2.981f + 0.018f * static_cast<float>(rand()) / RAND_MAX;
        }
        else{
            values[i] = 0.001f + 0.098f * static_cast<float>(rand()) / RAND_MAX;
        }
    }
}

