Author: Jessica Huang 
CSCI 49369 - Computational Vision

# Provide short descriptions of your algorithms / problem you encountered.

For Program 1, I set a default threshold value to 128. My program converts a grayscale image into a binary image using the thresholding algorithm. Pixels greater than or equal to the threshold are set to white, while pixels below the threshold are set to black. A small issue I encountered was ensuring the background remained black across different test images. 

For Program 2, I implemented a two-pass sequential labeling algorithm to segment a binary image into distinct connected regions. The algorithm uses a Disjoint 
Set (Union-Find) data structure to efficiently handle label equivalences.

For my first pass I set three neigbors for laebeling: 
- Upper-left diagonal pixel (row-1, column-1)
- Upper pixel (row-1, column)
- Left pixel (row, column-1)

I checked if all three neighbors are background (0) then I assign new label, increment counter. If upper-left diagonal has a label I would ese that label and same thing for upper pixel and left pixel. But if they have both upper and left labels, I assigned the minimum value and record equivalence using unionsets. 

Resolve Equivalences: For my second pass I resolve equivalence for each labeled pixel by replacing its label with the root label from the Disjoint Set structure.This ensures all equivalent labels are unified to the same representative. I set my integer vales to 1 and map each label to a visible gray level: gray_level = (label * 255) / num_objects. This spreads object labels across the 0-255 range for better visualization allowing it to print total number of objects found. 

Problem Encounter: I initially missed equivalence cases where both upper and left neighbors had different labels but was able to solved iy by using Disjoint Set Union-Find. Some the labels were not visible in output image and the binary image gray level were not found but later solved by remapping. Additionally I needed to ensure proper union-find operations to merge equivalent label sets

Program 3, The program computes geometric attributes for each labeled object by accumulating sums in a single pass (area, centroids, and second-order moments), then calculating orientation using θ = atan2(b, a-c)/2, minimum/maximum moments of inertia using the orientation angle, and roundedness as Emin/Emax, founded in the slides presented during the lecture. The output includes a text file with the object database (label, center position, Emin, area, roundedness, orientation in degrees) and a visualization image with 5×5 white dots at centroids and 100-pixel orientation lines through each object's principal axis. The results were written into a text file as an object database, and the orientation was visualized on the output image with a dot and a line. The challenge here was getting the orientation calculation correct and making sure the drawn line stayed inside the image boundaries. For some objects the line was not tilted to the same direction and I had to adjust it.  For my output txt I derived the answer: Object 1 - 127 263.546 349.333 3.88924e+06 7649 0.533632 17.8098 and Object 2 - 255 256.618 195.316 364196 2044 0.479964 -50.6065. 

Program 4 - I implements object recognition by comparing scene objects against a database, using area difference (<30%) and roundedness difference (<0.15) as matching criteria. Recognized objects are marked with crosses and orientation lines. If it was an match it was dipslay the object. The main challenge was tuning threshold values to achieve accurate recognition while minimizing false matches. I tested various values to balance false positives vs false negatives. Area threshold of 30% and roundedness threshold of 0.15 worked well for the test images.
