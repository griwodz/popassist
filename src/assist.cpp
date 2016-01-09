#include "assist.h"

using namespace std;

__host__
ostream& operator<<( ostream& ostr, const dim3& p )
{
    ostr << "(" << p.x << "," << p.y << "," << p.z << ")";
    return ostr;
}

