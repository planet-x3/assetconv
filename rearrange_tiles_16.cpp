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
#include <fstream>

using namespace std;

uint8_t img_in[32][8][2][8][4];  // y, x, f, v, u
uint8_t img_out[32][8][2][8][4];  // y, v, f, x, u

int main(int argc, char **argv)
{
  if (argc < 3)
    return 1;

  ifstream file_in(argv[1], ios::in | ios::binary);
  ofstream file_out(argv[2], ios::out | ios::trunc | ios::binary);

  file_in.read((char *)img_in, sizeof(img_in));

  for (int y = 0; y < 32; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      for (int f = 0; f < 2; f++)
      {
        for (int v = 0; v < 8; v++)
        {
          for (int u = 0; u < 4; u++)
          {
            img_out[y][v][f][x][u] = img_in[y][x][f][v][u];
          }
        }
      }
    }
  }

  file_out.write((char *)img_out, sizeof(img_out));

  file_in.close();
  file_out.close();

  return 0;
}
