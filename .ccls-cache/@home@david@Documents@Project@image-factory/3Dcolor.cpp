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

public:
  int relative_dist(Pixel &pixel) {
    float r_scale = 1;
    float g_scale = 1;
    float b_scale = 1;
    return r_scale * pow(r - pixel.z, 2) + g_scale * pow(g - pixel.y, 2) +
           b_scale * pow(b - pixel.x, 2);
  };
  void set_pixel_color(Pixel &pixel) {
    pixel.x = b;
    pixel.y = g;
    pixel.z = r;
  };
  Color(string hexstring) {
    int hex = std::stoul(hexstring, nullptr, 16);
    r = ((hex >> 16) & 0xFF); // Extract the RR byte
    g = ((hex >> 8) & 0xFF);  // Extract the GG byte
    b = ((hex)&0xFF);         // Extract the BB byte
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

int main() {
  Mat image;
  image = imread("wallpaper.jpg", 1);
  vector<Color> colors;
  ifstream pallete("pallete.txt");
  if (pallete.is_open()) {
    string hex_color;
    while (getline(pallete, hex_color)) {
      colors.push_back(Color(hex_color));
    };
  }
  image.forEach<Pixel>([colors](Pixel &pixel, const int *position) -> void {
    snap_pixel(pixel, colors);
  });
  imwrite("./newwallpaper.jpg", image);
};
