%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                                              %%%
%%%    Example showing EyeFace Expert SDK face detection.        %%%
%%%   --------------------------------------------------------   %%%
%%%                                                              %%%
%%%    Eyedea Recognition, Ltd. (C) 2013, Dec, 31st              %%%
%%%                                                              %%%
%%%    Contact:                                                  %%%
%%%               web: http://www.eyedea.cz                      %%%
%%%             email: info@eyedea.cz                            %%%
%%%                                                              %%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% clear workspace
clc, clear, close all;

% set Matlab path to include mex files
addpath '../compiled/'

% set up directories
MODULE_DIR = '../../../eyefacesdk';
INI_DIR    = '../../../eyefacesdk';
CONFIG_INI = 'config.ini';

% setup mex
if strcmp(computer, 'PCWIN64') || strcmp(computer, 'PCWIN')
    support_libs = {[MODULE_DIR '/lib/libopenblas.dll']};
else
    support_libs = {};
end

[eyeface_functions] = efMexSetup(MODULE_DIR, support_libs);

% image files
input_img_name = '../../../data/test-images-id/%04d.png';

% init EyeFace
[eyeface_state] = efInitEyeFace(MODULE_DIR, INI_DIR, CONFIG_INI, eyeface_functions);

detections = cell(150,1);

for img_i = 0:149
    
    img_name = sprintf(input_img_name, img_i);
    img = imread(img_name);
    
    % run face detection routine, only available in EyeFace Advanced SDK
    [detections{img_i+1}] = efRunFaceDetector(img, eyeface_state, eyeface_functions);
    
    % show image
    imshow(img);
    hold on
    
    % LANDMARKS ARE DISABLED BY DEFAULT IN CONFIG.INI.
    % compute landmarks for frontal faces
    % curr_landmarks = efRunFaceLandmark(img, detections{img_i+1}, [], eyeface_state, eyeface_functions);
    
    % process detections
    for i = 1 : length(detections{img_i+1})
        
        % plot detection bounding box
        t = detections{img_i+1}(i).position.bounding_box.top_left_row;
        l = detections{img_i+1}(i).position.bounding_box.top_left_col;
        b = detections{img_i+1}(i).position.bounding_box.bot_right_row;
        r = detections{img_i+1}(i).position.bounding_box.bot_right_col;
     
        plot([l r r l l],[t t b b t], 'r', 'LineWidth',2);
     
        % plot landmarks, if available
        % if curr_landmarks(i).recognized
        %     for j = 2 : curr_landmarks(i).points.length
        %         plot(curr_landmarks(i).points.cols(j), curr_landmarks(i).points.rows(j), 'xg');
        %     end
        % end
    end
    
    % face attributes
    EF_FACEATTRIBUTES_ALL = uint32(2^32-1); % FLAG from EyeFaceExpertType.h
    face_attributes = efRecognizeFaceAttributes(img, detections{img_i+1}, [], [], EF_FACEATTRIBUTES_ALL, ...
                                                0, true, eyeface_state, eyeface_functions);
    
    num_males = 0;
    for i = 1 : numel(face_attributes)
        if face_attributes(i).gender.value == -1
            num_males = num_males + 1;
        end
    end
    
    fprintf('Number of males in photo: %d.\n', num_males);
                                            
    % show axes
    axis on
        
    % show image on screen
    pause(0.1);
    hold off
end

% free EyeFace
efFreeEyeFace(eyeface_state, eyeface_functions);
