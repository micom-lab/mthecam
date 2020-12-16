// MTools1.cpp
// some tools to use
// OM 20200301 - V0.1

#include "MTools1.h"

// reduce precision of an integer
int MTools_redprec(int val, int prec)     // red. precision (1234, 2) -> 1200 / (-1234, 2) -> -1300
{    
    prec = pow(10, prec);                       // 2 -> 100 for xx00     
    return round( (float) val/prec) * prec;     // cut digits    
}
