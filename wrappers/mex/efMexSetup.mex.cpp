////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    MEX source wrapper of efMexSetup().                       ///
///   --------------------------------------------------------   ///
///   This function is unique to Matlab/Octave interface.        ///
///   It must be called prior to using the mex functions         ///
///   of EyeFace SDK, once per process. Using parfor based       ///
///   on parallel processes, it therefore must be called in      ///
///   each parfor body.                                          ///
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

#define MODULE_DIR_PNT              prhs[0]
#define SUPPORT_LIBS_PNT            prhs[1]

#define EYEFACE_FUNCTIONS_PNT       plhs[0]

EfFunctions global_functions;

// Explicit linking of API functions.
static EfFunctions efExplicitLinking(std::string lib_dir)
{
    EfFunctions func_state;

    memset(&func_state, 0, sizeof(EfFunctions));

    std::string eyeface_lib_path = lib_dir + std::string(EYEFACE_MEX_MODULE_NAME);

    // open dynamic library
    if (!ER_OPEN_SHLIB(func_state.handle, eyeface_lib_path.c_str()))
    {
        mexErrMsgTxt("Function ER_OPEN_SHLIB() failed. Missing openblas? Missing license?");
    }

    // Standard API explicit linkage
    if (!ER_LOAD_SHFCN(func_state.fcnErImageAllocate, fcn_erImageAllocate, func_state.handle, "erImageAllocate"))
    {
        mexErrMsgTxt("Explicit linking of erImageAllocate() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnErImageFree, fcn_erImageFree, func_state.handle, "erImageFree"))
    {
        mexErrMsgTxt("Explicit linking of erImageFree() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfInitEyeFace, fcn_efInitEyeFace, func_state.handle, "efInitEyeFace"))
    {
        mexErrMsgTxt("Explicit linking of efInitEyeFace() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfShutdownEyeFace, fcn_efShutdownEyeFace, func_state.handle, "efShutdownEyeFace"))
    {
        mexErrMsgTxt("Explicit linking of efShutdownEyeFace() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfResetEyeFace, fcn_efResetEyeFace, func_state.handle, "efResetEyeFace"))
    {
        mexErrMsgTxt("Explicit linking of efResetEyeFace() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfFreeEyeFace, fcn_efFreeEyeFace, func_state.handle, "efFreeEyeFace"))
    {
        mexErrMsgTxt("Explicit linking of efFreeEyeFace() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfGetLibraryVersion, fcn_efGetLibraryVersion, func_state.handle, "efGetLibraryVersion"))
    {
        mexErrMsgTxt("Explicit linking of efGetLibraryVersion() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfMain, fcn_efMain, func_state.handle, "efMain"))
    {
        mexErrMsgTxt("Explicit linking of efMain() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfGetTrackInfo, fcn_efGetTrackInfo, func_state.handle, "efGetTrackInfo"))
    {
        mexErrMsgTxt("Explicit linking of efGetTrackInfo() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfFreeTrackInfo, fcn_efFreeTrackInfo, func_state.handle, "efFreeTrackInfo"))
    {
        mexErrMsgTxt("Explicit linking of efFreeTrackInfo() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfLogToServerGetConnectionStatus, fcn_efLogToServerGetConnectionStatus, func_state.handle, "efLogToServerGetConnectionStatus"))
    {
        mexErrMsgTxt("Explicit linking of efLogToServerGetConnectionStatus() failed.");
    }

    // Expert API explicit linkage
    if (!ER_LOAD_SHFCN(func_state.fcnEfRunFaceDetector, fcn_efRunFaceDetector, func_state.handle, "efRunFaceDetector"))
    {
        mexErrMsgTxt("Explicit linking of efRunFaceDetector() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfFreeDetections, fcn_efFreeDetections, func_state.handle, "efFreeDetections"))
    {
        mexErrMsgTxt("Explicit linking of efFreeDetections() failed.");
    }
    
    if (!ER_LOAD_SHFCN(func_state.fcnEfUpdateTracker, fcn_efUpdateTracker, func_state.handle, "efUpdateTracker"))
    {
        mexErrMsgTxt("Explicit linking of efUpdateTracker() failed.");
    }
    
    if (!ER_LOAD_SHFCN(func_state.fcnEfRunFaceLandmark, fcn_efRunFaceLandmark, func_state.handle, "efRunFaceLandmark"))
    {
        mexErrMsgTxt("Explicit linking of efRunFaceLandmark() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfFreeLandmarks, fcn_efFreeLandmarks, func_state.handle, "efFreeLandmarks"))
    {
        mexErrMsgTxt("Explicit linking of efFreeLandmarks() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfRecognizeFaceAttributes, fcn_efRecognizeFaceAttributes, func_state.handle, "efRecognizeFaceAttributes"))
    {
        mexErrMsgTxt("Explicit linking of efRecognizeFaceAttributes() failed.");
    }
    
    if (!ER_LOAD_SHFCN(func_state.fcnEfFreeAttributes, fcn_efFreeAttributes, func_state.handle, "efFreeAttributes"))
    {
        mexErrMsgTxt("Explicit linking of efFreeAttributes() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfLogToFileWriteTrackInfo, fcn_efLogToFileWriteTrackInfo, func_state.handle, "efLogToFileWriteTrackInfo"))
    {
        mexErrMsgTxt("Explicit linking of efLogToFileWriteTrackInfo() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfLogToServerSendPing, fcn_efLogToServerSendPing, func_state.handle, "efLogToServerSendPing"))
    {
        mexErrMsgTxt("Explicit linking of efLogToServerSendPing() failed.");
    }

    if (!ER_LOAD_SHFCN(func_state.fcnEfLogToServerSendTrackInfo, fcn_efLogToServerSendTrackInfo, func_state.handle, "efLogToServerSendTrackInfo"))
    {
        mexErrMsgTxt("Explicit linking of efLogToServerSendTrackInfo() failed.");
    }

    return func_state;
}

static void efLoadSupportLibrary(std::string path)
{
    shlib_hnd blas_handle;
    if (!ER_OPEN_SHLIB(blas_handle, path.c_str()))
    {
        mexErrMsgTxt("Function EYE_OPEN_SHLIB() failed. Failed to load support library.");
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // check number of arguments
    if (nrhs < 1 || nrhs > 2 || nlhs != 1)
    {
        mexErrMsgTxt("Unsupported number of arguments!\n[eyeface_functions] = efMexSetup(module_dir, support_libs[optional]);\n");
    }
    
    // take care of support libs
    if (mxIsCell(SUPPORT_LIBS_PNT))
    {
        mwSize num_dims = mxGetNumberOfDimensions(SUPPORT_LIBS_PNT);
        
        if (num_dims < 0 || num_dims > 2)
        {
            mexErrMsgTxt("Unsupported argument type for 'support_libs'. Input a character array or cell array of character arrays.\n");
        }
        
        size_t num_elements = mxGetN(SUPPORT_LIBS_PNT) * mxGetM(SUPPORT_LIBS_PNT);
        
        for (size_t i = 0; i < num_elements; i++)
        {
            mxArray* single_string = mxGetCell(SUPPORT_LIBS_PNT, (int)i);
            
            if (mxIsChar(single_string))
            {
                char * support_lib_path  = mxArrayToString(single_string);
                efLoadSupportLibrary(support_lib_path);
                mxFree(support_lib_path);
            }
            else
            {
                mexErrMsgTxt("Unsupported argument type for 'support_libs'. Input a character array or cell array of character arrays.\n");
            }
        }
    }
    else
    {
        if (mxIsChar(SUPPORT_LIBS_PNT))
        {
            char * support_lib_path  = mxArrayToString(SUPPORT_LIBS_PNT);
            efLoadSupportLibrary(support_lib_path);
            mxFree(support_lib_path);
        }
        else
        {
            mexErrMsgTxt("Unsupported argument type for 'support_libs'. Input a character array or cell array of character arrays.\n");
        }
    }
    
    // explicitly link EyeFace functions
    /*EfFunctions * output = NULL;
    
    try
    {
         output = new EfFunctions();
    }
    catch (std::bad_alloc)
    {
        mexErrMsgTxt("Failed to allocate memory for EyeFace SDK functions.\n");
    }*/
    
    // get paths
    char * module_dir  = mxArrayToString(MODULE_DIR_PNT);
    
    if (!module_dir)
    {
        mexErrMsgTxt("Failed to convert the input string parameters.\n");
    }       
    
    std::string eyeface_lib_path = std::string(module_dir) + "/lib/";
    
    EfFunctions * global_functions_ptr = &global_functions;
    *global_functions_ptr = efExplicitLinking(eyeface_lib_path);
    
    // Put pointer to EyeFace functions to Matlab workspace
    int dims_state[] = { sizeof(void*) };
    EYEFACE_FUNCTIONS_PNT = mxCreateNumericArray(1, dims_state, mxUINT8_CLASS, mxREAL);
    memcpy(mxGetPr(EYEFACE_FUNCTIONS_PNT), &global_functions_ptr, sizeof(void*));
    
    mxFree(module_dir);
}
