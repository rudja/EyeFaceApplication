////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Standard API data header file of EyeFace SDK              ///
///   --------------------------------------------------------   ///
///    The interface described here is usable with both the      ///
///    Standard and the Expert license.                          ///
///                                                              ///
///    Eyedea Recognition, Ltd. (C) 2014, Nov 20th               ///
///                                                              ///
///    Contact:                                                  ///
///               web: http://www.eyedea.cz                      ///
///             email: info@eyedea.cz                            ///
///                                                              ///
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#ifndef EYEDEA_EYEFACE_EYEFACETYPE_H
#define EYEDEA_EYEFACE_EYEFACETYPE_H

#include "er_image.h"

// /////////////////////////////////////////////////////////////// //
//                                                                 //
// EYEFACE STANDARD API - C89 compatible                           //
//                                                                 //
// /////////////////////////////////////////////////////////////// //
#define EYEDEA_EYEFACE_EF2DPOINTS_MAX_SIZE 32   /* Maximal number of points at output. */

/*! Boolean data type. */
typedef enum
{
    EF_FALSE    = 0,        /*!< Boolean false value. */
    EF_TRUE     = 1         /*!< Boolean true value.  */
}EfBool;


/*! Set of 2D points structure. */
typedef struct
{
    unsigned int    length;                                         /*!< Number of points in the set.  */
    double          rows[EYEDEA_EYEFACE_EF2DPOINTS_MAX_SIZE];       /*!< Row coordinates of points.    */
    double          cols[EYEDEA_EYEFACE_EF2DPOINTS_MAX_SIZE];       /*!< Column coordinates of points. */
}Ef2dPoints;

/*! Bounding-box coordinates of a detection area */
typedef struct
{
    int top_left_col;       /*!< Column index of top left corner of BB.  */
    int top_left_row;       /*!<  Row index of top left corner of BB.    */
    int top_right_col;      /*!< Column index of top right corner of BB. */
    int top_right_row;      /*!< Row index of top right corner of BB.    */
    int bot_left_col;       /*!< Column index of bot left corner of BB.  */
    int bot_left_row;       /*!< Row index of bot left corner of BB.     */
    int bot_right_col;      /*!< Column index of bot right corner of BB. */
    int bot_right_row;      /*!< Row index of bot right corner of BB.    */
}EfBoundingBox;

/*! Facial Landmarks for a single face. Landmarks are not required and are turned off by default.*/
typedef struct
{
    EfBool      recognized;                 /*!< Only use EfLandmarks values if recognized == EF_TRUE.                                */
    Ef2dPoints  points;                     /*!< Landmark points. Point 0-7 can be further stabilized using landmark_precise_use_precise (See the Developer's Guide). [0=FACE_CENTER 1=L_CANTHUS_R_EYE 2=R_CANTHUS_L_EYE 3=MOUTH_R 4=MOUTH_L 5=R_CANTHUS_R_EYE 6=L_CANTHUS_L_EYE 7=NOSE_TIP 8=L_EYEBROW_L 9=L_EYEBROW_C 10=L_EYEBROW_R 11=R_EYEBROW_L 12=R_EYEBROW_C 13=R_EYEBROW_R 14=NOSE_ROOT 15=NOSE_L 16=NOSE_R 17=MOUTH_T 18=MOUTH_B 19=CHIN]*/
    double      angles[3];                  /*!< Roll, pitch, yaw angle as computed by landmarks. */
}EfLandmarks;

//////////////////////////////////////////////////////////
// FACE ATTRIBUTES                                      //
// Face attributes are properties of the face in image. //
// It can be age, gender, emotion and others.           //
//////////////////////////////////////////////////////////

/*! Age recognition result type. */
typedef struct
{
    EfBool      recognized;             /*!< Only use EfAge values if recognized == EF_TRUE.                                */
    double      value;                  /*!< Recognized age in years [0-99].                                                */
    double      response;               /*!< Age classifier score function response (for data analysts / statisticians).    */
}EfAge;

/*! Gender classes enum type. */
typedef enum
{
    EF_GENDER_MALE      = -1,  /*!< Male value. */
    EF_GENDER_UNKNOWN   =  0,  /*!< Non-gender value. */
    EF_GENDER_FEMALE    =  1   /*!< Female value. */
}EfGenderClass;

/*! Gender recognition result type. */
typedef struct
{
    EfBool          recognized;             /*!< Only use EfGender values if recognized == EF_TRUE.                             */
    EfGenderClass   value;                  /*!< Recognized gender class.                                                       */
    double          response;               /*!< Gender classifier score function response (for data analysts / statisticians). */
}EfGender;

/*! Emotion classes enum type. */
typedef enum
{
    EF_EMOTION_NOTSMILING    = -1,  /*!<      */
    EF_EMOTION_UNKNOWN       = 0,   /*!<      */
    EF_EMOTION_SMILING       = 1,   /*!<      */
}EfEmotionClass;

/*! Emotion recognition result type. */
typedef struct
{
    EfBool              recognized;             /*!< Only use EfEmotions values if recognized == EF_TRUE.                              */
    EfEmotionClass      value;                  /*!< Recognized emotion class.                                                         */
    double              response;               /*!< Emotion classifier score function response (for data analysts / statisticians).   */
}EfEmotion;

/*! Ancestry classes enum type. */
typedef enum
{
    EF_ANCESTRY_UNKNOWN     = 0,    /*!<      */
    EF_ANCESTRY_CAUCASIAN   = 1,    /*!<      */
    EF_ANCESTRY_ASIAN       = 2,    /*!<      */
    EF_ANCESTRY_AFRICAN     = 3,    /*!<      */
}EfAncestryClass;

/*! Ancestry recognition result type. */
typedef struct
{
    EfBool              recognized;             /*!< Only use EfEmotions values if recognized == EF_TRUE.                                       */
    EfAncestryClass     value;                  /*!< Recognized emotion class.                                                                  */
    double              response;               /*!< Estimate of probability that the person is of the given ancestry. (for data analysts / statisticians).   */
}EfAncestry;

/*! Face attributes recognition result type. */
typedef struct
{
    EfAge           age;                        /*!< Age results.*/
    EfGender        gender;                     /*!< Gender results.*/
    EfEmotion       emotion;                    /*!< Emotion results.*/
    EfAncestry      ancestry;                   /*!< Ancestry results.*/
}EfFaceAttributes;

/////////////////////////////////////////////////////
// TRACK INFO                                      //
// EfTrackInfo holds all information about the     //
// result of processing image from video sequence. //
/////////////////////////////////////////////////////
typedef enum
{
    EF_TRACKSTATUS_LIVE     = 0,
    EF_TRACKSTATUS_FINISHED = 1
}EfTrackStatus;

/*! Structure containing visualization / statistics data for a given track. */
typedef struct
{
    EfTrackStatus       status;                     /*!< Current status of this track. */
    unsigned int        track_id;                   /*!< Track unique number. Track is a space-time aggregation of face detections. */
    unsigned int        person_id;                  /*!< Person identity unique number based on face recognition, 0 if not known yet.*/
    
    EfBoundingBox       image_position;             /*!< Face position in image pixels. Internally aggregated over past frames. */
    double              world_position[2];          /*!< Groundplane real-world position relative to camera - [0] -> left-right, [1] -> forward-backward. In meters. Internally aggregated over past frames. Depends linearly on camera FOV.*/
    double              angles[3];                  /*!< Roll [0], Pitch [1], Yaw [2] angle of the detected face. Pitch angle not available. */
    
    EfLandmarks         landmarks;                  /*!< Landmark points. Not computed by default. Point 0-7 can be further stabilized using landmark_precise_use_precise (See the Developer's Guide). */
    
    EfFaceAttributes    face_attributes;            /*!< Face attributes (age, gender, emotion, etc.) storage.*/
    
    double              energy;                     /*!< Fade-out energy of the track: 1 if the face was detected in the current frame, less than 1 if not.*/
    
    double              start_time;                 /*!< Track start time [seconds]. */
    double              current_time;               /*!< Time of the last tracker update [seconds].*/
    
    double              total_time;                 /*!< Track duration [seconds].   */
    double              attention_time;             /*!< Estimate of time [seconds] the person looked at the device. Based on face angle. (See the Developer's Guide)*/
    EfBool              attention_now;              /*!< Estimate of whether the person looks at the device in the current frame. Based on face angle. */
     
    int                 detection_index;            /*!< Use in Expert API only. -1 if no detection in current frame. Tells the index to EfDetectionArray structure outputted by efRunFaceDetector, so that EfTrackInfo can be matched with particular EfDetection. */
}EfTrackInfo;

/*! Structure containing visualization / statistics data for the last frame. */
typedef struct
{
    unsigned int        num_tracks;                 /*!< Number of face tracks in 'track_info' array. */
    EfTrackInfo*        track_info;                 /*!< Array of face track visualization data. */
}EfTrackInfoArray;


// /////////////////////////////////////////////////////////////// //
//                                                                 //
// CONNECTION TO LOG SERVER                                        //
//                                                                 //
// /////////////////////////////////////////////////////////////// //
/*! Status of a connection to log server. */
typedef struct
{
    EfBool          server_is_reachable;    /*!< 0-not-reachable 1-reachable          */
    unsigned int    num_messages_ok;        /*!< number of messages successfully sent */
    unsigned int    num_messages_failed;    /*!< number of messages failed to send    */
}EfLogToServerStatus;


// //////////////////////////////////////////////////////////////// //
//                                                                  //
// EyeFace Standard API runtime linkage data types. (see EyeFace.h) //
//                                                                  //
// //////////////////////////////////////////////////////////////// //

typedef EfBool                  (*fcn_efInitEyeFace)(const char*, const char*, const char*, void**);
typedef void                    (*fcn_efShutdownEyeFace)(void*);
typedef void                    (*fcn_efResetEyeFace)(void*);
typedef void                    (*fcn_efFreeEyeFace)(void**);
typedef int                     (*fcn_efGetLibraryVersion)();

typedef EfBool                  (*fcn_efMain)(ERImage, EfBoundingBox*, double, void*);
typedef EfBool                  (*fcn_efGetTrackInfo) (EfTrackInfoArray*, void*);
typedef void                    (*fcn_efFreeTrackInfo)(EfTrackInfoArray*, void*);

typedef EfBool                  (*fcn_efLogToServerGetConnectionStatus)(EfLogToServerStatus*, void*);

typedef long long               (*fcn_efGetKeyID)(void*);


#endif

