PX3-OSE-related converters
==========================

This repository contains a small collection of conversion programs and helper files for PX3 OSE graphics files.

**CAUTION**: This is ad-hoc code lacking most sanity checks. Its use should be limited to known-working use cases.

The following programs are available:

- `tdy2cgt_screen`: Converts a full-screen bitmap in TDY format to the format needed by the modified text mode.
- `tdy2cgt_tileset`: Does the same for a tileset.
- `rearrange_tiles`: Rearranges the tiles in a VGA tileset bitmap for viewing and editing.
- `rearrange_tiles_16`: Does the same for a TDY (and technically CMP) tileset.

The following files will help with viewing and editing:

- `screen_bmp_header.bin`: Provides a Windows Bitmap stub for VGA screens.
- `screen_bmp_header_tdy.bin`: Provides a Windows Bitmap stub for TDY screens.
- `tileset_bmp_header.bin`: Provides a Windows Bitmap stub for VGA tilesets.
- `tileset_bmp_header_r.bin`: Provides a Windows Bitmap stub for rearranged VGA tilesets.
- `tileset_bmp_header_tdy.bin`: Provides a Windows Bitmap stub for TDY tilesets.
- `tileset_bmp_header_r_tdy.bin`: Provides a Windows Bitmap stub for rearranged TDY tilesets.

The program `cga_ramp.com` (source code in `cga_ramp.s`) will help you determine the gray ramp of a monochrome CGA monitor.
