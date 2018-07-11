import sys
import re
import matplotlib.pyplot as plt
import numpy as np
import cv2

if len(sys.argv) != 2:
    print("Usage: python " + sys.argv[0] + " [bmp data]")
    sys.exit()

width_pattern = re.compile("[Ww]idth\s*=\s*(\d+)")
height_pattern = re.compile("[Hh]eight\s*=\s*(\d+)")
channel_pattern = re.compile("[Cc]hannels?\s*=\s*(\d+)")
    
try:
    fd = open(sys.argv[1], 'r')
except IOError:
    print("Cannot open " + filename)

line = fd.readline()
while line[0] != '#':
    line = fd.readline()

m = width_pattern.search(line)
width = int(m.group(1))
m = height_pattern.search(line)
height = int(m.group(1))
m = channel_pattern.search(line)
channel = int(m.group(1))

print("Width = " + str(width))
print("Height = " + str(height))
print("Channel = " + str(channel))

#Python opencv tales numpy as framework
image = np.zeros((height, width, channel), np.uint8)

line = fd.readline()
fields = line.split()
print(len(fields))
idx = 0

for row_idx in range(0, height):
    for col_idx in range(0, width):
        for chan_idx in range(0, channel):
            image[row_idx, col_idx, chan_idx] = np.uint8(fields[idx])
            idx += 1

fd.close()

# plt.imshow(image)
# plt.show()

cv2.imshow("Image", image);
cv2.waitKey(0)
