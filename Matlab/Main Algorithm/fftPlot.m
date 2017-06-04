function [Y] = fftPlot(x, Fs)

    L = length(x);
    f = linspace(0,Fs/2,round(L/2));
    Y = abs(fft(x))/L;
    Y = Y(1:round(L/2));
    plot(f,Y)
    grid()
    
end

