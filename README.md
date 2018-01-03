# Tinyrenderer
My implemetation of Dmitri Sokolov's tinytenderer: a learning exercise in how graphics libraries,
like OpenGL, work. 

More information on Dmitri's series of articles can be found here: 
https://github.com/ssloy/tinyrenderer/wiki

## Installing / Getting started

Directly download this project.

## Building / Running

Build and run the project on a linux machine in the /tinyrenderer/tinyrenderer directory  by 
running the following commands.

```shell
make
./tinyrenderer
```

The above lines will build the executable from the project source, and run it.

## Features

The project currently implements the following features:
* Bresenham's Line Drawing Algorithm 
* Triangle Rasterization and back face culling

The project will implement the following features:
* Hidden faces removal using z buffer
* Perspective projection
* Camera movement
* Shaders
* Tangent space normal mapping
* Shadow mapping
* Ambient occlusion

## Configuration

There are no configuration options.

## Licensing

"The code in this project is licensed under MIT license."
