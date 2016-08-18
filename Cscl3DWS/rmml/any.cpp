// See http://www.boost.org/libs/any for Documentation.
#include "mlRMML.h"
#include "any.hpp"
   

any::~any()
{
        MP_DELETE( content);
}

