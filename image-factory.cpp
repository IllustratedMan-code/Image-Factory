#include <CLI/CLI.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

typedef Point3_<uint8_t> Pixel;

class Color {
  int r;
  int g;
  int b;
  float r_scale;
  float g_scale;
  float b_scale;

public:
  int relative_dist(Pixel &pixel) {
    // this uses the naive algorithm, there are better (more complicated) ways
    // omits square root from distance formula for speed
    // RGB seems to be more effective than HSV for this use-case
    return r_scale * pow(r - pixel.z, 2) + g_scale * pow(g - pixel.y, 2) +
           b_scale * pow(b - pixel.x, 2);
  };
  void set_pixel_color(Pixel &pixel) {
    pixel.x = b;
    pixel.y = g;
    pixel.z = r;
  };
  Color(string hexstring, int r_scale, int g_scale, int b_scale) {
    int hex = std::stoul(hexstring, nullptr, 16);
    r = ((hex >> 16) & 0xFF); // Extract the RR byte
    g = ((hex >> 8) & 0xFF);  // Extract the GG byte
    b = ((hex)&0xFF);         // Extract the BB byte
    this->r_scale = r_scale;
    this->g_scale = g_scale;
    this->b_scale = b_scale;
  }
};
void snap_pixel(Pixel &pixel, vector<Color> colors) {
  vector<int> dist;
  for (auto color : colors) {
    dist.push_back(color.relative_dist(pixel));
  };
  auto it = min_element(begin(dist), end(dist));
  auto index = distance(begin(dist), it);
  colors[index].set_pixel_color(pixel);
}
void convert_image(string input_image_path, string pallete_path,
                   string output_image_path, int r_scale, int g_scale,
                   int b_scale) {
  Mat image;
  image = imread(input_image_path, 1);
  vector<Color> colors;
  ifstream pallete(pallete_path);
  if (pallete.is_open()) {
    string hex_color;
    while (getline(pallete, hex_color)) {
      colors.push_back(Color(hex_color, r_scale, g_scale, b_scale));
    };
  }
  image.forEach<Pixel>([colors](Pixel &pixel, const int *position) -> void {
    snap_pixel(pixel, colors);
  });
  imwrite(output_image_path, image);
}

int main(int argc, char **argv) {
  CLI::App app{"apply color pallete to image"};
  string pallete_path;
  app.add_option("-p,--pallete", pallete_path, "path to pallete file")
      ->required()
      ->check(CLI::ExistingFile);
  string input_image_path;
  app.add_option("-i,--input", input_image_path, "Path to input image")
      ->required()
      ->check(CLI::ExistingFile);
  string output_image_path;
  app.add_option("-o,--output", output_image_path, "Path to output image")
      ->required();
  int r_scale;
  app.add_option("-r,--red", r_scale, "Red scale factor")->default_val(2);
  int g_scale;
  app.add_option("-g,--green", g_scale, "Green scale factor")->default_val(4);
  int b_scale;
  app.add_option("-b,--blue", b_scale, "Blue scale factor")->default_val(3);

  CLI11_PARSE(app, argc, argv)
  convert_image(input_image_path, pallete_path, output_image_path, r_scale,
                g_scale, b_scale);
};
