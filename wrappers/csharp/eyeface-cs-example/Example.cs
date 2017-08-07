////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//    C# EyeFace SDK Example                                                          //
// ---------------------------------------------------------------------------------- //
//                                                                                    //
// Copyright (c) 2017 by Eyedea Recognition, s.r.o.                                   //
//                                                                                    //
// Author: Eyedea Recognition, s.r.o.                                                 //
//                                                                                    //
// Contact:                                                                           //
//           web: http://www.eyedea.cz                                                //
//           email: info@eyedea.cz                                                    //
//                                                                                    //
// BSD License                                                                        //
// -----------------------------------------------------------------------------------//
// Copyright (c) 2017, Eyedea Recognition, s.r.o.                                     //
// All rights reserved.                                                               //
// Redistribution and use in source and binary forms, with or without modification,   //
// are permitted provided that the following conditions are met :                     //
//     1. Redistributions of source code must retain the above copyright notice,      //
//        this list of conditions and the following disclaimer.                       //
//     2. Redistributions in binary form must reproduce the above copyright notice,   //
//        this list of conditions and the following disclaimer in the documentation   //
//        and / or other materials provided with the distribution.                    //
//     3. Neither the name of the copyright holder nor the names of its contributors  //
//        may be used to endorse or promote products derived from this software       //
//        without specific prior written permission.                                  //
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"        //
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  //
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. //
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   //
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT  //
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR //
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  //
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)  //
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF             //
// THE POSSIBILITY OF SUCH DAMAGE.                                                    //
////////////////////////////////////////////////////////////////////////////////////////

using System;
using Eyedea.EyeFace;
using Eyedea.er;
using System.Drawing;
using System.IO;

namespace EyeFace
{
    class Example
    {
        private const string EYEFACE_DIR         = "../../../eyefacesdk/"; 
        private const string CONFIG_INI          = "config.ini";

        private const string IMG_DIR             = "../../../data/test-images-id/";
        private const int    NUM_IMG             = 150;

        private const bool   RENDER_IMAGES       = true;
        private const string RENDER_IMAGES_DIR_S = "./img-detection-output-standard-api/";
        private const string RENDER_IMAGES_DIR_E = "./img-detection-output-expert-api/";

        public static int Main(string[] args) {
            /*try {
                efEyeFaceStandardExample();
            }  catch (Exception e) {
                System.Console.WriteLine("ERROR: efEyeFaceStandardExample() failed: " + e.ToString());
                return -1;
            }*/
            
            try {
                efEyeFaceExpertExample();
            } catch (Exception e) {
                System.Console.WriteLine("ERROR: efEyeFaceExpertExample() failed: " + e.ToString());
                return -1;
            }
            
            return 0;
        }

        private static void renderAndSaveImage(ERImage image, 
                                               EfTrackInfoArray trackinfoArray,
                                               EfCsSDK efCsSDK, string imageSavePath) {
            Bitmap bitmap = efCsSDK.erImageToCsBitmap(image);

            using (Graphics g = Graphics.FromImage(bitmap)) {
                Pen pen = new Pen(Color.White, 2.0f);
                for (int i = 0; i < trackinfoArray.num_tracks; i++) {
                    EfTrackInfo trackinfo = trackinfoArray.track_info[i];
                    EfBoundingBox detBBox = trackinfo.image_position;
                    g.DrawPolygon(pen, detBBox.Points);
                    string text = "Track: " + trackinfo.track_id;
                    float fontSize = 8.0f;
                    Font font = new Font(new FontFamily("Arial"), fontSize);
                    SolidBrush brush = new SolidBrush(Color.White);
                    g.DrawString(text, font, brush, detBBox.top_left_col, detBBox.top_left_row-2.0f*fontSize);
                }
            }

            bitmap.Save(imageSavePath);
        }

        private static void renderAndSaveImage(ERImage image, 
                                               EfDetectionArray detectionArray, EfTrackInfoArray trackinfoArray,
                                               EfCsSDK efCsSDK, string imageSavePath) {
            Bitmap bitmap = efCsSDK.erImageToCsBitmap(image);

            using (Graphics g = Graphics.FromImage(bitmap)) {
                Pen pen = new Pen(Color.White, 2.0f);
                for (int i = 0; i < detectionArray.num_detections; i++) {
                    EfDetection detection = detectionArray.detections[i];
                    g.DrawPolygon(pen, detection.position.bounding_box.Points);
                }

                for (int i = 0; i < trackinfoArray.num_tracks; i++) {
                    EfTrackInfo trackinfo = trackinfoArray.track_info[i];
                    if (trackinfo.detection_index == -1) {
                        continue;
                    }
                    EfDetection detection = detectionArray.detections[trackinfo.detection_index];
                    EfBoundingBox detBBox = detection.position.bounding_box;
                    string text = "Track: " + trackinfo.track_id;
                    float fontSize = 8.0f;
                    Font font = new Font(new FontFamily("Arial"), fontSize);
                    SolidBrush brush = new SolidBrush(Color.White);
                    g.DrawString(text, font, brush, detBBox.top_left_col, detBBox.top_left_row-2.0f*fontSize);
                }
            }

            bitmap.Save(imageSavePath);
        }

        /// <summary>
        /// This is a C# version of EyeFace Standard API example on how to process a videostream.
        /// </summary>
        public static void efEyeFaceStandardExample() {
            // then instantiate EfCsSDK object
            EfCsSDK efCsSDK = new EfCsSDK(EYEFACE_DIR);
            System.Console.WriteLine("EyeFace C# interface initialized.");

            // Sentinel LDK license check                             
            long key = efCsSDK.efHaspGetCurrentLoginKeyId();

            EfHaspTime expDate;
            if (!efCsSDK.efHaspGetExpirationDate(key, out expDate)) {
                System.Console.Error.WriteLine("ERROR: HASP license verification failed.");
                return;
            }
            System.Console.WriteLine("HASP key = " + key.ToString() + " license expiration date [YYYY/MM/DD]: " + expDate.year + "/" + expDate.month.ToString() + "/" + expDate.day.ToString());

            // init EyeFace                                               
            System.Console.Write("EyeFace init ... ");
            bool initState = efCsSDK.efInitEyeFace(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
            if (!initState) {
                System.Console.Error.WriteLine("Error during EyeFace initialization.");
                return;
            }
            System.Console.WriteLine("done.\n");

            for (int iImgNo = 0; iImgNo < NUM_IMG; iImgNo++) {
                string imageFilename = iImgNo.ToString("D4") + ".png";
                string imgName = IMG_DIR + imageFilename;
                System.Console.WriteLine("///////////////////////////////////////////////");
                System.Console.WriteLine(imgName);
                System.Console.WriteLine("-----------------------------------------------");
                System.Console.Write((iImgNo + 1) + ". Loading image: " + imgName + " ... ");

                // load image
                ERImage image;
                try {
                    image = efCsSDK.erImageRead(imgName);
                } catch (ERException) {
                    System.Console.Error.WriteLine("Can't load the file: " + imgName);
                    return;
                }
                System.Console.WriteLine("done.");
                                
                // setup detection area                                         
                System.Console.Write("    Face detection ... ");
                EfBoundingBox bbox = new EfBoundingBox(image.width, image.height);

                // run face detector 
                double frameTime = Convert.ToDouble(iImgNo) / 10.0;
                bool detectionStatus = efCsSDK.efMain(image, bbox, frameTime);
                if (!detectionStatus) {
                    System.Console.Error.WriteLine("Error during detection on image " + iImgNo.ToString() + ".");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.\n");

                // get track infos
                EfTrackInfoArray trackInfoArray = efCsSDK.efGetTrackInfo();
                System.Console.WriteLine(trackInfoArray.ToString());

                // render detection result to image and save
                if (RENDER_IMAGES) {
                    Directory.CreateDirectory(RENDER_IMAGES_DIR_S);
                    string imageSavePath    = RENDER_IMAGES_DIR_S + imageFilename;
                    renderAndSaveImage(image, trackInfoArray, efCsSDK, imageSavePath);
                }

                // free the image
                efCsSDK.erImageFree(ref image);
            }

            // shutdown EyeFace SDK to force all tracks to finish and gather final results.
            efCsSDK.efShutdownEyeFace();
            // get track infos
            System.Console.WriteLine("///////////////////////////////////////////////");
            System.Console.WriteLine("Final tracks:");
            System.Console.WriteLine("-----------------------------------------------");
            EfTrackInfoArray trackInfoArrayFinal = efCsSDK.efGetTrackInfo();
            System.Console.WriteLine(trackInfoArrayFinal.ToString());
            
            System.Console.WriteLine("[Press ENTER to exit]");
            System.Console.ReadLine();
        }

        /// <summary>
        /// This is a C# version of EyeFace Expert API example on how to process image databases.
        /// </summary>
        public static void efEyeFaceExpertExample() {
            // then instantiate EfCsSDK object
            EfCsSDK efCsSDK = new EfCsSDK(EYEFACE_DIR);
            System.Console.WriteLine("EyeFace C# interface initialized.");
            
            // init EyeFace                                               
            System.Console.Write("EyeFace init ... ");
            bool initState = efCsSDK.efInitEyeFace(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
            if (!initState) {
                System.Console.Error.WriteLine("Error during EyeFace initialization.");
                return;
            }
            System.Console.WriteLine("done.\n");

            for (int iImgNo = 0; iImgNo < NUM_IMG; iImgNo++) {
                string imageFilename = iImgNo.ToString("D4") + ".png";
                string imgName = IMG_DIR + imageFilename;
                System.Console.WriteLine("///////////////////////////////////////////////");
                System.Console.WriteLine(imgName);
                System.Console.WriteLine("-----------------------------------------------");
                System.Console.Write((iImgNo + 1) + ". Loading image: " + imgName + " ... ");

                // load image
                ERImage image;
                try {
                    image = efCsSDK.erImageRead(imgName);
                } catch (ERException) {
                    System.Console.Error.WriteLine("Can't load the file: " + imgName);
                    return;
                }
                System.Console.WriteLine("done.");

                // run face detector
                System.Console.Write("    Face detection ... ");
                EfDetectionArray detectionArray;
                try {
                    detectionArray = efCsSDK.efRunFaceDetector(image);
                } catch (EfException) {
                    System.Console.Error.WriteLine("Error during detection on image " + iImgNo.ToString() + ".");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.");

                // run landmark
                System.Console.Write("    Landmark detection ... ");
                EfLandmarksArray landmarksArray = new EfLandmarksArray();
                try {
                    landmarksArray = efCsSDK.efRunFaceLandmark(image, detectionArray);
                } catch (EfException) {
                    System.Console.Error.WriteLine("Error during landmark detection on image " + iImgNo.ToString() + ".");
                }
                System.Console.WriteLine("done.");

                // use real frame time in your application
                double frameTime = 0.1 * iImgNo;

                // run face attributes recognition
                System.Console.Write("    Face attributes ... ");

                EfFaceAttributesArray attributesArray;
                try {
                    attributesArray = efCsSDK.efRecognizeFaceAttributes(image, detectionArray, landmarksArray, frameTime);
                } catch (EfException) {
                    System.Console.Error.WriteLine("Error during landmark detection on image " + iImgNo.ToString() + ".");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.\n");
                System.Console.WriteLine("    Recognized face attributes:");
                System.Console.WriteLine("    --------------------------");
                for (int i = 0; i < attributesArray.num_detections; i++) {
                    System.Console.WriteLine("    Detection " + (i + 1).ToString());
                    System.Console.WriteLine(attributesArray.face_attributes[i].ToString() + "\n");
                }

                // update the tracker, used for detection joining
                System.Console.Write("    Tracker update ... ");
                bool updateState = efCsSDK.efUpdateTracker(image, detectionArray, frameTime);
                if (!updateState) {
                    System.Console.Error.WriteLine("Error during tracker update on image " + iImgNo.ToString() + ".");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.\n");

                // get track infos
                EfTrackInfoArray trackInfoArray = efCsSDK.efGetTrackInfo();
                System.Console.WriteLine("    Tracks:");
                System.Console.WriteLine("    --------------------------");
                System.Console.WriteLine(trackInfoArray.ToString());

                // render detection result to image and save
                if (RENDER_IMAGES) {
                    Directory.CreateDirectory(RENDER_IMAGES_DIR_E);
                    string imageSavePath    = RENDER_IMAGES_DIR_E + imageFilename;
                    renderAndSaveImage(image, detectionArray, trackInfoArray, efCsSDK, imageSavePath);
                }

                // free the image
                efCsSDK.erImageFree(ref image);
            }

            // shutdown EyeFace SDK to force all tracks to finish and gather final results.
            efCsSDK.efShutdownEyeFace();
            // get track infos
            System.Console.WriteLine("///////////////////////////////////////////////");
            System.Console.WriteLine("Final tracks:");
            System.Console.WriteLine("-----------------------------------------------");
            EfTrackInfoArray trackInfoArrayFinal = efCsSDK.efGetTrackInfo();
            System.Console.WriteLine(trackInfoArrayFinal.ToString());

            System.Console.WriteLine("[Press ENTER to exit]");
            System.Console.ReadLine();
        }
    }
}
