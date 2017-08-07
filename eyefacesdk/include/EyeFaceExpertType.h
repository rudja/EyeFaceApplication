////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Expert API data header file of EyeFace SDK                ///
///   --------------------------------------------------------   ///
///    The interface described here is usable with the Expert    ///
///    license only.                                             ///
///                                                              ///
///    Eyedea Recognition, Ltd. (C) 2014, Nov 20th               ///
///                                                              ///
///    Contact:                                                  ///
///               web: http://www.eyedea.cz                      ///
///             email: info@eyedea.cz                            ///
///                                                              ///
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#ifndef EYEDEA_EYEFACE_EYEFACEEXPERTTYPE_H
#define EYEDEA_EYEFACE_EYEFACEEXPERTTYPE_H

#include "EyeFaceType.h"

/*! Position  */
typedef struct
{
    EfBoundingBox   bounding_box;   /*!< Bounding box of face detection.      */
    double          center_col;     /*!< Center column of detection.          */
    double          center_row;     /*!< Center row of detection.             */
    double          size;           /*!< Scanning window height at detection. */
}EfPosition;


/*! Data related to a face detection. All data are related to a single frame, they are not aggregated as in EfVisualData. */
typedef struct
{
    double          confidence;             /*!< Detection confidence factor.    */
    EfPosition      position;               /*!< Position of detection in image. */
    double          angles[3];              /*!< Roll, Pitch, Yaw - only Yaw and Roll now used.     */
}EfDetection;


/*! Detection result structure. */
typedef struct
{
    unsigned int           num_detections;          /*!< Number of face detections. */
    EfDetection*           detections;              /*!< Array of detections.  */
}EfDetectionArray;


/*! Landmarks result structure. */
typedef struct
{
    unsigned int           num_detections;          /*!< Number of face detections on which landmarks has been computed. */
    EfLandmarks*           landmarks;               /*!< Array of face landmarks. */
}EfLandmarksArray;


/*! Face Attributes result structure. */
typedef struct
{
    unsigned int           num_detections;          /*!< Number of face detections on which face attributes has been computed. */
    EfFaceAttributes*      face_attributes;         /*!< Array of face landmarks. */
}EfFaceAttributesArray;


/*! mask for efRecognizeFaceAttributes parameter "request_flag"*/
const unsigned int EF_FACEATTRIBUTES_AGE            = 0x01;
const unsigned int EF_FACEATTRIBUTES_GENDER         = 0x02;
const unsigned int EF_FACEATTRIBUTES_EMOTION        = 0x04;
const unsigned int EF_FACEATTRIBUTES_ANCESTRY       = 0x08;
const unsigned int EF_FACEATTRIBUTES_SMARTTRACKING  = 0x10;
const unsigned int EF_FACEATTRIBUTES_ALL            = 0xFFFFFFFF;



// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EyeFace Expert Api Functions typedefs (see EyeFaceExpert.h)   //
//                                                                 //
// /////////////////////////////////////////////////////////////// //
// face detection
typedef EfBool              (*fcn_efRunFaceDetector)(ERImage, EfDetectionArray*, void*);
typedef void                (*fcn_efFreeDetections)(EfDetectionArray*, void*);

// tracker
typedef EfBool              (*fcn_efUpdateTracker)(ERImage, EfDetectionArray, double, void*);

// landmarks
typedef EfBool              (*fcn_efRunFaceLandmark)(ERImage, EfDetectionArray, EfBool*, EfLandmarksArray*, void*);
typedef void                (*fcn_efFreeLandmarks)(EfLandmarksArray*, void*);

// face attributes
typedef EfBool              (*fcn_efRecognizeFaceAttributes)(ERImage, EfDetectionArray, const EfLandmarksArray*, EfBool*, unsigned int, double, EfBool, EfFaceAttributesArray*, void*);
typedef void                (*fcn_efFreeAttributes)(EfFaceAttributesArray*, void*);

// logging
typedef EfBool              (*fcn_efLogToFileWriteTrackInfo)(void*);
typedef EfBool              (*fcn_efLogToServerSendPing)(void*);
typedef EfBool              (*fcn_efLogToServerSendTrackInfo)(void*);

#endif