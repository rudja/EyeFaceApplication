function make_eyeface_sdk_mex
% make_eyeface_sdk_mex Compiles all MEX files of EyeFace SDK.
%
% Synopsis:
%  make_eyeface_sdk_mex
%
% Description:
%  Compiles the EyeFace SDK mex files. 

% Author: Eyedea Recognition Ltd., 2013-2017

if ismac || isunix
    system_specific_libs = '-ldl';
elseif ispc
    system_specific_libs = '';
else
   error('Platform not supported. Use Windows, Linux or Mac OS'); 
end
    
eyeface_path = '../../eyefacesdk/';

fprintf('Compiling MEX files...\n');
   
root=pwd;              % get current directory
% -- List of functions to be complied ---------------------------
i = 1;

fun(i).source={'$efMexSetup.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efMexSetup'};
i = i + 1;

fun(i).source={'$efInitEyeFace.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efInitEyeFace'};
i = i + 1;

fun(i).source={'$efShutdownEyeFace.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efShutdownEyeFace'};
i = i + 1;

fun(i).source={'$efResetEyeFace.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efResetEyeFace'};
i = i + 1;

fun(i).source={'$efFreeEyeFace.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efFreeEyeFace'};
i = i + 1;

fun(i).source={'$efGetLibraryVersion.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efGetLibraryVersion'};
i = i + 1;

fun(i).source={'$efStructIO.mex.cpp', '$efMain.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
     ['-I"' eyeface_path 'include" ']};
fun(i).lib = {};
fun(i).outname = {'$efMain'};
i = i + 1;
 
fun(i).source={'$efStructIO.mex.cpp','$efGetTrackInfo.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {};
fun(i).outname = {'$efGetTrackInfo'};
i = i + 1;

fun(i).source={'$efStructIO.mex.cpp','$efLogToServerGetConnectionStatus.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {};
fun(i).outname = {'$efLogToServerGetConnectionStatus'};
i = i + 1;

% EXPERT API
fun(i).source={'$efStructIO.mex.cpp', '$efRunFaceDetector.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {};
fun(i).outname = {'$efRunFaceDetector'};
i = i + 1;

fun(i).source={'$efStructIO.mex.cpp', '$efUpdateTracker.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" '] };
fun(i).lib = {};
fun(i).outname = {'$efUpdateTracker'};
i = i + 1;

fun(i).source={'$efStructIO.mex.cpp', '$efRunFaceLandmark.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" '] };
fun(i).lib = {};
fun(i).outname = {'$efRunFaceLandmark'};
i = i + 1;

fun(i).source={'$efStructIO.mex.cpp', '$efRecognizeFaceAttributes.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" '] };
fun(i).lib = {};
fun(i).outname = {'$efRecognizeFaceAttributes'};
i = i + 1;

fun(i).source={'$efLogToFileWriteTrackInfo.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efLogToFileWriteTrackInfo'};
i = i + 1;

fun(i).source={'$efLogToServerSendPing.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efLogToServerSendPing'};
i = i + 1;

fun(i).source={'$efLogToServerSendTrackInfo.mex.cpp'};
fun(i).outdir='$';
fun(i).include={'$ ', ['-I"' matlabroot '/extern/include" '], ...
    ['-I"' eyeface_path 'include" ']};
fun(i).lib = {system_specific_libs};
fun(i).outname = {'$efLogToServerSendTrackInfo'};
i = i + 1;

% -- Compile functions -----------------------------
for i=1:length(fun)
             
    if ~exist('OCTAVE_VERSION', 'builtin')  %is MATLAB
        mexstr = ['mex -g -I' translate(fun(i).include,root) ...
                  translate(fun(i).lib,root)...
                  ' -outdir ' translate(fun(i).outdir, root) ' ' '-output ' ...
                  translate(fun(i).outname, root) ' '];
    else  %is OCTAVE
        mexstr = ['mkoctfile --mex -o -I' translate(fun(i).include,root) ...
                  translate(fun(i).lib,root)...
                  ' ' '-output ' ...
                  translate(fun(i).outname, root) ' '];
    end          
             
  
    mexstr = [mexstr{:}];
         
    for j=1:length(fun(i).source),    
        mexstr = [mexstr translate(char(fun(i).source(j)),root) ' '];
    end

    fprintf('%s\n',mexstr);
  
    eval(mexstr);
end

fprintf('MEX-files compiled successfully.\n');

if ~exist('./compiled', 'dir')
    mkdir('compiled');
else
    rmdir('compiled', 's')
    mkdir('compiled')
end

for i = 1 : length(fun)
   outfile = translate(fun(i).outname, root);
   outfile2 = [outfile{:} '.' mexext];
   movedfile = ['./compiled/' fun(i).outname{:}(2:end) '.' mexext];
   movefile(outfile2, movedfile, 'f');
   
   pdbfile = [outfile{:} '.' mexext '.pdb'];
   if exist(pdbfile, 'file')
        movefile(pdbfile, ['./compiled/' fun(i).outname{:}(2:end) '.' mexext '.pdb'], 'f');
   end
end

fprintf('MEX-files moved to ./compiled.\n');

return;

%--translate ---------------------------------------------------------
function p = translate(p,toolboxroot);
%TRANSLATE Translate unix path to platform specific path
%   TRANSLATE fixes up the path so that it's valid on non-UNIX platforms
%
% This function was derived from MathWork M-file "pathdef.m"

cname = computer;

% Look for PC
if strncmp(cname,'PC',2)
  p = strrep(p,'/','\');
 % p = strrep(p,':',';');
  p = strrep(p,'$',[toolboxroot '\']);

% Look for MAC
%elseif strncmp(cname,'MAC',3)
%  p = strrep(p,':',':;');
%  p = strrep(p,'/',':');
%  m = toolboxroot;
%  if m(end) ~= ':'
%    p = strrep(p,'$',[toolboxroot '/']);
%  else
%    p = strrep(p,'$',toolboxroot);
%  end
else
  p = strrep(p,'$',[toolboxroot '/']);
end
