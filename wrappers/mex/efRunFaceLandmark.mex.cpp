////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efRunFaceLandmark().                ///
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
#define DETECTION_ARRAY_PNT     prhs[1]
#define DETECTIONS_TO_PROC_PNT  prhs[2]
#define EYEFACE_STATE_PNT       prhs[3]
#define EYEFACE_FUNCTIONS_PNT   prhs[4]

#define LANDMARKS_ARRAY_PNT     plhs[0]

void mexFunction (int nlhs, mxArray * plhs[],
                  int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs!=5 || nlhs!=1)
        mexErrMsgTxt("Unsupported number of arguments!\n[facial_landmarks_array] "
                "= efRunFaceLandmark(image, detection_array, detections_to_process, eyeface_state, eyeface_functions);\n");
    
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
    EfDetectionArray detection_array = convertEfDetectionArrayFromMatlab(DETECTION_ARRAY_PNT);
    
    // get detections_to_process
    EfBool * detections_to_process = NULL;
    
    if (!mxIsEmpty(DETECTIONS_TO_PROC_PNT))
    {
        if (!mxIsLogical(DETECTIONS_TO_PROC_PNT) || mxGetNumberOfElements(DETECTIONS_TO_PROC_PNT) != detection_array.num_detections)
        {
            mexErrMsgTxt("Conversion of 'detections_to_procc' failed. Must be Logical array with detection_array.num_detections elements.\n");
        }
        
        detections_to_process = new EfBool[detection_array.num_detections]();
        mxLogical * dtp_matlab = mxGetLogicals(DETECTIONS_TO_PROC_PNT);
        
        for (unsigned int i = 0; i < detection_array.num_detections; i++)
        {
            detections_to_process[i] = dtp_matlab[i] ? EF_TRUE : EF_FALSE;
        }
    }
    
    // run the landmark function
    EfLandmarksArray landmarks_array;
    EfBool success = eyeface_functions->fcnEfRunFaceLandmark(eyeface_image, detection_array, detections_to_process,
            &landmarks_array, eyeface_state);
    
    delete [] detections_to_process;    
    convertEfDetectionArrayFromMatlabFree(detection_array);
    convertERImageFromMatlabFree(&eyeface_image, eyeface_functions);
    
    if (success != EF_TRUE)
    {
        mexErrMsgTxt("Function efRunFaceDetector() failed.");
    }
    
    // Put landmarks to Matlab
    LANDMARKS_ARRAY_PNT = convertEfLandmarksArrayToMatlab(landmarks_array);
    
    eyeface_functions->fcnEfFreeLandmarks(&landmarks_array, eyeface_state);

}
