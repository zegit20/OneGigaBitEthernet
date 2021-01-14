clc
clear all
format long g;

ReadSize = 1000000;
file_data = zeros(1, ReadSize);
diff = zeros(1, ReadSize-1);

filename = 'E:\All_Project\GSR\Demo_ethernet\Tina_Ethernet\20-8-92sw\Tina_Ethernet_Software\Test.hex';
%filename = 'C:\test.dat';
fid = fopen(filename, 'r');

% make sure the file is not empty
finfo = dir(filename);
fsize = finfo.bytes / 4; % Number of uint32 words

N = floor(fsize / ReadSize);

for ii=1:N
	%ii
    file_data = fread(fid, ReadSize, 'uint32');
    
    %stem(file_data)
    
    diff = zeros(1, ReadSize-1);
    diff = file_data(2:ReadSize-1) - file_data(1:ReadSize-2);
    a = find(diff ~= 1);
    if length(a) ~= 0
        disp('Error');
        a
    end
end
 disp(' finish');
fclose all