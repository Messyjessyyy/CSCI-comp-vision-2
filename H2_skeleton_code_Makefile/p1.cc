// Program 1 - HW2
// Author: Jessica Huang

#include <iostream>
#include <string>
#include <fstream>

#include "image.h"
using namespace ComputerVisionProjects;

int main(int argc, char **argv){
  
  if (argc != 4) {
    std::cout << "Usage: " <<
      argv[0] << " {input_image_name} {threshold} {output_image_name}" << std::endl;
    return 0;
  }

  const std::string input_file(argv[1]);
  const int threshold = std::stoi(argv[2]);
  const std::string output_file(argv[3]);

  std::cout << "Running p1 " << input_file << " " 
            << threshold << " " << output_file << std::endl;


    //create image object on heap (pointer)
    Image *img = new Image();

    //load imagex
    if (!ReadImage(input_file, img)) {
        std::cerr << "Error reading image: " << input_file << std::endl;
        return 1;
    }

    //create binary image on heap also uisng pointer
    Image *binary_img = new Image(); // create new image object
    binary_img->AllocateSpaceAndSetSize(img->num_rows(), img->num_columns()); // Allocating space
    binary_img->SetNumberGrayLevels(255); // Binary image has 2 levels: 0 and 1

    //thresholding values
    for (size_t i = 0; i < img->num_rows(); i++) {  
        for (size_t j = 0; j < img->num_columns(); j++) {
            int pixel_value = img->GetPixel(i, j); // Get pixel value
            if (pixel_value >= threshold) {
                binary_img->SetPixel(i, j, 255); // everything above threshold set to white
            } else {
                binary_img->SetPixel(i, j, 0); // Background set to black 
            }
        }
    }

    //save binary image
    if (!WriteImage(output_file, *binary_img)) {
        std::cerr << "Error writing image: " << output_file << std::endl;
        return 1;
    }

    delete binary_img; // free memory
    return 0;
}