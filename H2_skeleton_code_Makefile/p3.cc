// Program 3 - HW2
// Author: Jessica Huang

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include "image.h"

using namespace ComputerVisionProjects;

int main(int argc, char **argv){
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] 
                  << " {input_image_name} {output descriptions} {output_image_name}" << std::endl;
        return 0;
    }
    
    const std::string input_file(argv[1]);
    const std::string output_object_descriptions(argv[2]);
    const std::string output_file(argv[3]);
    
    std::cout << "Running p3 " << input_file << " "
              << output_object_descriptions << " " << output_file << std::endl;
    
    // Reading input image
    Image labeledImage;
    if (!ReadImage(input_file, &labeledImage)) {
        std::cout << "Invalid Image." << std::endl;
        return 1;
    }
    
    // Maps to accumulate values for each label
    std::map<int, int> area;
    std::map<int, double> xSum, ySum, xxSum, xySum, yySum;
    
    // First pass: accumulate sums
    for (int row = 0; row < labeledImage.num_rows(); row++) {
        for (int col = 0; col < labeledImage.num_columns(); col++) {
            int label = labeledImage.GetPixel(row, col);
            if (label != 0) {
                xSum[label] += row;
                ySum[label] += col;
                area[label]++;
                xxSum[label] += row * row;
                xySum[label] += row * col;
                yySum[label] += col * col;
            }
        }
    }
    
    // Open output file
    std::ofstream outputFile(output_object_descriptions);
    if (!outputFile) { 
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }
    
    // Process each object
    for (auto x : area) {
        int label = x.first;
        
        // Compute center
        double xCenter = xSum[label] / area[label];
        double yCenter = ySum[label] / area[label];
        
        // Compute a, b, c based on formulas in slides
        double a = xxSum[label] - area[label] * xCenter * xCenter;
        double b = 2.0 * (xySum[label] - area[label] * xCenter * yCenter);
        double c = yySum[label] - area[label] * yCenter * yCenter;
        
        // theta1 in radians
        const double theta1 = atan2(b, a - c) / 2.0;
        
        // If you want to see it in degrees
        const double theta1_degrees = 180.0 * theta1 / M_PI;
        
        // Emin
        const double e_min = a * sin(theta1) * sin(theta1) - 
                            b * sin(theta1) * cos(theta1) + 
                            c * cos(theta1) * cos(theta1);
        
        // Angle maximizing E
        const double theta2 = theta1 + M_PI / 2.0;
        
        // Emax (same as Emin, but use theta2)
        const double e_max = a * sin(theta2) * sin(theta2) - 
                            b * sin(theta2) * cos(theta2) + 
                            c * cos(theta2) * cos(theta2);
        
        // Roundness
        const double roundness = e_min / e_max;
        
        // Draw center dot
        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {
                int r = (int)(xCenter) + dr;
                int c = (int)(yCenter) + dc;
                if (r >= 0 && r < labeledImage.num_rows() && 
                    c >= 0 && c < labeledImage.num_columns()) {
                    labeledImage.SetPixel(r, c, 255);
                }
            }
        }
        
        // Draw orientation line in both directions from center
        // This shows the principal axis of the object
        int line_length = 50;
        int end_row1 = (int)(xCenter + line_length * sin(theta1));
        int end_col1 = (int)(yCenter + line_length * cos(theta1));
        int end_row2 = (int)(xCenter - line_length * sin(theta1));
        int end_col2 = (int)(yCenter - line_length * cos(theta1));
        
        // Draw line from one end to the other, passing through center
        DrawLine(end_row2, end_col2, end_row1, end_col1, 255, &labeledImage);
        
        // Write to output file
        outputFile << label << " " 
                   << xCenter << " " 
                   << yCenter << " " 
                   << e_min << " " 
                   << area[label] << " " 
                   << roundness << " " 
                   << theta1_degrees << std::endl;

    }
    
    outputFile.close();
    std::cout << "Object descriptions written to " << output_object_descriptions << std::endl;
    
    // Write output image
    if (!WriteImage(output_file, labeledImage)) {
        std::cout << "Could not write image." << std::endl;
        return 1;
    }
    
    std::cout << "Output image saved to " << output_file << std::endl;
    
    return 0;
}