////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efUpdateTracker().                  ///
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
#include "efStructIO.mex.h"

#define IMAGE                   prhs[0]
#define DETECTION_ARRAY_PNT     prhs[1]
#define TIME_PNT                prhs[2]
#define EYEFACE_STATE_PNT       prhs[3]
#define EYEFACE_FUNCTIONS_PNT   prhs[4]

void mexFunction (int nlhs, mxArray * plhs[],
                  int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs != 5)
    {
        mexErrMsgTxt("Unsupported number of arguments!\n[] = efUpdateTracker(image, detection_array, time, eyeface_state, eyeface_functions);\n");
    }
    
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
    
    // convert image
    ERImage eyeface_image;
    
    if (!convertERImageFromMatlab(IMAGE, &eyeface_image, eyeface_functions))
    {
        mexErrMsgTxt("Conversion of Matlab image to EfImage failed.\n");
    }
    
    // get detection array
    EfDetectionArray detection_array;
    
    detection_array = convertEfDetectionArrayFromMatlab(DETECTION_ARRAY_PNT);
    
    // get frame time from Matlab
    double frame_time = mxGetScalar(TIME_PNT);
    
    // update tracker state
    EfBool success = eyeface_functions->fcnEfUpdateTracker(eyeface_image, detection_array, frame_time, eyeface_state);

    convertEfDetectionArrayFromMatlabFree(detection_array);
    convertERImageFromMatlabFree(&eyeface_image, eyeface_functions);

    if (success != EF_TRUE)
    {
        mexErrMsgTxt("Function pfUpdateTracker() failed.");
    }
}
