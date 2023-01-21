
all: tdy2cgt_screen tdy2cgt_tileset rearrange_tiles rearrange_tiles_16

tdy2cgt_screen: tdy2cgt.cpp
	g++ -O2 -o tdy2cgt_screen -DSINGLEPAGE tdy2cgt.cpp

tdy2cgt_tileset: tdy2cgt.cpp
	g++ -O2 -o tdy2cgt_tileset -DSINGLEPAGE -DTILESET tdy2cgt.cpp

rearrange_tiles: rearrange_tiles.cpp
	g++ -O2 -o rearrange_tiles rearrange_tiles.cpp

rearrange_tiles_16: rearrange_tiles_16.cpp
	g++ -O2 -o rearrange_tiles_16 rearrange_tiles_16.cpp

.PHONY: clean

clean:
	rm tdy2cgt_screen
	rm tdy2cgt_tileset
	rm rearrange_tiles
	rm rearrange_tiles_16
