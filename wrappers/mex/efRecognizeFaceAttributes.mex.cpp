////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efRecognizeFaceAttributes().        ///
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

#define IMAGE                       prhs[0]
#define DETECTION_ARRAY_PNT         prhs[1]
#define LANDMARK_ARRAY_PNT          prhs[2]
#define DETECTIONS_TO_PROC_PNT      prhs[3]
#define REQUEST_FLAG                prhs[4]
#define FRAME_TIME                  prhs[5]
#define PROCESS_SEQUENTIALLY        prhs[6]
#define EYEFACE_STATE_PNT           prhs[7]
#define EYEFACE_FUNCTIONS_PNT       prhs[8]

#define FACE_ATTRIBUTES_ARRAY_PNT   plhs[0]

void mexFunction (int nlhs, mxArray * plhs[],
                  int nrhs, const mxArray * prhs[])
{
    // check number of arguments
    if (nrhs!=9 || nlhs!=1)
        mexErrMsgTxt("Unsupported number of arguments!\n[face_attributes_array] "
                "= efRecognizeFaceAttributes(image, detection_array, facial_landmarks_array, "
                "detections_to_process, request_flag, frame_time, process_sequentially, eyeface_state, eyeface_functions);\n");
    
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
    
    // LANDMARKS OPTIONAL
    EfLandmarksArray facial_landmarks_array_output;
    facial_landmarks_array_output.num_detections = 0;
    facial_landmarks_array_output.landmarks = NULL;
    EfLandmarksArray * facial_landmarks_array = NULL;
    
    if (!mxIsEmpty(LANDMARK_ARRAY_PNT))
    {
        facial_landmarks_array_output = convertEfLandmarksArrayFromMatlab(LANDMARK_ARRAY_PNT);
        facial_landmarks_array = &facial_landmarks_array_output;
    }
    
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
    
    // get the request_flag
    unsigned int request_flag = (unsigned int)mxGetScalar(REQUEST_FLAG); // TODO check uint32
    
    // get frame time from Matlab
    double frame_time = mxGetScalar(FRAME_TIME);
    
    // read process sequentially
    if (!mxIsLogical(PROCESS_SEQUENTIALLY))
    {
        mexErrMsgTxt("Input parameter process_sequentially not logical.");
    }
    mxLogical * proc_seq = mxGetLogicals(PROCESS_SEQUENTIALLY);
    EfBool process_sequentially = proc_seq[0] ? EF_TRUE : EF_FALSE;
    
    // run the face attribute function
    EfFaceAttributesArray face_attributes_array;
    EfBool success = eyeface_functions->fcnEfRecognizeFaceAttributes(eyeface_image, detection_array, facial_landmarks_array,
            detections_to_process, request_flag, frame_time, process_sequentially, &face_attributes_array, eyeface_state);
    
    delete [] detections_to_process;
    convertEfLandmarksArrayFromMatlabFree(facial_landmarks_array_output);
    convertEfDetectionArrayFromMatlabFree(detection_array);
    convertERImageFromMatlabFree(&eyeface_image, eyeface_functions);
    
    if (success == EF_FALSE)
    {
        mexErrMsgTxt("Function efRunFaceDetector() failed.");
    }
    
    // Put face attributes to Matlab
    FACE_ATTRIBUTES_ARRAY_PNT = convertEfFaceAttributesArrayToMatlab(face_attributes_array);
    
    eyeface_functions->fcnEfFreeAttributes(&face_attributes_array, eyeface_state);

}
