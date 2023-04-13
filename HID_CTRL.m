pkg load instrument-control
clear; clc; close all; % Good coding practice

deviceList = instrhwinfo();

dataLength = NaN; %NaN; % Number of Data Points
dataInputInterval = 0.1; % Seconds between data outputs


fflush(stdout); % Octave flush input
while isnan(dataLength) || !isnumeric(dataLength) % NaN for length of data, numeric check
  dataLength = input("Enter Number of Time Points (Or Enter to Skip): ");
endwhile
% If enter pressed, use NaN as the input.
if (size(dataLength) == 0)
  dataLength = NaN;
endif
% Data
keyInfiniteOutput = false; % Whether data is being output indefinite
if !isnan(dataLength)
  dataValues = NaN(dataLength);
  xDataValues = linspace(0, dataInputInterval * dataLength, dataLength);
  keyInfiniteOutput = false;
else
  disp("\nUsing Indefinite Data Output (Ctrl + C to End)\n"); % Notify User of infinite data
  xDataValues = NaN;
  keyInfiniteOutput = true;
endif
finalDataIndex = 1; % Most recent plot index. Starts at 1

noiseFloor = -59; % -59 dBm per datasheet
peakDBM = 5; %
randTrendValue = randi(abs(noiseFloor) + peakDBM); % Initial Trend value randomizer
freqMax = 2700; % Testing
freq1 = 1000;
freq2 = 7500;
sampleF = 1 / dataInputInterval;
timeSteps = linspace(0, 1, 1E4); % require numeric input
freqVal1 = 15 * sin(timeSteps * freq1/(2*pi));
freqVal2 = -20 * sin(timeSteps * freq2/(2*pi));
n = 2**16;

% Capture data & Plot in real time!
figure(1,"name","dBEngineers Plotter"); % Create window
subplot(1,2,1);
title("dBm Values");
xlabel("Time (s)");
ylabel("Power (dBm)");
subplot(1,2,2); % Enter plot 2
title("FFT Spectrum");
while isnan(dataLength) || finalDataIndex < dataLength
  %disp(strcat("IDX: ", num2str(finalDataIndex))); % Display data index

  % RNG Baby
  if (randi(100) == 1) % 1/100 chance
    randTrendValue = mod(randi(abs(noiseFloor) + peakDBM) * 0.25 + randTrendValue, abs(noiseFloor))
  endif
  %dataValues(finalDataIndex) = noiseFloor + randi(10) * 0.1 + randTrendValue + sin(finalDataIndex) * 15; % Hehe random values
  %dataValues(finalDataIndex) = noiseFloor + randTrendValue + freqVal1(finalDataIndex); %+ freqVal2(finalDataIndex);
  dataValues(finalDataIndex) = freqVal1(finalDataIndex);
  % Check if data infinite
  if (keyInfiniteOutput)
    xDataValues = linspace(0, dataInputInterval * finalDataIndex, finalDataIndex); % Rescale x-Axis
  endif
  % FFT Values quickly
  yFFTvalues = fft(dataValues);
  %yFFTvalues = fftshift(yFFTvalues);

  xFFTvalues = (0:(length(dataValues)-1)) / length(dataValues);
  %xFFTvalues = (-n/2:n/2-1)/n;
  %yFFTvalues = abs(yFFTvalues);
  %xFFTvalues = (-length(dataValues)/2:length(dataValues)/2-1) / length(dataValues);
  %xFFTvalues = (0:(length(dataValues)));
  %xFFTvalues = linspace(0, freqMax, length(yFFTvalues));

  % Plot data
  subplot(1,2,1); % enter first plot
  plot(xDataValues, dataValues);
  subplot(1,2,2);
  plot(xFFTvalues, yFFTvalues);

  drawnow; % Instantly plot

  finalDataIndex = finalDataIndex + 1; % Increment plot index
endwhile

disp("Done")

