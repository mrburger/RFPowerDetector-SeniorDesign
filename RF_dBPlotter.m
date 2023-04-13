pkg load instrument-control
clc; close all; % Clear command line and close plots


supportedSerial = serialportlist();
supportedSerial

%fclose("\\\\.\\COM5");
% TODO: Detect all serial ports, or make USB HID
deviceTeensy = serial("\\\\.\\COM5");
dataLength = 1000;
dataList = nan(1, dataLength); % -59 is noise floor, nan makes it easier.
dataSampleInterval = 0.25; % 250ms per sample avg.
%fwrite(deviceTeensy, "M");
srl_flush(deviceTeensy);

readR = 1;
figure(1,"name","dBEngineers Plotter"); % Create window

while (readR <= size(dataList,2))
  [dataIn, numPoints] = srl_read(deviceTeensy, 255);
  p = 1;


  newC
  numC = str2num(newC);
  dataList(readR) = numC;
  readR = readR + 1;
  xlim([0 size(dataList,2)]);
  xData = linspace(0,size(dataList,2),size(dataList,2));
  plot(xData, dataList);
  title("dBm Values");
  xlabel("Time (s)");
  ylabel("Power (dBm)");
  drawnow;
endwhile

fclose(deviceTeensy);



