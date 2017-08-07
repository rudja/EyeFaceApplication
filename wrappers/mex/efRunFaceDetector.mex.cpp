////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efRunFaceDetector().                ///
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

#define IMAGE                   prhs[0]
#define EYEFACE_STATE_PNT       prhs[1]
#define EYEFACE_FUNCTIONS_PNT   prhs[2]

#define DETECTION_ARRAY_PNT     plhs[0]

void mexFunction (int nlhs, mxArray * plhs[],
                  int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs!=3 || nlhs!=1)
        mexErrMsgTxt("Unsupported number of arguments!\n[detection_array] = efRunFaceDetector(image, eyeface_state, eyeface_functions);\n");
    
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
    
    // run the detection function
    EfDetectionArray detection_array;
    EfBool success = eyeface_functions->fcnEfRunFaceDetector(eyeface_image, &detection_array, eyeface_state);
    
    convertERImageFromMatlabFree(&eyeface_image, eyeface_functions);
    
    if (success != EF_TRUE)
    {
        mexErrMsgTxt("Function efRunFaceDetector() failed.");
    }
    
    // Put detections to Matlab
    DETECTION_ARRAY_PNT = convertEfDetectionArrayToMatlab(detection_array);
    
    eyeface_functions->fcnEfFreeDetections(&detection_array, eyeface_state);

}
