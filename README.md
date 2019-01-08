# Global Illumination: Monte Carlo Ray Tracer

### Features
* Monte Carlo ray tracing with antialiasing
* Area light source
* Materials: diffuse, specular, glossy, refractive

### Sample Images
![alt text](https://github.com/skwon19/ray-tracer/raw/master/images/slide_montecarlo_20indirectbounces_50subsamples_1000.png "Cornell Box: one specular, one diffuse sphere")

![alt text](https://github.com/skwon19/ray-tracer/raw/master/images/slide_glossy_10indirectbounces_50subsamples_1000.png "Cornell Box: one specular, one glossy sphere")

![alt text](https://github.com/skwon19/ray-tracer/raw/master/images/slide_refractive_20indirectbounces_50subsamples_1000.png "Cornell Box: one specular, one refractive sphere")

### How to compile
* Clone repo
* Delete `build` directory
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

### How to run
From `build` directory:

`./a4 -input ../data/{scene file name}.txt -output {output file name}.png -size {width in pixels} {height in pixels}`

Optional parameters when running:
* `-shadows`
* `-bounces {number of bounces}`
* `-subsamples {number of subsamples}`
* `-indirectBounces {max number of indirect bounces}`
* `-jitter`

_See Ray_Tracer_Writeup.pdf for full write-up._
