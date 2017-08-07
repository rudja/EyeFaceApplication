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
// EyeFace SDK: example-expert-api.cpp                   //
// ----------------------------------------------------- //
//                                                       //
// This example shows how to use the Expert API          //
// interface. It requires the Expert license             //
// to run. A videosequence decomposed to individual      //
// images is processed. Faces are tracked, and their     //
// attributes like age, gender, emotions and unique      //
// counting identifier are computed on per frame basis.  //
// Expert API is suitable for processing of image        //
// databases, where each image is independent.           //
// It can also be used if you need a precise face        //
// detection, not the aggregated one from Standard API.  //
//                                                       //
///////////////////////////////////////////////////////////

// Headers that define the standard C++ interfaces.
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>
//#include <cstdio>

// Header that defines EyeFace SDK Expert API.
#include "EyeFaceExpert.h"

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

// List of testing images
const int NUM_IMAGES = 4;
std::string test_images[NUM_IMAGES] = { "../../data/test-images-id/0000.png", "../../data/eyeface-color.jpg", "../../data/test-images-id/0037.png", "../../data/sample_image.jpg" };

///////////////////////////////////////////////////////////
// Explicit linking of EyeFace SDK library functions.    //
// See the Developer's Guide to understand the need for  //
// explicit (also called runtime or dynamic) linking.    //
///////////////////////////////////////////////////////////

// Global pointer to EyeFace SDK shared library.
shlib_hnd shlib_handle = NULL;

// Declaration of pointers to EyeFace SDK library functions.
fcn_erImageRead                         fcnErImageRead                  = NULL;
fcn_erImageFree                         fcnErImageFree                  = NULL;
fcn_efInitEyeFace                       fcnEfInitEyeFace                = NULL;
fcn_efShutdownEyeFace                   fcnEfShutdownEyeFace            = NULL;
fcn_efFreeEyeFace                       fcnEfFreeEyeFace                = NULL;
fcn_efRunFaceDetector                   fcnEfRunFaceDetector            = NULL;
fcn_efFreeDetections                    fcnEfFreeDetections             = NULL;
fcn_efRunFaceLandmark                   fcnEfRunFaceLandmark            = NULL;
fcn_efFreeLandmarks                     fcnEfFreeLandmarks              = NULL;
fcn_efRecognizeFaceAttributes           fcnEfRecognizeFaceAttributes    = NULL;
fcn_efFreeAttributes                    fcnEfFreeAttributes             = NULL;

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

// Explicit linking helper function. Returns 0 on success and non-zero on failure.
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
    
    // Link efRunFaceDetector() function.
    ER_LOAD_SHFCN(fcnEfRunFaceDetector, fcn_efRunFaceDetector, shlib_handle, "efRunFaceDetector");
    
    if (!fcnEfRunFaceDetector)
    {
        return 7;
    }
    
    // Link efFreeDetections() function.
    ER_LOAD_SHFCN(fcnEfFreeDetections, fcn_efFreeDetections, shlib_handle, "efFreeDetections");
    
    if (!fcnEfFreeDetections)
    {
        return 8;
    }
    
    // Link efRunFaceLandmark() function.
    ER_LOAD_SHFCN(fcnEfRunFaceLandmark, fcn_efRunFaceLandmark, shlib_handle, "efRunFaceLandmark");
    
    if (!fcnEfRunFaceLandmark)
    {
        return 9;
    }
    
    // Link efFreeLandmarks() function.
    ER_LOAD_SHFCN(fcnEfFreeLandmarks, fcn_efFreeLandmarks, shlib_handle, "efFreeLandmarks");
    
    if (!fcnEfFreeLandmarks)
    {
        return 10;
    }
    
    // Link efRecognizeFaceAttributes() function.
    ER_LOAD_SHFCN(fcnEfRecognizeFaceAttributes, fcn_efRecognizeFaceAttributes, shlib_handle, "efRecognizeFaceAttributes");
    
    if (!fcnEfRecognizeFaceAttributes)
    {
        return 11;
    }
    
    // Link efFreeAttributes() function.
    ER_LOAD_SHFCN(fcnEfFreeAttributes, fcn_efFreeAttributes, shlib_handle, "efFreeAttributes");
    
    if (!fcnEfFreeAttributes)
    {
        return 12;
    }
    
    return 0;
}



// This function will run during execution.
int main (int argc, char * argv[])
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
    
    // process image database
    for (size_t img_i = 0; img_i < NUM_IMAGES; img_i++)
    {
        ERImage input_image;
        
        // load image
        std::string filename = test_images[img_i];
        std::cout << "Loading image ... " << img_i << ": " << filename << "... " << std::endl;
        if (fcnErImageRead(&input_image, filename.c_str()) != 0)
        {
            std::cerr << "Can't load the image file!" << std::endl;
            return 105;
        }
        std::cout << "Loading image ... done." << std::endl;
        
        // run face detector
        std::cout << "Running face detector ... " << std::endl;
        EfDetectionArray detection_array;
        
        EfBool run_ok = fcnEfRunFaceDetector(input_image, &detection_array, eyeface_state);
        if (run_ok == EF_FALSE)
        {
            std::cerr << "Can't run the face detector!" << std::endl;
            return 106;
        }
        std::cout << "Running face detector ... done." << std::endl;;
        
        // run face landmark - disabled by default in config ini!
        // std::cout << "Running face landmark ... " << std::endl;
        // EfLandmarksArray landmarks_array;
        //
        // EfBool get_ok = fcnEfRunFaceLandmark(input_image, detection_array, NULL, &landmarks_array, eyeface_state);
        //
        // if (get_ok != EF_TRUE)
        // {
        //     std::cerr << "Can't run the landmark detector!" << std::endl;
        //     return 108;
        // }
        // std::cout << "Running face landmark ... done." << std::endl;
        // fcnEfFreeLandmarks(&landmarks_array, eyeface_state);

        // result for the given database image
        std::cout << "Result for image " << filename << ":" << std::endl;
        
        // run attribute recognition in synchronous mode
        EfFaceAttributesArray face_attributes_array;
        EfBool attributes_ok = fcnEfRecognizeFaceAttributes(input_image, detection_array, NULL, NULL, EF_FACEATTRIBUTES_ALL, 0, EF_TRUE, &face_attributes_array, eyeface_state);
        
        if (attributes_ok == EF_FALSE)
        {
            std::cerr << "Can't compute face attributes!" << std::endl;
            return 1061;
        }
        
        for (unsigned int i = 0; i < detection_array.num_detections; i++)
        {
            EfFaceAttributes& face_attributes = face_attributes_array.face_attributes[i];
            std::cout << "\t Face IDX=" << i << ":" << std::endl;

            EfAge age           = face_attributes.age;
            EfGender gender     = face_attributes.gender;
            EfEmotion emotion   = face_attributes.emotion;
            EfAncestry ancestry = face_attributes.ancestry;

            std::ostringstream s;
            s << age.value;

            std::cout << "\t\t location: [" << detection_array.detections[i].position.center_col << ","
                                            << detection_array.detections[i].position.center_row << "]" << std::endl;
            std::cout << "\t\t age: " << (age.recognized == EF_TRUE ? s.str() : std::string("not recognized")) << std::endl;
            std::cout << "\t\t gender: " << (gender.recognized == EF_TRUE ?
                                            gender.value == EF_GENDER_FEMALE ? std::string("female") :
                                            gender.value == EF_GENDER_MALE ? std::string("male") : std::string("unknown")
                                            : std::string("not recognized")) << std::endl;
            std::cout << "\t\t emotion: " << (emotion.recognized == EF_TRUE ? 
                emotion.value == EF_EMOTION_SMILING ? std::string("smiling") :
                emotion.value == EF_EMOTION_NOTSMILING ? std::string("not smiling") : std::string("unknown")
                : std::string("not recognized")) << std::endl;
            std::cout << "\t\t ancestry: " << (ancestry.recognized == EF_TRUE ?
                                               ancestry.value == EF_ANCESTRY_CAUCASIAN ? std::string("caucasian") :
                                               ancestry.value == EF_ANCESTRY_ASIAN ? std::string("asian") :
                                               ancestry.value == EF_ANCESTRY_AFRICAN ? std::string("african") : std::string("unknown"):
                                               std::string("not recognized")) << std::endl;
        }
        
        fcnEfFreeAttributes(&face_attributes_array, eyeface_state);
        
        //free detections
        fcnEfFreeDetections(&detection_array, eyeface_state);
        // free image
        fcnErImageFree(&input_image);
    }
    
    // Free EyeFace SDK internal memory.
    std::cout << std::endl << "Freeing the EyeFace state ... " << std::endl;
    fcnEfShutdownEyeFace(eyeface_state);
    fcnEfFreeEyeFace(&eyeface_state);
    std::cout << "Freeing the EyeFace state ... done." << std::endl;
    
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
    
    return 0;
}
