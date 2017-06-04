function [i] = WaitForEvent(x, p_thresh)
% This function iterates over x in intervals of 32 samples
% And return the first index in which the total energy crossed
% the p_thresh value

    N = 32; % Each 1ms, 32 samples are handles by the DMA (1 packet)
    num_of_packets = floor(length(x)/N); % number of 32 samples packets
    
    for i=0:num_of_packets-1
       energy = sum(abs(x(i*N + 1 : (i+1)*N))); 
       if (energy > p_thresh)
           break;
       end
    end
    
end

