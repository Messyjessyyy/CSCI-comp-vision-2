// Program 2 - HW2
// Author: Jessica Huang

#include <iostream>
#include <string>
#include <map>
#include "image.h"
#include "DisjSets.h"

using namespace ComputerVisionProjects;

int main(int argc, char **argv){
  //given code
  if (argc!=3) {
    std::cout << "Usage: " <<
      argv[0] << " {input_image_name} {output_image_name}" << std::endl;
    return 0;
  }
  const std::string input_file(argv[1]);
  const std::string output_file(argv[2]);
  std::cout << "Running p2 " << input_file << " " << output_file << std::endl;
  
  // reading input image in to convertedImage
  Image convertedImage;
  if(ReadImage(input_file, &convertedImage) == 0){
    std::cout << "Invalid Image. " << std::endl;
    return 1;
  }
  
  Image labeledImage = convertedImage;
  int labelCounter = 0;
  DisjSets disjSets(labeledImage.num_rows()*labeledImage.num_columns());
  
  //first pass
  for(int row = 1; row <(labeledImage.num_rows()); row++){
    for(int column = 1; column < (labeledImage.num_columns()); column++){
      if(labeledImage.GetPixel(row, column) != 0 ){
        // check the neighbors and change it to the neighbor label, otherwise increment the label and set it with a new label
        if(labeledImage.GetPixel(row-1, column) == 0 && labeledImage.GetPixel(row-1, column-1) == 0 && labeledImage.GetPixel(row, column-1)==0){
          labelCounter++;
          labeledImage.SetPixel(row, column, labelCounter);
        }
        //check the one on the diagonal
        else if(labeledImage.GetPixel(row-1, column-1) != 0) {
          labeledImage.SetPixel(row, column, labeledImage.GetPixel(row-1, column-1));
        // the upper guy has a label
       }
      else if(labeledImage.GetPixel(row-1, column) != 0 && labeledImage.GetPixel(row, column-1) == 0){
          labeledImage.SetPixel(row, column, labeledImage.GetPixel(row-1, column));
       }  
        //check the one in the left has label but the one above doesnt have
        else if(labeledImage.GetPixel(row, column-1) != 0 && labeledImage.GetPixel(row-1, column) == 0)  {
          labeledImage.SetPixel(row, column, labeledImage.GetPixel(row, column-1));
        }
        else{
          int above = labeledImage.GetPixel(row-1, column);
          int left = labeledImage.GetPixel(row, column-1);
          auto minLabel = min(left, above);
          labeledImage.SetPixel(row, column, minLabel);
          int aboveLabel = disjSets.find(above);
          int leftLabel = disjSets.find(left);
          if(aboveLabel != leftLabel){
            disjSets.unionSets(aboveLabel, leftLabel);
          }
        }
      }
    } 
  }
  
  //second pass
  for(int row = 1; row <(labeledImage.num_rows()); row++){
    for(int column = 1; column < (labeledImage.num_columns()); column++){
      int newlabel = disjSets.find(labeledImage.GetPixel(row, column));
      labeledImage.SetPixel(row, column, newlabel);
    }
  }
  
  // Remap labels to visible gray levels
  int label_map[256] = {0};
  int new_label = 1;
  for(int row = 1; row < labeledImage.num_rows(); row++){
    for(int column = 1; column < labeledImage.num_columns(); column++){
      int label = labeledImage.GetPixel(row, column);
      if(label > 0 && label_map[label] == 0){
        label_map[label] = new_label++;
      }
    }
  }
  
  int num_objects = new_label - 1;
  std::cout << "Found " << num_objects << " objects" << std::endl;
  
  // Apply visible gray levels (spread across 0-255)
  for(int row = 1; row < labeledImage.num_rows(); row++){
    for(int column = 1; column < labeledImage.num_columns(); column++){
      int label = labeledImage.GetPixel(row, column);
      if(label > 0){
        int remapped = label_map[label];
        int gray_level = (remapped * 255) / num_objects;
        labeledImage.SetPixel(row, column, gray_level);
      }
    }
  }
  
  //writing the image to the output
  if(!WriteImage(output_file, labeledImage)){
    std::cout << "Could not write " << std::endl;
    return 1;
  }
  return 0;
}