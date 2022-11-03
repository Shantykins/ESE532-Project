#include <iostream>

//
// DO NOT COMPILE. WILL NOT WORK.
//

int main()
{
    //
    // Read Data from Encoder.cpp
    //


    //
    // RUN CDC
    //
    cdc();

    //
    // SHA table
    //
    build_SHA();

    //
    // De-Dup
    //

    run_DeDup();

    run_LZW();



    return 0;
}