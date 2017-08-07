////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efInitEyeFace().                    ///
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
#include <string>
#include <cstdio>
#include <mex.h>

#include "efEyeFace.mex.h"

#define MODULE_DIRECTORY            prhs[0]
#define INI_DIRECTORY               prhs[1]
#define CONFIG_FILE                 prhs[2]
#define EYEFACE_FUNCTIONS_PNT       prhs[3]

#define EYEFACE_STATE_PNT           plhs[0]


void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // check number of arguments
    if (nrhs != 4 || nlhs != 1)
    {
        mexErrMsgTxt("Unsupported number of arguments!\n[eyeface_state] = efInitEyeFace(eyefacesdk_dir, config_ini_dir, config_ini_filename, eyeface_functions);\n");
    }
    
    // copy the memory address of EyeFace functions
    EfFunctions * eyeface_functions = NULL;
    double * input_data = mxGetPr(EYEFACE_FUNCTIONS_PNT);
    memcpy(&eyeface_functions, input_data, sizeof(void*));
    
    if (!eyeface_functions)
    {
        mexErrMsgTxt("EyeFace SDK functions pointer NULL!\n");
    }
    
    // get paths
    char * module_dir  = mxArrayToString(MODULE_DIRECTORY);
    char * ini_dir     = mxArrayToString(INI_DIRECTORY);
    char * config_file = mxArrayToString(CONFIG_FILE);
    
    if (!module_dir || !ini_dir || !config_file)
    {
        mexErrMsgTxt("Failed to convert the input string parameters.\n");
    }
    
    // initialize EyeFace
    void * eyeface_state = NULL;
    EfBool ret_val = eyeface_functions->fcnEfInitEyeFace(module_dir, ini_dir, config_file, &eyeface_state);
    if (ret_val != EF_TRUE)
    {
        mexErrMsgTxt("Function efInitEyeFace() failed.");
    }
        
    // Put pointer to EyeFace state to Matlab workspace
    int dims_state[] = { sizeof(void*) };
    EYEFACE_STATE_PNT = mxCreateNumericArray(1, dims_state, mxUINT8_CLASS, mxREAL);
    memcpy(mxGetPr(EYEFACE_STATE_PNT), &eyeface_state, sizeof(void*));
    
    mxFree(config_file);
    mxFree(ini_dir);
    mxFree(module_dir);
}
