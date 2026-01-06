# Super Spectrogram for Audacity

This repository is a fork from the original [Audacity GitHub project](https://github.com/audacity/audacity), which implements an add-on feature: a detailed Spectrogram for musical purposes.
The Super Spectrogram performs a Gaussian-window Short-Term Fourier Transform on the input signal and display it which lines corresponding to the musical notes.
The window length of the STFT is dependent on the value of sigma, the standard-deviation of the Gaussian distribution. 
The window length defines the number of frequency bins (rows) in the output matrix.
The number of columns is defined by the number of samples of audio selected at the moment of calculation and the level of detail.

Five levels of detail are available:

- **C4** displays notes up to a frequency of 275.625 Hz. The value of sigma is 16 and the window length is 128 samples;
- **C5** displays notes up to a frequency of 551.25 Hz. The value of sigma is 32 and the window length is 256 samples;
- **C6** displays notes up to a frequency of 1102.5 Hz. The value of sigma is 64 and the window length is 512 samples;
- **C7** displays notes up to a frequency of 2205 Hz. The value of sigma is 128 and the window length is 1024 samples;
- **C8** displays notes up to a frequency of 4410 Hz. The value of sigma is 256 and the window length is 2048 samples.

The level of detail defines the decimation factor of the input signal. The target frequency is the double of the highest frequency shown.
The user may select the level of detail by a Selector GUI element. Once the value changes, the whole matrix is recalculated.
For ensuring at least one full window is computed at the highest level of detail, the minimum number of samples selected corresponds to 2048 samples after decimation.
The maximum processed signal corresponds to 30 seconds of audio at any sampling frequency.

> ⚠️ Note: The Super Spectrogram calculation is computationally expensive. 
If your hardware does not have at least 8 GB of RAM memory, do not select **C8** as the highest note, and limit the selection to a few seconds, or the program may crash for lack of memory.
The recommended amount of memory is 16 GB of RAM.

**Controls:**

- **Left-click**: Used for pan the display of the spectrogram. Clicking and dragging may stretch the display for a more approximated view;
- **Right-click**: Used for resetting the spectrogram display to the initial view;
- **Mouse wheel**: Used for zoom in/zoom out. This may be used to focus on specific regions of the spectrogram;
- **Noise Floor Selector**: Defines the value in dB of the lowest amplitude to be seen in the spectrogram. Five options are available;
- **Highest Note Selector**: Defines the highest note to be shown in the display. It also changes the size of the spectrogram matrix and level of detail;
- **Export button**: There are two options to export: Export the current view as a PNG image or export the whole spectrogram matrix in TXT format for opening in external applications.

## Installation

The recommended approach is building from source code. The build process follows the same approach of Audacity.

For configuring the project, clone this repository, then run from within it:

```bash
cmake -S . -B <build_folder> -DCMAKE_INSTALL_PREFIX=<installation_folder>
```

> Note: Replace `<build_folder>` by the path where the build artifacts will be stored.
> Replace `<installation_folder>` by the path where the build artifacts will be installed.

For compiling the project, run:

```bash
cmake --build <build_folder> --config Release
```

For installing the artifacts into the `<installation_folder>`, run:

```bash
cmake --install <build_folder> --config Release
```

Besides, some pre-compiled artifacts will be uploaded to the Releases page.

# Audacity


[**Audacity**](https://www.audacityteam.org) is an easy-to-use, multi-track audio editor and recorder for Windows, macOS, GNU/Linux and other operating systems.

- **Recording** from any real or virtual audio device that is available to the host system.
- **Export / Import** a wide range of audio formats, extensible with FFmpeg.
- **High quality** using 32-bit float audio processing.
- **Plugin Support** for multiple audio plugin formats, including VST, LV2, and AU.
- **Macros** for chaining commands and batch processing.
- **Scripting** in Python, Perl, or any other language that supports named pipes.
- **Nyquist** a powerful built-in scripting language that may also be used to create plugins.
- **Editing** multi-track editing with sample accuracy and arbitrary sample rates.
- **Accessibility** for VI users.
- **Analysis and visualization** tools to analyze audio or other signal data.

## Users

For end users, the latest Windows and macOS release version of Audacity is available from the [Audacity website](https://www.audacityteam.org/download/).
Help with using Audacity is available [here](https://audacityteam.org/help/).

## Developers
Build instructions are available [here](https://github.com/audacity/audacity/blob/master/BUILDING.md).

Additional development resources may be found [here](https://audacity.gitbook.io/dev/).

## License

Audacity is open source software licensed GPLv3. Most code files are GPLv2-or-later, with the notable exceptions being /lib-src (which contains third party libraries), as well as VST3-related code. Documentation is licensed CC-by 3.0 unless otherwise noted. Details can be found in the [license file](LICENSE.txt).
