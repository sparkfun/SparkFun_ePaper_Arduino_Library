This script takes in a 24 bit bitmap and converts the bitmap into a black/red/white image. 
A preview bitmap is generated. 
A text file is generated that can be saved to a micro SD card to be loaded onto a SparkFun eInk display.
An array of bytes is generated that can be copy/pasted into a header file.

Three bitmaps are provided as an example. Requires python v2.7.

**To use your own image:**

* Convert your JPEG or whatever format to (24bit) BMP
* Run the python script against your bitmap
* An output.txt file will be generated. Copy and paste the contents of this file to a image.h file
* Send this array to the display of your choice. See the example below about how to implement the header file.
           if no limits, will be prompted for white limit and red limit later
           the higher the value, the less of that color you will see
Usage:

    python bmp2eink.py strawberries.bmp [whiteLimit(0-255)] [redLimit(0-255)]


Output:

     preview.bmp
     img.txt
     array.txt



License Information
-------------------

This product is _**open source**_!

The **code** is released under the GPL v3 license. See the included LICENSE.md for more information.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.