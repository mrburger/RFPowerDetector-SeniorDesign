pkg load instrument-control
clear; clc; close all % Clear command line and close plots


supportedSerial = serialportlist();
supportedSerial; % Maybe not necessary

% close Serial Communication if it exists
if (exist("deviceTeensy"))
  fclose(deviceTeensy);
endif
% TODO: Detect all serial ports, or make USB HID
deviceTeensy = serial("\\\\.\\COM5");
dataLength = inf;
%dataList = nan(1, dataLength); % -59 is noise floor, nan makes it easier.
dataSampleInterval = 0.25; % 10ms per sample avg.
noiseFloor = -59; % Magic number
%fwrite(deviceTeensy, "M");

readR = 1;
figurePlot = figure(1,"name","dBEngineers Plotter"); % Create window
%set(figurePlot,'KeyPressFcn', "closePlotFunction");
continueLoop = true; % Start with continuous

srl_flush(deviceTeensy); % Flush buffer (Why doesnt it work always?)
% Wait until next line
while (srl_read(deviceTeensy,1) != 13)
  % wait
endwhile

while (continueLoop && readR <= dataLength)
  % Read in data line
  clear dataIn; % Reset value
  while ((inputSerialChar = srl_read(deviceTeensy, 1)) != 13) % no newline
    if (exist("dataIn"))
          %disp(strcat(num2str(uint8(inputSerialChar)), " : ", char(inputSerialChar))) % Debug
          dataIn = [dataIn, char(inputSerialChar)]; % Add character at end of array
    else
          dataIn = char(inputSerialChar);
    endif
  endwhile
  % Process channels into separate arrays
  regexMatches = regexp(dataIn, ",\ ", "split"); % NASTY REGEX for a comma and space
  for (channelPosition = 1 : length(regexMatches))
    if (!isnan(str2double(cellstr(regexMatches(channelPosition))))) % Not empty & Numeric
      dataList(channelPosition, readR) = str2double(cellstr(regexMatches(channelPosition))); % Convert
      if (dataList(channelPosition, readR) == noiseFloor)
        dataList(channelPosition, readR) = nan; % Reset to NAN for invalid data
      endif
    endif
  endfor
  readR = readR + 1;

  xData = 0 : dataSampleInterval : size(dataList,2) * dataSampleInterval;
  xData = xData(1:end-1);

  clf; % remove previous plots
  hold on % Turn on hold for multiple channels
  for channelPosition = 1 : size(dataList, 1) % Get channel count (First dimension)
      plot(xData, dataList(channelPosition, :), "displayName", num2str(channelPosition));
  endfor
  hold off % Turn off hold

  title("dBm Values");
  xlabel("Time (s)");
  ylabel("Power (dBm)");
  legend; % Create legend
  drawnow;
endwhile

fclose(deviceTeensy);

