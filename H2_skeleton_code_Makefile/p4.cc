// Program 4 - HW2 (Extra Credit)
// Author: Jessica Huang

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <vector>
#include "image.h"

using namespace ComputerVisionProjects;

struct ObjectAttributes {
    int label;
    double center_row, center_col, emin, roundedness, orientation;
    int area;
};

// ---- Helper: load database ----
std::vector<ObjectAttributes> LoadDatabase(const std::string& filename) {
    std::vector<ObjectAttributes> db;
    std::ifstream fin(filename);
    if (!fin) {
        std::cerr << "Error: cannot open database file " << filename << std::endl;
        return db;
    }
    ObjectAttributes obj;
    while (fin >> obj.label >> obj.center_row >> obj.center_col
               >> obj.emin >> obj.area >> obj.roundedness >> obj.orientation) {
        db.push_back(obj);
    }
    return db;
}

// ---- Helper: extract objects from labeled image ----
std::vector<ObjectAttributes> ExtractObjects(const Image& img) {
    std::map<int, std::vector<std::pair<int,int>>> pixels_by_label;
    for (int r = 0; r < img.num_rows(); r++) {
        for (int c = 0; c < img.num_columns(); c++) {
            int label = img.GetPixel(r,c);
            if (label > 0) pixels_by_label[label].push_back({r,c});
        }
    }

    std::vector<ObjectAttributes> objs;
    for (auto &entry : pixels_by_label) {
        ObjectAttributes obj;
        obj.label = entry.first;
        obj.area = entry.second.size();

        double sum_r=0, sum_c=0;
        for (auto &p : entry.second) { sum_r += p.first; sum_c += p.second; }
        obj.center_row = sum_r / obj.area;
        obj.center_col = sum_c / obj.area;

        // moments
        double a=0,b=0,c=0;
        for (auto &p : entry.second) {
            double y = p.first - obj.center_row;
            double x = p.second - obj.center_col;
            a += x*x; b += 2*x*y; c += y*y;
        }
        double theta = 0.5*atan2(b,a-c);
        obj.orientation = theta*180.0/M_PI;
        double emin = a*pow(sin(theta),2) - b*sin(theta)*cos(theta) + c*pow(cos(theta),2);
        double theta2 = theta+M_PI/2.0;
        double emax = a*pow(sin(theta2),2) - b*sin(theta2)*cos(theta2) + c*pow(cos(theta2),2);
        obj.emin = emin;
        obj.roundedness = (emax>0)?emin/emax:0;

        objs.push_back(obj);
    }
    return objs;
}

// ---- Helper: draw recognition results ----
void DrawObject(Image* img, const ObjectAttributes& obj) {
    int r = (int)(obj.center_row+0.5);
    int c = (int)(obj.center_col+0.5);
    // cross at centroid
    for (int d=-2; d<=2; d++) {
        if (r+d>=0 && r+d<img->num_rows()) img->SetPixel(r+d,c,0);
        if (c+d>=0 && c+d<img->num_columns()) img->SetPixel(r,c+d,0);
    }
    // orientation line
    double theta = obj.orientation*M_PI/180.0;
    int end_r = (int)(r + 20*sin(theta));
    int end_c = (int)(c + 20*cos(theta));
    DrawLine(c,r,end_c,end_r,0,img);
}
int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0]
                  << " {input labeled image} {input database} {output image}\n";
        return 0;
    }

    const std::string input_image_file(argv[1]);
    const std::string db_file(argv[2]);
    const std::string output_file(argv[3]);

    Image img;
    if (!ReadImage(input_image_file,&img)) {
        std::cerr << "Error reading " << input_image_file << std::endl;
        return -1;
    }

    auto db = LoadDatabase(db_file);
    auto objs = ExtractObjects(img);

    std::cout << "Database objects: " << db.size() << "\n";
    std::cout << "Scene objects: " << objs.size() << "\n";

    // ---- Compare ----
    for (auto &scene_obj : objs) {
        for (auto &model_obj : db) {
            double area_diff = fabs(scene_obj.area - model_obj.area)/model_obj.area;
            double round_diff = fabs(scene_obj.roundedness - model_obj.roundedness);

            if (area_diff < 0.3 && round_diff < 0.15) {
                std::cout << "Recognized object (model " << model_obj.label
                          << ") at (" << scene_obj.center_row << ","
                          << scene_obj.center_col << ")\n";
                DrawObject(&img, scene_obj);
            }
        }
    }

    if (!WriteImage(output_file,img)) {
        std::cerr << "Error writing " << output_file << std::endl;
        return -2;
    }

    std::cout << "Recognition complete. Results in " << output_file << "\n";
    return 0;
}
