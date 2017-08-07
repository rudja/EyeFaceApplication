%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                                                              %%%
%%%    Example showing EyeFace Standard SDK face detection.      %%%
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

version = efGetLibraryVersion(eyeface_functions);

fprintf('Initialized EyeFace SDK v%d.\n', version);

% set up requested detection area
area.top_left_col = 1;
area.top_left_row = 1;
area.top_right_col = 640;
area.top_right_row = 1;
area.bot_left_col = 1;
area.bot_left_row = 480;
area.bot_right_col = 640;
area.bot_right_row = 480;

track_info = cell(150, 1);

for img_i = 0 : 149
    
    % load image
    img_name = sprintf(input_img_name, img_i);
    img = imread(img_name);
    
    % show image
    imshow(img);
    hold on
    
    %run efMain
    efMain(img, area, img_i/20, eyeface_state, eyeface_functions);
    
    %get track info
    track_info{img_i+1} = efGetTrackInfo(eyeface_state, eyeface_functions);

    for i = 1 : numel(track_info{img_i+1})
                
        % plot detection bounding box
        t = track_info{img_i+1}(i).image_position.top_left_row;
        l = track_info{img_i+1}(i).image_position.top_left_col;
        b = track_info{img_i+1}(i).image_position.bot_right_row;
        r = track_info{img_i+1}(i).image_position.bot_right_col;
        
        plot([l r r l l],[t t b b t], 'r', 'LineWidth',2);
        
        % plot landmarks, if available
        if track_info{img_i+1}(i).landmarks.points.length > 0
            for j = 2 : 8
                plot(track_info{img_i+1}(i).landmarks.points.cols(j),...
                    track_info{img_i+1}(i).landmarks.points.rows(j), 'xg');
            end
        end
    end
    
    % show axes
    axis on
    
    % show image on screen
    pause(0.1);
    hold off
end

% flush EyeFace buffers
efShutdownEyeFace(eyeface_state, eyeface_functions);

%free eyeface state
efFreeEyeFace(eyeface_state, eyeface_functions);
