# Global Illumination: Monte Carlo Ray Tracer

### How to compile:
* Clone repo
* Delete `build` directory
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

### How to run (from `build` directory):

`./a4 -input ../data/{scene file name}.txt -output {output file name}.png -size {width in pixels} {height in pixels}`

Optional parameters when running:
* `-shadows`
* `-bounces {number of bounces}`
* `-subsamples {number of subsamples}`
* `-indirectBounces {max number of indirect bounces}`
* `-jitter`

See Ray_Tracer_Writeup.pdf pages 4-10 for rendered images.
