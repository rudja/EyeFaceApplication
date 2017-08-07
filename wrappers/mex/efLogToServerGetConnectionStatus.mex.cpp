////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efLogToServerGetConnectionStatus(). ///
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
#include "efStructIO.mex.h"

#define EYEFACE_STATE_PNT           prhs[0]
#define EYEFACE_FUNCTIONS_PNT       prhs[1]

#define LOGTOSERVER_STATUS_PNT      plhs[0]

void mexFunction (int nlhs, mxArray * plhs[],
    int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs!=2 || nlhs!=1)
        mexErrMsgTxt("Unsupported number of arguments!\n[log_to_server_status] = efLogToServerGetConnectionStatus(eyeface_state, eyeface_functions);\n");

    // copy the memory address of EyeFace state
    void * eyeface_state = NULL;
    double * input_data = mxGetPr(EYEFACE_STATE_PNT);
    memcpy(&eyeface_state, input_data, sizeof(void*));

    if (!eyeface_state)
    {
        mexErrMsgTxt("Pointer NULL, EyeFace not properly initialized!\n");
    }

    // copy the memory address of EyeFace functions
    EfFunctions * eyeface_functions = NULL;
    input_data = mxGetPr(EYEFACE_FUNCTIONS_PNT);
    memcpy(&eyeface_functions, input_data, sizeof(void*));

    if (!eyeface_functions)
    {
        mexErrMsgTxt("EyeFace SDK functions pointer NULL!\n");
    }

    // get log to server status
    EfLogToServerStatus log_to_server_status;
    EfBool ret_val = eyeface_functions->fcnEfLogToServerGetConnectionStatus(&log_to_server_status, eyeface_state);

    if (ret_val != EF_TRUE)
    {
        mexErrMsgTxt("Function efLogToServerGetConnectionStatus() failed.");
    }

    // convert EfLogToServerStatus to Matlab
    LOGTOSERVER_STATUS_PNT = convertEfLogToServerStatusToMatlab(log_to_server_status);
}
