'''

    This script takes in a 24 bit bitmap and outputs both a text file that is a
    byte array used in Arduino header files as well as a text file specific
    to the SparkFun_eInk_Library load function.

    You'll need python 2.7

    usage: python bmp2eink.py strawberries.bmp [whiteLimit(0-255)] [redLimit(0-255)]
           if no limits, will be prompted for white limit and red limit later
           the higher the value, the less of that color you will see
           
    output: array.txt, img.txt, preview.bmp
        Copy and paste this content into a image.h file that
        is called in an Arduino sketch.
        For an example see: https://github.com/sparkfun/SparkFun_eInk_Arduino_Library/tree/master/examples/Example4_Array

    This is helpful if you need to display a static bitmap on a
    3 Color display (like the 3 Color eInk Display - 1.54" (SPX-14892)).

    This script converts the image to black, white, and red
    in the form of two arrays, as expected by the eInk display.

'''

#Various inspiration from
#http://www.dragonwins.com/domains/getteched/bmp/bmpfileformat.htm
#https://github.com/yy502/ePaperDisplay

import sys
import struct


#Open our input file which is defined by the first commandline argument
#then dump it into a list of bytes
infile = open(sys.argv[1],"rb") #b is for binary
contents = bytearray(infile.read())
infile.close()

#Get the size of this image
data = [contents[2], contents[3], contents[4], contents[5]]
fileSize = struct.unpack("I", bytearray(data))

print "Size of file:", fileSize[0]

#Get the header offset amount
data = [contents[10], contents[11], contents[12], contents[13]]
offset = struct.unpack("I", bytearray(data))

print "Offset:", offset[0]

#Get the size of the header
data = [contents[14], contents[15], contents[16], contents[17]]
headerSize = struct.unpack("I", bytearray(data))

print "Size of header:", headerSize[0]

if headerSize[0] != 40:
    print "Unexpected bitmap format. Sorry!"
    exit()

#Get the width of this image
data = [contents[18], contents[19], contents[20], contents[21]]
imgWidth = struct.unpack("I", bytearray(data))

print "Width:", imgWidth[0]

#Get the height of this image
data = [contents[22], contents[23], contents[24], contents[25]]
imgHeight = struct.unpack("I", bytearray(data))

print "Height:", imgHeight[0]

#Get the number of bits per pixel
data = [contents[28], contents[29], 0, 0]
bpp = struct.unpack("I", bytearray(data))
print "BPP:", bpp[0]

    
#Get the number of colors used
data = [contents[46], contents[47], contents[48], contents[49]]
numColors = struct.unpack("I", bytearray(data))
if numColors[0] == 0:
    colorsUsed = 2**bpp[0]
    print "Number of colors used:", colorsUsed

else:
    colorsUsed  = numColors[0]
    print "Number of colors used:", colorsUsed

whiteMin=0
redMin=0
if len(sys.argv) == 4:
    whiteMin=int(sys.argv[2])
    redMin=int(sys.argv[3])
else :
    whiteMin = 128
    redMin = 128

print "white: ",whiteMin
print "red: ",redMin
print ""
print ""

#Make strings to hold the output of our script
#Writing left to right, bottom to top
arraySize = ((len(contents) - offset[0]) / 3 ) / 8
bwOutputString = "\r};"
rOutputString = "\r};"
bwTempString = ""
rTempString = ""
sdOutputString=""
sdTempStringBW=""
sdTempStringR=""

tempBW=0
tempR=0
n = 0
#Start coverting pixels to array values
#Start at the offset and go to the end of the file
for i in range(offset[0], fileSize[0], 3):
    b = contents[i]
    g = contents[i+1]
    r = contents[i+2]
    #white
    if b > whiteMin and g > whiteMin and r > whiteMin: 
        tempBW |= (1<<(7-(n%8)))
        tempR  |= (1<<(7-(n%8)))
        contents[i] = 255
        contents[i+1] = 255
        contents[i+2] = 255
    #red
    elif r > redMin:
        tempBW &= ~(1<<(7-(n%8)))
        tempR  &= ~(1<<(7-(n%8)))
        contents[i] = 0
        contents[i+1] = 0
        contents[i+2] = 255
    #black
    else: 
        tempBW &= ~(1<<(7-(n%8)))
        tempR  |= (1<<(7-(n%8)))
        contents[i] = 0
        contents[i+1] = 0
        contents[i+2] = 0
    if n % 8 == 7 :
        bwTempString += hex(tempBW) + ", " 
        rTempString += hex(tempR) + ", "
        sdTempStringBW +=chr(tempBW)
        sdTempStringR +=chr(tempR)

    if n % (imgWidth[0]) == imgWidth[0]-1 :
        bwOutputString = bwTempString + bwOutputString
        rOutputString = rTempString + rOutputString
        sdOutputString = sdTempStringBW + sdTempStringR + sdOutputString
        bwTempString=""
        rTempString=""
        sdTempStringBW=""
        sdTempStringR=""

    #Break the array into a new line every 8 entries
    if n % 128 == 0:
        bwOutputString = '\r' +bwOutputString
        rOutputString = '\r' +rOutputString
    n+=1;

bwOutputString = "const byte bwData[" + str(arraySize) + "] PROGMEM = {" + '\r' + bwOutputString
rOutputString = "const byte rData[" + str(arraySize) + "] PROGMEM = {" + '\r' + rOutputString
sdOutputString = chr(0xFF & (imgWidth[0]>>8))+chr(0xFF & (imgWidth[0]))+chr(0xFF & (imgHeight[0]>>8))+chr(0xFF & (imgHeight[0]))+sdOutputString

#Write updated values to output bmp image
outimg = open("preview.bmp","w")
outimg.write(contents)
outimg.close()

#Write the output string to our output file
outfile = open("EPDArray.txt","w")
outfile.write(bwOutputString)
outfile.write(rOutputString)
outfile.close()

outsd = open("img.txt","w")
outsd.write(sdOutputString)
outsd.close()


print "preview.bmp complete"
print ""
print "img.txt complete"
print "Save this file to SD card to load bitmap onto display with SparkFun eInk Library"
print "EPDArray.txt complete"
print "Copy and paste this array into a image.h or other header file"
