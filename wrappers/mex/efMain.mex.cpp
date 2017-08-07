////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efMain().                           ///
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
#define BOUNDING_BOX            prhs[1]
#define TIME                    prhs[2]
#define EYEFACE_STATE_PNT       prhs[3]
#define EYEFACE_FUNCTIONS_PNT   prhs[4]

void mexFunction (int nlhs, mxArray * plhs[],
                  int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nlhs != 0 || nrhs != 5)
    {
        mexErrMsgTxt("Unsupported number of arguments!\n[] = efMain(image, bounding_box, frame_time, eyeface_state, eyeface_functions);\n");
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
    
    // get bounding box from Matlab (expects BOUNDING_BOX in 1-based coordinates )
    EfBoundingBox eyeface_bounding_box = convertEfBoundingBoxFromMatlab(BOUNDING_BOX);
    
    // get frame time
    double frame_time = mxGetScalar(TIME);
    
    // run the detection function
    EfBool success = eyeface_functions->fcnEfMain(eyeface_image, &eyeface_bounding_box, frame_time, eyeface_state);
    
    if (success != EF_TRUE)
    {
        mexErrMsgTxt("Function efMain() failed.");
    }
    
    // Tidy-up
    convertERImageFromMatlabFree(&eyeface_image, eyeface_functions);
}
