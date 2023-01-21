// PX3-OSE-related converters.
// Copyright (C) 2019-2023  Benedikt Freisen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <cstdint>
#include <utility>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

struct pixel
{
  uint8_t r, g, b;
};

pixel cga_pal[16] = 
{
  {0x00, 0x00, 0x00},
  {0x00, 0x00, 0xaa},
  {0x00, 0xaa, 0x00},
  {0x00, 0xaa, 0xaa},
  {0xaa, 0x00, 0x00},
  {0xaa, 0x00, 0xaa},
  {0xaa, 0x55, 0x00},
  {0xaa, 0xaa, 0xaa},
  {0x55, 0x55, 0x55},
  {0x55, 0x55, 0xff},
  {0x55, 0xff, 0x55},
  {0x55, 0xff, 0xff},
  {0xff, 0x55, 0x55},
  {0xff, 0x55, 0xff},
  {0xff, 0xff, 0x55},
  {0xff, 0xff, 0xff}
};
uint8_t cga_font[256][8];

void render_char_line(pixel *dest, uint8_t ch, uint8_t attr, uint8_t line)
{
  pixel fg = cga_pal[attr & 0x0f];
  pixel bg = cga_pal[attr >> 4];
  uint8_t pattern = cga_font[ch][line];
  for (uint8_t mask = 0x80; mask; mask >>= 1)
    *(dest++) = (pattern & mask) ? fg : bg;
}

int pixel_diff(pixel a, pixel b)
{
  return abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b);
}

pixel pixel_average4(pixel a, pixel b, pixel c, pixel d)
{
  return {(uint8_t)((a.r + b.r + c.r + d.r) / 4), (uint8_t)((a.g + b.g + c.g + d.g) / 4), (uint8_t)((a.b + b.b + c.b + d.b) / 4)};
}

pixel pixel_average2(pixel a, pixel b)
{
  return {(uint8_t)((a.r + b.r) / 2), (uint8_t)((a.g + b.g) / 2), (uint8_t)((a.b + b.b) / 2)};
}

int diff_score(pixel *line_a1, pixel *line_a2, pixel *line_b1, pixel *line_b2)
{
  int score = 0;
  for (int i = 0; i < 8; i++)
  {
    score += pixel_diff(line_a1[i], line_b1[i]);
    score += pixel_diff(line_a2[i], line_b2[i]);
  }
  pixel avgavg_a;
  pixel avgavg_b;
  for (int i = 0; i < 8; i += 2)
  {
    pixel avg_a = pixel_average4(line_a1[i], line_a1[i + 1], line_a2[i], line_a2[i + 1]);
    pixel avg_b = pixel_average4(line_b1[i], line_b1[i + 1], line_b2[i], line_b2[i + 1]);
    score += pixel_diff(avg_a, avg_b) * 2;
//     if (i % 2 == 0)
//     {
//       avgavg_a = avg_a;
//       avgavg_b = avg_b;
//     }
//     else
//     {
//       avgavg_a = pixel_average2(avgavg_a, avg_a);
//       avgavg_b = pixel_average2(avgavg_b, avg_b);
//       score += pixel_diff(avgavg_a, avgavg_b) * 4;
//     }
  }
  return score;
}

pair<uint8_t, uint8_t> convert(pixel *line1, pixel *line2)
{
  uint8_t ch, attr;
  int min_diff = numeric_limits<int>::max();
  for (int i = 0; i < 256; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      pixel char_line1[8];
      pixel char_line2[8];
      render_char_line(char_line1, i, j, 0);
      render_char_line(char_line2, i, j, 1);
      int new_diff = diff_score(line1, line2, char_line1, char_line2);
      if (new_diff < min_diff)
      {
        min_diff = new_diff;
        ch = i;
        attr = j;
      }
    }
  }
  return pair<uint8_t,uint8_t>(ch, attr);
}

uint8_t clamp_u8(int val)
{
  if (val < 0)
    return 0;
  if (val > 255)
    return 255;
  return (uint8_t)val;
}

pair<uint8_t, uint8_t> convert_with_error_distribution(pixel *line1, pixel *line2, pair<uint8_t, uint8_t> prev)
{
  pixel char_line1[8];
  pixel char_line2[8];
  render_char_line(char_line1, prev.first, prev.second, 0);
  render_char_line(char_line2, prev.first, prev.second, 1);

  pixel temp_line1[8];
  pixel temp_line2[8];
  for (int i = 0; i < 8; i++)
  {
    temp_line1[i].r = clamp_u8(line1[i].r - (char_line1[i].r - line1[i].r));
    temp_line1[i].g = clamp_u8(line1[i].g - (char_line1[i].g - line1[i].g));
    temp_line1[i].b = clamp_u8(line1[i].b - (char_line1[i].b - line1[i].b));
    temp_line2[i].r = clamp_u8(line2[i].r - (char_line2[i].r - line2[i].r));
    temp_line2[i].g = clamp_u8(line2[i].g - (char_line2[i].g - line2[i].g));
    temp_line2[i].b = clamp_u8(line2[i].b - (char_line2[i].b - line2[i].b));
  }

  return convert(temp_line1, temp_line2);
}


#ifdef TILESET
#define Y_DIM 32
#define X_DIM 8
#define V_DIM 8
#define U_DIM 4
#else
#define Y_DIM 1
#define X_DIM 1
#define V_DIM 1
#define U_DIM 8192
#endif

int main(int argc, char **argv)
{
  uint8_t img_in[Y_DIM][X_DIM][2][V_DIM][U_DIM];  // y, x, f, v, u
  uint8_t img_out[Y_DIM][X_DIM][2][V_DIM][U_DIM];  // y, x, f, v, u

  if (argc < 4)
    return 1;

  // read font ROM (e.g. "mda.rom") from file
  ifstream file_font(argv[1], ios::binary | ios::in);
  file_font.seekg(6144);
  file_font.read((char *)cga_font, sizeof(cga_font));
  file_font.close();

  // read input file
  ifstream file_in(argv[2], ios::binary);
  file_in.read((char *)img_in, sizeof(img_in));
  file_in.close();

  cout << "Converting file from TDY to CGT format..." << endl;

  for (int y = 0; y < Y_DIM; y++)
  {
    for (int x = 0; x < X_DIM; x++)
    {
      for (int v = 0; v < V_DIM; v++)
      {
        for (int u = 0; u < U_DIM; u += 2)
        {
          pixel line1[8];
          pixel line2[8];

          for (int r = 0; r < 2; r++)
          {
            line1[4 * r] = cga_pal[img_in[y][x][0][v][u + r] >> 4];
            line1[4 * r + 1] = cga_pal[img_in[y][x][0][v][u + r] >> 4];
            line1[4 * r + 2] = cga_pal[img_in[y][x][0][v][u + r] & 0xf];
            line1[4 * r + 3] = cga_pal[img_in[y][x][0][v][u + r] & 0xf];
            line2[4 * r] = cga_pal[img_in[y][x][1][v][u + r] >> 4];
            line2[4 * r + 1] = cga_pal[img_in[y][x][1][v][u + r] >> 4];
            line2[4 * r + 2] = cga_pal[img_in[y][x][1][v][u + r] & 0xf];
            line2[4 * r + 3] = cga_pal[img_in[y][x][1][v][u + r] & 0xf];
          }

          pair<uint8_t, uint8_t> ch = convert(line1, line2);

          img_out[y][x][0][v][u] = ch.first;
          img_out[y][x][0][v][u + 1] = ch.second;

#ifdef SINGLEPAGE
          ch = {0, 0};
#else
          ch = convert_with_error_distribution(line1, line2, ch);
#endif

          img_out[y][x][1][v][u] = ch.first;
          img_out[y][x][1][v][u + 1] = ch.second;
        }
      }
    }
  }

  // write output file
  ofstream file_out(argv[3], ios::binary | ios::out);
  file_out.write((char *)img_out, sizeof(img_out));
  file_out.close();

  return 0;
}
