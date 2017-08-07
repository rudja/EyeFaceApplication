////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Header file for MEX wrapper functions.                    ///
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

#ifndef EYEDEA_EYEFACE_EYEFACE_MEX_H
#define EYEDEA_EYEFACE_EYEFACE_MEX_H

#include "EyeFaceExpert.h"

// Multiplatform linking definitions
#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS_)
    // WIN32 Platform 
    #define EYEFACE_MEX_MODULE_NAME "EyeFace.dll"
#else
    // Linux and Mac OS
    #if !defined(__i386__)
        #define EYEFACE_MEX_MODULE_NAME "libeyefacesdk-x86_64.so"
    #else
        #define EYEFACE_MEX_MODULE_NAME "libeyefacesdk-x86_32.so"
    #endif
#endif

// EyeFace function declarations
struct EfFunctions
{
    shlib_hnd handle;
    
    // Standard API
    //  - image handling (use Matlab/Octave im* functions instead)
    fcn_erImageAllocate                     fcnErImageAllocate;
    fcn_erImageFree                         fcnErImageFree;
    
    //  - module setup
    fcn_efInitEyeFace                       fcnEfInitEyeFace;
    fcn_efShutdownEyeFace                   fcnEfShutdownEyeFace;
    fcn_efResetEyeFace                      fcnEfResetEyeFace;
    fcn_efFreeEyeFace                       fcnEfFreeEyeFace;
    fcn_efGetLibraryVersion                 fcnEfGetLibraryVersion;
    
    //  - processing and results
    fcn_efMain                              fcnEfMain;
    fcn_efGetTrackInfo                      fcnEfGetTrackInfo;
    fcn_efFreeTrackInfo                     fcnEfFreeTrackInfo;
    
    //  - log to server status
    fcn_efLogToServerGetConnectionStatus    fcnEfLogToServerGetConnectionStatus;
    
    
    // Expert API
    //  - face detection
    fcn_efRunFaceDetector                   fcnEfRunFaceDetector;
    fcn_efFreeDetections                    fcnEfFreeDetections;
    
    //  - tracking
    fcn_efUpdateTracker                     fcnEfUpdateTracker;
    
    //  - face landmark detection
    fcn_efRunFaceLandmark                   fcnEfRunFaceLandmark;
    fcn_efFreeLandmarks                     fcnEfFreeLandmarks;    
    
    //  - face attributes recognition
    fcn_efRecognizeFaceAttributes           fcnEfRecognizeFaceAttributes;
    fcn_efFreeAttributes                    fcnEfFreeAttributes;
    
    //  - logging
    fcn_efLogToFileWriteTrackInfo           fcnEfLogToFileWriteTrackInfo;
    fcn_efLogToServerSendPing               fcnEfLogToServerSendPing;
    fcn_efLogToServerSendTrackInfo          fcnEfLogToServerSendTrackInfo;
};

#endif
