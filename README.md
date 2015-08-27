<!--- coding: utf-8; fill-column: 80 --->
# ligō C implementation


* [Homepage](https://github.com/nibua-r/ligo-c#readme)
* [Issues](https://github.com/nibua-r/ligo-c/issues)
* [Email](mailto:root at renaud.io)

## Description

ligō is a C library implementing the
[Android Open Accessory Protocol](http://source.android.com/tech/accessories/aoap/aoa.html)
to interact with Android devices via USB.

Android-side application is needed to make some use of this project. A
[sample LigoTextDemo Android application](https://github.com/nibua-r/LigoTextDemo)
is available for usage demonstration.

## Features

Just USB I/O using the Android Open Accessory Protocol…

## Compile and install

    $ git clone https://github.com/nibua-r/ligo-c.git
    $ cd ligo-c
    $ mkdir build && cd build
    $ cmake ../ && make install

You should be able to use `pkg-config` to include and link ligo to your own project.

## Contributing to ligō

Follow the usual fork/branch/PR workflow to send changes, if I like them I'll merge them

## Copyright

Copyright (c) 2015 Renaud AUBIN

See {file:LICENSE.txt} for details.
