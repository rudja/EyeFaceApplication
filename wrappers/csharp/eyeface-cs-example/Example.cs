////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//    C# EyeFace SDK Example                                                          //
// ---------------------------------------------------------------------------------- //
//                                                                                    //
// Copyright (c) 2017 by Eyedea Recognition, s.r.o.                                   //
//                                                                                    //
// Author: Eyedea Recognition, s.r.o.                                                 //
// Second Author: Rudja RULLE    
//
// Contact:                                                                           //
//           web: http://www.eyedea.cz                                                //
//           email: info@eyedea.cz 
//           email: rudja971@hotmail.com
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

using Emgu.CV;
using Emgu.CV.Structure;
using Newtonsoft.Json.Linq;

namespace EyeFace
{
    class Example
    {
        private const string EYEFACE_DIR = "../../../eyefacesdk/";
        private const string CONFIG_INI = "config.ini";

        public static int Main(string[] args)
        {
            try
            {
                efEyeFaceStandardExample();
            }
            catch (Exception e)
            {
                System.Console.WriteLine("ERROR: efEyeFaceStandardExample() failed: " + e.ToString());
                return -1;
            }
            return 0;
        }

        //Functions that can be use for save taken image into a directory
        private static void renderAndSaveImage(ERImage image,
                                               EfTrackInfoArray trackinfoArray,
                                               EfCsSDK efCsSDK, string imageSavePath)
        {
            Bitmap bitmap = efCsSDK.erImageToCsBitmap(image);

            using (Graphics g = Graphics.FromImage(bitmap))
            {
                Pen pen = new Pen(Color.White, 2.0f);
                for (int i = 0; i < trackinfoArray.num_tracks; i++)
                {
                    EfTrackInfo trackinfo = trackinfoArray.track_info[i];
                    EfBoundingBox detBBox = trackinfo.image_position;
                    g.DrawPolygon(pen, detBBox.Points);
                    string text = "Track: " + trackinfo.track_id;
                    float fontSize = 8.0f;
                    Font font = new Font(new FontFamily("Arial"), fontSize);
                    SolidBrush brush = new SolidBrush(Color.White);
                    g.DrawString(text, font, brush, detBBox.top_left_col, detBBox.top_left_row - 2.0f * fontSize);
                }
            }

            bitmap.Save(imageSavePath);
        }

        private static void renderAndSaveImage(ERImage image,
                                               EfDetectionArray detectionArray, EfTrackInfoArray trackinfoArray,
                                               EfCsSDK efCsSDK, string imageSavePath)
        {
            Bitmap bitmap = efCsSDK.erImageToCsBitmap(image);

            using (Graphics g = Graphics.FromImage(bitmap))
            {
                Pen pen = new Pen(Color.White, 2.0f);
                for (int i = 0; i < detectionArray.num_detections; i++)
                {
                    EfDetection detection = detectionArray.detections[i];
                    g.DrawPolygon(pen, detection.position.bounding_box.Points);
                }

                for (int i = 0; i < trackinfoArray.num_tracks; i++)
                {
                    EfTrackInfo trackinfo = trackinfoArray.track_info[i];
                    if (trackinfo.detection_index == -1)
                    {
                        continue;
                    }
                    EfDetection detection = detectionArray.detections[trackinfo.detection_index];
                    EfBoundingBox detBBox = detection.position.bounding_box;
                    string text = "Track: " + trackinfo.track_id;
                    float fontSize = 8.0f;
                    Font font = new Font(new FontFamily("Arial"), fontSize);
                    SolidBrush brush = new SolidBrush(Color.White);
                    g.DrawString(text, font, brush, detBBox.top_left_col, detBBox.top_left_row - 2.0f * fontSize);
                }
            }

            bitmap.Save(imageSavePath);
        }

        /// <summary>
        /// This is a C# version of EyeFace Standard API example on how to process a videostream.
        /// </summary>
        public static void efEyeFaceStandardExample()
        {
            // then instantiate EfCsSDK object
            EfCsSDK efCsSDK = new EfCsSDK(EYEFACE_DIR);
            System.Console.WriteLine("EyeFace C# interface initialized.");

            // init EyeFace                                               
            System.Console.Write("EyeFace init ... ");
            bool initState = efCsSDK.efInitEyeFace(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
            if (!initState)
            {
                System.Console.Error.WriteLine("Error during EyeFace initialization.");
                return;
            }
            System.Console.WriteLine("done.\n");

            VideoCapture capture = new VideoCapture();                                          //create a camera capture. Work with last EmguCV version 3.2
            Mat captureFrame = null;

            int iImgNo = 0;
            bool firstIteration = true;

            while (!(Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape))
            {
                //For incrementation of iImgNo variable
                if (firstIteration)
                {
                    //Do nothing
                    firstIteration = false;
                }
                else
                {
                    iImgNo++;
                }

                //Get a frame from capture and convert it into a bitmap image
                //I try to get this bitmap because eyeface sdk have a convertor image (bitmap->Erimage)
                captureFrame = capture.QueryFrame();                                            //I got some access violation here.   
                Image<Bgr, Byte> myImage = captureFrame.ToImage<Bgr, Byte>();
                Bitmap myBitmap = myImage.ToBitmap();

                // load image
                ERImage image;
                try
                {
                    image = efCsSDK.csBitmapToERImage(myBitmap);
                }
                catch (ERException)
                {
                    System.Console.Error.WriteLine("Can't load the file: ");
                    return;
                }
                System.Console.WriteLine("done.");

                // setup detection area                                         
                System.Console.Write("    Face detection ... ");
                EfBoundingBox bbox = new EfBoundingBox(image.width, image.height);

                // run face detector 
                double frameTime = Convert.ToDouble(iImgNo) / 10.0;
                bool detectionStatus = efCsSDK.efMain(image, bbox, frameTime);
                if (!detectionStatus)
                {
                    System.Console.Error.WriteLine("Error during detection on image " + iImgNo.ToString() + ".");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.\n");

                // Get track infos object from the image
                EfTrackInfoArray trackInfoArray = efCsSDK.efGetTrackInfo();
                
                /// We will create a JSON object and fill it with the data we need
                /// Before we need to verify that all datas are set before stocking them
                /// We free the image taken when we are done with it. 
                JTokenWriter person = new JTokenWriter();
                for (int i = 0; i < trackInfoArray.num_tracks; i++)         //num_tracks equal to the number of person detected on the image
                {
                    EfTrackInfo track_info = trackInfoArray.track_info[i];

                    //Verify if all parameters are set before formatting them into a JSON object
                    if (trackInfoArray.track_info[i].person_id != 0 && trackInfoArray.track_info[i].face_attributes.gender.recognized 
                                                                    && trackInfoArray.track_info[i].face_attributes.age.recognized
                                                                    && trackInfoArray.track_info[i].face_attributes.emotion.recognized
                                                                    && trackInfoArray.track_info[i].face_attributes.ancestry.recognized)
                    {
                        //Save data into the JSON
                        Console.WriteLine("Data in the JSON \n");
                        person.WriteStartObject();

                        person.WritePropertyName("PersonID");
                        person.WriteValue(trackInfoArray.track_info[i].person_id);

                        person.WritePropertyName("Gender");
                        //person.WriteValue(attributesArray.face_attributes[i].gender.ToString());
                        person.WriteValue(trackInfoArray.track_info[i].face_attributes.gender.ToString());

                        person.WritePropertyName("Age");
                        person.WriteValue(trackInfoArray.track_info[i].face_attributes.age.value);

                        person.WritePropertyName("Emotion");
                        person.WriteValue(trackInfoArray.track_info[i].face_attributes.emotion.ToString());

                        person.WritePropertyName("Ancestry");
                        person.WriteValue(trackInfoArray.track_info[i].face_attributes.ancestry.ToString());

                        person.WritePropertyName("Attention_time");
                        person.WriteValue(trackInfoArray.track_info[i].attention_time);

                        person.WriteEndObject();

                        JObject o = (JObject)person.Token;
                        Console.WriteLine(o.ToString());

                        // free the image
                        efCsSDK.erImageFree(ref image);
                    }
                    else
                    {
                        Console.WriteLine("Data not set yet...");
                    }
                }
            }

            // shutdown EyeFace SDK to force all tracks to finish and gather final results.
            efCsSDK.efShutdownEyeFace();

            System.Console.WriteLine("[Press ENTER to exit]");
            System.Console.ReadLine();
        }

        /// <summary>
        /// This is a C# version of EyeFace Expert API example on how to process image databases.
        /// </summary>
        public static void efEyeFaceExpertExample()
        {
            // then instantiate EfCsSDK object
            EfCsSDK efCsSDK = new EfCsSDK(EYEFACE_DIR);
            System.Console.WriteLine("EyeFace C# interface initialized.");

            // init EyeFace                                               
            System.Console.Write("EyeFace init ... ");
            bool initState = efCsSDK.efInitEyeFace(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
            if (!initState)
            {
                //System.Console.Error.WriteLine("Error during EyeFace initialization.");
                return;
            }
            System.Console.WriteLine("done.\n");

            VideoCapture capture = new VideoCapture();                                          //create a camera capture. Work with last EmguCV version 3.2
            Mat captureFrame = null;

            //Variable to get trace on the taken image
            double iImgNo = 0;
            bool firstPass = false;

            //for (int iImgNo = 0; iImgNo < NUM_IMG; iImgNo++)
            while (!(Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape))
            {
                //Incrementation of iImgNo variable
                if (firstPass == false)
                {
                    //Do nothing
                }
                else
                {
                    iImgNo++;
                }
                firstPass = true;

                //Get a frame from capture and convert it into a bitmap image
                //I try to get this bitmap because eyeface sdk have a convertor image (bitmap->Erimage)
                captureFrame = capture.QueryFrame();                                            //Access violation here.   
                Image<Bgr, Byte> myImage = captureFrame.ToImage<Bgr, Byte>();
                Bitmap myBitmap = myImage.ToBitmap();

                // load image
                ERImage image;
                try
                {
                    //image = efCsSDK.erImageRead(imgName);
                    image = efCsSDK.csBitmapToERImage(myBitmap);
                }
                catch (ERException)
                {
                    System.Console.Error.WriteLine("Can't load the file: ");
                    return;
                }
                //System.Console.WriteLine("done.");

                // run face detector
                System.Console.Write("    Face detection ... ");
                EfDetectionArray detectionArray;
                try
                {
                    detectionArray = efCsSDK.efRunFaceDetector(image);
                }
                catch (EfException)
                {
                    //System.Console.Error.WriteLine("Error during detection on image ");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                System.Console.WriteLine("done.");

                //run landmark
                System.Console.Write("    Landmark detection ... ");
                EfLandmarksArray landmarksArray = new EfLandmarksArray();
                try
                {
                    landmarksArray = efCsSDK.efRunFaceLandmark(image, detectionArray);
                }
                catch (EfException)
                {
                    //System.Console.Error.WriteLine("Error during landmark detection on image ");
                }
                System.Console.WriteLine("done.");

                //// use real frame time in your application
                double frameTime = 0.1 * iImgNo;

                //// run face attributes recognition
                //System.Console.Write("    Face attributes ... ");

                EfFaceAttributesArray attributesArray;
                try
                {
                    attributesArray = efCsSDK.efRecognizeFaceAttributes(image, detectionArray, landmarksArray, frameTime);
                }
                catch (EfException)
                {
                    //System.Console.Error.WriteLine("Error during landmark detection on image ");
                    efCsSDK.erImageFree(ref image);
                    return;
                }



                //System.Console.WriteLine("done.\n");
                //System.Console.WriteLine("    Recognized face attributes:");
                //System.Console.WriteLine("    --------------------------");
                //for (int i = 0; i < attributesArray.num_detections; i++)
                //{
                //    System.Console.WriteLine("    Detection " + (i + 1).ToString());
                //    System.Console.WriteLine(attributesArray.face_attributes[i].ToString() + "\n");
                //}

                // update the tracker, used for detection joining
                //System.Console.Write("    Tracker update ... ");
                bool updateState = efCsSDK.efUpdateTracker(image, detectionArray, frameTime);
                if (!updateState)
                {
                    System.Console.Error.WriteLine("Error during tracker update on image ");
                    efCsSDK.erImageFree(ref image);
                    return;
                }
                //System.Console.WriteLine("done.\n");

                // get track infos
                EfTrackInfoArray trackInfoArray = efCsSDK.efGetTrackInfo();
                Console.WriteLine(trackInfoArray.num_tracks);

                //System.Console.WriteLine("    Tracks:");
                //System.Console.WriteLine("    --------------------------");
                //System.Console.WriteLine(trackInfoArray.ToString());

                // render detection result to image and save
                //if (RENDER_IMAGES)
                //{
                //    Directory.CreateDirectory(RENDER_IMAGES_DIR_E);
                //    string imageSavePath = RENDER_IMAGES_DIR_E + imageFilename;
                //    renderAndSaveImage(image, detectionArray, trackInfoArray, efCsSDK, imageSavePath);
                //}

                //Create the JSON object with all elements
                JTokenWriter person = new JTokenWriter();
                //for (int i = 0; i < attributesArray.num_detections; i++)
                Console.WriteLine("Formatting JSON data! \n");
                for (int i = 0; i < trackInfoArray.num_tracks; i++)
                {
                    EfTrackInfo track_info = trackInfoArray.track_info[i];
                    //num_tracks equal to the number of person detected on the image
                    //Save data into the JSON
                    Console.WriteLine("Data in the JSON \n");
                    //Console.WriteLine("Number of person detected:" + attributesArray.num_detections);
                    person.WriteStartObject();

                    person.WritePropertyName("PersonID");
                    person.WriteValue(trackInfoArray.track_info[i].person_id);

                    person.WritePropertyName("Gender");
                    //person.WriteValue(attributesArray.face_attributes[i].gender.ToString());
                    person.WriteValue(trackInfoArray.track_info[i].face_attributes.gender.ToString());

                    person.WritePropertyName("Age");
                    person.WriteValue(trackInfoArray.track_info[i].face_attributes.age.value);

                    person.WritePropertyName("Emotion");
                    person.WriteValue(trackInfoArray.track_info[i].face_attributes.emotion.ToString());

                    person.WritePropertyName("Ancestry");
                    person.WriteValue(trackInfoArray.track_info[i].face_attributes.ancestry.ToString());

                    person.WritePropertyName("Attention_time");
                    person.WriteValue(trackInfoArray.track_info[i].attention_time);

                    person.WriteEndObject();

                    JObject o = (JObject)person.Token;
                    Console.WriteLine(o.ToString());
                }
                // free the image
                efCsSDK.erImageFree(ref image);
            }

            // shutdown EyeFace SDK to force all tracks to finish and gather final results.
            efCsSDK.efShutdownEyeFace();
            // get track infos
            //System.Console.WriteLine("///////////////////////////////////////////////");
            //System.Console.WriteLine("Final tracks:");
            //System.Console.WriteLine("-----------------------------------------------");
            //EfTrackInfoArray trackInfoArrayFinal = efCsSDK.efGetTrackInfo();
            //System.Console.WriteLine(trackInfoArrayFinal.ToString());

            System.Console.WriteLine("[Press ENTER to exit]");
            System.Console.ReadLine();
        }
    }
}
