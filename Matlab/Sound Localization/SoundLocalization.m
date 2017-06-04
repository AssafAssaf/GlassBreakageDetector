clc
clear all


% Defines
speed_of_sound = 340; % m/s
mic_dist = 0.04; % m
source_angle = 0;
Fs = 32e3;

% Simulation 
time_diff = cos(source_angle) * mic_dist / speed_of_sound;
t = 0:1/Fs:200/Fs;
f = 0.3e3;
sample_diff = ceil(Fs * time_diff);
print_str = sprintf('Difference in number of samples: %d', sample_diff);
disp(print_str);


%% Real Recordings
load('left_correlation.mat');
left_mic_1 = mic_1;
left_mic_2 = mic_2;
load('right_correlation.mat');
right_mic_1 = mic_1;
right_mic_2 = mic_2;

% left correlation
[acor_left, lag_left] = xcorr(left_mic_1, left_mic_2);
[~,I] = max(abs(acor_left));
lagDiff_left = lag_left(I);
power1_left = rms(left_mic_1);
power2_left = rms(left_mic_2);
print_str = sprintf('For left correlation: lag diff=%d, RMS: right_mic=%.1d, left_mic=%.1d', lagDiff_left, power2_left, power1_left);
disp(print_str);

% right correlation
[acor_right, lag_right] = xcorr(right_mic_1, right_mic_2);
[~,I] = max(abs(acor_right));
lagDiff_right = lag_right(I);
power1_right = rms(right_mic_1);
power2_right = rms(right_mic_2);
print_str = sprintf('For right correlation: lag diff=%d, RMS: right_mic=%.1d, left_mic=%.1d', lagDiff_right, power2_right, power1_right);
disp(print_str);



figure
subplot(2,2,1)
plot(right_mic_1, 'b')
hold on
plot(right_mic_2, 'r')
xlabel('Sample')
ylabel('Amplitude')
title('Two Microphones Input for Right Source Sound')
legend('Mic1 - Left', 'Mic2 - Right')
grid()

subplot(2,2,2)
plot(lag_right, acor_right)
xlabel('Sample')
ylabel('Magnitude')
title('Cross Correlation Vs Sample (Right Correlation)')
grid()

subplot(2,2,3)
plot(left_mic_1, 'b')
hold on
plot(left_mic_2, 'r')
xlabel('Sample')
ylabel('Amplitude')
title('Two Microphones Input for Left Source Sound')
legend('Mic1 - Left', 'Mic2 - Right')
grid()

subplot(2,2,4)
plot(lag_left, acor_left)
xlabel('Sample')
ylabel('Magnitude')
title('Cross Correlation Vs Sample (Left Correlation)')
grid()





