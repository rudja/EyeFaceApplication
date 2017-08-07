////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//    C# interface to EyeFace SDK                                                     //
// ---------------------------------------------------------------------------------- //
//                                                                                    //
// Copyright (c) 2014-2017 by Eyedea Recognition, s.r.o.                              //
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
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Eyedea.er;
using System.IO;

/// <summary>
/// Namespace containing all the classes, functions, structures and enumerators of the EyeFace SDK.
/// </summary>
namespace Eyedea.EyeFace
{
    /// <summary>
    /// Class containing all EyeFace constant values.
    /// </summary>
    public static class EfConstants {
        // Masks for efRecognizeFaceAttributes() parameter "request_flag"
        public const UInt32 EF_FACEATTRIBUTES_AGE           = 0x01;
        public const UInt32 EF_FACEATTRIBUTES_GENDER        = 0x02;
        public const UInt32 EF_FACEATTRIBUTES_EMOTION       = 0x04;
        public const UInt32 EF_FACEATTRIBUTES_ANCESTRY      = 0x08;
        public const UInt32 EF_FACEATTRIBUTES_SMARTTRACKING = 0x10;
        public const UInt32 EF_FACEATTRIBUTES_ALL           = 0xFFFFFFFF;

        /// <summary>Maximal number of elements contained in the <see cref="Ef2dPoints"/> structure.</summary>
        public const int    EF2DPOINTS_MAX_SIZE             = 32;

        public const Int32  EF_FALSE = 0;
        public const Int32  EF_TRUE  = 1;
    }

    /// <summary>EyeFace SDK boolean representation using integer value.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfBool
    {
        /// <summary>Integer value representing boolean value: 1 - true, 0 - false.</summary>
        private Int32 value;

        public static implicit operator bool(EfBool efBool) {
            if (efBool.value == EfConstants.EF_TRUE) {
                return true;
            } else {
                return false;
            }
        }

        public static implicit operator EfBool(bool csBool) {
            EfBool efBool = new EfBool();
            efBool.value = EfConstants.EF_FALSE;
            if (csBool) {
                efBool.value = EfConstants.EF_TRUE;
            }

            return efBool;
        }

        public static EfBool[] boolArrayToEfBoolArray(bool[] csBoolArray) {
            if (csBoolArray == null) {
                return null;
            }
            EfBool[] efBoolArray = new EfBool[csBoolArray.Length];
            for (int i = 0; i < csBoolArray.Length; i++) {
                efBoolArray[i] = csBoolArray[i];
            }

            return efBoolArray;
        }
    }

    /// <summary>Set of 2D points structure.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct Ef2dPoints {
        /// <summary>Number of points in the set.</summary>
        public int length;
        /// <summary>Row coordinates of points.</summary
        private unsafe fixed double rows[EfConstants.EF2DPOINTS_MAX_SIZE];
        /// <summary>Column coordinates of points.</summary>
        private unsafe fixed double cols[EfConstants.EF2DPOINTS_MAX_SIZE];

        /// <summary>
        /// Returns the <see cref="List{PointF}"/> containing all the included points. 
        /// Point coordinates are converted form double to float.
        /// </summary>
        /// <returns>List containing the points.</returns>
        public List<PointF> Points {
            get {
                if (length < 0 ||
                    length > EfConstants.EF2DPOINTS_MAX_SIZE) {
                    throw new EfException("Invalid field length in the Ef2dPoints structure.");
                }

                List<PointF> points = new List<PointF>();
                unsafe {
                    for (int i = 0; i < length; i++) {
                        fixed (double* colsArr = cols) {
                            fixed (double* rowsArr = rows) {
                                points.Add(new PointF((float)colsArr[i], (float)rowsArr[i]));
                            }
                        }
                    }
                }

                return points;
            }
        }

        /// <summary>
        /// Rows (Y coordinates) property.
        /// </summary>
        public double[] Rows {
            get {
                int length = Math.Min(this.length, EfConstants.EF2DPOINTS_MAX_SIZE);
                double[] rows = new double[length];
                unsafe {
                    fixed (double* rowsPtr = this.rows) {
                        for (int i = 0; i < length; i++) {
                            rows[i] = rowsPtr[i];
                        }
                    }
                }

                return rows;
            }
        }

        /// <summary>
        /// Cols (X coordinates) property.
        /// </summary>
        public double[] Cols {
            get {
                int length = Math.Min(this.length, EfConstants.EF2DPOINTS_MAX_SIZE);
                double[] cols = new double[length];
                unsafe {
                    fixed (double* colsPtr = this.cols) {
                        for (int i = 0; i < length; i++) {
                            cols[i] = colsPtr[i];
                        }
                    }
                }

                return cols;
            }
        }
    };

    /// <summary>Bounding-box coordinates of a detection area</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfBoundingBox
    {
        /// <summary>Column index of top left corner of BB.</summary>
        public Int32 top_left_col;
        /// <summary>Row index of top left corner of BB.</summary>
        public Int32 top_left_row;
        /// <summary>Column index of top right corner of BB.</summary>
        public Int32 top_right_col;
        /// <summary>Row index of top right corner of BB.</summary>
        public Int32 top_right_row;
        /// <summary>Column index of bot left corner of BB.</summary>
        public Int32 bot_left_col;
        /// <summary>Row index of bot left corner of BB.</summary>
        public Int32 bot_left_row;
        /// <summary>Column index of bot right corner of BB.</summary>
        public Int32 bot_right_col;
        /// <summary>Row index of bot right corner of BB.</summary>
        public Int32 bot_right_row;

        /// <summary>
        /// Creates the instance of orthogonal <see cref="EfBoundingBox"/> with origin 
        /// (top left corner) on coordinates (0, 0) and with defined dimensions (width, height).
        /// </summary>
        /// <param name="width">Width of the bounding box to create.</param>
        /// <param name="height">Height of the bounding box to create.</param>
        public EfBoundingBox(uint width, uint height) {
            top_left_col  = 0;
            top_left_row  = 0;
            top_right_col = (int)(width - 1);
            top_right_row = 0;
            bot_left_col  = 0;
            bot_left_row  = (int)(height - 1);
            bot_right_col = (int)(width - 1);
            bot_right_row = (int)(height - 1);
        }

        /// <summary>
        /// Creates the instance of orthogonal <see cref="EfBoundingBox"/> with origin 
        /// (top left corner) on coordinates (x, y) and with defined dimensions (width, height).
        /// </summary>
        /// <param name="x">X coordinate of the bonding box origin.</param>
        /// <param name="y">Y coordinate of the bonding box origin.</param>
        /// <param name="width">Width of the bounding box to create.</param>
        /// <param name="height">Height of the bounding box to create.</param>
        public EfBoundingBox(int x, int y, uint width, uint height) {
            top_left_col  = x;
            top_left_row  = y;
            top_right_col = (int)(top_left_col + width - 1);
            top_right_row = y;
            bot_left_col  = x;
            bot_left_row  = (int)(top_left_row + height - 1);
            bot_right_col = (int)(bot_left_col + width - 1);
            bot_right_row = (int)(top_right_row + height - 1);
        }

        /// <summary>
        /// Creates the instance of orthogonal <see cref="EfBoundingBox"/> with 
        /// top left point on coordinates (topLeft.X, topLeft.Y) and 
        /// bottom right point on coordinates (botRight.X, botRight.Y).
        /// </summary>
        /// <param name="topLeft">Top left point of the bounding box.</param>
        /// <param name="botRight">Bottom left point of the bounding box.</param>
        public EfBoundingBox(Point topLeft, Point botRight) {
            top_left_col  = topLeft.X;
            top_left_row  = topLeft.Y;
            top_right_col = botRight.X;
            top_right_row = topLeft.Y;
            bot_left_col  = topLeft.X;
            bot_left_row  = botRight.Y;
            bot_right_col = botRight.X;
            bot_right_row = botRight.Y;
        }

        /// <summary>
        /// Creates the instance of general <see cref="EfBoundingBox"/> with 
        /// top left point on coordinates (topLeft.X, topLeft.Y), 
        /// top right point on coordinates (topRight.X, topRight.Y), 
        /// bottom right point on coordinates (botRight.X, botRight.Y) and 
        /// bottom left point on coordinates (botLeft.X, botLeft.Y).
        /// </summary>
        /// <param name="topLeft">Top left point of the bounding box.</param>
        /// <param name="topRight">Top right point of the bounding box.</param>
        /// <param name="botRight">Bottom left right of the bounding box.</param>
        /// <param name="botLeft">Bottom left point of the bounding box.</param>
        public EfBoundingBox(Point topLeft, Point topRight, Point botRight, Point botLeft) {
            top_left_col  = topLeft.X;
            top_left_row  = topLeft.Y;
            top_right_col = topRight.X;
            top_right_row = topRight.Y;
            bot_left_col  = botLeft.X;
            bot_left_row  = botLeft.Y;
            bot_right_col = botRight.X;
            bot_right_row = botRight.Y;
        }

        /// <summary>
        /// Returns the <see cref="List{Point}"/> containing all the included points 
        /// in the following order: top left, top right, bottom right and bottom left point. 
        /// </summary>
        /// <returns>List containing the points.</returns>
        public Point[] Points {
            get {
                Point[] points = new Point[4];
                points[0] = new Point(top_left_col , top_left_row );
                points[1] = new Point(top_right_col, top_right_row);
                points[2] = new Point(bot_right_col, bot_right_row);
                points[3] = new Point(bot_left_col , bot_left_row );

                return points;
            }
        }

        public override string ToString() {
            return "[" + top_left_col  + "," + top_left_row  + "]," + 
                   "[" + top_right_col + "," + top_right_row + "]," + 
                   "[" + bot_right_col + "," + bot_right_row + "]," + 
                   "[" + bot_left_col  + "," + bot_left_row  + "]";
        }
    };

    /// <summary>Angle information - Roll, pitch, yaw</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfAngles
    {
        /// <summary>Roll angle</summary>
        public double roll;
        /// <summary>Pitch angle</summary>
        public double pitch;
        /// <summary>Yaw angle</summary>
        public double yaw;

        public double this[int index] {
            get {
                switch (index) {
                    case  0: { return  roll; }
                    case  1: { return pitch; }
                    case  2: { return   yaw; }
                    default: { throw new IndexOutOfRangeException(); }
                }
            }
            set {
                switch (index) {
                    case  0: { roll  = value; return; }
                    case  1: { pitch = value; return; }
                    case  2: { yaw   = value; return; }
                    default: { throw new IndexOutOfRangeException(); }
                }
            }
        }

        public int Length {
            get {
                return 3;
            }
        }
    }

    /// <summary>World position information</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfWorldPosition
    {
        /// <summary>X coordinate</summary>
        public double x;
        /// <summary>Y coordinate</summary>
        public double y;

        public double this[int index] {
            get {
                switch (index) {
                    case  0: { return x; }
                    case  1: { return y; }
                    default: { throw new IndexOutOfRangeException(); }
                }
            }
            set {
                switch (index) {
                    case  0: { x = value; return; }
                    case  1: { y = value; return; }
                    default: { throw new IndexOutOfRangeException(); }
                }
            }
        }

        public int Length {
            get {
                return 2;
            }
        }
    }

    /// <summary>
    /// Facial Landmarks for a single face. Landmarks are not required and are turned off by default.
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfLandmarks
    {
        /// <summary>Only use EfLandmarks values if recognized == true.</summary>
        public EfBool recognized;
        /// <summary>Landmark points. Point 0-7 can be further stabilized using 
        /// landmark_precise_use_precise (See the Developer's Guide).<para />
        /// [0=FACE_CENTER 1=L_CANTHUS_R_EYE 2=R_CANTHUS_L_EYE 3=MOUTH_R 4=MOUTH_L 
        /// 5=R_CANTHUS_R_EYE 6=L_CANTHUS_L_EYE 7=NOSE_TIP 8=L_EYEBROW_L 9=L_EYEBROW_C 
        /// 10=L_EYEBROW_R 11=R_EYEBROW_L 12=R_EYEBROW_C 13=R_EYEBROW_R 14=NOSE_ROOT 
        /// 15=NOSE_L 16=NOSE_R 17=MOUTH_T 18=MOUTH_B 19=CHIN]
        /// </summary>
        public Ef2dPoints points;
        /// <summary>Roll, pitch, yaw angle as computed by landmarks.</summary>
        public EfAngles angles;
    };

    /// <summary>Age recognition result type.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfAge
    {
        /// <summary>Only use <seealso cref="EfAge"/> values if recognized == true</summary>
        public EfBool recognized;
        /// <summary>Recognized age in years [0-99].</summary>
        public double value;
        /// <summary>Age classifier score function response (for data analysts / statisticians).</summary>
        public double response;

        public override string ToString() {
            return "Age\t  = " + value + " years";
        }
    };

    /// <summary>Gender classes enum type.</summary>
    public enum EfGenderClass
    {
        /// <summary>Male value.</summary>
        EF_GENDER_MALE    = -1,
        /// <summary>Non-gender value.</summary>
        EF_GENDER_UNKNOWN =  0,
        /// <summary>Female value.</summary>
        EF_GENDER_FEMALE  =  1
    };

    /// <summary>Gender recognition result type.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfGender
    {
        /// <summary>Only use EfGender values if recognized == true.</summary>
        public EfBool recognized;
        /// <summary>Recognized gender class.</summary>
        public EfGenderClass value;
        /// <summary>Gender classifier score function response (for data analysts / statisticians).</summary>
        public double response;

        public override string ToString() {
            String res = "Gender\t  = ";
            if (value == EfGenderClass.EF_GENDER_FEMALE) {
                return res + "female";
            }
            if (value == EfGenderClass.EF_GENDER_MALE) {
                return res + "male";
            }
            return res + "unknown";
        }
    };

    /// <summary>Emotion classes enum type.</summary>
    public enum EfEmotionClass
    {
        /// <summary></summary>
        EF_EMOTION_NOTSMILING = -1,
        /// <summary></summary>
        EF_EMOTION_UNKNOWN    =  0,
        /// <summary></summary>
        EF_EMOTION_SMILING    =  1
    };

    /// <summary>Emotion recognition result type.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfEmotion
    {
        /// <summary>Only use EfEmotions values if recognized == true</summary>
        public EfBool recognized;
        /// <summary>Recognized emotion class.</summary>
        public EfEmotionClass value;
        /// <summary>Emotion classifier score function response (for data analysts / statisticians).</summary>
        public double response;

        public override string ToString() {
            String res = "Emotion\t  = ";
            if (value == EfEmotionClass.EF_EMOTION_NOTSMILING) {
                return res + "not smiling";
            }
            if (value == EfEmotionClass.EF_EMOTION_SMILING) {
                return res + "smiling";
            }
            return res + "unknown";
        }
    };

    /// <summary>Ancestry classes enum type.</summary>
    public enum EfAncestryClass
    {
        /// <summary></summary>
        EF_ANCESTRY_UNKNOWN = 0,
        /// <summary></summary>
        EF_ANCESTRY_CAUCASIAN = 1,
        /// <summary></summary>
        EF_ANCESTRY_ASIAN = 2,
        /// <summary></summary>
        EF_ANCESTRY_AFRICAN = 3
    };

    /// <summary>Ancestry recognition result type.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfAncestry
    {
        /// <summary>Only use EfEmotions values if recognized == true</summary>
        public EfBool recognized;
        /// <summary>Recognized emotion class.</summary>
        public EfAncestryClass value;
        /// <summary>Estimate of probability that the person is of the given ancestry. (for data analysts / statisticians).</summary>
        public double response;

        public override string ToString() {
            String res = "Ancestry  = ";
            if (value == EfAncestryClass.EF_ANCESTRY_AFRICAN) {
                return res + "african";
            }
            if (value == EfAncestryClass.EF_ANCESTRY_ASIAN) {
                return res + "asian";
            }
            if (value == EfAncestryClass.EF_ANCESTRY_CAUCASIAN) {
                return res + "caucasian";
            }
            return res + "unknown";
        }
    };

    /// <summary>Face attributes recognition result type.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfFaceAttributes
    {
        /// <summary>Age results.</summary>
        public EfAge age;
        /// <summary>Gender results.</summary>
        public EfGender gender;
        /// <summary>Emotion results.</summary>
        public EfEmotion emotion;
        /// <summary>Ancestry results.</summary>
        public EfAncestry ancestry;

        /// <summary>
        /// Returns recognized age in years.
        /// </summary>
        /// <returns>Recognized age in years.</returns>
        public int getAge() {
            return (int)age.value;
        }

        /// <summary>
        /// Returns recognized gender.
        /// </summary>
        /// <returns>Recognized gender.</returns>
        public EfGenderClass getGender() {
            return gender.value;
        }

        /// <summary>
        /// Returns recognized emotion.
        /// </summary>
        /// <returns>Recognized emotion.</returns>
        public EfEmotionClass getEmotion() {
            return emotion.value;
        }

        /// <summary>
        /// Returns recognized ancestry.
        /// </summary>
        /// <returns>Recognized ancestry.</returns>
        public EfAncestryClass getAncestry() {
            return ancestry.value;
        }

        public override string ToString() {
            StringBuilder sb = new StringBuilder();
            sb.Append("\t" + age.ToString()      + "\n");
            sb.Append("\t" + gender.ToString()   + "\n");
            sb.Append("\t" + emotion.ToString()  + "\n");
            sb.Append("\t" + ancestry.ToString());

            return sb.ToString();
        }
    };

    /// <summary>Enumerator describing the state of a <see cref="EfTrackInfo"/>.</summary>
    public enum EfTrackStatus
    {
        /// <summary>Live track</summary>
        EF_TRACKSTATUS_LIVE     = 0,
        /// <summary>Finished track</summary>
        EF_TRACKSTATUS_FINISHED = 1
    };

    /// <summary><see cref="EfTrackInfo"/> holds all information about the result of processing image from video sequence.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfTrackInfo
    {
        /// <summary>Current status of this track.</summary>
        public EfTrackStatus status;
        /// <summary>Track unique number. Track is a space-time aggregation of face detecions.</summary>
        public UInt32 track_id;
        /// <summary>Person identity unique number based on face recognition, 0 if not known yet.</summary>
        public UInt32 person_id;
        /// <summary>Face position in image pixels. Internally aggregated over past frames.</summary>
        public EfBoundingBox image_position;
        /// <summary>Groundplane real-world position relative to camera - [0] -> left-right, [1] -> forward-backward. 
        /// In meters. Internally aggregated over past frames. Depends linearly on camera FOV.</summary>
        public EfWorldPosition world_position;
        /// <summary>Roll [0], Pitch [1], Yaw [2] angle of the detected face. Pitch angle not available.</summary>
        public EfAngles angles;
        /// <summary>Landmark points. Not computed by default. Point 0-7 can be further stabilized 
        /// using landmark_precise_use_precise (See the Developer's Guide).</summary>
        public EfLandmarks landmarks;
        /// <summary>Face attributes (age, gender, emotion, etc.) storage.</summary>
        public EfFaceAttributes face_attributes;
        /// <summary>Fade-out energy of the track: 1 if the face was detected in the current frame, less than 1 if not.</summary>
        public double energy;
        /// <summary>Track start time [seconds].</summary>
        public double start_time;
        /// <summary>Time of the last tracker update [seconds].</summary>
        public double current_time;
        /// <summary>Track duration [seconds].</summary>
        public double total_time;
        /// <summary>Estimate of time [seconds] the person looked at the device. Based on face angle. (See the Developer's Guide)</summary>
        public double attention_time;
        /// <summary>Estimate of whether the person looks at the device in the current frame. Based oon face angle.</summary>
        public EfBool attention_now;
        /// <summary>Use in Expert API only. -1 if no detection in current frame. Tells the index to EfDetectionArray structure 
        /// outputted by efRunFaceDetector, so that EfTrackInfo can be matched with particular EfDetection.</summary>
        public int detection_index;

        public override string ToString() {
            StringBuilder sb = new StringBuilder();
            sb.Append("    Track "        + (track_id).ToString() + "\n");
            if (status == EfTrackStatus.EF_TRACKSTATUS_FINISHED) {
                sb.Append("\tStatus\t  = finished\n");
            } else {
                sb.Append("\tStatus\t  = live\n");
            }
            sb.Append("\tPerson ID = " + person_id.ToString() + "\n");
            sb.Append("\tPosition  = " + image_position.ToString() + "\n");
            sb.Append(face_attributes.ToString());
            sb.Append("\n");

            return sb.ToString();
        }
    };

    /// <summary>Internal structure of general EyeFace array 
    /// used for passing array structures from C/C++ to C# 
    /// such as EfTrackInfoArray, EfDetectionArray, 
    /// EfLandmarksArray and EfFaceAttributesArray.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    internal struct EfUnmanagedArray
    {
        /// <summary>Number of array elements.</summary>
        public UInt32 num_elements;
        /// <summary>Pointer to the array elements data.</summary>
        public IntPtr array_elements;

        /// <summary>
        /// Generic function which returns the array containing elements copied from the C/C++ environment.
        /// <para>WARNING: It is up to user to check the type safety during conversion from C/C++ to C#!</para>
        /// </summary>
        /// <typeparam name="T">Type of the output array elements.</typeparam>
        /// <returns>Array containing elements copied from the C/C++ environment.</returns>
        public T[] getArray<T>() {
            T[] array = new T[num_elements];
            for (int i = 0; i < num_elements; i++) {
                IntPtr ptr = array_elements + i * Marshal.SizeOf(typeof(T));
                array[i] = (T)Marshal.PtrToStructure(ptr, typeof(T));
            }

            return array;
        }
    }

    /// <summary>Structure containing visualisation / statistics data for the last frame.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfTrackInfoArray
    {
        /// <summary>Number of face tracks in 'track_info' array.</summary>
        public readonly uint num_tracks;
        /// <summary>Array of face track visualisation data.</summary>
        public readonly EfTrackInfo[] track_info;

        /// <summary>
        /// Creates <see cref="EfTrackInfoArray"/> from <see cref="EfUnmanagedArray"/>.
        /// <para>WARNING: It is up to user to check the type safety during conversion from C/C++ to C#!</para>
        /// </summary>
        /// <param name="array">Array to convert to the <see cref="EfTrackInfoArray"/>.</param>
        internal EfTrackInfoArray(EfUnmanagedArray array) {
            num_tracks = array.num_elements;
            track_info = array.getArray<EfTrackInfo>();
        }

        public override string ToString() {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < num_tracks; i++) {
                sb.Append(track_info[i].ToString() + "\n");
            }
            return sb.ToString();
        }
    };

    /// <summary>Status of a connection to log server.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfLogToServerStatus
    {
        /// <summary>0-not-reachable 1-reachable</summary>
        public EfBool server_is_reachable;
        /// <summary>number of messages successfully sent</summary>
        public UInt32 num_messages_ok;
        /// <summary>number of messages failed to send</summary>
        public UInt32 num_messages_failed;
    };

    /// <summary>Position</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfPosition
    {
        /// <summary>Bounding box of face detection.</summary>
        public EfBoundingBox bounding_box;
        /// <summary>Center column of detection.</summary>
        public double center_col;
        /// <summary>Center row of detection.</summary>
        public double center_row;
        /// <summary>Scanning window height at detection.</summary>
        public double size;
    };

    /// <summary>Data related to a face detection. All data are related to a single frame, they are not aggregated as in EfVisualData.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfDetection
    {
        /// <summary>Detection confidence factor.</summary>
        public double confidence;
        /// <summary>Position of detection in image.</summary>
        public EfPosition position;
        /// <summary>Roll, Pitch, Yaw - only Yaw and Roll now used.</summary>
        public  EfAngles angles;
    };

    /// <summary>Detection result structure.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfDetectionArray
    {
        /// <summary>Number of face detections.</summary>
        public readonly uint num_detections;
        /// <summary>Array of detections.</summary>
        public readonly EfDetection[] detections;

        /// <summary>
        /// Creates <see cref="EfDetectionArray"/> from <see cref="EfUnmanagedArray"/>.
        /// <para>WARNING: It is up to user to check the type safety during conversion from C/C++ to C#!</para>
        /// </summary>
        /// <param name="array">Array to convert to the <see cref="EfDetectionArray"/>.</param>
        internal EfDetectionArray(EfUnmanagedArray array) {
            num_detections = array.num_elements;
            detections = array.getArray<EfDetection>();
        }
    };

    /// <summary>Landmarks result structure.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfLandmarksArray
    {
        /// <summary>Number of face detections on which landmarks has been computed.</summary>
        public UInt32 num_detections;
        /// <summary>Array of face landmarks.</summary>
        public EfLandmarks[] landmarks;

        /// <summary>
        /// Creates <see cref="EfLandmarksArray"/> from <see cref="EfUnmanagedArray"/>.
        /// <para>WARNING: It is up to user to check the type safety during conversion from C/C++ to C#!</para>
        /// </summary>
        /// <param name="array">Array to convert to the <see cref="EfLandmarksArray"/>.</param>
        internal EfLandmarksArray(EfUnmanagedArray array) {
            num_detections = array.num_elements;
            landmarks = array.getArray<EfLandmarks>();
        }
    };

    /// <summary>Face Attributes result structure.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfFaceAttributesArray
    {
        /// <summary>Number of face detections on which face attributes has been computed.</summary>
        public UInt32 num_detections;
        /// <summary>Array of face attributes.</summary>
        public EfFaceAttributes[] face_attributes;

        /// <summary>
        /// Creates <see cref="EfFaceAttributes"/> from <see cref="EfUnmanagedArray"/>.
        /// <para>WARNING: It is up to user to check the type safety during conversion from C/C++ to C#!</para>
        /// </summary>
        /// <param name="array">Array to convert to the <see cref="EfFaceAttributes"/>.</param>
        internal EfFaceAttributesArray(EfUnmanagedArray array) {
            num_detections = array.num_elements;
            face_attributes = array.getArray<EfFaceAttributes>();
        }
    };

    /// <summary>Structure representing the time and date information used by HASP.</summary>
    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct EfHaspTime
    {
        /// <summary>Year</summary>
        public int year;
        /// <summary>Month</summary>
        public int month;
        /// <summary>Day</summary>
        public int day;
        /// <summary>Hour</summary>
        public int hour;
        /// <summary>Minute</summary>
        public int minute;
        /// <summary>Second</summary>
        public int second;

        public EfHaspTime(int year, int month, int day, int hour, int minute, int second) {
            this.year   = year;
            this.month  = month;
            this.day    = day;
            this.hour   = hour;
            this.minute = minute;
            this.second = second;
        }

        public DateTime toDateTime() {
            return new DateTime(year, month, day, hour, minute, second);
        }
    };

    /// <summary>
    /// General EyeFace SDK exception.
    /// </summary>
    [Serializable]
    public class EfException : ApplicationException
    {
        private static string strEXHeader = "EyeFace Exception: ";

        public EfException() { }
        public EfException(string message)
            : base(strEXHeader + message) { }
        public EfException(string message, System.Exception inner)
            : base(strEXHeader + message, inner) { }

        protected EfException(System.Runtime.Serialization.SerializationInfo info,
            System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    };

    /// <summary>
    /// Exception used when a EyeFace SDK function called without proper initialization.
    /// </summary>
    [Serializable]
    public class EfUninitializedModule : EfException
    {
        private static string strEXHeader = "EyeFace module is not initialized.";

        public EfUninitializedModule()
            : base(strEXHeader) { }
        public EfUninitializedModule(string message)
            : base(strEXHeader + "\n" + message) { }
        public EfUninitializedModule(string message, System.Exception inner)
            : base(strEXHeader + "\n" + message, inner) { }

        protected EfUninitializedModule(System.Runtime.Serialization.SerializationInfo info,
            System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    };

    /// <summary>
    /// Class representing the EyeFace SDK module instance and containing all needed methods.
    /// </summary>
    public class EfCsSDK {
        /// <summary>
        /// Native methods for explicit linking.
        /// </summary>
        static class NativeMethods {
            [DllImport("kernel32.dll")]
            public static extern IntPtr LoadLibrary(string dllToLoad);

            [DllImport("kernel32.dll")]
            public static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

            [DllImport("kernel32.dll")]
            public static extern bool FreeLibrary(IntPtr hModule);
        }

        ///////
        // EyeFace Standard API function types
        ///////
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efInitEyeFace(string eyefacesdk_dir, string config_ini_dir, string config_ini_filename, void** eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efShutdownEyeFace(void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efResetEyeFace(void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efFreeEyeFace(void** eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate Int32 fcn_efGetLibraryVersion();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efMain(ERImage image, EfBoundingBox* bounding_box, double frame_time, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efGetTrackInfo(EfUnmanagedArray* track_info_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efFreeTrackInfo(EfUnmanagedArray* track_info_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efLogToServerGetConnectionStatus(EfLogToServerStatus* connection_status, void* eyeface_state);

        ///////
        // EyeFace Expert API function types
        ///////
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efRunFaceDetector(ERImage image, EfUnmanagedArray* detection_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efFreeDetections(EfUnmanagedArray* detection_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efUpdateTracker(ERImage image, EfUnmanagedArray detection_array, double frame_time, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efRunFaceLandmark(ERImage image, EfUnmanagedArray detection_array, IntPtr detections_to_process,
                                                            EfUnmanagedArray* facial_landmarks_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efFreeLandmarks(EfUnmanagedArray* facial_landmarks_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efRecognizeFaceAttributes(ERImage image, EfUnmanagedArray detection_array, EfUnmanagedArray* facial_landmarks_array,
                                                                    IntPtr detections_to_process, UInt32 request_flag, double frame_time, EfBool process_sequentially,
                                                                    EfUnmanagedArray* face_attributes_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate void fcn_efFreeAttributes(EfUnmanagedArray* face_attributes_array, void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efLogToFileWriteTrackInfo(void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efLogToServerSendPing(void* eyeface_state);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate EfBool fcn_efLogToServerSendTrackInfo(void* eyeface_state);

        // Sentinel LDK
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate long fcn_efHaspGetCurrentLoginKeyId();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        unsafe private delegate int fcn_efHaspGetExpirationDate(long key_id, EfHaspTime* exp_time);

        ////////
        // Define dll and function pointers
        ////////
        IntPtr pDll = IntPtr.Zero;

        IntPtr pEfInitEyeFace                    = IntPtr.Zero;
        IntPtr pEfShutdownEyeFace                = IntPtr.Zero;
        IntPtr pEfResetEyeFace                   = IntPtr.Zero;
        IntPtr pEfFreeEyeFace                    = IntPtr.Zero;
        IntPtr pEfGetLibraryVersion              = IntPtr.Zero;
        IntPtr pEfMain                           = IntPtr.Zero;
        IntPtr pEfGetTrackInfo                   = IntPtr.Zero;
        IntPtr pEfFreeTrackInfo                  = IntPtr.Zero;
        IntPtr pEfLogToServerGetConnectionStatus = IntPtr.Zero;

        IntPtr pEfRunFaceDetector                = IntPtr.Zero;
        IntPtr pEfFreeDetections                 = IntPtr.Zero;
        IntPtr pEfUpdateTracker                  = IntPtr.Zero;
        IntPtr pEfRunFaceLandmark                = IntPtr.Zero;
        IntPtr pEfFreeLandmarks                  = IntPtr.Zero;
        IntPtr pEfRecognizeFaceAttributes        = IntPtr.Zero;
        IntPtr pEfFreeAttributes                 = IntPtr.Zero;
        IntPtr pEfLogToFileWriteTrackInfo        = IntPtr.Zero;
        IntPtr pEfLogToServerSendPing            = IntPtr.Zero;
        IntPtr pEfLogToServerSendTrackInfo       = IntPtr.Zero;

        // Sentinel LDK
        IntPtr pEfHaspGetCurrentLoginKeyId       = IntPtr.Zero;
        IntPtr pEfHaspGetExpirationDate          = IntPtr.Zero;

        ///////////////
        // Define delegates of functions
        ///////////////
        fcn_efInitEyeFace                    fcnEfInitEyeFace;
        fcn_efShutdownEyeFace                fcnEfShutdownEyeFace;
        fcn_efResetEyeFace                   fcnEfResetEyeFace;
        fcn_efFreeEyeFace                    fcnEfFreeEyeFace;
        fcn_efGetLibraryVersion              fcnEfGetLibraryVersion;
        fcn_efMain                           fcnEfMain;
        fcn_efGetTrackInfo                   fcnEfGetTrackInfo;
        fcn_efFreeTrackInfo                  fcnEfFreeTrackInfo;
        fcn_efLogToServerGetConnectionStatus fcnEfLogToServerGetConnectionStatus;

        fcn_efRunFaceDetector                fcnEfRunFaceDetector;
        fcn_efFreeDetections                 fcnEfFreeDetections;
        fcn_efUpdateTracker                  fcnEfUpdateTracker;
        fcn_efRunFaceLandmark                fcnEfRunFaceLandmark;
        fcn_efFreeLandmarks                  fcnEfFreeLandmarks;
        fcn_efRecognizeFaceAttributes        fcnEfRecognizeFaceAttributes;
        fcn_efFreeAttributes                 fcnEfFreeAttributes;
        fcn_efLogToFileWriteTrackInfo        fcnEfLogToFileWriteTrackInfo;
        fcn_efLogToServerSendPing            fcnEfLogToServerSendPing;
        fcn_efLogToServerSendTrackInfo       fcnEfLogToServerSendTrackInfo;

        // Sentinel LDK
        fcn_efHaspGetCurrentLoginKeyId       fcnEfHaspGetCurrentLoginKeyId;
        fcn_efHaspGetExpirationDate          fcnEfHaspGetExpirationDate;

        /// <summary>
        /// Pointer to the module instance.
        /// </summary>
        unsafe private void* pvModuleState = null;

        /// <summary>
        /// Allows image manipulation in the <see cref="ERImage"/> structure.
        /// </summary>
        ERImageUtils erImageUtils = null;

        private IntPtr loadFunctionFromDLL(IntPtr dllPtr, string functionName) {
            IntPtr functionPtr = NativeMethods.GetProcAddress(dllPtr, functionName);
            if (functionPtr == IntPtr.Zero) {
                throw new EfException(functionName + " null");
            }

            return functionPtr;
        }

        /// <summary>
        /// EyeFace SDK DLL loading.
        /// </summary>
        /// <param name="eyefacesdkDir">Path to the EyeFace SDK folder.</param>
        public EfCsSDK(string eyefacesdkDir) {
            eyefacesdkDir.Replace('\\', '/');
            string eyefacesdkLibDir = Path.Combine(eyefacesdkDir, "lib/");

            // Add EyeFace SDK lib path to the PATH variable
            // to met dependencies loading during explicit linking.
            string pathVariable = Environment.GetEnvironmentVariable("PATH");
            if (!pathVariable.Contains(eyefacesdkLibDir)) {
                Environment.SetEnvironmentVariable("PATH", eyefacesdkLibDir + ";" + pathVariable);
            }

            string dllPath = Path.Combine(eyefacesdkLibDir, "EyeFace.dll");
            // open dll
            pDll = NativeMethods.LoadLibrary(@dllPath);
            if (pDll == IntPtr.Zero) {
                throw new EfException("Loading library " + dllPath + " failed!");
            }

            //////////////////////////
            // load functions from dll
            //////////////////////////
            pEfInitEyeFace                    = loadFunctionFromDLL(pDll, "efInitEyeFace");
            pEfShutdownEyeFace                = loadFunctionFromDLL(pDll, "efShutdownEyeFace");
            pEfResetEyeFace                   = loadFunctionFromDLL(pDll, "efResetEyeFace");
            pEfFreeEyeFace                    = loadFunctionFromDLL(pDll, "efFreeEyeFace");
            pEfGetLibraryVersion              = loadFunctionFromDLL(pDll, "efGetLibraryVersion");
            pEfMain                           = loadFunctionFromDLL(pDll, "efMain");
            pEfGetTrackInfo                   = loadFunctionFromDLL(pDll, "efGetTrackInfo");
            pEfFreeTrackInfo                  = loadFunctionFromDLL(pDll, "efFreeTrackInfo");
            pEfLogToServerGetConnectionStatus = loadFunctionFromDLL(pDll, "efLogToServerGetConnectionStatus");

            pEfRunFaceDetector                = loadFunctionFromDLL(pDll, "efRunFaceDetector");
            pEfFreeDetections                 = loadFunctionFromDLL(pDll, "efFreeDetections");
            pEfUpdateTracker                  = loadFunctionFromDLL(pDll, "efUpdateTracker");
            pEfRunFaceLandmark                = loadFunctionFromDLL(pDll, "efRunFaceLandmark");
            pEfFreeLandmarks                  = loadFunctionFromDLL(pDll, "efFreeLandmarks");
            pEfRecognizeFaceAttributes        = loadFunctionFromDLL(pDll, "efRecognizeFaceAttributes");
            pEfFreeAttributes                 = loadFunctionFromDLL(pDll, "efFreeAttributes");
            pEfLogToFileWriteTrackInfo        = loadFunctionFromDLL(pDll, "efLogToFileWriteTrackInfo");
            pEfLogToServerSendPing            = loadFunctionFromDLL(pDll, "efLogToServerSendPing");
            pEfLogToServerSendTrackInfo       = loadFunctionFromDLL(pDll, "efLogToServerSendTrackInfo");

            // Try to load HASP functions if available
            try {
                pEfHaspGetCurrentLoginKeyId   = loadFunctionFromDLL(pDll, "efHaspGetCurrentLoginKeyId");
                pEfHaspGetExpirationDate      = loadFunctionFromDLL(pDll, "efHaspGetExpirationDate");
            } catch (Exception) {
                pEfHaspGetCurrentLoginKeyId   = IntPtr.Zero;
                pEfHaspGetExpirationDate      = IntPtr.Zero;
            }

            ///////////////////////
            // Setup delegates
            ///////////////////////
            fcnEfInitEyeFace                    = (fcn_efInitEyeFace)                   Marshal.GetDelegateForFunctionPointer(pEfInitEyeFace,                    typeof(fcn_efInitEyeFace));
            fcnEfShutdownEyeFace                = (fcn_efShutdownEyeFace)               Marshal.GetDelegateForFunctionPointer(pEfShutdownEyeFace,                typeof(fcn_efShutdownEyeFace));
            fcnEfResetEyeFace                   = (fcn_efResetEyeFace)                  Marshal.GetDelegateForFunctionPointer(pEfResetEyeFace,                   typeof(fcn_efResetEyeFace));
            fcnEfFreeEyeFace                    = (fcn_efFreeEyeFace)                   Marshal.GetDelegateForFunctionPointer(pEfFreeEyeFace,                    typeof(fcn_efFreeEyeFace));
            fcnEfGetLibraryVersion              = (fcn_efGetLibraryVersion)             Marshal.GetDelegateForFunctionPointer(pEfGetLibraryVersion,              typeof(fcn_efGetLibraryVersion));
            fcnEfMain                           = (fcn_efMain)                          Marshal.GetDelegateForFunctionPointer(pEfMain,                           typeof(fcn_efMain));
            fcnEfGetTrackInfo                   = (fcn_efGetTrackInfo)                  Marshal.GetDelegateForFunctionPointer(pEfGetTrackInfo,                   typeof(fcn_efGetTrackInfo));
            fcnEfFreeTrackInfo                  = (fcn_efFreeTrackInfo)                 Marshal.GetDelegateForFunctionPointer(pEfFreeTrackInfo,                  typeof(fcn_efFreeTrackInfo));
            fcnEfLogToServerGetConnectionStatus = (fcn_efLogToServerGetConnectionStatus)Marshal.GetDelegateForFunctionPointer(pEfLogToServerGetConnectionStatus, typeof(fcn_efLogToServerGetConnectionStatus));

            fcnEfRunFaceDetector                = (fcn_efRunFaceDetector)               Marshal.GetDelegateForFunctionPointer(pEfRunFaceDetector,                typeof(fcn_efRunFaceDetector));
            fcnEfFreeDetections                 = (fcn_efFreeDetections)                Marshal.GetDelegateForFunctionPointer(pEfFreeDetections,                 typeof(fcn_efFreeDetections));
            fcnEfUpdateTracker                  = (fcn_efUpdateTracker)                 Marshal.GetDelegateForFunctionPointer(pEfUpdateTracker,                  typeof(fcn_efUpdateTracker));
            fcnEfRunFaceLandmark                = (fcn_efRunFaceLandmark)               Marshal.GetDelegateForFunctionPointer(pEfRunFaceLandmark,                typeof(fcn_efRunFaceLandmark));
            fcnEfFreeLandmarks                  = (fcn_efFreeLandmarks)                 Marshal.GetDelegateForFunctionPointer(pEfFreeLandmarks,                  typeof(fcn_efFreeLandmarks));
            fcnEfRecognizeFaceAttributes        = (fcn_efRecognizeFaceAttributes)       Marshal.GetDelegateForFunctionPointer(pEfRecognizeFaceAttributes,        typeof(fcn_efRecognizeFaceAttributes));
            fcnEfFreeAttributes                 = (fcn_efFreeAttributes)                Marshal.GetDelegateForFunctionPointer(pEfFreeAttributes,                 typeof(fcn_efFreeAttributes));
            fcnEfLogToFileWriteTrackInfo        = (fcn_efLogToFileWriteTrackInfo)       Marshal.GetDelegateForFunctionPointer(pEfLogToFileWriteTrackInfo,        typeof(fcn_efLogToFileWriteTrackInfo));
            fcnEfLogToServerSendPing            = (fcn_efLogToServerSendPing)           Marshal.GetDelegateForFunctionPointer(pEfLogToServerSendPing,            typeof(fcn_efLogToServerSendPing));
            fcnEfLogToServerSendTrackInfo       = (fcn_efLogToServerSendTrackInfo)      Marshal.GetDelegateForFunctionPointer(pEfLogToServerSendTrackInfo,       typeof(fcn_efLogToServerSendTrackInfo));

            try {
                fcnEfHaspGetCurrentLoginKeyId   = (fcn_efHaspGetCurrentLoginKeyId)      Marshal.GetDelegateForFunctionPointer(pEfHaspGetCurrentLoginKeyId,       typeof(fcn_efHaspGetCurrentLoginKeyId));
                fcnEfHaspGetExpirationDate      = (fcn_efHaspGetExpirationDate)         Marshal.GetDelegateForFunctionPointer(pEfHaspGetExpirationDate,          typeof(fcn_efHaspGetExpirationDate));
            } catch (Exception) {
                fcnEfHaspGetCurrentLoginKeyId   = null;
                fcnEfHaspGetExpirationDate      = null;
            }

            erImageUtils = new ERImageUtils(pDll);
        }

        /// <summary>
        /// EyeFace SDK instance destructor. Unloads the EyeFace SDK instance and the DLL library.
        /// </summary>
        ~EfCsSDK() {
            try {
                unsafe {
                    if (pDll != IntPtr.Zero) {
                        if (pvModuleState != null) {
                            fixed (void** ppvModuleState = &pvModuleState) {
                                fcnEfFreeEyeFace(ppvModuleState);
                            }
                        }
                        NativeMethods.FreeLibrary(pDll);
                    }
                }
            }
            catch { }
        }

        /// <summary>
        /// Checks whether the module is loaded.
        /// </summary>
        /// <returns>Loading state of the module.</returns>
        public bool IsLoaded() {
            unsafe {
                return pvModuleState == null ? false : true;
            }
        }

        private void checkModuleInitialized(bool checkSDKInit = true) {
            unsafe {
                if ((pvModuleState == null && checkSDKInit) ||
                    pDll == IntPtr.Zero ||
                    erImageUtils == null) {
                    throw new EfUninitializedModule();
                }
            }
        }

        /// <summary>
        /// Creates the instance of the <seealso cref="ERImage"/> and fills it with the image data from <seealso cref="Bitmap"/>.
        /// </summary>
        /// <param name="bitmap">Input bitmap to convert.</param>
        /// <returns>Created <seealso cref="ERImage"/> with input image data.</returns>
        public ERImage csBitmapToERImage(Bitmap bitmap) {
            checkModuleInitialized(false);
            return erImageUtils.csBitmapToERImage(bitmap);
        }

        /// <summary>
        /// Converts the <see cref="ERImage"/> to the <see cref="Bitmap"/>. Image data is copied during the conversion.
        /// <para/>
        /// WARNING: Float images are not supported by <see cref="Bitmap"/>. 
        /// All <see cref="ERImage"/> structures with float image data are converted 
        /// to the <see cref="byte"/> data type ((<see cref="byte"/>)(erImage.data[i]*255)).
        /// </summary>
        /// <param name="image">Input image <see cref="ERImage"/> to convert.</param>
        /// <returns>Bitmap containing image data.</returns>
        /// <exception cref="ERException">When unsupported color model used (<see cref="ERImageColorModel.ER_IMAGE_COLORMODEL_YCBCR420"/>).</exception>
        public Bitmap erImageToCsBitmap(ERImage image) {
            checkModuleInitialized(false);
            return erImageUtils.erImageToCsBitmap(image);
        }

        /// <summary>
        /// Reads the image <seealso cref="ERImage"/> from the the file.
        /// </summary>
        /// <param name="filename">Path to the file to read the image from.</param>
        /// <returns>Image <seealso cref="ERImage"/> read from the file.</returns>
        public ERImage erImageRead(string filename) {
            checkModuleInitialized(false);
            return erImageUtils.erImageRead(filename);
        }

        /// <summary>
        /// Writes the input <seealso cref="ERImage"/> to the file.
        /// </summary>
        /// <param name="image">Input image to write.</param>
        /// <param name="filename">Path to the file to write the image.</param>
        public void erImageWrite(ERImage image, string filename) {
            checkModuleInitialized(false);
            erImageUtils.erImageWrite(image, filename);
        }

        /// <summary>
        /// Frees the input <seealso cref="ERImage"/>.
        /// </summary>
        /// <param name="image">Input image to free.</param>
        public void erImageFree(ref ERImage image) {
            checkModuleInitialized(false);
            erImageUtils.erImageFree(ref image);
        }

        /// <summary>
        /// EyeFace SDK initialization.
        /// </summary>
        /// <param name="eyefacesdkDir">Path to the EyeFace SDK folder.</param>
        /// <param name="configIniDir">Path to the folder containing EyeFace SDK config ini file.</param>
        /// <param name="configIniFilename">Filename of the EyeFace SDK config ini file.</param>
        public bool efInitEyeFace(string eyefacesdkDir, string configIniDir, string configIniFilename) {
            unsafe {
                if (pDll == IntPtr.Zero) {
                    return false;
                }

                if (pvModuleState != null) {
                    fixed (void** ppvModuleState = &pvModuleState) {
                        fcnEfFreeEyeFace(ppvModuleState);
                    }
                }

                fixed (void** ppvModuleState = &pvModuleState) {
                    bool initStatus = fcnEfInitEyeFace(eyefacesdkDir, configIniDir, configIniFilename, ppvModuleState);
                    return initStatus;
                }
            }
        }

        /// <summary>
        /// Flushes tracking buffers to logs and outputs before EyeFace termination / reset. This enables clients to get tracking result on last frame.
        /// </summary>
        public void efShutdownEyeFace() {
            unsafe {
                checkModuleInitialized();
                fcnEfShutdownEyeFace(pvModuleState);
            }
        }

        /// <summary>
        /// Clears/resets state structure without deallocation. All tracks are deleted.
        /// </summary>
        public void efResetEyeFace() {
            unsafe {
                checkModuleInitialized();
                fcnEfResetEyeFace(pvModuleState);
            }
        }

        /// <summary>
        /// Frees EyeFace SDK engine. Releases the license sessions.
        /// </summary>
        public void efFreeEyeFace() {
            unsafe {
                checkModuleInitialized();
                fixed (void** ppvModuleState = &pvModuleState) {
                    fcnEfFreeEyeFace(ppvModuleState);
                }
                pvModuleState = null;
            }
        }

        /// <summary>
        /// Returns the version of the library, for example if v4.01.0915 then the return value is 40010915. 
        /// The return value must be the same as defined in EYEFACE_VERSION_NUMBER global variable, otherwise there is a header/library version mismatch.
        /// </summary>
        /// <returns>Version number.</returns>
        public int efGetLibraryVersion() {
            unsafe {
                int? version = fcnEfGetLibraryVersion?.Invoke();
                return version.GetValueOrDefault(0);
            }
        }

        /// <summary>
        /// Processes current image, i.e. detects faces, runs age/gender/etc recognition and tracks the detected faces. 
        /// The image must be part of a videosequence.
        /// </summary>
        /// <param name="image">Input image in <see cref="ERImage"/> format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="boundingBox">Bonding-box selecting the active image area where the detection will take place (null to process whole image). 
        /// Automatically orthogonalizes input bounding box.</param>
        /// <param name="frameTime">Current frame time since <see cref="efInitEyeFace(string, string, string)"/> 
        /// in seconds with millisecond precision (i.e. the first frame time can be 0.000).</param>
        /// <returns>true on success, false on failure.</returns>
        public bool efMain(ERImage image, EfBoundingBox boundingBox, double frameTime) {
            unsafe {
                checkModuleInitialized();
                bool mainStatus = fcnEfMain(image, &boundingBox, frameTime, pvModuleState);
                return mainStatus;
            }
        }

        /// <summary>
        /// Processes current image, i.e. detects faces, runs age/gender/etc recognition and tracks the detected faces. 
        /// </summary>
        /// <param name="image">Input image in <see cref="ERImage"/> format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="frameTime">Current frame time since <see cref="efInitEyeFace(string, string, string)"/> 
        /// in seconds with millisecond precision (i.e. the first frame time can be 0.000).</param>
        /// <returns>true on success, false on failure.</returns>
        public bool efMain(ERImage image, double frameTime) {
            EfBoundingBox boundingBox = new EfBoundingBox(image.width, image.height);
            return efMain(image, boundingBox, frameTime);
        }

        /// <summary>
        /// Returns the aggregated result related to a single frame of video. Can be used for real-time visualisation or statistics.
        /// </summary>
        /// <returns>Data structure filled with results. Must be freed with <see cref="efFreeTrackInfo(EfTrackInfoArray)"/>.</returns>
        public EfTrackInfoArray efGetTrackInfo() {
            unsafe {
                checkModuleInitialized();
                EfUnmanagedArray unmanagedArray = new EfUnmanagedArray();
                bool trackInfoStatus = fcnEfGetTrackInfo(&unmanagedArray, pvModuleState);
                if (!trackInfoStatus) {
                    throw new EfException("Cannot get EfTrackInfoArray.");
                }
                EfTrackInfoArray trackInfoArray = new EfTrackInfoArray(unmanagedArray);
                fcnEfFreeTrackInfo(&unmanagedArray, pvModuleState);

                return trackInfoArray;
            }
        }

        /// <summary>
        /// Get status of connection to log-server.
        /// </summary>
        /// <returns><see cref="EfLogToServerStatus"/> data structure, where the result will be stored.</returns>
        public EfLogToServerStatus efLogToServerGetConnectionStatus() {
            unsafe {
                checkModuleInitialized();
                EfLogToServerStatus connectionStatus = new EfLogToServerStatus();
                bool getStatus = fcnEfLogToServerGetConnectionStatus(&connectionStatus, pvModuleState);
                if (!getStatus) {
                    throw new EfException("Error during getting log to server connection status.");
                }

                return connectionStatus;
            }
        }

        /// <summary>
        /// Runs face detection on a single image. Suitable both for video-sequence images (Expert) manipulation 
        /// or standalone images (e.g. image databases).
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <returns>On successful completition, structure filled with face detections for the given image.</returns>
        public EfDetectionArray efRunFaceDetector(ERImage image) {
            unsafe {
                checkModuleInitialized();
                EfUnmanagedArray unmanagedArray = new EfUnmanagedArray();
                bool detectionStatus = fcnEfRunFaceDetector(image, &unmanagedArray, pvModuleState);
                if (!detectionStatus) {
                    throw new EfException("Error during detection.");
                }
                EfDetectionArray detectionArray = new EfDetectionArray(unmanagedArray);
                fcnEfFreeDetections(&unmanagedArray, pvModuleState);

                return detectionArray;
            }
        }

        /// <summary>
        /// Updates the tracker state after a frame is processed.
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="detectionArray"><see cref="EfDetectionArray"/> structure filled by <see cref="efRunFaceDetector(ERImage)"/>. 
        /// Implementation is guaranteed not to write to detectionArray.detections.</param>
        /// <param name="frameTime">Image frame time for tracking purposes (in seconds). 
        /// MUST be increasing. If timestamp is repeated or goes back in time, it results in undefined behavior.</param>
        /// <returns>true on success, false on failure.</returns>
        public bool efUpdateTracker(ERImage image, EfDetectionArray detectionArray, double frameTime) {
            unsafe {
                checkModuleInitialized();
                EfUnmanagedArray unmanagedArray = new EfUnmanagedArray();
                unmanagedArray.num_elements = detectionArray.num_detections;

                GCHandle handle = GCHandle.Alloc(detectionArray.detections, GCHandleType.Pinned);
                try {
                    unmanagedArray.array_elements = handle.AddrOfPinnedObject();
                    bool updateStatus = fcnEfUpdateTracker(image, unmanagedArray, frameTime, pvModuleState);
                    return updateStatus;
                } finally {
                    if (handle.IsAllocated) {
                        handle.Free();
                    }
                }
            }
        }

        /// <summary>
        /// Runs face landmark detection on face detections from EfDetResult. 
        /// Landmarks are only used for visualization - turned off by default during initialization. 
        /// This function has a side efect of storing the landmarks to tracks.
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="detectionArray"><see cref="EfDetectionArray"/> structure returned by <see cref="efRunFaceDetector(ERImage)"/>. 
        /// Implementation is guaranteed not to write to detectionArray.detections.</param>
        /// <param name="detectionsToProcess">User allocated array of detectionArray.num_detections <see cref="bool"/>s. 
        /// true value at index i means that the i-th detection in detectionArray will be processed. 
        /// If null, all detections are processed.</param>
        /// <returns>On successful completion, structure filled with landmark data of the faces in detectionArray.</returns>
        public EfLandmarksArray efRunFaceLandmark(ERImage image, EfDetectionArray detectionArray, bool[] detectionsToProcess) {
            unsafe {
                checkModuleInitialized();
                EfUnmanagedArray detectionArrayUnmg = new EfUnmanagedArray();
                detectionArrayUnmg.num_elements = detectionArray.num_detections;

                EfBool[] detectionsToProcessEf = EfBool.boolArrayToEfBoolArray(detectionsToProcess);
                EfUnmanagedArray landmarksArrayUnmg = new EfUnmanagedArray();

                GCHandle handleDet  = GCHandle.Alloc(detectionArray.detections, GCHandleType.Pinned);
                GCHandle handleProc = GCHandle.Alloc(detectionsToProcessEf, GCHandleType.Pinned);
                try {
                    detectionArrayUnmg.array_elements = handleDet.AddrOfPinnedObject();
                    IntPtr detectionsToProcesPtr = handleProc.AddrOfPinnedObject();
                    bool landmarkStatus = fcnEfRunFaceLandmark(image, detectionArrayUnmg, detectionsToProcesPtr, &landmarksArrayUnmg, pvModuleState);
                    if (!landmarkStatus) {
                        throw new EfException("Error during landmarks detection.");
                    }
                    EfLandmarksArray landmarksArray = new EfLandmarksArray(landmarksArrayUnmg);
                    fcnEfFreeLandmarks(&landmarksArrayUnmg, pvModuleState);

                    return landmarksArray;
                } finally {
                    if (handleDet.IsAllocated) {
                        handleDet.Free();
                    }
                    if (handleProc.IsAllocated) {
                        handleProc.Free();
                    }
                }
            }
        }

        /// <summary>
        /// Runs face landmark detection on face detections from EfDetResult. 
        /// Landmarks are only used for visualization - turned off by default during initialization. 
        /// This function has a side efect of storing the landmarks to tracks.
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="detectionArray"><see cref="EfDetectionArray"/> structure returned by <see cref="efRunFaceDetector(ERImage)"/>. 
        /// Implementation is guaranteed not to write to detectionArray.detections.</param>
        /// <returns>On successful completion, structure filled with landmark data of the faces in detectionArray.</returns>
        public EfLandmarksArray efRunFaceLandmark(ERImage image, EfDetectionArray detectionArray) {
            return efRunFaceLandmark(image, detectionArray, null);
        }

        /// <summary>
        /// Files a request to compute face attributes on a given face.
        /// <para>NOTE: For video-sequence processing, it is typical to set processSequentially = false. 
        /// This way, face attributes are assigned to tracks asynchronously, resulting in good performance. 
        /// This is the way <see cref="efMain(ERImage, double)"/> recognizes the face attributes. 
        /// For image database processing, it is required to set processSequentially = true 
        /// and insert a pointer to user allocated faceAttributesArray structure.</para>
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="detectionArray"><see cref="EfDetectionArray"/> structure filled by <see cref="efRunFaceDetector(ERImage)"/>. 
        /// Implementation is guaranteed not to write to detectionArray.detections.</param>
        /// <param name="facialLandmarksArray"><see cref="EfLandmarksArray"/> structure filled by <see cref="efRunFaceLandmark(ERImage, EfDetectionArray)"/>. 
        /// Optional, set to null if you did not compute the landmarks (default). Using the landmarks might improve the performance in future release.</param>
        /// <param name="detectionsToProcess">User allocated array of detectionArray.num_detections <see cref="bool"/>s. 
        /// true value at index i means that the i-th detection in detectionArray will be processed. 
        /// If null, all detections are processed.</param>
        /// <param name="requestFlag">Bit array (of <see cref="EfConstants"/>.EF_FACEATTRIBUTES_* flags) 
        /// setting which attributes to compute, typically <see cref="EfConstants.EF_FACEATTRIBUTES_ALL"/>.</param>
        /// <param name="frameTime">Image frame time for tracking purposes (in seconds with millisecond precision). 
        /// If timestamp is repeated or goes back in time, it results in undefined behavior. Set to zero for image database processing.</param>
        /// <param name="processSequentially">If true, waits until the processing is finished and optionally returns a result, 
        /// if false, returns immediatly and the results are appended to tracks later.</param>
        /// <returns>Optional output of face attributes of the faces in detectionArray, only filled if processSequentially == true. 
        /// Returned empty structure if processSequentially == false.</returns>
        public EfFaceAttributesArray efRecognizeFaceAttributes(ERImage image, EfDetectionArray detectionArray, EfLandmarksArray facialLandmarksArray, 
                                                               bool[] detectionsToProcess, uint requestFlag, double frameTime, bool processSequentially) {
            unsafe {
                checkModuleInitialized();
                EfUnmanagedArray detectionArrayUnmg = new EfUnmanagedArray();
                detectionArrayUnmg.num_elements = detectionArray.num_detections;

                EfUnmanagedArray landmarksArrayUnmg = new EfUnmanagedArray();
                landmarksArrayUnmg.num_elements = facialLandmarksArray.num_detections;

                EfBool[] detectionsToProcessEf = EfBool.boolArrayToEfBoolArray(detectionsToProcess);
                EfBool  processSequentiallyEf = processSequentially;

                EfUnmanagedArray faceAttributesArrayUnmg = new EfUnmanagedArray();

                GCHandle handleDet  = GCHandle.Alloc(detectionArray.detections, GCHandleType.Pinned);
                GCHandle handleLand = GCHandle.Alloc(facialLandmarksArray.landmarks, GCHandleType.Pinned);
                GCHandle handleProc = GCHandle.Alloc(detectionsToProcessEf, GCHandleType.Pinned);
                try {
                    detectionArrayUnmg.array_elements = handleDet.AddrOfPinnedObject();
                    landmarksArrayUnmg.array_elements = handleLand.AddrOfPinnedObject();
                    IntPtr detectionsToProcesPtr = handleProc.AddrOfPinnedObject();
                    bool faceAttributesStatus = fcnEfRecognizeFaceAttributes(image, detectionArrayUnmg, &landmarksArrayUnmg, 
                                                                             detectionsToProcesPtr, requestFlag, frameTime, 
                                                                             processSequentiallyEf, &faceAttributesArrayUnmg, pvModuleState);
                    if (!faceAttributesStatus) {
                        throw new EfException("Error during face attributes recognition.");
                    }
                    EfFaceAttributesArray faceAttributesArray = new EfFaceAttributesArray(faceAttributesArrayUnmg);
                    fcnEfFreeAttributes(&faceAttributesArrayUnmg, pvModuleState);

                    return faceAttributesArray;
                } finally {
                    if (handleDet.IsAllocated) {
                        handleDet.Free();
                    }
                    if (handleLand.IsAllocated) {
                        handleLand.Free();
                    }
                    if (handleProc.IsAllocated) {
                        handleProc.Free();
                    }
                }
            }
        }

        /// <summary>
        /// Files a request to compute face attributes on a given face and processes it sequentially.
        /// <para>NOTE: Since the processing is sequential this function is intended to be used 
        /// for image database processing.</para>
        /// </summary>
        /// <param name="image">Input image in ERImage format. Implementation is guaranteed not to write into image buffers.</param>
        /// <param name="detectionArray"><see cref="EfDetectionArray"/> structure filled by <see cref="efRunFaceDetector(ERImage)"/>. 
        /// Implementation is guaranteed not to write to detectionArray.detections.</param>
        /// <param name="facialLandmarksArray"><see cref="EfLandmarksArray"/> structure filled by <see cref="efRunFaceLandmark(ERImage, EfDetectionArray)"/>. 
        /// Optional, set to null if you did not compute the landmarks (default). Using the landmarks might improve the performance in future release.</param>
        /// <param name="frameTime">Image frame time for tracking purposes (in seconds with millisecond precision). 
        /// If timestamp is repeated or goes back in time, it results in undefined behavior. Set to zero for image database processing.</param>
        /// <returns>Output of face attributes of the faces in detectionArray.</returns>
        public EfFaceAttributesArray efRecognizeFaceAttributes(ERImage image, EfDetectionArray detectionArray, 
                                                               EfLandmarksArray facialLandmarksArray, double frameTime) {
            return efRecognizeFaceAttributes(image, detectionArray, facialLandmarksArray, 
                                             null, EfConstants.EF_FACEATTRIBUTES_ALL, frameTime, true);
        }

        /// <summary>
        /// Writes information about current state of tracks into a log file. The log file must be specified and enabled in config.ini.
        /// </summary>
        /// <returns>true on success, false on failure.</returns>
        public bool efLogToFileWriteTrackInfo() {
            unsafe {
                checkModuleInitialized();
                bool logStatus = fcnEfLogToFileWriteTrackInfo(pvModuleState);

                return logStatus;
            }
        }

        /// <summary>
        /// Sends a ping via a server connection specified in the [LOG TO SERVER] part of the config.ini. 
        /// The log server connection must be specified and enabled in config.ini.
        /// </summary>
        /// <returns>true on success, false on failure.</returns>
        public bool efLogToServerSendPing() {
            unsafe {
                checkModuleInitialized();
                bool logStatus = fcnEfLogToServerSendPing(pvModuleState);

                return logStatus;
            }
        }

        /// <summary>
        /// Writes information about current state of tracks into a server connection. 
        /// The log server connection must be specified and enabled in config.ini.
        /// </summary>
        /// <returns>true on success, false on failure.</returns>
        public bool efLogToServerSendTrackInfo() {
            unsafe {
                checkModuleInitialized();
                bool logStatus = fcnEfLogToServerSendTrackInfo(pvModuleState);

                return logStatus;
            }
        }

        /// <summary>
        /// Gets a single license key number  which will be used at startup by default. 
        /// Must be used prior to <see cref="efInitEyeFace(string, string, string)"/> to work.
        /// </summary>
        /// <returns>Key number of key containing EyeFace SDK license.</returns>
        public long efHaspGetCurrentLoginKeyId() {
            unsafe {
                long? key = fcnEfHaspGetCurrentLoginKeyId?.Invoke();
                if (!key.HasValue) {
                    throw new EfException("Function efHaspGetCurrentLoginKeyId is not available.");
                }

                return key.Value;
            }
        }

        /// <summary>
        /// Gets an expiration date for the given HASP key ID.
        /// </summary>
        /// <param name="key">ID of the key in question.</param>
        /// <param name="expirationTime"><see cref="EfHaspTime"/> structure pointer, for expiration date output.</param>
        /// <returns>true for valid license, false for not valid.</returns>
        public bool efHaspGetExpirationDate(long key, out EfHaspTime expirationTime) {
            unsafe {
                expirationTime = new EfHaspTime(0, 0, 0, 0, 0, 0);
                int? valid;
                fixed (EfHaspTime* expTimePtr = &expirationTime) {
                    valid = fcnEfHaspGetExpirationDate?.Invoke(key, expTimePtr);
                }
                if (!valid.HasValue) {
                    throw new EfException("Function efHaspGetExpirationDate is not available.");
                }

                if (valid == 1) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    };
}
