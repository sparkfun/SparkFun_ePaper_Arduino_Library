This script takes in a 24 bit bitmap and converts the bitmap into a black/red/white image. 
A preview bitmap is generated. 
A text file is generated that can be saved to a micro SD card to be loaded onto a SparkFun ePaper display.
An array of bytes is generated that can be copy/pasted into a header file.

Three bitmaps are provided as an example. Requires python v2.7.

**To use your own image:**

* Convert your JPEG or whatever format to (24bit) BMP
* Run the python script against your bitmap
* A preview.bmp file will be generated. Play with white and red limits to acheive desired effect
* An img.txt file will be generated. Save this to an SD card and call myEPaper.loadFromSD("img.txt"); in Arduino
* An array.txt file will be generated. Copy and paste the contents of this file to a image.h file

Usage:

    python bmp2epaper.py strawberries.bmp [whiteLimit(0-255)] [redLimit(0-255)]
      //if no limits, will be default to 128
      //the higher the value, the less of that color you will see


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
