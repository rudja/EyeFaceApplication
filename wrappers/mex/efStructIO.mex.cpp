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

#include <mex.h>
#include <cstring>
#include <string>
#include "efStructIO.mex.h"

mxArray * convertEfBoolToMatlab(EfBool value)
{
    return mxCreateLogicalScalar(value != EF_FALSE);
}

EfBool convertEfBoolFromMatlab(const mxArray * matlab_logical)
{
    if (!mxIsLogicalScalar(matlab_logical))
    {
        mexErrMsgTxt("ERROR: convertLogical2EfBool(): Input value not logical!\n");
    }

    return mxIsLogicalScalarTrue(matlab_logical) ? EF_TRUE : EF_FALSE;
}


mxArray * convertEf2dPointsToMatlab(Ef2dPoints points)
{
    const char * struct_fields[] = {"length", "rows", "cols"};
    
    mxArray * data_field = NULL;
    if (!(data_field = mxCreateStructMatrix(1, 1, 3, struct_fields)))
    {
        mexErrMsgTxt("ERROR: convertEf2dPointsToMatlab(): Could not create mx structure.");
    }
    
    mxArray * data_subfield = mxCreateDoubleScalar(points.length);
    mxSetField(data_field, 0, "length", data_subfield);
    
    if (points.length > 0)
    {
        int dims[] = {points.length,1};
        data_subfield = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
        double * values = mxGetPr(data_subfield);
        
        for (unsigned int j = 0; j < points.length; j++)
        {
            values[j] = points.rows[j] + 1;
        }
        
        mxSetField(data_field, 0, "rows", data_subfield);
        
        data_subfield = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
        values = mxGetPr(data_subfield);
        
        for (unsigned int j = 0; j < points.length; j++)
        {
            values[j] = points.cols[j] + 1;
        }
        
        mxSetField(data_field, 0, "cols", data_subfield);
    }

    return data_field;
}

Ef2dPoints convertEf2dPointsFromMatlab(const mxArray * matlab_points)
{
    mxArray * data_field;
    Ef2dPoints points;
    
    if (!(data_field = mxGetField(matlab_points, 0, "length")))
    {
        mexErrMsgTxt("Missing 'length' field.");
    }
    points.length = (unsigned int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_points, 0, "rows")))
    {
        mexErrMsgTxt("Missing 'rows' field.");
    }
    
    double * values = mxGetPr(data_field);
    for (unsigned int i = 0; i < points.length && i < EYEDEA_EYEFACE_EF2DPOINTS_MAX_SIZE; i++)
    {
        points.rows[i] = values[i];
    }
    
    if (!(data_field = mxGetField(matlab_points, 0, "cols")))
    {
        mexErrMsgTxt("Missing 'cols' field.");
    }
    
    values = mxGetPr(data_field);
    for (unsigned int i = 0; i < points.length && i < EYEDEA_EYEFACE_EF2DPOINTS_MAX_SIZE; i++)
    {
        points.cols[i] = values[i];
    }
    
    return points;
}


// expects uint8 image, RGB or gray
bool convertERImageFromMatlab(const mxArray * image, ERImage * er_image, const EfFunctions * eyeface_functions)
{
    if (!eyeface_functions)
    {
        mexErrMsgTxt("ERROR: convertErImageFromMatlabFree(): EyeFace SDK functions pointer NULL!\n");
    }
    
    ERImageColorModel color_model;
    ERImageDataType   data_type;
    
    if (!mxIsUint8(image))
    {
        mexErrMsgTxt("ERROR: convertErImageFromMatlab(): Not implemented, input image must be UINT8.");
    }
    
    const mwSize * input_sizes = mxGetDimensions(image);
    unsigned char num_channels = 0;
    
    if (mxGetNumberOfDimensions(image) == 2)
    {
        num_channels = 1;
    }
    else if (mxGetNumberOfDimensions(image) == 3)
    {
        num_channels = input_sizes[2];
    }
    else
    {
        mexErrMsgTxt("ERROR: convertErImageFromMatlab(): Input image dimensions must be 2 or 3.");
    }
    
    if (num_channels == 3)
    {
        color_model = ER_IMAGE_COLORMODEL_BGR;
        data_type   = ER_IMAGE_DATATYPE_UCHAR; 
    } else if (num_channels == 1)
    {
        color_model = ER_IMAGE_COLORMODEL_GRAY;
        data_type = ER_IMAGE_DATATYPE_UCHAR;
    }
    else
    {
        mexErrMsgTxt("ERROR: convertErImageFromMatlab(): Input image channels must be 1 or 3.");
    }
    
    // alloc EfImage
    int alloced = eyeface_functions->fcnErImageAllocate(er_image, input_sizes[1], input_sizes[0], color_model, data_type);
    
    if (alloced != 0)
    {
        mexErrMsgTxt("ERROR: convertErImageFromMatlab(): Failed to allocate image.");
    }
    
    // copy Matlab image to erimage
    unsigned char* data_manip = (unsigned char*) mxGetPr(image);
    
    for (unsigned int i = 0; i < er_image->height; i++)
    {
        for (unsigned int j = 0; j < er_image->width; j++)
        {
            if (num_channels == 3)
            {
                er_image->row_data[i][3 * j + 0] = data_manip[i + j*input_sizes[0] + 2*input_sizes[0]*input_sizes[1]];
                er_image->row_data[i][3 * j + 1] = data_manip[i + j*input_sizes[0] + 1*input_sizes[0]*input_sizes[1]];
                er_image->row_data[i][3 * j + 2] = data_manip[i + j*input_sizes[0] + 0*input_sizes[0]*input_sizes[1]];
            }
            else
            {
                er_image->row_data[i][j] = data_manip[i + j*input_sizes[0]];
            }
        }
    }
    
    return true;
}

void convertERImageFromMatlabFree(ERImage * image, const EfFunctions * eyeface_functions)
{
    if (!image)
    {
        return;
    }
    
    if (!eyeface_functions)
    {
        mexErrMsgTxt("ERROR: convertEfImageFromMatlabFree(): EyeFace SDK functions pointer NULL!\n");
    }
    
    eyeface_functions->fcnErImageFree(image);
}




// Converts bounding box from EyeFace representation to Matlab representation.
// EyeFace has 0-based coordinates, Matlab uses 1-based.
mxArray * convertEfBoundingBoxToMatlab(EfBoundingBox bounding_box)
{
    mxArray * matlab_bounding_box, * data_field;
    const char* struct_fields[] = {"top_left_col","top_left_row","top_right_col","top_right_row",
        "bot_left_col","bot_left_row","bot_right_col","bot_right_row"};
    
    if (!(matlab_bounding_box = mxCreateStructMatrix(1, 1, 8, struct_fields)))
    {
        mexErrMsgTxt("Could not create mx structure.");
    }
    
    data_field = mxCreateDoubleScalar((double) bounding_box.top_left_col + 1);
    mxSetField(matlab_bounding_box, 0, "top_left_col", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.top_left_row + 1);
    mxSetField(matlab_bounding_box, 0, "top_left_row", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.top_right_col + 1);
    mxSetField(matlab_bounding_box, 0, "top_right_col", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.top_right_row + 1);
    mxSetField(matlab_bounding_box, 0, "top_right_row", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.bot_left_col + 1);
    mxSetField(matlab_bounding_box, 0, "bot_left_col", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.bot_left_row + 1);
    mxSetField(matlab_bounding_box, 0, "bot_left_row", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.bot_right_col + 1);
    mxSetField(matlab_bounding_box, 0, "bot_right_col", data_field);
    
    data_field = mxCreateDoubleScalar((double) bounding_box.bot_right_row + 1);
    mxSetField(matlab_bounding_box, 0, "bot_right_row", data_field);
    
    return matlab_bounding_box;
}

EfBoundingBox convertEfBoundingBoxFromMatlab(const mxArray * matlab_bounding_box)
{
    mxArray * data_field;
    EfBoundingBox bounding_box;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "top_left_col")))
    {
        mexErrMsgTxt("Missing 'top_left_col' field.");
    }
    bounding_box.top_left_col = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "top_left_row")))
    {
        mexErrMsgTxt("Missing 'top_left_row' field.");
    }
    bounding_box.top_left_row = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "top_right_col")))
    {
        mexErrMsgTxt("Missing 'top_right_col' field.");
    }
    bounding_box.top_right_col = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "top_right_row")))
    {
        mexErrMsgTxt("Missing 'top_right_row' field.");
    }
    bounding_box.top_right_row = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "bot_left_col")))
    {
        mexErrMsgTxt("Missing 'bot_left_col' field.");
    }
    bounding_box.bot_left_col = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "bot_left_row")))
    {
        mexErrMsgTxt("Missing 'bot_left_row' field.");
    }
    bounding_box.bot_left_row = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "bot_right_col")))
    {
        mexErrMsgTxt("Missing 'bot_right_col' field.");
    }
    bounding_box.bot_right_col = (int) mxGetScalar(data_field) - 1;
    
    if (!(data_field = mxGetField(matlab_bounding_box, 0, "bot_right_row")))
    {
        mexErrMsgTxt("Missing 'bot_right_row' field.");
    }
    bounding_box.bot_right_row = (int) mxGetScalar(data_field) - 1;
    
    return bounding_box;
}


mxArray* convertEfLandmarksToMatlab(EfLandmarks landmarks)
{
    const char * substruct_fields[] = {"recognized", "points", "angles"};
    mxArray * output = NULL;
    
    if (!(output = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }
    
    mxSetField(output, 0, "recognized", mxCreateLogicalScalar(landmarks.recognized != EF_FALSE));
    mxSetField(output, 0, "points", convertEf2dPointsToMatlab(landmarks.points));
    
    {
        int dims[] = {3,1};
        mxArray* data_field = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
        double * values = mxGetPr(data_field);
        values[0] = landmarks.angles[0];
        values[1] = landmarks.angles[1];
        values[2] = landmarks.angles[2];
        mxSetField(output, 0, "angles", data_field);
    }
    
    return output;
}


mxArray* convertEfFaceAttributesToMatlab(EfFaceAttributes face_attributes)
{
    EfFaceAttributesArray faked_array;
    faked_array.num_detections = 1;
    faked_array.face_attributes = &face_attributes;
    return convertEfFaceAttributesArrayToMatlab(faked_array);
}


mxArray * convertEfTrackInfoArrayToMatlab(EfTrackInfoArray track_info_array)
{
    const char * substruct_fields[] = {"status", "track_id", "person_id",
                                       "image_position", "real_world_position", "angles",
                                       "landmarks",
                                       "face_attributes",
                                       "energy",
                                       "start_time", "current_time",
                                       "total_time", "attention_time", "attention_now",
                                       "detection_index"};
    mxArray* output = NULL;

    if (!(output = mxCreateStructMatrix(1, track_info_array.num_tracks, 15, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }

    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
        const EfTrackInfo & info = track_info_array.track_info[i];

        mxSetField(output, i, "status", mxCreateDoubleScalar(info.status));
        mxSetField(output, i, "track_id", mxCreateDoubleScalar(info.track_id));
        mxSetField(output, i, "person_id", mxCreateDoubleScalar(info.person_id));

        mxSetField(output, i, "image_position", convertEfBoundingBoxToMatlab(info.image_position));
        {
            int dims[] = {2,1};
            mxArray* data_field = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
            double * values = mxGetPr(data_field);
            values[0] = info.world_position[0];
            values[1] = info.world_position[1];
            mxSetField(output, i, "real_world_position", data_field);
        }
        {
            int dims[] = {3,1};
            mxArray* data_field = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
            double * values = mxGetPr(data_field);
            values[0] = info.angles[0];
            values[1] = info.angles[1];
            values[2] = info.angles[2];
            mxSetField(output, i, "angles", data_field);
        }
        
        mxSetField(output, i, "landmarks", convertEfLandmarksToMatlab(info.landmarks));
        
        mxSetField(output, i, "face_attributes", convertEfFaceAttributesToMatlab(info.face_attributes));
        
        mxSetField(output, i, "energy", mxCreateDoubleScalar(info.energy));
        
        mxSetField(output, i, "start_time", mxCreateDoubleScalar(info.start_time));
        mxSetField(output, i, "current_time", mxCreateDoubleScalar(info.current_time));
        
        mxSetField(output, i, "total_time", mxCreateDoubleScalar(info.total_time));
        mxSetField(output, i, "attention_time", mxCreateDoubleScalar(info.attention_time));
        mxSetField(output, i, "attention_now", mxCreateLogicalScalar(info.attention_now != EF_FALSE));
        
        mxSetField(output, i, "detection_index", mxCreateDoubleScalar(info.detection_index + 1)); // from C index to Matlab index
    }

    return output;
}

mxArray * convertEfLogToServerStatusToMatlab(EfLogToServerStatus log_to_server_status)
{
    const char * substruct_fields[] = {"server_is_reachable", "num_messages_ok", "num_messages_failed"};
    mxArray* output = NULL;
    
    if (!(output = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }
    
    mxSetField(output, 0, "server_is_reachable", mxCreateLogicalScalar(log_to_server_status.server_is_reachable != EF_FALSE));
    mxSetField(output, 0, "num_messages_ok", mxCreateDoubleScalar(log_to_server_status.num_messages_ok));
    mxSetField(output, 0, "num_messages_failed", mxCreateDoubleScalar(log_to_server_status.num_messages_failed));

    return output;
}


//////////////////////////////
//
//
// EXPERT API 
//
//
//////////////////////////////
mxArray * convertEfPositionToMatlab(EfPosition position)
{
    const char * substruct_fields[] = {"bounding_box", "center_col", "center_row", "size"};
    mxArray* output = NULL;

    if (!(output = mxCreateStructMatrix(1, 1, 4, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }

    mxSetField(output, 0, "bounding_box", convertEfBoundingBoxToMatlab(position.bounding_box));
    mxSetField(output, 0, "center_col", mxCreateDoubleScalar(position.center_col));
    mxSetField(output, 0, "center_row", mxCreateDoubleScalar(position.center_row));
    mxSetField(output, 0, "size", mxCreateDoubleScalar(position.size));

    return output;
}

EfPosition convertEfPositionFromMatlab(mxArray * matlab_position)
{
    EfPosition position;
    memset(&position, 0, sizeof(EfPosition));
    
    if (!matlab_position)
    {
        mexErrMsgTxt("Input array NULL.");
    }
    
    mxArray * data_field = NULL;

    if (!(data_field = mxGetField(matlab_position, 0, "bounding_box")))
    {
        mexErrMsgTxt("Missing 'bounding_box' field.");
    }
    position.bounding_box = convertEfBoundingBoxFromMatlab(data_field);

    if (!(data_field = mxGetField(matlab_position, 0, "center_col")))
    {
        mexErrMsgTxt("Missing 'center_col' field.");
    }
    position.center_col = mxGetScalar(data_field);

    if (!(data_field = mxGetField(matlab_position, 0, "center_row")))
    {
        mexErrMsgTxt("Missing 'center_row' field.");
    }
    position.center_row = mxGetScalar(data_field);

    if (!(data_field = mxGetField(matlab_position, 0, "size")))
    {
        mexErrMsgTxt("Missing 'size' field.");
    }
    position.size = mxGetScalar(data_field);
    
    return position;
}

mxArray * convertEfDetectionArrayToMatlab(EfDetectionArray detection_array)
{
    const char * substruct_fields[] = {"confidence", "position", "angles"};
    mxArray* output = NULL;

    if (!(output = mxCreateStructMatrix(1, detection_array.num_detections, 3, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }

    for (unsigned int i = 0; i < detection_array.num_detections; i++)
    {
        const EfDetection & det = detection_array.detections[i];

        mxSetField(output, i, "confidence", mxCreateDoubleScalar(det.confidence));
        mxSetField(output, i, "position", convertEfPositionToMatlab(det.position));

        {
            int dims[] = {3,1};
            mxArray* data_field = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
            double * values = mxGetPr(data_field);
            values[0] = det.angles[0];
            values[1] = det.angles[1];
            values[2] = det.angles[2];
            mxSetField(output, i, "angles", data_field);
        }
    }

    return output;
}

EfDetectionArray convertEfDetectionArrayFromMatlab(const mxArray * matlab_detection_array)
{
    EfDetectionArray detection_array;
    detection_array.num_detections = 0;
    detection_array.detections = NULL;

    if (!matlab_detection_array)
    {
        return detection_array;
    }

    // get number of detections in the array
    size_t num_detections = mxGetNumberOfElements(matlab_detection_array);

    try
    {
        detection_array.detections = new EfDetection[num_detections]();
    }
    catch (std::bad_alloc)
    {
        mexErrMsgTxt("Failed to allocate memory.");
    }

    detection_array.num_detections = (unsigned int)num_detections;

    // copy detections
    for (unsigned int i = 0; i < detection_array.num_detections; i++)
    {
        mxArray * data_field = NULL;
        EfDetection & detection = detection_array.detections[i];

        if (!(data_field = mxGetField(matlab_detection_array, i, "confidence")))
        {
            mexErrMsgTxt("Missing 'confidence' field.");
        }
        detection.confidence = mxGetScalar(data_field);

        if (!(data_field = mxGetField(matlab_detection_array, i, "position")))
        {
            mexErrMsgTxt("Missing 'position' field.");
        }
        detection.position = convertEfPositionFromMatlab(data_field);

        if (!(data_field = mxGetField(matlab_detection_array, i, "angles")))
        {
            mexErrMsgTxt("Missing 'angles' field.");
        }
        double * values = mxGetPr(data_field);
        detection.angles[0] = values[0];
        detection.angles[1] = values[1];
        detection.angles[2] = values[2];
    }

    return detection_array;
}


void convertEfDetectionArrayFromMatlabFree(EfDetectionArray detection_array)
{
    delete [] detection_array.detections;
}

mxArray* convertEfLandmarksArrayToMatlab(EfLandmarksArray landmarks_array)
{
    const char * substruct_fields[] = {"recognized", "points", "angles"};
    mxArray* output = NULL;

    if (!(output = mxCreateStructMatrix(1, landmarks_array.num_detections, 3, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }

    for (unsigned int i = 0; i < landmarks_array.num_detections; i++)
    {
        const EfLandmarks & lm = landmarks_array.landmarks[i];

        mxSetField(output, i, "recognized", mxCreateLogicalScalar(lm.recognized != EF_FALSE));
        mxSetField(output, i, "points", convertEf2dPointsToMatlab(lm.points));

        {
            int dims[] = {3,1};
            mxArray* data_field = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
            double * values = mxGetPr(data_field);
            values[0] = lm.angles[0];
            values[1] = lm.angles[1];
            values[2] = lm.angles[2];
            mxSetField(output, i, "angles", data_field);
        }
    }

    return output;
}

EfLandmarksArray convertEfLandmarksArrayFromMatlab(const mxArray* matlab_landmarks)
{
    EfLandmarksArray landmarks_array;
    landmarks_array.num_detections = 0;
    landmarks_array.landmarks = NULL;

    if (!matlab_landmarks)
    {
        return landmarks_array;
    }

    // get number of detections in the array
    size_t num_detections = mxGetNumberOfElements(matlab_landmarks);

    try
    {
        landmarks_array.landmarks = new EfLandmarks[num_detections]();
    }
    catch (std::bad_alloc)
    {
        mexErrMsgTxt("Failed to allocate memory.");
    }

    landmarks_array.num_detections = (unsigned int)num_detections;

    // copy landmarks
    for (unsigned int i = 0; i < landmarks_array.num_detections; i++)
    {
        mxArray * data_field = NULL;
        EfLandmarks & landmarks = landmarks_array.landmarks[i];

        if (!(data_field = mxGetField(matlab_landmarks, i, "recognized")))
        {
            mexErrMsgTxt("Missing 'recognized' field.");
        }
        mxLogical * logicals = mxGetLogicals(data_field);
        landmarks.recognized = logicals[0] ? EF_TRUE : EF_FALSE;

        if (!(data_field = mxGetField(matlab_landmarks, i, "points")))
        {
            mexErrMsgTxt("Missing 'points' field.");
        }
        landmarks.points = convertEf2dPointsFromMatlab(data_field);

        if (!(data_field = mxGetField(matlab_landmarks, i, "angles")))
        {
            mexErrMsgTxt("Missing 'angles' field.");
        }
        double * values = mxGetPr(data_field);
        landmarks.angles[0] = values[0];
        landmarks.angles[1] = values[1];
        landmarks.angles[2] = values[2];
    }

    return landmarks_array;
}

void convertEfLandmarksArrayFromMatlabFree(EfLandmarksArray landmarks_array)
{
    delete [] landmarks_array.landmarks;
}

mxArray* convertEfFaceAttributesArrayToMatlab(EfFaceAttributesArray face_attributes_array)
{
    const char * substruct_fields[] = {"age", "gender", "emotion", "ancestry"};
    mxArray* output = NULL;
    
    if (!(output = mxCreateStructMatrix(1, face_attributes_array.num_detections, 4, substruct_fields)))
    {
        mexErrMsgTxt("Could not create mx sub-structure.");
    }
    
    for (unsigned int i = 0; i < face_attributes_array.num_detections; i++)
    {
        EfFaceAttributes & face_attributes = face_attributes_array.face_attributes[i];
        // age
        {
            const char * substruct_fields[] = {"recognized", "value", "response"};
            mxArray* attribute = NULL;

            if (!(attribute = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
            {
                mexErrMsgTxt("Could not create mx sub-structure.");
            }
        
            mxSetField(attribute, 0, "recognized", mxCreateLogicalScalar(face_attributes.age.recognized != EF_FALSE));
            mxSetField(attribute, 0, "value", mxCreateDoubleScalar(face_attributes.age.value));
            mxSetField(attribute, 0, "response", mxCreateDoubleScalar(face_attributes.age.response));
            
            mxSetField(output, i, "age", attribute);
        }
        
        // gender
        {
            const char * substruct_fields[] = {"recognized", "value", "response"};
            mxArray* attribute = NULL;
            
            if (!(attribute = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
            {
                mexErrMsgTxt("Could not create mx sub-structure.");
            }
            
            mxSetField(attribute, 0, "recognized", mxCreateLogicalScalar(face_attributes.gender.recognized != EF_FALSE));
            mxSetField(attribute, 0, "value", mxCreateDoubleScalar(face_attributes.gender.value));
            mxSetField(attribute, 0, "response", mxCreateDoubleScalar(face_attributes.gender.response));
            
            mxSetField(output, i, "gender", attribute);
        }
        
        // emotion
        {
        const char * substruct_fields[] = {"recognized", "value", "response"};
        mxArray* attribute = NULL;

        if (!(attribute = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
        {
            mexErrMsgTxt("Could not create mx sub-structure.");
        }

            mxSetField(attribute, 0, "recognized", mxCreateLogicalScalar(face_attributes.emotion.recognized != EF_FALSE));
            mxSetField(attribute, 0, "value", mxCreateDoubleScalar(face_attributes.emotion.value));
            mxSetField(attribute, 0, "response", mxCreateDoubleScalar(face_attributes.emotion.response));
            
            mxSetField(output, i, "emotion", attribute);
        }
        
        // ancestry
        {
            const char * substruct_fields[] = {"recognized", "value", "response"};
            mxArray* attribute = NULL;
            
            if (!(attribute = mxCreateStructMatrix(1, 1, 3, substruct_fields)))
            {
                mexErrMsgTxt("Could not create mx sub-structure.");
            }
            
            mxSetField(attribute, 0, "recognized", mxCreateLogicalScalar(face_attributes.ancestry.recognized != EF_FALSE));
            mxSetField(attribute, 0, "value", mxCreateDoubleScalar(face_attributes.ancestry.value));
            mxSetField(attribute, 0, "response", mxCreateDoubleScalar(face_attributes.ancestry.response));      
            
            mxSetField(output, i, "ancestry", attribute);
        }
    }
    
    return output;
}

