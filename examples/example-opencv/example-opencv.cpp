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
// EyeFace SDK: example-opencv.cpp                       //
// ----------------------------------------------------- //
//                                                       //
// This example uses OpenCV library to grab image        //
// from a USB camera and show real-time evaluation       //
// of face attributes using EyeFace SDK. See             //
// the Developer's Guide on how to build OpenCV library  //
// on your machine. Requires OpenCV 3.x.                 //
//                                                       //
// Press                                                 //
//   'F' to toggle displaying of fps on/off.             //
//   'L' to toggle displaying of landmark points on/off. //
//       (Landmarks are off by default in config.ini.)   //
//   'A' to show attention.                              //
//                                                       //
///////////////////////////////////////////////////////////

// Headers that define the standard C++ interfaces.
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

//For use rapidjson library easier
using namespace rapidjson;
using namespace std;

// Header that defines EyeFace SDK Standard API.
#include "EyeFace.h"

// Hides MSVC's min and max macros.
#define NOMINMAX

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

// Header that defines OpenCV API.
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Set default camera capture options, image width and height and also horizontal flip of input image.
const bool HORIZONTAL_FLIP = true;
const int IMG_WIDTH        = 640;
const int IMG_HEIGHT       = 480;

// Path to a directory containing config.ini file and models. Config filename.
const std::string EYEFACE_DIR = "../../eyefacesdk";
const std::string CONFIG_INI = "config.ini";

// Global switches changing appearance.
bool show_fps = false;
bool show_landmarks = false;    // Landmarks are off by default in config.ini.
bool show_attention = false;

// Get time in milisecond precision in pre-C++11 compilers.
#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS_)
// Windows
#include <windows.h>
static double myGetTime()
{
    return GetTickCount()/1000.;
}
#elif defined(__MACH__)
// macOS
#include <time.h>
#include <mach/mach.h>
#include <mach/clock.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
static double myGetTime()
{
    struct timespec ts;
    clock_serv_t cclock;
    mach_timespec_t mts;
    kern_return_t err_code = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    if (err_code != 0)
    {
        return -1.0;
    }
    
    err_code = clock_get_time(cclock, &mts);
    
    if (err_code != 0)
    {
        return -1.0;
    }
    
    err_code = mach_port_deallocate(mach_task_self(), cclock);
    
    if (err_code != 0)
    {
        return -1.0;
    }
    
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    
    return (double)((1000*1000*1000) * ts.tv_sec + ts.tv_nsec) / (1000.*1000.*1000.);
}
#else
// Linux
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
static double myGetTime()
{
   struct timespec ts;
   
   if (!clock_gettime(CLOCK_REALTIME, &ts))
   {
      return (double)((1000*1000*1000) * ts.tv_sec + ts.tv_nsec) / (1000.*1000.*1000.);
   }
   
   return -1.;
}
#endif

///////////////////////////////////////////////////////////
// Explicit linking of EyeFace SDK library functions.    //
// See the Developer's Guide to understand the need for  //
// explicit (also called runtime or dynamic) linking.    //
///////////////////////////////////////////////////////////

// Global pointer to EyeFace SDK shared library.
shlib_hnd shlib_handle = NULL;

// Declaration of pointers to EyeFace SDK library functions.
fcn_erImageRead                         fcnErImageRead          = NULL;
fcn_erImageAllocate                     fcnErImageAllocate      = NULL;
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
    
    // Link efReadImage() function.
    ER_LOAD_SHFCN(fcnErImageAllocate, fcn_erImageAllocate, shlib_handle, "erImageAllocate");
    
    if (!fcnErImageAllocate)
    {
        return 3;
    }
    
    // Link efFreeImage() function.
    ER_LOAD_SHFCN(fcnErImageFree, fcn_erImageFree, shlib_handle,  "erImageFree");
    
    if (!fcnErImageFree)
    {
        return 4;
    }
    
    // Link efInitEyeFace() function.
    ER_LOAD_SHFCN(fcnEfInitEyeFace, fcn_efInitEyeFace, shlib_handle, "efInitEyeFace");
    
    if (!fcnEfInitEyeFace)
    {
        return 5;
    }
    
    // Link efShutdownEyeFace() function.
    ER_LOAD_SHFCN(fcnEfShutdownEyeFace, fcn_efShutdownEyeFace, shlib_handle, "efShutdownEyeFace");
    
    if (!fcnEfShutdownEyeFace)
    {
        return 6;
    }
    
    // Link efFreeEyeFaceState() function.
    ER_LOAD_SHFCN(fcnEfFreeEyeFace, fcn_efFreeEyeFace, shlib_handle, "efFreeEyeFace");
    
    if (!fcnEfFreeEyeFace)
    {
        return 7;
    }
    
    // Link efResetEyeFaceState() function.
    ER_LOAD_SHFCN(fcnEfResetEyeFace, fcn_efResetEyeFace, shlib_handle, "efResetEyeFace");
    
    if (!fcnEfResetEyeFace)
    {
        return 8;
    }
    
    // Link efMain() function.
    ER_LOAD_SHFCN(fcnEfMain, fcn_efMain, shlib_handle, "efMain");
    
    if (!fcnEfMain)
    {
        return 9;
    }
    
    // Link efGetTrackInfo() function.
    ER_LOAD_SHFCN(fcnEfGetTrackInfo, fcn_efGetTrackInfo, shlib_handle, "efGetTrackInfo");

    if (!fcnEfGetTrackInfo)
    {
        return 10;
    }
    
    // Link efFreeTrackInfo() function.
    ER_LOAD_SHFCN(fcnEfFreeTrackInfo, fcn_efFreeTrackInfo, shlib_handle, "efFreeTrackInfo");

    if (!fcnEfFreeTrackInfo)
    {
        return 11;
    }
    
    return 0;
}

// Conversion of cv::Mat to ErImage.
bool convertCvImage2ERImage(const cv::Mat & cv_image, ERImage& er_image)
{
    ERImageColorModel color_model;
    ERImageDataType   data_type;
    
    unsigned int depth = cv_image.type() & CV_MAT_DEPTH_MASK;
    unsigned int num_channels = 1 + (cv_image.type() >> CV_CN_SHIFT);
    
    if (depth == CV_8U && num_channels == 3)
    {
        color_model = ER_IMAGE_COLORMODEL_BGR;
        data_type   = ER_IMAGE_DATATYPE_UCHAR; 
    } else if (depth == CV_8U && num_channels == 1)
    {
        color_model = ER_IMAGE_COLORMODEL_GRAY;
        data_type = ER_IMAGE_DATATYPE_UCHAR;
    }
    else
    {
        return false;
    }
    
    fcnErImageAllocate(&er_image, cv_image.size().width, cv_image.size().height, color_model, data_type);
    
    // copy opencv image to efimage
    for (int i = 0; i < cv_image.size().height; i++)
    {
        for (int j = 0; j < cv_image.size().width; j++)
        {
            if (num_channels == 3)
            {
                er_image.row_data[i][3 * j + 0] = cv_image.at<cv::Vec3b>(i, j)[0];
                er_image.row_data[i][3 * j + 1] = cv_image.at<cv::Vec3b>(i, j)[1];
                er_image.row_data[i][3 * j + 2] = cv_image.at<cv::Vec3b>(i, j)[2];
            }
            else
            {
                er_image.row_data[i][j] = cv_image.at<unsigned char>(i, j);
            }
        }
    }
    
    return true;
}

// Forward definition of function.
void drawVisuals(cv::Mat & image, EfTrackInfoArray track_info_array, double fps);

string maleOrFemaleFunct(double response);
string ancestryFunct(int response);


// This function will run during execution.
int main(int argc, char * argv[])
{
    // Initialize OpenCV camera capture.
    cv::VideoCapture capture(0);
    
    if (!capture.isOpened())
    {
        std::cout << "ERROR -100: Failed to open the camera capture device." << std::endl;
        return -1;
    }
    
    // Try to set the camera parameters.
    capture.set(CV_CAP_PROP_FRAME_WIDTH, IMG_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, IMG_HEIGHT);
    capture.set(CV_CAP_PROP_FPS, 30);
    
    // Set presentation window name.
    std::string window_name = "EyeFace OpenCV Example";
    
    // Prepare image buffers for camera capture.
    cv::Mat grabbed_image, buffer_image;
    
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
        std::cerr << "Explicit linking failed. Failed to load EyeFace SDK." << std::endl;
        
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
    
    // Initialize timers to get timestamps of individual images.
    double start_time = myGetTime();
    double time = start_time, prev_time = start_time, fps = 0.0;
    
    // Create GUI window.
    //cvNamedWindow(window_name.c_str(), 1);
    
    while (true)
    {
        // If you can't get the frame time from a video source, then you can set the timestamp of grabbing.
        prev_time = time;
        time = myGetTime();
        fps = 1.0 / (time - prev_time);
        
        // Grab frame from camera.
        if (!capture.read(grabbed_image))
        {
            std::cerr << "ERROR: Cannot grab image!" << std::endl;
            break;
        }
        
        buffer_image = grabbed_image.clone();
        
        // Mirror image if required.
        if (HORIZONTAL_FLIP)
        {
            cv::flip(buffer_image, buffer_image, 1);
        }
        
        // Convert cv::Mat image to EfImage format. See the function body to understand EyeFace SDK's image structure.
        ERImage input_image;
        
        bool conversion_ok = convertCvImage2ERImage(buffer_image, input_image);
        
        if (!conversion_ok)
        {
            std::cerr << "ERROR: Running convertCvImage2ERImage() failed." << std::endl;
            break;
        }
        
        // Process new image as a part of a videosequence.
        bool main_ok = fcnEfMain(input_image, NULL, time - start_time, eyeface_state) == EF_TRUE;
        
        if (!main_ok)
        {
            std::cerr << "ERROR: Running efMain() failed." << std::endl;
            break;
        }
        
        // Free image internal memory.
        fcnErImageFree(&input_image);
        
        // get EfTrackInfo information
        EfTrackInfoArray track_info_array;
        
        bool get_ti_ok = fcnEfGetTrackInfo(&track_info_array, eyeface_state) == EF_TRUE;
        
        if (!get_ti_ok)
        {
            printf("Running efGetTrackInfo() failed.\n");
            continue;
        }

		// For every track, get the information (age, gender, emotion, ancestry)
		for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
		{
			EfTrackInfo * track_info = &(track_info_array.track_info[i]);

			//All verification before sending any data to server
			if (track_info->face_attributes.gender.recognized == EF_TRUE && track_info->face_attributes.age.recognized && track_info->face_attributes.emotion.recognized && track_info->face_attributes.ancestry.recognized)
			{
				if (track_info->person_id != 0) {
					std::cout << "PersonId?" << track_info->person_id << std::endl;						//0 if not known yet, anything else if known. 

					//Verification of the gender
					string gender = maleOrFemaleFunct(track_info->face_attributes.gender.response);
					std::cout << "Male or female?" << gender << std::endl;

					std::cout << "Age?" << track_info->face_attributes.age.value << std::endl;			//Can be change if new detection
					std::cout << "Emotion?" << track_info->face_attributes.emotion.value << std::endl;	//Can be change if new detection
					
					//Verification of the ancestor
					//I casted the value into a int because switch function do not accept double for comparaison
					string ancestry = ancestryFunct(static_cast<int>(track_info->face_attributes.ancestry.value));
					std::cout << "Ancestry?" << ancestry << std::endl;
					std::cout << "Attention_time?" << track_info->attention_time << std::endl;			//Attention time of the person in second
				}
				else {
					std::cout << "New person detected!" << std::endl;
				}
			}
			else 
			{
				std::cout << "Values not set yet" << std::endl;
			}

			//Format data into a json object
			/*StringBuffer s;
			Writer<StringBuffer> writer(s);

			writer.StartObject();               // Between StartObject()/EndObject(), 
			writer.Key("PersonId");                // output a key,
			writer.Uint(track_info->person_id);
			writer.Key("Gender");
			writer.Bool(true);
			writer.Key("Age");
			writer.Bool(false);
			writer.Key("Emotion");
			writer.Null();
			writer.Key("Ancestry");
			writer.Uint(123);
			writer.Key("Attention_time");
			writer.Double(3.1416);
			writer.EndObject();

			// {"hello":"world","t":true,"f":false,"n":null,"i":123,"pi":3.1416,"a":[0,1,2,3]}
			cout << s.GetString() << endl;*/
		}

        
        // Draw EfTrackInfo to the image.
        //drawVisuals(buffer_image, track_info_array, fps);
        
        // Show image with visualizations.
        //cv::imshow(window_name, buffer_image);
        
        // tidy up the visuals
        //fcnEfFreeTrackInfo(&track_info_array, eyeface_state);
        
        // Wait for key press and quit / customize GUI based on key.
        /*char key = cvWaitKey(1);
        
        // Quit.
        //if (key == 27)
        {
            std::cout << "Key ESC pressed." << std::endl;
            break;
        }
        
        switch (key)
        {
            // Turn on / off fps.
        case 'F':
        case 'f':
            show_fps = !show_fps;
            break;
            // Turn on / off landmark visualisation. (Must be enabled in config.ini as well.)
        case 'L':
        case 'l':
            show_landmarks = !show_landmarks;
            break;
            // Turn on / off attention visualisation (green).
        case 'A':
        case 'a':
            show_attention = !show_attention;
            break;
        }
		*/
    }
    
    std::cout << "Exiting - shutting down." << std::endl;
    
    // Shutdown EyeFace SDK to force all tracks to finish.
    fcnEfShutdownEyeFace(eyeface_state);
    
    // Free EyeFace SDK internal memory.
    std::cout << std::endl << "Freeing the EyeFace state ... " << std::endl;
    fcnEfFreeEyeFace(&eyeface_state);
    std::cout << "Freeing the EyeFace state ... done." << std::endl;
    
    // Close OpenCV window.
    //cv::destroyWindow(window_name);
    
    std::cout << std::endl << "Example succesfully finished." << std::endl;
    
    return 0;
}

// Draw TrackInfo to image.
void drawVisuals(cv::Mat & image, EfTrackInfoArray track_info_array, double fps)
{
    // Setup fonts.
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 0.3;
    cv::Size font_size;
    
    // For every track, draw the information.
    for (unsigned int i = 0; i < track_info_array.num_tracks; i++)
    {
        EfTrackInfo * track_info = &(track_info_array.track_info[i]);
        
        int tl_c, tl_r, tr_c, tr_r, bl_c, bl_r, br_c, br_r;
        tl_r = track_info->image_position.top_left_row;
        tl_c = track_info->image_position.top_left_col;
        tr_r = track_info->image_position.top_right_row;
        tr_c = track_info->image_position.top_right_col;
        bl_r = track_info->image_position.bot_left_row;
        bl_c = track_info->image_position.bot_left_col;
        br_r = track_info->image_position.bot_right_row;
        br_c = track_info->image_position.bot_right_col;
        
        // Male / female color coding based on response strength.
        int R = 220,  G = 220, B = 220;
        int Rf = 255, Gf = 0,  Bf = 255; // RGB 100% female
        int Rm = 0,   Gm = 0,  Bm = 255; // RGB 100% male
        
        if (track_info->face_attributes.gender.recognized == EF_TRUE)
        {
            if (track_info->face_attributes.gender.response < 0)
            {
                double a = -track_info->face_attributes.gender.response;
                R = (int) ( (1.0-a) * 220 + a * Rm );
                G = (int) ( (1.0-a) * 220 + a * Gm );
                B = (int) ( (1.0-a) * 220 + a * Bm );
            }
            else
            {
                double a = track_info->face_attributes.gender.response;
                R = (int) ( (1.0-a) * 220 + a * Rf );
                G = (int) ( (1.0-a) * 220 + a * Gf );
                B = (int) ( (1.0-a) * 220 + a * Bf );
            }
        }
        
        // Draw rectangle around the face - roll and yaw supported.
        double angle = track_info->angles[2];
        angle = std::min(angle, 90.0);
        angle = std::max(angle, -90.0);
        double width = sqrt((double)((tl_r - tr_r)*(tl_r - tr_r) + (tl_c - tr_c)*(tl_c - tr_c)));
        double offset_l = 1.0/4.0 * angle / 90.0 * width - width/2.0;
        double offset_r = 1.0/4.0 * angle / 90.0 * width + width/2.0;
        double unit_vec_c = (tr_c-tl_c)/width, unit_vec_r = (tr_r - tl_r)/width;
        double center_c = (tl_c+br_c)/2.0, center_r = (tl_r+br_r)/2.0;
        
        cv::Point * curve_face[1];
        cv::Point face_det[6];
        face_det[0] = cv::Point(tl_c, tl_r);
        face_det[1] = cv::Point(tr_c, tr_r);
        face_det[2] = cv::Point(int(center_c + offset_r*unit_vec_c), int(center_r+offset_r*unit_vec_r));
        face_det[3] = cv::Point(br_c, br_r);
        face_det[4] = cv::Point(bl_c, bl_r);
        face_det[5] = cv::Point(int(center_c + offset_l*unit_vec_c), int(center_r + offset_l*unit_vec_r));
        
        curve_face[0] = face_det;
        int num_face_points[1] = {6};
        cv::polylines(image, curve_face, num_face_points, 1, true, CV_RGB(R, G, B), 3);
        
        // Visualize attention by setting the bounding box to green if the person is in attention.
        if (show_attention)
        {
            if (track_info->attention_now)
            {
                cv::Point * curves[1];
                cv::Point attention_bb[4];
                
                attention_bb[0] = cv::Point(tl_c, tl_r);
                attention_bb[1] = cv::Point(tr_c, tr_r);
                attention_bb[2] = cv::Point(br_c, br_r);
                attention_bb[3] = cv::Point(bl_c, bl_r);
                
                curves[0] = attention_bb;
                
                int ncurvepoints[1] = { 4 };
                
                cv::polylines(image, curves, ncurvepoints, 1, 1, CV_RGB(0, 255, 0), 3);
            }
        }
        
        // Visualize TrackInfo recognition results rectangle.
        int b_face = std::max(tl_r, std::max(tr_r, std::max(bl_r, br_r)));
        int l_face = std::min(tl_c, std::min(tr_c, std::min(bl_c, br_c)));
        int r_face = std::max(tl_c, std::max(tr_c, std::max(bl_c, br_c)));
        
        int horizontal_offset = 0;
        int bck_frame_width = (int)std::max(130.0, r_face-l_face+1.0);
        int bck_frame_height = 30;
        int gen_frame_width = bck_frame_width - 10;
        int gen_frame_height = 10;
        
        int baseline;
        int offset_yaw = 0 ? 14 + gen_frame_height : 0;
        int frame_height = bck_frame_height+50+offset_yaw;
        
        int left_b = std::max(0, l_face);
        int right_b = std::min(image.size().width, l_face+bck_frame_width);
        int top_b = std::max(0, b_face);
        int bottom_b = std::min(image.size().height - 5 - frame_height, b_face);
        int width_diff = std::min(l_face-left_b, right_b-(l_face+bck_frame_width));
        int height_diff = std::min(b_face-top_b, bottom_b-b_face);
        
        // Draw shaded rectangle under the information pane.
        if (bottom_b + 5 < image.size().width && height_diff > - frame_height)
        {
            cv::Rect roi_rect = cv::Rect(left_b, top_b + 5, bck_frame_width + width_diff, frame_height + height_diff);
            cv::Mat roi = image(roi_rect);
            roi = 0.4 * roi;
        }
        
        // Draw gender recognition results.
        bool show_gender_confidence = true;
        if (show_gender_confidence)
        {
            horizontal_offset = bck_frame_height;
            
            CvPoint gen_frame_tl = cvPoint(l_face + 5,  b_face + 7);
            CvPoint gen_frame_br = cvPoint(gen_frame_tl.x + gen_frame_width,
                                        gen_frame_tl.y + gen_frame_height);
            
            double pos = 0.5 * (track_info->face_attributes.gender.response + 1);
            
            CvPoint gen_conf_tl = cvPoint(gen_frame_tl.x + (int)(pos * (gen_frame_width - 6)),
                                        gen_frame_tl.y + 1);
            CvPoint gen_conf_br = cvPoint(gen_conf_tl.x + 4,
                                        gen_conf_tl.y + gen_frame_height - 2);
            
            int gen_center_x = gen_frame_tl.x + gen_frame_width/2;
            CvPoint gen_ignore_tl = cvPoint(gen_center_x - gen_frame_width/10,
                                           gen_frame_tl.y);
            CvPoint gen_ignore_br = cvPoint(gen_center_x + gen_frame_width/10,
                                          gen_frame_br.y);
            
            cv::rectangle(image, gen_ignore_tl, gen_ignore_br, CV_RGB(255,255,255), 1);
            
            if (track_info->face_attributes.gender.recognized == EF_TRUE)
            {
                cv::rectangle(image, gen_conf_tl, gen_conf_br, CV_RGB(255, 255, 255), CV_FILLED);
            }
            
            cv::rectangle(image, gen_frame_tl, gen_frame_br, CV_RGB(255,255,255), 1);
            
            font_size = cv::getTextSize("FEMALE", font_face, font_scale, 1, &baseline);
            cv::putText(image, "MALE", cvPoint(gen_frame_tl.x - 1, gen_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(100, 100, 100), 2);
            cv::putText(image, "MALE", cvPoint(gen_frame_tl.x - 1, gen_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255), 1);
            cv::putText(image, "FEMALE", cvPoint(gen_frame_br.x - 1 - font_size.width, gen_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(100, 100, 100), 2);
            cv::putText(image, "FEMALE", cvPoint(gen_frame_br.x - 1 - font_size.width, gen_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255), 1);
        }
        
        // Draw age recognition results.
        bool show_age_response = true;
        if (show_age_response)
        {
            CvPoint age_frame_tl = cvPoint(l_face + 5, b_face + horizontal_offset + 2);
            CvPoint age_frame_br = cvPoint(age_frame_tl.x + gen_frame_width, age_frame_tl.y + gen_frame_height);
            double pos = (double)track_info->face_attributes.age.value / 80.0;
            
            CvPoint age_conf_tl = cvPoint(age_frame_tl.x + (int)(pos * (gen_frame_width - 6)),
                                        age_frame_tl.y + 1);
            CvPoint age_conf_br = cvPoint(age_conf_tl.x + 4,
                                        age_conf_tl.y + gen_frame_height - 2);
            
            CvPoint pt20_1    = cvPoint(age_frame_tl.x + (int)(20.0/80.0 * (gen_frame_width -4)),
                                        age_frame_tl.y+1);
            CvPoint pt20_2    = cvPoint(age_frame_tl.x + (int)(20.0/80.0 * (gen_frame_width -4)),
                                        age_frame_br.y-1);
            CvPoint pt40_1    = cvPoint(age_frame_tl.x + (int)(40.0/80.0 * (gen_frame_width -4)),
                                        age_frame_tl.y+1);
            CvPoint pt40_2    = cvPoint(age_frame_tl.x + (int)(40.0/80.0 * (gen_frame_width -4)),
                                        age_frame_br.y-1);
            CvPoint pt60_1    = cvPoint(age_frame_tl.x + (int)(60.0/80.0 * (gen_frame_width -4)),
                                        age_frame_tl.y+1);
            CvPoint pt60_2    = cvPoint(age_frame_tl.x + (int)(60.0/80.0 * (gen_frame_width -4)),
                                        age_frame_br.y-1);
            
            cv::line(image,pt20_1,pt20_2,CV_RGB(200, 200, 200));
            cv::line(image,pt40_1,pt40_2,CV_RGB(200, 200, 200));
            cv::line(image,pt60_1,pt60_2,CV_RGB(200, 200, 200));
            
            if (track_info->face_attributes.age.recognized == EF_TRUE)
            {
                cv::rectangle(image, age_conf_tl, age_conf_br, CV_RGB(255, 255, 255), CV_FILLED);
            }
            cv::rectangle(image, age_frame_tl, age_frame_br, CV_RGB(255,255,255), 1);
            
            font_size = cv::getTextSize("0", font_face, font_scale, 1, &baseline);
            cv::putText(image, "0", cvPoint(age_frame_tl.x, age_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255));
            font_size = cv::getTextSize("2", font_face, font_scale, 1, &baseline);
            cv::putText(image, "20", cvPoint(pt20_1.x - font_size.width, age_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255));
            font_size = cv::getTextSize("4", font_face, font_scale, 1, &baseline);
            cv::putText(image, "40", cvPoint(pt40_1.x - font_size.width, age_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255));
            font_size = cv::getTextSize("6", font_face, font_scale, 1, &baseline);
            cv::putText(image, "60", cvPoint(pt60_1.x - font_size.width, age_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255));
            font_size = cv::getTextSize("80", font_face, font_scale, 1, &baseline);
            cv::putText(image, "80", cvPoint(age_frame_br.x - font_size.width, age_frame_br.y + font_size.height + 1), font_face, font_scale, CV_RGB(255, 255, 255));
        }
        
        // Draw Track ID and Person ID recognition results.
        bool show_id_response = true;
        if (show_id_response)
        {
            char id_text[16];
            
            CvPoint age_frame_tl = cvPoint(l_face + 5, b_face + horizontal_offset +offset_yaw);
            CvPoint age_frame_br = cvPoint(age_frame_tl.x + gen_frame_width, age_frame_tl.y + gen_frame_height);
            
            sprintf(id_text, "TID: %d", track_info->track_id);
            
            font_size = cv::getTextSize(id_text, font_face, font_scale, 1, &baseline);
            cv::putText(image, id_text, cvPoint(age_frame_tl.x, age_frame_br.y + 2 * font_size.height + 11), font_face, font_scale, CV_RGB(255, 0, 0));
            
            if (track_info->person_id)
            {
                sprintf(id_text, "PID: %d", track_info->person_id);
            }
            else
            {
                sprintf(id_text, "PID: UNK");
            }
            
            font_size = cv::getTextSize(id_text, font_face, font_scale, 1, &baseline);
            cv::putText(image, id_text, cvPoint(age_frame_br.x - font_size.width, age_frame_br.y + 2 * font_size.height + 11), font_face, font_scale, CV_RGB(255, 0, 0));
        }
        
        // distance depends linearly on tan(FOV/2), field of view (FOV) is a parameter of camera         //
        // Default is 78 degrees horizontally, if your camera differs, change it in the config.ini file. //
        bool show_distance = true;
        if (show_distance)
        {
            CvPoint age_frame_tl = cvPoint(l_face + 5, b_face + horizontal_offset + 13 + offset_yaw);
            CvPoint age_frame_br = cvPoint(age_frame_tl.x + gen_frame_width, age_frame_tl.y + gen_frame_height);
            char dist_text[16];
            
            // compute Euclidean distance
            double distance = sqrt(track_info->world_position[0] * track_info->world_position[0] + track_info->world_position[1] * track_info->world_position[1]);
            
            if (!distance)
            {
                sprintf(dist_text, "---");
            }
            else
            {
                sprintf(dist_text, "DIST: %2.1fm", distance);
            }
            
            font_size = cv::getTextSize(dist_text, font_face, font_scale, 1, &baseline);
            cv::putText(image, dist_text, cvPoint(age_frame_br.x - font_size.width, age_frame_br.y + 2 * font_size.height + 11), font_face, font_scale, CV_RGB(255, 0, 0));
        }
        
        // Draw smile recognition results.
        bool show_smile = true;
        if (show_smile)
        {
            CvPoint age_frame_tl = cvPoint(l_face + 5, b_face + horizontal_offset + 13 + offset_yaw);
            CvPoint age_frame_br = cvPoint(age_frame_tl.x + gen_frame_width, age_frame_tl.y + gen_frame_height);
            char dist_text[16];
            
            if (track_info->face_attributes.emotion.recognized != EF_TRUE)
            {
                sprintf(dist_text, "SMILE: UNK");
            }
            else
            {
                sprintf(dist_text, "SMILE: %0.0f%%", 100.0*track_info->face_attributes.emotion.response);
            }
            
            font_size = cv::getTextSize(dist_text, font_face, font_scale, 1, &baseline);
            cv::putText(image, dist_text, cvPoint(age_frame_tl.x, age_frame_br.y + 2 * font_size.height + 11), font_face, font_scale, CV_RGB(255, 0, 0));
        }
         
        // Draw landmark recognition results. Landmark are disabled by default in config.ini.
        if (show_landmarks && track_info->landmarks.points.length >= 8)
        {
            int point_size = (int)((track_info->landmarks.points.cols[2] - track_info->landmarks.points.cols[1])/8.0);
            
            if (point_size > 0)
            {
                CvPoint PPP;
                for (unsigned int i = 1; i < track_info->landmarks.points.length; i++)
                {
                    PPP = cvPoint(cvRound(track_info->landmarks.points.cols[i]), cvRound(track_info->landmarks.points.rows[i]));
                    cv::line(image, PPP, PPP, CV_RGB(150,150,150), point_size);
                }
            }
            
            CvPoint PPP = cvPoint(1, 1);
            cv::line(image, PPP, PPP, CV_RGB(255,0,0), 1);
        }
    }
    
    
    // Average the fps over past 30 frames.
    const static long N_FPS = 30;
    static long n_fps_now = 0;
    static double last_fps[N_FPS] = {0};
    double sum_fps = 0.0;
    n_fps_now++;
    
    for (int j = 1; j < N_FPS; j++)
    {
        last_fps[j-1] = last_fps[j];
        sum_fps += last_fps[j-1];
    }
    
    last_fps[N_FPS-1] = fps;
    sum_fps += fps;
    
    // Draw fps.
    if (show_fps)
    {
        char dist_text[16];
        
        sprintf(dist_text, "fps=%3.2f", sum_fps/std::min(N_FPS,n_fps_now));
        
        cv::putText(image, dist_text, cvPoint(10, 10), font_face, font_scale, CV_RGB(255,0,0));
    }
}


string maleOrFemaleFunct(double response) {
	string gender = "";
	if (response < 0) {
		return gender = "male";
	}
	else if (response > 0) {
		return gender = "female";
	}
	else {
		return gender = "No gender";
	}
}


string ancestryFunct(int response) {
	string ancestry = "";
	switch (response) {
	case 0:
		return ancestry = "unknown";
	case 1:
		return ancestry = "caucasian";
	case 2:
		return ancestry = "asian";
	case 3:
		return ancestry = "africain";
	}
}

