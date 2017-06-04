clc
clear all

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 1. Load recording of glass breaking and other similar sounds.
% 2. Pass the samples through filters.
% 3. Identify the beginning of the sound event.
% 4. Analyse incoming data for 2 seconds.
% 5. Normalize results and create the features vector.
% 6. Calculate the weight vector theta using logistic regression.
% 7. Plot results.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

files = [dir('Samples/*.wav'); dir('Samples/*.mp3')];

%% Defines
Plot = 0;
do_shuffle = 1;

%% Filters
B_IIR_10K = [0.0099, 0, -0.0198, 0, 0.0099];
A_IIR_10K = [1.0000, -0.9310, 1.9174, -0.7997, 0.7392];
B_IIR_5K = [0.0172, -0.0003, -0.0339, -0.0003, 0.0172];
A_IIR_5K = [1.0000, -2.7954, 3.6375, -2.3874, 0.7332];
A_IIR_300 = [1 -1.9706 0.9724];
B_IIR_300 = [0.0138 0 -0.0138];
A_IIR_35 = [1.0000, -2.9715, 2.9434, -0.9719];
B_IIR_35 = [0.0356, 0.1069, 0.1069, 0.0356]*1e-5;

A_IIR_HP = [1.0000, -2.9986, 2.9972, -0.9986];
B_IIR_HP = [0.9993, -2.9979, 2.9979, -0.9993];

%% Thresholds
p_thresh = 1;
interval = 2; % 2 seconds

%% Features
features = zeros(length(files), 1+interval*10*4);

for i=1:length(files)
    
    [y, Fs] = audioread(['Samples\' files(i).name]);
    y = y(:,1); % Stereo to Mono
    % resample to sample rate of 44.1kHz
    [P,Q] = rat(44.1e3/Fs);
    y = resample(y,P,Q);
    
    % IIR's
    y_iir_10 = filter(B_IIR_10K, A_IIR_10K, y); % 10 kHz BPF
    y_iir_5 = filter(B_IIR_5K, A_IIR_5K, y); % 5 kHZ BPF
    y_iir_300 = filter(B_IIR_300, A_IIR_300, y); % 300 Hz BPF
    y_iir_35 = filter(B_IIR_35, A_IIR_35, y);  % 35 Hz LPF
    
    % High Pass Filter
    y_iir_10 = filter(B_IIR_HP, A_IIR_HP, y_iir_10);
    y_iir_5 = filter(B_IIR_HP, A_IIR_HP, y_iir_5);
    y_iir_300 = filter(B_IIR_HP, A_IIR_HP, y_iir_300);
    y_iir_35 = filter(B_IIR_HP, A_IIR_HP, y_iir_35);
    
    % Simulate MCU operation - wait for starting event
    start_packet = WaitForEvent(y_iir_300, p_thresh);
    
    % Now the algorithm is triggered and the MCU starts to save packets
    % data (mean values) for 2 seconds
    y_iir_10_features = AnalyseSamples(y_iir_10, Fs, start_packet, interval);
    y_iir_5_features = AnalyseSamples(y_iir_5, Fs, start_packet, interval);
    y_iir_300_features = AnalyseSamples(y_iir_300, Fs, start_packet, interval);
    y_iir_35_features = AnalyseSamples(y_iir_35, Fs, start_packet, interval);
    
    max_5 = max(y_iir_5_features);
    y_iir_10_features = y_iir_10_features / max_5;
    y_iir_5_features = y_iir_5_features / max_5;
    y_iir_300_features = y_iir_300_features / max_5;
    y_iir_35_features = y_iir_35_features / max_5;

    rec_features = [y_iir_10_features, y_iir_5_features, y_iir_300_features, y_iir_35_features];
    
    features(i,1) = str2double(files(i).name(1)); % true or false audio type
    features(i,2:1+length(rec_features)) = rec_features;
    
    if (Plot == 1)
        % Plot the features vs time
        figure(i)
        time = linspace(0,2,30);
        plot(y_iir_10_features, 'LineWidth', 2)
        hold on
        plot(y_iir_5_features, 'LineWidth', 2)
        hold on
        plot(y_iir_300_features, 'LineWidth', 2)
        hold on
        plot(y_iir_35_features, 'LineWidth', 2)
        legend('10000', '5000', '300', '35')
        grid()
        if (features(i,1) == 1)
            title('TRUE')
        else
            title('FALSE')
        end
    end


end


%% Logistic Regression Traning And Testing

features(isnan(features)) = 0; % remove NaN elements from features matrix (?)
lambda = 0.001;
X = [ones(length(files),1) features(:,2:end)];
Y = features(:,1);

if (do_shuffle == 1) 
    % Seperate data to training and testing
    train = [5:7,8:29];
    test = [1:2,6:7,30:33];
    X_test = X(test,:);
    Y_test = Y(test,:);
    X = X(train,:);
    Y = Y(train,:);
end

% Optimize
theta = zeros(size(X, 2), 1);
options = optimset('GradObj', 'on', 'MaxIter', 4);
[theta, J, exit_flag] = ...
	fminunc(@(t)(costFunctionReg(t, X, Y, lambda)), theta, options);

if (do_shuffle == 1) 
    % Plot results for testing data
    figure(1)
    predict = sigmoid(X_test*theta);
    stem(test, predict)
    hold on
    stem(test, -1*Y_test)
    
%     b = bar([predict, Y_test])
%     ylim([0 1.1])
%     b(1).FaceColor = 'b';
%     b(2).FaceColor = 'm';
%     legend('Predicted', 'True/False', 'Location', 'NorthWest')
%     xlabel('Test Event Number')
%     ylabel('P(Event = True)')
%     title('Testing Set Results for \Theta')

    predict = sigmoid(X*theta);
    figure
    %b = bar([predict, Y]);
    figure
    b = bar(y');
    b(1).FaceColor = 'b';
    b(2).FaceColor = 'm';
    %ylim([0 1.1])
    %xlim([0 34])
    legend('Estimated', 'True/False', 'Location', 'NorthWest')

    hold on
    plot(xlim,[0.5 0.5], 'r')
    
    
    xlabel('Training Event Number')
    ylabel('P(Event = True)')
    title('Results for \Theta')
    

else
    figure(1)
    predict = sigmoid(X*theta);
    bar([predict; Y])
    %stem(predict)
    %hold on
    %stem(-1*Y)
    
end



% itrue = find(features(:,1) == 1);
% ifalse = find(features(:,1) == 0);
% 
% subplot(2,2,1)
% t1 = plot(features(itrue, 2:21)', 'b-o');
% hold on
% t2 = plot(features(ifalse, 2:21)', 'r-o');
% hold off
% legend([t1',t2'],'True', 'False')

for i=1:33
   if (features(i,1) == 0)
       c = 'r-o';
   else
       c = 'b-o';
   end
   subplot(2,2,1)
   plot(features(i,2:21), c)
   hold on
   subplot(2,2,2)
   plot(features(i,22:41), c)
   hold on
   subplot(2,2,3)
   plot(features(i,42:61), c)
   hold on
   subplot(2,2,4)
   plot(features(i,62:81), c)
   hold on
end

