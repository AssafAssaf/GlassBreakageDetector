# Glass Breakage Detector

An ultra-low power glass breakage detector based on STM32L476 microcontroller.

[Project's Final Report](https://github.com/AssafAssaf/GlassBreakageDetector/blob/master/Final%20Report.pdf)

[Matlab - Main Algorithm](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Main%20Algorithm)

[Matlab - Sound Localization](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Sound%20Localization)

[Sound Recordings](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Main%20Algorithm/Samples)


The detector correctly distinguish between everyday sounds and the sound of breaking glass whatever the type of glass involved, using signal processing and machine learning algorithms.

## Getting Started

It is possible to run the project on both Simba board and Nucleo-L476 (with CCA02M1 extentsion board).


![Alt text](/SimbaBoard.jpg?raw=true "Title")


## How It Works

The sound classification algorithm steps are:
* Detect Low frequency thud
* Determine thud location by sound localization
* Features extraction by recording the event for 2 seconds and process it in real time
* Sound type estimation using logistic regression

The following diagram simplifies the classification process:
![Alt text](/Alg_Simplified.jpg?raw=true "Title")



