///////////////////////////////////////////////////////////
//                                                       //
// Copyright (c) 2014-2017 by Eyedea Recognition, s.r.o. //
//                  ALL RIGHTS RESERVED.                 //
//                                                       //
// Author: Eyedea Recognition, s.r.o.                    //
//                                                       //
// Contact:                                              //
//           web: http://www.eyedea.cz                   //
//           email: info@eyedea.cz                       //
//                                                       //
// Consult your license regarding permissions and        //
// restrictions.                                         //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//                                                       //
// EyeFace SDK: example-standard-api.cpp                 //
// ----------------------------------------------------- //
//                                                       //
// This example shows how to use the Standard API        //
// interface. It requires Standard or Expert license     //
// to run. A videosequence decomposed to individual      //
// images is processed. Faces are tracked, and their     //
// attributes like age, gender, emotions and unique      //
// counting identifier are computed on per track basis.  //
// Standard API can only be used to process sequences,   //
// not individual images, as results are aggregated over //
// time.                                                 //
//                                                       //
///////////////////////////////////////////////////////////

// Headers that define the standard C++ interfaces.
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>

// Header that defines EyeFace SDK Standard API.
#include "EyeFace.h"

// EyeFace SDK shared library filepath. EyeFace SDK has only one dependency, Openblas, to speed up computation. 
// It is loaded from system (Linux) or must be located in a directory visible to the executable - PATH directory or executable directory (Windows).
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
// Windows
std::string shlib_path = "..\\..\\eyefacesdk\\lib\\EyeFace.dll";
std::vector<std::string> support_libs = {"..\\..\\eyefacesdk\\lib\\libopenblas.dll"};
#else
// Linux
#if defined(x86_32) || defined(__i386__)
// 32 bit Linux
std::string shlib_path = "../../eyefacesdk/lib/libeyefacesdk-x86_32.so";
std::vector<std::string> support_libs = {};
#else
// 64 bit Linux
std::string shlib_path = "../../eyefacesdk/lib/libeyefacesdk-x86_64.so";
std::vector<std::string> support_libs = {};
#endif
#endif

// Path to a directory containing config.ini file and models. Config filename.
const std::string EYEFACE_DIR = "../../eyefacesdk";
const std::string CONFIG_INI  = "config.ini";

// Number of images in a sample videosequence to process.
const unsigned int NUM_IMGS = 150;

///////////////////////////////////////////////////////////
// Explicit linking of EyeFace SDK library functions.    //
// See the Developer's Guide to understand the need for  //
// explicit (also called runtime or dynamic) linking.    //
///////////////////////////////////////////////////////////

// Global pointer to EyeFace SDK shared library.
shlib_hnd shlib_handle = NULL;

// Declaration of pointers to EyeFace SDK library functions.
fcn_erImageRead                         fcnErImageRead          = NULL;
fcn_erImageFree                         fcnErImageFree          = NULL;
fcn_efInitEyeFace                       fcnEfInitEyeFace        = NULL;
fcn_efShutdownEyeFace                   fcnEfShutdownEyeFace    = NULL;
fcn_efFreeEyeFace                       fcnEfFreeEyeFace        = NULL;
fcn_efResetEyeFace                      fcnEfResetEyeFace       = NULL;
fcn_efMain                              fcnEfMain               = NULL;
fcn_efGetTrackInfo                      fcnEfGetTrackInfo       = NULL;
fcn_efFreeTrackInfo                     fcnEfFreeTrackInfo      = NULL;

// Explicit linking helper function. Returns 0 on success and non-zero on failure.
int loadSupportDll(std::vector<std::string> support_libs)
{
    for (size_t i = 0; i < support_libs.size(); i++)
    {
        shlib_hnd tmp_handle;
        ER_OPEN_SHLIB(tmp_handle, support_libs[i].c_str());

        if (!tmp_handle)
        {
            return 1;
        }
    }

    return 0;
}

int loadDll(const char * dll_filename)
{
    // Open shared library.
    ER_OPEN_SHLIB(shlib_handle, dll_filename);
    
    if (!shlib_handle)
    {
        return 1;
    }
    
    // Link efReadImage() function.
    ER_LOAD_SHFCN(fcnErImageRead, fcn_erImageRead, shlib_handle, "erImageRead");
    
    if (!fcnErImageRead)
    {
        return 2;
    }
    
    // Link efFreeImage() function.
    ER_LOAD_SHFCN(fcnErImageFree, fcn_erImageFree, shlib_handle,  "erImageFree");
    
    if (!fcnErImageFree)
    {
        return 3;
    }

    // Link efInitEyeFace() function.
    ER_LOAD_SHFCN(fcnEfInitEyeFace, fcn_efInitEyeFace, shlib_handle, "efInitEyeFace");
    
    if (!fcnEfInitEyeFace)
    {
        return 4;
    }
    
    // Link efShutdownEyeFace() function.
    ER_LOAD_SHFCN(fcnEfShutdownEyeFace, fcn_efShutdownEyeFace, shlib_handle, "efShutdownEyeFace");
    
    if (!fcnEfShutdownEyeFace)
    {
        return 5;
    }
    
    // Link efFreeEyeFace() function.
    ER_LOAD_SHFCN(fcnEfFreeEyeFace, fcn_efFreeEyeFace, shlib_handle, "efFreeEyeFace");
    
    if (!fcnEfFreeEyeFace)
    {
        return 6;
    }
    
    // Link efResetEyeFace() function.
    ER_LOAD_SHFCN(fcnEfResetEyeFace, fcn_efResetEyeFace, shlib_handle, "efResetEyeFace");
    
    if (!fcnEfResetEyeFace)
    {
        return 7;
    }
    
    // Link efMain() function.
    ER_LOAD_SHFCN(fcnEfMain, fcn_efMain, shlib_handle, "efMain");
    
    if (!fcnEfMain)
    {
        return 8;
    }
    
    // Link efGetTrackInfo() function.
    ER_LOAD_SHFCN(fcnEfGetTrackInfo, fcn_efGetTrackInfo, shlib_handle, "efGetTrackInfo");
    
    if (!fcnEfGetTrackInfo)
    {
        return 9;
    }
    
    // Link efFreeTrackInfo() function.
    ER_LOAD_SHFCN(fcnEfFreeTrackInfo, fcn_efFreeTrackInfo, shlib_handle, "efFreeTrackInfo");
    
    if (!fcnEfFreeTrackInfo)
    {
        return 10;
    }
    
    return 0;
}

// Helper function to write EfTrackInfoArray to the output stream. Writes the live track info.
void printTrackLiveInfo(EfTrackInfoArray track_info_array)
{
    std::cout << "TrackInfo output:" << std::endl;

    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
        if (track_info_array.track_info[i].status != EF_TRACKSTATUS_LIVE)
        {
            continue;
        }
        
        if (i != 0)
        {
            std::cout << std::endl;
        }
        
        int         person_id   = track_info_array.track_info[i].person_id;
        EfGender    gender      = track_info_array.track_info[i].face_attributes.gender;
        EfAge       age         = track_info_array.track_info[i].face_attributes.age;
        EfEmotion   emotion     = track_info_array.track_info[i].face_attributes.emotion;
        EfAncestry  ancestry    = track_info_array.track_info[i].face_attributes.ancestry;
        
        std::cout << " Track ID  :   " << track_info_array.track_info[i].track_id << std::endl;
        
        if (person_id == 0)
        {
            std::cout << " Person ID :   Not yet recognized." << std::endl;
        }
        else
        {
            std::cout << " Person ID :   " << person_id << std::endl;
        }
        
        if (gender.recognized == EF_FALSE)
        {
            std::cout << " Gender    :   Not yet recognized." << std::endl;
        }
        else
        {
            std::cout << " Gender    :   ";
            
            switch (gender.value)
            {
            case EF_GENDER_UNKNOWN:
                std::cout << "unknown" << std::endl;
                break;
            case EF_GENDER_MALE:
                std::cout << "male" << std::endl;
                break;
            case EF_GENDER_FEMALE:
                std::cout << "female" << std::endl;
                break;
            }
        }
        
        if (age.recognized == EF_FALSE)
        {
            std::cout << " Age       :   Not yet recognized." << std::endl;
        }
        else
        {
            std::cout << " Age       :   " << age.value << std::endl;
        }
        
        if (emotion.recognized == EF_FALSE)
        {
            std::cout << " Emotion   :   Not yet recognized." << std::endl;
        }
        else
        {
            std::cout << " Emotion   :   ";
            
            switch (emotion.value)
            {
            case EF_EMOTION_UNKNOWN:
                std::cout << "unknown" << std::endl;
                break;
            case EF_EMOTION_SMILING:
                std::cout << "smiling" << std::endl;
                break;
            case EF_EMOTION_NOTSMILING:
                std::cout << "not smiling" << std::endl;
                break;
            }
        }
        
        if (ancestry.recognized == EF_FALSE)
        {
            std::cout << " Ancestry   :   Not yet recognized." << std::endl;
        }
        else
        {
            std::cout << " Ancestry  :   ";

            switch (ancestry.value)
            {
            case EF_ANCESTRY_UNKNOWN:
                std::cout << "unknown" << std::endl;
                break;
            case EF_ANCESTRY_AFRICAN:
                std::cout << "african" << std::endl;
                break;
            case EF_ANCESTRY_ASIAN:
                std::cout << "asian" << std::endl;
                break;
            case EF_ANCESTRY_CAUCASIAN:
                std::cout << "caucasian" << std::endl;
                break;
            }
        }
    }
}

// Helper function to write EfTrackInfoArray to the output stream. Writes the finished track info.
void printTrackFinalInfo(EfTrackInfoArray track_info_array)
{
    if (track_info_array.num_tracks == 0)
    {
        return;
    }
    
    // count finished tracks
    unsigned int num_finished = 0;
    
    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
        if (track_info_array.track_info[i].status == EF_TRACKSTATUS_FINISHED)
        {
            num_finished++;
        }
    }
    
    if (num_finished == 0)
    {
        return;
    }

    std::cout << num_finished << " track(s) finished in current frame:" << std::endl;
    
    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
        EfTrackInfo & track_info = track_info_array.track_info[i];
        
        EfGender    gender      = track_info_array.track_info[i].face_attributes.gender;
        EfAge       age         = track_info_array.track_info[i].face_attributes.age;
        EfAncestry  ancestry    = track_info_array.track_info[i].face_attributes.ancestry;
        
        if (track_info_array.track_info[i].status != EF_TRACKSTATUS_FINISHED)
        {
            continue;
        }
        
        std::cout << "\tTrack ID: " << track_info.track_id << ", Person ID: " << track_info.person_id <<
            ", Start Time: " << track_info.start_time << ", Finish time: " << track_info.start_time + track_info.total_time <<
            ", Gender: ";
        
        if (gender.recognized == EF_FALSE)
        {
            std::cout << "unknown";
        }
        else
        {
            switch (gender.value)
            {
            case EF_GENDER_UNKNOWN:
                std::cout << "unknown";
                break;
            case EF_GENDER_MALE:
                std::cout << "male";
                break;
            case EF_GENDER_FEMALE:
                std::cout << "female";
                break;
            }
        }
        
        std::cout << ", Age: ";
        
        if (age.recognized == EF_FALSE)
        {
            std::cout << "unknown";
        }
        else
        {
            std::cout << age.value;
        }
        
        std::cout << ", Ancestry: ";
        
        if (ancestry.recognized == EF_FALSE)
        {
            std::cout << "unknown";
        }
        else
        {
            switch (ancestry.value)
            {
            case EF_ANCESTRY_UNKNOWN:
                std::cout << "unknown";
                break;
            case EF_ANCESTRY_AFRICAN:
                std::cout << "african";
                break;
            case EF_ANCESTRY_ASIAN:
                std::cout << "asian";
                break;
            case EF_ANCESTRY_CAUCASIAN:
                std::cout << "caucasian";
                break;
            }
        }
    }
    
    std::cout << std::endl;
}

// Helper function to write EfTrackInfoOutput to the output stream.
void printTrackInfo(EfTrackInfoArray track_info_array)
{
    printTrackLiveInfo(track_info_array);
    printTrackFinalInfo(track_info_array);
}



// This function will run during execution.
int main(int argc, char * argv[])
{
    std::cout << std::endl << "Example started." << std::endl;
    
    // Pointer to EyeFace SDK state.
    void * eyeface_state = NULL;
    
    std::cout << std::endl << "Explicit linking ... " << std::endl;
    
    // Verify the EyeFace SDK shared library file can be find. If this fails, there is an error in lib_name path.
    FILE * file = fopen(shlib_path.c_str(), "rb");
    
    if (!file)
    {
        std::cerr << "Explicit linking ... failed. EyeFace SDK shared library file not found." << std::endl;
        return 100;
    }
    
    fclose(file);
    
    // Link the EyeFace SDK library.
    int linking_error = 0;

    if ((linking_error = loadSupportDll(support_libs)) != 0)
    {
        std::cerr << "Explicit linking of support libraries failed." << std::endl;
        return linking_error;
    }
    
    if ((linking_error = loadDll(shlib_path.c_str())) != 0)
    {
        std::cerr << "Explicit linking failed. Failed to load EyeFace SDK. Code 0x" << linking_error << "." << std::endl;
        return linking_error;
    }
    
    std::cout << "Explicit linking ... done." << std::endl << std::endl;
    
    // Initialize EyeFace SDK library. Internal state is stored in memory pointed to by eyeface_state.
    std::cout << "The EyeFace SDK init ... " << std::endl;
    
    bool init_ok = fcnEfInitEyeFace(EYEFACE_DIR.c_str(), EYEFACE_DIR.c_str(), CONFIG_INI.c_str(), &eyeface_state) == EF_TRUE;

    if (!init_ok)
    {
        std::cerr << "The EyeFace SDK init ... failed." << std::endl;
        return 101;
    }
    std::cout << "The EyeFace SDK init ... done." << std::endl << std::endl;
    
    // Process the videosequence images in a loop.
    time_t processing_time = clock();
    
    for (unsigned int img_num = 0; img_num < NUM_IMGS; img_num++)
    {
        ERImage input_image;
        
        std::stringstream filename_stream;
        filename_stream << "../../data/test-images-id/" << std::setfill('0') << std::setw(4) << img_num << ".png";
        
        std::cout << std::endl << "Processing image " << img_num << "." << std::endl << "Loading image: " << filename_stream.str() << " ... " << std::endl;
        
        // Load image from file.
        if (fcnErImageRead(&input_image, filename_stream.str().c_str()) != 0)
        {
            std::cout << "Loading image: " << filename_stream.str() << " ... failed." << std::endl;
            return 102;
        }
        
        std::cout << "Loading image: " << filename_stream.str() << " ... done." << std::endl;
        
        // Process new image as a part of a videosequence.
        std::cout << "Processing image ..." << std::endl;
        
        double time = 0.04 * (double)img_num;  // This is a zero started videosequence image time.
        
        bool main_ok = fcnEfMain(input_image, NULL, time, eyeface_state) == EF_TRUE;

        if (!main_ok)
        {
            std::cerr << "Processing image: ... failed." << std::endl;
            fcnErImageFree(&input_image);
            fcnEfFreeEyeFace(&eyeface_state);
            return 103;
        }
        
        std::cout << "Processing image ... done." << std::endl;
        
        // Gather live track results of the processing. The EyeFace SDK system is internally asynchronous.
        // In Standard API you have access to the results aggregated over whole tracks in time.
        std::cout << "Gathering TrackInfo results ..." << std::endl;
        
        EfTrackInfoArray track_info_array;
        
        bool get_ti_ok = fcnEfGetTrackInfo(&track_info_array, eyeface_state) == EF_TRUE;
        
        if (!get_ti_ok) 
        {
            std::cerr << "Gathering TrackInfo results ... failed." << std::endl;
            fcnErImageFree(&input_image);
            fcnEfFreeEyeFace(&eyeface_state);
            return 104;
        }
        
        printTrackInfo(track_info_array);
        
        fcnEfFreeTrackInfo(&track_info_array, eyeface_state);
        
        std::cout << "Gathering TrackInfo results ... done." << std::endl;
        
        // Free image internal memory.
        fcnErImageFree(&input_image);
    }
    
    // Print processing time.
    processing_time = clock() - processing_time;
    std::cout << std::endl << "Processing of " << NUM_IMGS << " frames finished in " << double(processing_time) / CLOCKS_PER_SEC << " seconds." << std::endl;
    
    // Shutdown EyeFace SDK to force all tracks to finish and gather final results.
    std::cout << std::endl << "Gathering last TrackFinalInfo results ..." << std::endl;
    
    fcnEfShutdownEyeFace(eyeface_state);
    
    // get tracks finished info on tracks that were life in the last frame
    EfTrackInfoArray track_info_array; 
    
    bool get_ti_ok = fcnEfGetTrackInfo(&track_info_array, eyeface_state) == EF_TRUE;
    
    if (!get_ti_ok) 
    {
        std::cerr << "Gathering TrackInfo results ... failed." << std::endl;
        fcnEfFreeEyeFace(&eyeface_state);
        return 106;
    }
    
    printTrackInfo(track_info_array);
    
    fcnEfFreeTrackInfo(&track_info_array, eyeface_state);
    
    std::cout << "Gathering last TrackFinalInfo results ... done." << std::endl;
    
    // Free EyeFace SDK internal memory.
    std::cout << std::endl << "Freeing the EyeFace state ... " << std::endl;
    fcnEfFreeEyeFace(&eyeface_state);
    std::cout << "Freeing the EyeFace state ... done." << std::endl;
    
    std::cout << std::endl << "Example succesfully finished." << std::endl;
    
    return 0;
}
