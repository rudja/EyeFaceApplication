////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Standard API header file of EyeFace SDK                   ///
///   --------------------------------------------------------   ///
///    The interface described in this file is usable with       ///
///    both the Expert license only. The interface               ///
///    is intended for processing image databases.               ///
///                                                              ///
///    Eyedea Recognition, Ltd. (C) 2016, Sep 15th               ///
///                                                              ///
///    Contact:                                                  ///
///               web: http://www.eyedea.cz                      ///
///             email: info@eyedea.cz                            ///
///                                                              ///
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


#ifndef EYEDEA_EYEFACE_EYEFACEEXPERT_H
#define EYEDEA_EYEFACE_EYEFACEEXPERT_H

#include "EyeFace.h"
#include "EyeFaceExpertType.h"

// //////////////////////////////////////////////////////////////////////////////////// //
//                                                                                      //
// EYEFACE SDK EXPERT API:  SINGLE IMAGE PROCESSING / PRECISE VIDEO SEQUENCE PROCESSING //
//                                                                                      //
// //////////////////////////////////////////////////////////////////////////////////// //
/*! \addtogroup EyeFace
 @{
*/

/*! \defgroup Expert EyeFace Expert API
 @{
*/


/*! \fn EfBool efRunFaceDetector(ERImage image, EfDetectionArray* detection_array, void* eyeface_state)
  \brief  Runs face detection on a single image. Suitable both for video-sequence images (Expert) manipulation or standalone images (e.g. image databases).
  \param  image Input image in ERImage format. Implementation is guaranteed not to write into image buffers.
  \param  detection_array On successful completition, this user allocated structure is filled with face detections for the given image.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
  thread_safety: NO. You can increase number of internal processing threads via configuration - you can use more threads than number 2x number of cores in HT environment (see [DETECTOR:num_threads] in config.ini).
*/
ER_FUNCTION_PREFIX EfBool efRunFaceDetector(ERImage image, EfDetectionArray* detection_array, void* eyeface_state);


/*! \fn void efFreeDetections(EfDetectionArray* detection_array, void* eyeface_state)
  \brief Frees internals of user allocated EfDetectionArray structure returned by efRunFaceDetector().
  \param  detection_array Pointer to user allocated EfDetectionArray structure, internals of detection_array will be freed.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efFreeDetections(EfDetectionArray* detection_array, void* eyeface_state);


/*! \fn  EfBool efUpdateTracker(ERImage image, EfDetectionArray detection_array, double frame_time, void* eyeface_state)
  \brief  Updates the tracker state after a frame is processed.
  \param  image Input image in ERImage format. Implementation is guaranteed not to write into image buffers.
  \param  detection_array EfDetectionArray structure filled by efRunFaceDetector(). Implementation is guaranteed not to write to detection_array->detections.
  \param  frame_time Image frame time for tracking purposes (in seconds). MUST be increasing. If timestamp is repeated or goes back in time, it results in undefined behavior.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
thread_safety: NO.
*/
ER_FUNCTION_PREFIX EfBool efUpdateTracker(ERImage image, EfDetectionArray detection_array, double frame_time, void* eyeface_state);



/*! \fn EfBool efRunFaceLandmark(ERImage image, EfDetectionArray detection_array, EfBool* detections_to_process, EfLandmarksArray* facial_landmarks_array, void* eyeface_state)
  \brief Runs face landmark detection on face detections from EfDetResult. Landmarks are only used for visualization - turned off by default during initialization. This function has a side efect of storing the landmarks to tracks.
  \param  image Input image in ERImage format. Implementation is guaranteed not to write into image buffers.
  \param  detection_array Pointer to EfDetectionArray structure returned by efRunFaceDetector(). Implementation is guaranteed not to write to detection_array->detections.
  \param  detections_to_process User allocated array of detection_array.num_detections EfBools. EF_TRUE value at index i means that the i-th detection in detection_array will be processed. If NULL, all detections are processed.
  \param  facial_landmarks_array On successful completion, this user allocated structure is filled with landmark data of the faces in detection_array. If NULL, only the side effect takes place.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
  thread_safety: NO.
*/
ER_FUNCTION_PREFIX EfBool efRunFaceLandmark(ERImage image, EfDetectionArray detection_array, EfBool* detections_to_process,
                                                 EfLandmarksArray* facial_landmarks_array, void* eyeface_state);


/*! \fn void efFreeLandmarks(EfLandmarksArray* facial_landmarks_array, void* eyeface_state)
  \brief Frees internals of user allocated EfLandmarksArray structure returned by efRunFaceLandmark().
  \param  facial_landmarks_array Pointer to user allocated EfLandmarksArray structure, internals of facial_landmarks_array will be freed.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efFreeLandmarks(EfLandmarksArray* facial_landmarks_array, void* eyeface_state);


/*! \fn  EfBool efRecognizeFaceAttributes(ERImage image, EfDetectionArray detection_array, const EfLandmarksArray* facial_landmarks_array, EfBool* detections_to_process, unsigned int request_flag, double frame_time, EfBool process_sequentially, EfFaceAttributesArray* face_attributes_array, void* eyeface_state)
  \brief  Files a request to compute face attributes on a given face. 
  \note   For video-sequence processing, it is typical to set process_sequentially = EF_FALSE. This way, face attributes are assigned to tracks asynchronously,
  \note   resulting in good performance. This is the way efMain() recognizes the face attributes. For image database processing, it is required to set 
  \note   process_sequentially = EF_TRUE and insert a pointer to user allocated "face_attributes_array" structure.
  \note   In case of process_sequentially = EF_FALSE, only faces with YAW < 35 degrees are processed (only frontal faces), otherwise all faces. The frontal only processing is the case of efMain().
  \param  image Input image in ERImage format. Implementation is guaranteed not to write into image buffers.
  \param  detection_array EfDetectionArray structure filled by efRunFaceDetector(). Implementation is guaranteed not to write to detection_array->detections.
  \param  facial_landmarks_array EfLandmarksArray structure filled by efRunFaceLandmark(). Optional, set to NULL if you did not compute the landmarks (default). Using the landmarks might improve the performance in future release.
  \param  detections_to_process User allocated array of detection_array.num_detections EfBools. EF_TRUE value at index i means that the i-th detection in detection_array will be processed. Optional, set to NULL to process all detections.
  \param  request_flag Bit array (of EF_FACEATTRIBUTES_* flags) setting which attributes to compute, typically EF_FACEATTRIBUTES_ALL.
  \param  frame_time Image frame time for tracking purposes (in seconds with millisecond precision). If timestamp is repeated or goes back in time, it results in undefined behavior. Set to zero for image database processing.
  \param  process_sequentially If EF_TRUE, waits until the processing is finished and optionally returns a result, if EF_FALSE, returns immediately and the results are appended to tracks later.
  \param  face_attributes_array Optional output of face attributes of the faces in detection_array, user allocated structure, only filled if process_sequentially == EF_TRUE. Optional. Set to NULL if process_sequentially == EF_FALSE.
  \param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
  \return EF_TRUE on success, EF_FALSE on failure.
thread_safety: NO. Use internal threading.
*/
ER_FUNCTION_PREFIX EfBool efRecognizeFaceAttributes(ERImage image, EfDetectionArray detection_array,
    const EfLandmarksArray* facial_landmarks_array, EfBool* detections_to_process,
    unsigned int request_flag, double frame_time, EfBool process_sequentially, EfFaceAttributesArray* face_attributes_array, void* eyeface_state);


/*! \fn void efFreeAttributes(EfFaceAttributesArray* face_attributes_array, void* eyeface_state)
\brief Frees internals of user allocated EfFaceAttributesArray structure returned by efRecognizeFaceAttributes().
\param  face_attributes_array Pointer to user allocated EfFaceAttributesArray structure, internals of face_attributes_array will be freed.
\param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
*/
ER_FUNCTION_PREFIX void efFreeAttributes(EfFaceAttributesArray* face_attributes_array, void* eyeface_state);


/*! \fn  EfBool efLogToFileWriteTrackInfo(void* eyeface_state)
\brief  Writes information about current state of tracks into a log file. The log file must be specified and enabled in config.ini.
\param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
\return EF_TRUE on success, EF_FALSE on failure.
thread_safety: NO.
*/
ER_FUNCTION_PREFIX EfBool efLogToFileWriteTrackInfo(void* eyeface_state);


/*! \fn  EfBool efLogToServerSendPing(void* eyeface_state)
\brief  Sends a ping via a server connection specified in the [LOG TO SERVER] part of the config.ini. The log server connection must be specified and enabled in config.ini.
\param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
\return EF_TRUE on success, EF_FALSE on failure.
thread_safety: NO.
*/
ER_FUNCTION_PREFIX EfBool efLogToServerSendPing(void* eyeface_state);


/*! \fn  EfBool efLogToServerSendTrackInfo(void* eyeface_state)
\brief  Writes information about current state of tracks into a server connection. The log server connection must be specified and enabled in config.ini.
\param  eyeface_state Pointer to EyeFace SDK internal state initialized by efInitEyeFace().
\return EF_TRUE on success, EF_FALSE on failure.
thread_safety: NO.
*/
ER_FUNCTION_PREFIX EfBool efLogToServerSendTrackInfo(void* eyeface_state);



/*! @} */



/*! @} */



#endif
