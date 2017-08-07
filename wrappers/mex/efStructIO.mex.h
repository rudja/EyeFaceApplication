////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///                                                              ///
///    Matlab - EyeFace conversion functions.                    ///
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

#ifndef EYEDEA_EYEFACE_STRUCT_IO_H
#define EYEFACE_EYEFACE_STRUCT_IO_H

#include <mex.h>
#include "efEyeFace.mex.h"

mxArray* convertEfBoolToMatlab(EfBool value);
EfBool convertEfBoolFromMatlab(const mxArray * matlab_logical);
mxArray* convertEf2dPointsToMatlab(Ef2dPoints points);
Ef2dPoints convertEf2dPointsFromMatlab(const mxArray * matlab_points);
bool convertERImageFromMatlab(const mxArray * image, ERImage * er_image, const EfFunctions * eyeface_functions);
void convertERImageFromMatlabFree(ERImage * image, const EfFunctions * eyeface_functions);
mxArray* convertEfBoundingBoxToMatlab(EfBoundingBox bounding_box);
EfBoundingBox convertEfBoundingBoxFromMatlab(const mxArray * matlab_bounding_box);
mxArray* convertEfTrackInfoArrayToMatlab(EfTrackInfoArray track_info_array);
mxArray* convertEfLogToServerStatusToMatlab(EfLogToServerStatus log_to_server_status);
mxArray* convertEfDetectionArrayToMatlab(EfDetectionArray detection_array);
mxArray* convertEfPositionToMatlab(EfPosition position);
EfPosition convertEfPositionFromMatlab(mxArray * matlab_position);
EfDetectionArray convertEfDetectionArrayFromMatlab(const mxArray * matlab_detection);
void convertEfDetectionArrayFromMatlabFree(EfDetectionArray detection_array);
mxArray* convertEfLandmarksArrayToMatlab(EfLandmarksArray landmarks_array);
EfLandmarksArray convertEfLandmarksArrayFromMatlab(const mxArray * matlab_landmarks);
void convertEfLandmarksArrayFromMatlabFree(EfLandmarksArray landmarks_array);
mxArray* convertEfFaceAttributesArrayToMatlab(EfFaceAttributesArray face_attributes_array);

#endif
