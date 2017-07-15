# Glass Breakage Detector

An ultra-low power glass breakage detector based on STM32L476 microcontroller.

The detector correctly distinguish between everyday sounds and the sound of breaking glass whatever the type of glass involved, using signal processing and machine learning algorithms.

* [Project Final Report](https://github.com/AssafAssaf/GlassBreakageDetector/blob/master/Final%20Report.pdf)

* [Matlab - Main Algorithm](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Main%20Algorithm)

* [Matlab - Sound Localization](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Sound%20Localization)

* [Sound Recordings](https://github.com/AssafAssaf/GlassBreakageDetector/tree/master/Matlab/Main%20Algorithm/Samples)



## Getting Started

It is possible to run the project on both Simba board and Nucleo-L476 (with CCA02M1 extentsion board).

The default version is for the Nucleo board. In order to use the Simba board, simply define the preprocessor symbol SIMBA_BOARD.

Note that in order to use the Nucleo board version, some physical modifications should be made. Please refer to "Board Modifications" section below.

![Alt text](/SimbaBoard.jpg?raw=true "Title")


## How It Works

The sound classification algorithm steps are:
* Detect Low frequency thud
* Determine thud location by sound localization
* Features extraction by recording the event for 2 seconds and process it in real time
* Sound type estimation using logistic regression

The following diagram simplifies the classification process:
![Alt text](/Alg_Simplified.jpg?raw=true "Title")


## Board Modification

In order to use the Nucleo board with two MEMS microphones, change the MEMSMIC-1 extension board solder bridges to the following configuration:

![Alt text](/SB_Configuration.jpg?raw=true "Title")

In addition, J2 must be placed in position 2-3.
More information available [here](http://www.st.com/en/embedded-software/x-cube-memsmic1.html).


## Burning Tools

If you dont want to install Keil IDE, you can program the burn using [ST-Link](http://www.st.com/en/embedded-software/stsw-link004.html) utility. 
The binary files for both versions located [here]().


## Further Reading

For more information, please refer to the project final report.



