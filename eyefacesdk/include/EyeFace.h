////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Standard API header file of EyeFace SDK                   ///
///   --------------------------------------------------------   ///
///    The interface described in this file is usable with       ///
///    both the Standard and the Expert license. The interface   ///
///    is intended for processing of video sequences.            ///
///    The client converts the video sequence to images, which   ///
///    are fed into EyeFace SDK Standard API.                    ///
///                                                              ///
///    Eyedea Recognition, Ltd. (C) 2016, Sep 15th               ///
///                                                              ///
///    Contact:                                                  ///
///               web: http://www.eyedea.cz                      ///
///             email: info@eyedea.cz                            ///
///                                                              ///
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#ifndef EYEDEA_EYEFACE_EYEFACE_H
#define EYEDEA_EYEFACE_EYEFACE_H

#include "EyeFaceType.h"        // Types used in EyeFace SDK, Standard API

#define EYEFACE_VERSION_NUMBER 40040615    // EyeFace SDK header version

/*! \defgroup EyeFace EyeFace API.
 @{ 
*/
/*! @} */

/*! \addtogroup EyeFace
 @{
*/

/*! \defgroup Standard EyeFace Standard API
 @{
*/

// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EYEFACE STANDARD API: MODULE SETUP                              //
//                                                                 //
// EyeFace SDK evaluates clients images using precise artificial   //
// inteligence models including deep learning. The models are      //
// stored as binary blobs. The models are preloaded on startup     //
// during initialization of EyeFace SDK and stored in              //
// the so called eyeface_state. After processing the sequence,     //
// eyeface_state must be properly disposed of. Clients can also    //
// reset the eyeface_state to process additional sequences         //
// without a need to rerun time demanding initialization.          //
// Details can be found in the examples projects.                  //
//                                                                 //
// /////////////////////////////////////////////////////////////// //


/*! \fn EfBool efInitEyeFace(const char* eyefacesdk_dir, const char* config_ini_dir, const char* config_ini_filename, void** eyeface_state)
  \brief  Initializes EyeFace SDK engine and loads detection and recognition models from [eyeface_root/]eyefacesdk/models. Initializes the license sessions.
          efInitEyeFace MUST be called in the part of the software where only single thread of execution is running. This means not only threads running 
          EyeFace SDK, but all threads of the running software. This function is not thread safe nor reentrant. When starting multiple instances,
          always initialize the EyeFaceSDK sequentially in the main thread.
  \param  eyefacesdk_dir Path to the "eyefacesdk" directory containg the "lib/EyeFace.dll" ["lib/libeyefacesdk*.so" on Linux] file and "models" directory.
  \param  config_ini_dir Path to a directory containing configuration ini file.
  \param  config_ini_filename Configuration file filename (e.g. config.ini).
  \param  eyeface_state Pointer to the pointer to EyeFace SDK internal state. Must be freed with "efFreeEyeFaceState" after use.
  \return EF_TRUE on success, EF_FALSE on failure.
*/
ER_FUNCTION_PREFIX  EfBool efInitEyeFace(const char* eyefacesdk_dir, const char* config_ini_dir, const char* config_ini_filename, void** eyeface_state);


/*! \fn void* efShutdownEyeFace(void* eyeface_state)
  \brief  Flushes tracking buffers to logs and outputs before EyeFace termination / reset. This enables clients to get tracking result on last frame.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efShutdownEyeFace(void* eyeface_state);


/*! \fn void efResetEyeFace(void* eyeface_state)
  \brief  Clears/resets state structure without deallocation. All tracks are deleted.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efResetEyeFace(void* eyeface_state);


/*! \fn void efFreeEyeFace(void** eyeface_state)
  \brief  Frees EyeFace SDK engine. Releases the license sessions.
  \param  eyeface_state Pointer to the pointer to EyeFace SDK internal state initialized by efInitEyeFace(). The original pointer to eyeface_state is set to NULL;
*/
ER_FUNCTION_PREFIX void efFreeEyeFace(void** eyeface_state); 


/*! \fn int efGetLibraryVersion()
  \brief  Returns the version of the library, for example if v4.01.0915 then the return value is 40010915. The return value must be the same as defined in EYEFACE_VERSION_NUMBER global variable, otherwise there is a header/library version mismatch.
  \return Version number.
*/
ER_FUNCTION_PREFIX int efGetLibraryVersion();



// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EYEFACE STANDARD API: PROCESSING AND RESULTS                    //
//                                                                 //
// When the EyeFace SDK is initialized, images from video          //
// sequences can be fed into it.                                   //
// To insert images for processing, function efMain is used.       //
// Function efMain tracks faces and recognizes face attributes.    //
// Information is internally aggregated over past frames.          //
// In EyeFace SDK Standard, the processing result is               //
// EfTrackInfo, which holds information for the frame currently    //
// processed. The information is aggregated over past frames.      //
// This result can be presented in real-time over video stream     //
// or used for video statistics estimates.                         //
//                                                                 //
// /////////////////////////////////////////////////////////////// //


/*! \fn EfBool efMain(ERImage image, EfBoundingBox* bounding_box, double frame_time, void* eyeface_state)
  \brief  Processes current image, i.e. detects faces, runs age/gender/etc recognition and tracks the detected faces. The image must be part of a videosequence.
  \param  image Input image in ERImage format. Implementation is guaranteed not to write into image buffers.
  \param  bounding_box Bonding-box selecting the active image area where the detection will take place (NULL to process whole image). Automatically orthogonalizes input bounding box.
  \param  frame_time Current frame time since efInitEyeFace() in seconds with millisecond precision (i.e. the first frame time can be 0.000).
  \param  eyeface_state Pointer to EyeFace state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
*/
ER_FUNCTION_PREFIX EfBool efMain(ERImage image, EfBoundingBox* bounding_box, double frame_time, void* eyeface_state);


/*! \fn EfBool efGetTrackInfo(EfTrackInfoArray* track_info_array, void* eyeface_state)
  \brief  Returns the aggregated result related to a single frame of video. Can be used for real-time visualization or statistics.
  \param  track_info_array Pointer to a user allocated data structure which will be filled with results. Must be freed with "efFreeTrackInfo".
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
*/
ER_FUNCTION_PREFIX EfBool efGetTrackInfo(EfTrackInfoArray* track_info_array, void* eyeface_state);


/*! \fn void efFreeTrackInfo(EfTrackInfoArray* track_info_array, void* eyeface_state)
  \brief  Frees the internals of EfTrackInfoArray filled by "efGetTrackInfo".
  \param  track_info_array Pointer to a user allocated data structure, internals of track_info_array will be freed.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efFreeTrackInfo(EfTrackInfoArray* track_info_array, void* eyeface_state);


// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EYEFACE STANDARD API: LOG SERVER                                //
// EyeFace SDK contains capability of logging the results to       //
// a remote server via http/https POST in JSON format.             //
// Standard API provides interface to verify the connection state. //
// More information is provided in the Developer's Guide.          //
//                                                                 //
// /////////////////////////////////////////////////////////////// //
/*! \fn EfBool efLogToServerGetConnectionStatus(EfLogToServerStatus* connection_status, void* eyeface_state)
  \brief  Get status of connection to log-server.
  \param  connection_status Pointer to user allocated EfLogToServerStatus, where the result will be stored.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
*/
ER_FUNCTION_PREFIX EfBool efLogToServerGetConnectionStatus(EfLogToServerStatus* connection_status, void* eyeface_state);

// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EYEFACE STANDARD API: LICENSE                                   //
// In case of DRM build, EyeFace SDK can be queried for the        //
// license information.                                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
/*! \fn long long efGetKeyID(void* eyeface_state)
\brief Get current license key ID after call to efInitEyeFace().
\param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
\return License key ID on success, -1 on failure.
*/
ER_FUNCTION_PREFIX long long efGetKeyID(void* eyeface_state);

/*! @} */

/*! @} */

/*! \addtogroup EyeFace
 @{
*/

/*! @} */

#endif

