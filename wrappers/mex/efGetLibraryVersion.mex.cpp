////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efGetLibraryVersion().              ///
///   --------------------------------------------------------   ///
///                                                              ///
///    Eyedea Recognition, Ltd. (C) 2013-2017                    ///
///                                                              ///
///    Contact:                                                  ///
///               web: http://www.eyedea.cz                      ///
///             email: info@eyedea.cz                            ///
///                                                              ///
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#include <string.h>
#include <mex.h>

#include "efEyeFace.mex.h"

#define VERSION_PNT             plhs[0]

#define EYEFACE_FUNCTIONS_PNT   prhs[0]

void mexFunction (int nlhs, mxArray * plhs[],
    int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs != 1 || nlhs != 1)
    {
        mexErrMsgTxt("Unsupported number of arguments!\n[version] = efGetLibraryVersion(eyeface_functions);\n");
    }
    
    // copy the memory address of EyeFace functions
    EfFunctions * eyeface_functions = NULL;
    double * input_data = mxGetPr(EYEFACE_FUNCTIONS_PNT);
    memcpy(&eyeface_functions, input_data, sizeof(void*));

    if (!eyeface_functions)
    {
        mexErrMsgTxt("EyeFace SDK functions pointer NULL!\n");
    }

    int version = eyeface_functions->fcnEfGetLibraryVersion();
    
    VERSION_PNT = mxCreateDoubleScalar(version);
}
