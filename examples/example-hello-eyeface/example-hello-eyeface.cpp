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
// EyeFace SDK: example-hello-eyeface.cpp                //
// ----------------------------------------------------- //
//                                                       //
// This example shows how to use the Standard API in     //
// the simplest way. Beware that most of the error       //
// checks are omitted for simplicity. See other          //
// examples for proper error handling.                   //
//                                                       //
///////////////////////////////////////////////////////////

// Headers that define the standard C++ interfaces.
#include <iostream>
#include <string>
#include <vector>

// Header that defines EyeFace SDK Standard API.
#include "EyeFace.h"

// Path to a directory containing config.ini file and models. Config filename.
const std::string EYEFACE_DIR = "../../eyefacesdk";
const std::string CONFIG_INI = "config.ini";

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

// Global pointer to EyeFace SDK shared library.
shlib_hnd shlib_handle = NULL;

// Declaration of pointers to EyeFace SDK library functions.
fcn_erImageRead                         fcnErImageRead          = NULL;
fcn_erImageFree                         fcnErImageFree          = NULL;
fcn_efInitEyeFace                       fcnEfInitEyeFace        = NULL;
fcn_efFreeEyeFace                       fcnEfFreeEyeFace        = NULL;
fcn_efMain                              fcnEfMain               = NULL;
fcn_efGetTrackInfo                      fcnEfGetTrackInfo       = NULL;
fcn_efFreeTrackInfo                     fcnEfFreeTrackInfo      = NULL;

// Path to the test image, that will emulate 1 second of a video sequence.
const std::string IMAGE_FILENAME = "../../data/test-images-id/0000.png";

// This function will run during execution.
int main()
{
    // Load support libraries
    for (size_t i = 0; i < support_libs.size(); i++)
    {
        shlib_hnd tmp_handle;
        ER_OPEN_SHLIB(tmp_handle, support_libs[i].c_str());
    }

    // Load shared library - explicit linking.
    ER_OPEN_SHLIB(shlib_handle, shlib_path.c_str());
    
    // Library linking failed. (File not found, license invalid, or linker dependency missing.)
    if (!shlib_handle)
    {
        return -1;
    }
    
    // Get pointers to functions from loaded library.
    ER_LOAD_SHFCN(fcnErImageRead, fcn_erImageRead, shlib_handle, "erImageRead");
    ER_LOAD_SHFCN(fcnErImageFree, fcn_erImageFree, shlib_handle, "erImageFree");
    ER_LOAD_SHFCN(fcnEfInitEyeFace, fcn_efInitEyeFace, shlib_handle, "efInitEyeFace");
    ER_LOAD_SHFCN(fcnEfFreeEyeFace, fcn_efFreeEyeFace, shlib_handle, "efFreeEyeFace");
    ER_LOAD_SHFCN(fcnEfMain, fcn_efMain, shlib_handle, "efMain");
    ER_LOAD_SHFCN(fcnEfGetTrackInfo, fcn_efGetTrackInfo, shlib_handle, "efGetTrackInfo");
    ER_LOAD_SHFCN(fcnEfFreeTrackInfo, fcn_efFreeTrackInfo, shlib_handle, "efFreeTrackInfo");
    
    // Init EyeFace state.
    void * eyeface_state = NULL;
    EfBool init_success = fcnEfInitEyeFace(EYEFACE_DIR.c_str(), EYEFACE_DIR.c_str(), CONFIG_INI.c_str(), &eyeface_state);
    
    // EyeFace SDK init failed. (License is probably invalid.)
    if (init_success != EF_TRUE)
    {
        return -1;
    }
    
    // Run face detection and recognition on imaginary video sequence of 1 second with 25 fps.
    for (unsigned int i = 0; i < 25; i++)
    {
        // Load image. Imagine the sequence consists of this one image repeated several times.
        ERImage input_image;
        fcnErImageRead(&input_image, IMAGE_FILENAME.c_str());
        
        fcnEfMain(input_image, NULL, i / 25.0, eyeface_state);
        
        fcnErImageFree(&input_image);
    }
    
    // Gather EfTrackInfo's for the last image.
    EfTrackInfoArray track_info_array;
    fcnEfGetTrackInfo(&track_info_array, eyeface_state);
    
    std::cout << "Number of detected faces: " << track_info_array.num_tracks << "." << std::endl;
    
    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
            EfGender gender = track_info_array.track_info[i].face_attributes.gender;
            EfAge age       = track_info_array.track_info[i].face_attributes.age;
            
            if (gender.recognized == EF_FALSE)
            {
                std::cout << i+1 << ". Gender    :   Not yet recognized." << std::endl;
            }
            else
            {
                std::cout << i+1 << ". Gender    :   ";
                
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
                std::cout << i+1 << ". Age       :   Not yet recognized." << std::endl;
            }
            else
            {
                std::cout << i+1 << ". Age       :   " << age.value << std::endl;
            }
    }
    
    // Release all memory.
    fcnEfFreeTrackInfo(&track_info_array, eyeface_state);
    
    fcnEfFreeEyeFace(&eyeface_state);
    
    return 0;
}
