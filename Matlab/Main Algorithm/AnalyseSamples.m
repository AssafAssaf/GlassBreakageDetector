function [y] = AnalyseSamples(x, Fs, start, interval)
% Store packets in specific data structure, over 'interval' seconds.
% Each 100 ms time interval is one point in vector y.
    
    x = x(start*32:end);
    N_100ms = floor(Fs/10);
    num_of_packets = interval * 10;
    
    y = zeros(1, num_of_packets);
    
    for i=0:num_of_packets-1
       % y maximum length is fixed = 'interval' * 10
       % this condition must be verified here because the audio files
       % are not in the same length.
       % won't be needed in real application since we will always
       % record 2 seconds from starting event
       if ((i+1)*N_100ms > length(x))
           break;
       else
           y(i+1) = sum(abs(x(i*N_100ms + 1 : (i+1)*N_100ms)));
       end
    end
    
    

end

