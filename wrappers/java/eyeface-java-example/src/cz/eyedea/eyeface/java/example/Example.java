////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2017 by Eyedea Recognition, s.r.o.                              //
//                                                                                    //
// Author: Eyedea Recognition, s.r.o.                                                 //
//                                                                                    //
// Contact:                                                                           //
//           web: http://www.eyedea.cz                                                //
//           email: info@eyedea.cz                                                    //
//                                                                                    //
// BSD License                                                                        //
// -----------------------------------------------------------------------------------//
// Copyright (c) 2016-2017, Eyedea Recognition, s.r.o.                                //
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

package cz.eyedea.eyeface.java.example;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

import cz.eyedea.eyeface.java.EfJavaSDK;
import cz.eyedea.eyeface.java.structures.EfBoundingBox;
import cz.eyedea.eyeface.java.structures.EfDetection;
import cz.eyedea.eyeface.java.structures.EfDetectionArray;
import cz.eyedea.eyeface.java.structures.EfException;
import cz.eyedea.eyeface.java.structures.EfFaceAttributes;
import cz.eyedea.eyeface.java.structures.EfFaceAttributesArray;
import cz.eyedea.eyeface.java.structures.EfLandmarks;
import cz.eyedea.eyeface.java.structures.EfLandmarksArray;
import cz.eyedea.eyeface.java.structures.EfTrackInfo;
import cz.eyedea.eyeface.java.structures.EfTrackInfoArray;
import cz.eyedea.er.java.structures.ERImage;
import cz.eyedea.er.java.structures.ERException.NoImageException;
import cz.eyedea.er.java.structures.ERException.UnsupportedImageColorModelException;

public class Example {
	
	public static final String  EYEFACE_DIR = "../../../eyefacesdk/";
	public static final String  CONFIG_INI  = "config.ini";
	
	public static final String  IMG_DIR     = "../../../data/test-images-id/";
	public static final int     NUM_IMAGES     = 150;
	
	public static final boolean RENDER_IMAGES = true;
	public static final String  RENDER_IMAGES_DIR_S = "./img-detection-output-standard-api/";
	public static final String  RENDER_IMAGES_DIR_E = "./img-detection-output-expert-api/";
	
	public static void main(String[] args) {
		efEyeFaceStandardExample();
		efEyeFaceExpertExample();
	}
	
	public static void drawImage(BufferedImage img, EfDetectionArray detectionArray,
			                     EfTrackInfoArray trackInfoArray, EfLandmarksArray landmarksArray,
			                     String pathToSave) {
		Graphics2D g2 = img.createGraphics();
		g2.setColor(Color.white);
		
		if (detectionArray != null) {
			for (int i = 0; i < detectionArray.getNumDetections(); i++) {
				EfBoundingBox bbox = detectionArray.detections[i].position.bounding_box;
				g2.drawPolygon(bbox.getXCoords(), bbox.getYCoords(), 4);
			}
		}
		
		if (landmarksArray != null) {
			for (int i = 0; i < landmarksArray.getNumLandmarks(); i++) {
				EfLandmarks lm = landmarksArray.landmarks[i];
				if (lm != null) {
					for (int j = 0; j < lm.points.length; j++) {
						g2.drawLine((int)lm.points.cols[j]  , (int)lm.points.rows[j],
								    (int)lm.points.cols[j]+1, (int)lm.points.rows[j]);
					}
				}
			}
		}
		
		for (int t = 0; t < trackInfoArray.getNumTrackInfos(); t++) {
			EfTrackInfo trackInfo = trackInfoArray.track_info[t];
			
			EfBoundingBox bbox = trackInfo.image_position;
			if (trackInfo.detection_index != -1 && detectionArray != null) {
				bbox = detectionArray.detections[trackInfo.detection_index].position.bounding_box;
			}

			if (detectionArray == null) {
				g2.drawPolygon(bbox.getXCoords(), bbox.getYCoords(), 4);
			}

			if (landmarksArray == null) {
				EfLandmarks lm = trackInfo.landmarks;
				if (lm != null) {
					for (int j = 0; j < lm.points.length; j++) {
						g2.drawLine((int)lm.points.cols[j]  , (int)lm.points.rows[j],
								    (int)lm.points.cols[j]+1, (int)lm.points.rows[j]);
					}
				}
			}
			
			String text = "Track: " + trackInfo.track_id;
			g2.drawString(text, bbox.top_left_col, bbox.top_left_row);
		}
		
		
		try {
			ImageIO.write(img, "png", new File(pathToSave));
		} catch (IOException e) {
			System.err.println("Error during writing image to file " + pathToSave);
		}
		g2.dispose();
	}

	/**
	 * This is a Java version of EyeFace Standard API example on how to process a videostream.
	 */
	public static void efEyeFaceStandardExample() {
		// init EyeFace engine
		EfJavaSDK efJavaSDK = null;
		try {
			efJavaSDK = new EfJavaSDK(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
		} catch (EfException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
			return;
		}
		
		double frameTime = 0.0;
		final double FPS = 10.0;
		
		// read images
		for (int i = 0; i < NUM_IMAGES; i++) {
			String imagePath = IMG_DIR + String.format("%04d", i) + ".png";
			ERImage erImage = null;
			try {
				erImage = new ERImage(imagePath);
			} catch (NullPointerException     |
					 IllegalArgumentException |
					 NoImageException         |
					 IOException e1) {
				System.err.println("Unable to read image " + imagePath);
				continue;
			} catch (UnsupportedImageColorModelException e1) {
				System.err.println("Unsupported format of image " + imagePath);
				continue;
			}
			
			// set detection area
			EfBoundingBox bbox = new EfBoundingBox(erImage.getWidth(), erImage.getHeight());
			
			// run face detector
			boolean detectionStatus = efJavaSDK.efMain(erImage, bbox, frameTime);
            if (!detectionStatus) {
            	System.err.println("Error during detection on image " + i);
            }
            
            // get track infos
            EfTrackInfoArray trackInfoArray = efJavaSDK.efGetTrackInfo();
            // print tracks
 			System.out.println("//////////////////////////");
 			System.out.println("Tracks on image " + i + ":");
 			System.out.println("//////////////////////////");
 			if (trackInfoArray.getNumTrackInfos() == 0) {
				System.out.println("\tNo tracks...");
			}
			for (int j = 0; j < trackInfoArray.getNumTrackInfos(); j++) {
				EfTrackInfo trackInfo = trackInfoArray.track_info[j];
				System.out.println("--------------------");
				System.out.println("Track no. " + j + ":");
				System.out.println("--------------------");
				System.out.println(trackInfo.toString());
				System.out.println("\t----------");
				System.out.println("\tDetection:");
				System.out.println("\t----------");
				System.out.println(trackInfo.image_position.toString());
				System.out.println("\t----------");
				System.out.println("--------------------");
			}
			System.out.println("//////////////////////////\n");
			
			// render detection result to image and save
			if (RENDER_IMAGES) {
				File imgsOutDir = new File(RENDER_IMAGES_DIR_S);
				imgsOutDir.mkdir();
				String imgFilename = RENDER_IMAGES_DIR_S + String.format("%04d", i) + ".png";
				BufferedImage img = null;
				try {
					img = erImage.getBufferedImage();
				} catch (Exception e) {
					System.err.println("Error converting image " + i + " to the BufferedImage.");
				}
				if (img != null) {
					drawImage(img, null, trackInfoArray, null, imgFilename);
				}
			}
			
			// Update time
			frameTime += (1.0/FPS);
		}
		
		// free EyeFace engine
		efJavaSDK.dispose();
	}
	
	/**
	 * This is a Java version of EyeFace Expert API example on how to process image databases.
	 */
	public static void efEyeFaceExpertExample() {
		// init EyeFace engine
		EfJavaSDK efJavaSDK = null;
		try {
			efJavaSDK = new EfJavaSDK(EYEFACE_DIR, EYEFACE_DIR, CONFIG_INI);
		} catch (EfException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
			return;
		}
		
		double frameTime = 0.0;
		final double FPS = 10.0;
		
		// read images
		for (int i = 0; i < NUM_IMAGES; i++) {
			String imagePath = IMG_DIR + String.format("%04d", i) + ".png";
			ERImage erImage = null;
			try {
				erImage = new ERImage(imagePath);
			} catch (NullPointerException     |
					 IllegalArgumentException |
					 NoImageException         |
					 IOException e1) {
				System.err.println("Unable to read image " + imagePath);
				continue;
			} catch (UnsupportedImageColorModelException e1) {
				System.err.println("Unsupported format of image " + imagePath);
				continue;
			}
				
			// run detection
			EfDetectionArray detectionArray = efJavaSDK.efRunFaceDetector(erImage);
			if (detectionArray == null) {
				System.err.println("Error during detection on image " + i);
			}
			
			// Update tracker
			boolean updateStatus = efJavaSDK.efUpdateTracker(erImage, detectionArray, frameTime);
			if (updateStatus == false) {
				System.err.println("Error during tracker update on image " + i);
			}
			
			// run landmarks
			EfLandmarksArray landmarksArray = efJavaSDK.efRunFaceLandmark(erImage, detectionArray, null);
			if (landmarksArray == null) {
				System.err.println("Error during landmarks on image " + i);
			}
			
			//recognize face attributes
			EfFaceAttributesArray faceAttributesArray = efJavaSDK.efRecognizeFaceAttributes(erImage, detectionArray, landmarksArray, null, 
																							EfFaceAttributes.EF_FACEATTRIBUTES_ALL, frameTime, true);
			if (faceAttributesArray == null) {
				System.err.println("Error during face attributes on image " + i);
			}
			
			// print detections
			System.out.println("//////////////////////////");
			System.out.println("Detections on image " + i + ":");
			System.out.println("//////////////////////////");
			if (detectionArray.getNumDetections() == 0) {
				System.out.println("\tNo detections...");
			}
			for (int j = 0; j < detectionArray.getNumDetections(); j++) {
				System.out.println("--------------------");
				System.out.println("Detection no. " + (j+1) + ":");
				System.out.println("--------------------");
				System.out.println(detectionArray.detections[j].toString());
				System.out.println("Landmarks: ");
				if (landmarksArray == null) {
					System.out.println("\tNo landmarks...");
				} else {
					if (landmarksArray.landmarks[j] == null) {
						System.out.println("\tNo landmarks...");
					}
					System.out.println(landmarksArray.landmarks[j].toString());
				}
				System.out.println("--------------------");
			}
			System.out.println("//////////////////////////");
			
			// Get track information
			EfTrackInfoArray trackInfoArray = efJavaSDK.efGetTrackInfo();
			
			// print tracks
			System.out.println("//////////////////////////");
			System.out.println("Tracks on image " + i + ":");
			System.out.println("//////////////////////////");
			if (trackInfoArray.getNumTrackInfos() == 0) {
				System.out.println("\tNo tracks...");
			}
			for (int j = 0; j < trackInfoArray.getNumTrackInfos(); j++) {
				EfTrackInfo trackInfo = trackInfoArray.track_info[j];
				System.out.println("--------------------");
				System.out.println("Track no. " + j + ":");
				System.out.println("--------------------");
				System.out.println(trackInfo.toString());
				// Get corresponding detection
				int detIdx = trackInfo.detection_index;
				if (detIdx >= 0 && detIdx < detectionArray.getNumDetections()) {
					EfDetection detection = detectionArray.detections[detIdx];
					System.out.println("\t----------");
					System.out.println("\tDetection:");
					System.out.println("\t----------");
					System.out.println(detection.position.toString());
					System.out.println("\t----------");
					///////////////////////////////////////////////
					// Face recognition processing can be run here.
					///////////////////////////////////////////////
				}
				System.out.println("--------------------");
			}
			System.out.println("//////////////////////////\n");
			
			// render detection result to image and save
			if (RENDER_IMAGES) {
				File imgsOutDir = new File(RENDER_IMAGES_DIR_E);
				imgsOutDir.mkdir();
				String imgFilename = RENDER_IMAGES_DIR_E + String.format("%04d", i) + ".png";
				BufferedImage img = null;
				try {
					img = erImage.getBufferedImage();
				} catch (Exception e) {
					System.err.println("Error converting image " + i + " to the BufferedImage.");
				}
				if (img != null) {
					drawImage(img, detectionArray, trackInfoArray, landmarksArray, imgFilename);
				}
			}
			
			// Update time
			frameTime += (1.0/FPS);
		}
				
		// free EyeFace engine
		efJavaSDK.dispose();
	}
}
