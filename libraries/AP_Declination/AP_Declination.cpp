/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 *	Adam M Rivera
 *	With direction from: Andrew Tridgell, Jason Short, Justin Beech
 *
 *	Adapted from: http://www.societyofrobots.com/robotforum/index.php?topic=11855.0
 *	Scott Ferguson
 *	scottfromscott@gmail.com
 *

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation; either version 2.1
 of the License, or (at your option) any later version.
*/

#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Declination.h>
#include <avr/pgmspace.h>
#include <math.h>

static const int16_t dec_tbl[37][73] PROGMEM = \
{{150,145,140,135,130,125,120,115,110,105,100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0,-4,-9,-14,-19,-24,-29,-34,-39,-44,-49,-54,-59,-64,-69,-74,-79,-84,-89,-94,-99,104,109,114,119,124,129,134,139,144,149,154,159,164,169,174,179,175,170,165,160,155,150}, \
{143,137,131,126,120,115,110,105,100,95,90,85,80,75,71,66,62,57,53,48,44,39,35,31,27,22,18,14,9,5,1,-3,-7,-11,-16,-20,-25,-29,-34,-38,-43,-47,-52,-57,-61,-66,-71,-76,-81,-86,-91,-96,101,107,112,117,123,128,134,140,146,151,157,163,169,175,178,172,166,160,154,148,143}, \
{130,124,118,112,107,101,96,92,87,82,78,74,70,65,61,57,54,50,46,42,38,34,31,27,23,19,16,12,8,4,1,-2,-6,-10,-14,-18,-22,-26,-30,-34,-38,-43,-47,-51,-56,-61,-65,-70,-75,-79,-84,-89,-94,100,105,111,116,122,128,135,141,148,155,162,170,177,174,166,159,151,144,137,130}, \
{111,104,99,94,89,85,81,77,73,70,66,63,60,56,53,50,46,43,40,36,33,30,26,23,20,16,13,10,6,3,0,-3,-6,-9,-13,-16,-20,-24,-28,-32,-36,-40,-44,-48,-52,-57,-61,-65,-70,-74,-79,-84,-88,-93,-98,103,109,115,121,128,135,143,152,162,172,176,165,154,144,134,125,118,111}, \
{85,81,77,74,71,68,65,63,60,58,56,53,51,49,46,43,41,38,35,32,29,26,23,19,16,13,10,7,4,1,-1,-3,-6,-9,-13,-16,-19,-23,-26,-30,-34,-38,-42,-46,-50,-54,-58,-62,-66,-70,-74,-78,-83,-87,-91,-95,100,105,110,117,124,133,144,159,178,160,141,125,112,103,96,90,85}, \
{62,60,58,57,55,54,52,51,50,48,47,46,44,42,41,39,36,34,31,28,25,22,19,16,13,10,7,4,2,0,-3,-5,-8,-10,-13,-16,-19,-22,-26,-29,-33,-37,-41,-45,-49,-53,-56,-60,-64,-67,-70,-74,-77,-80,-83,-86,-89,-91,-94,-97,101,105,111,130,109,84,77,74,71,68,66,64,62}, \
{46,46,45,44,44,43,42,42,41,41,40,39,38,37,36,35,33,31,28,26,23,20,16,13,10,7,4,1,-1,-3,-5,-7,-9,-12,-14,-16,-19,-22,-26,-29,-33,-36,-40,-44,-48,-51,-55,-58,-61,-64,-66,-68,-71,-72,-74,-74,-75,-74,-72,-68,-61,-48,-25,2,22,33,40,43,45,46,47,46,46}, \
{36,36,36,36,36,35,35,35,35,34,34,34,34,33,32,31,30,28,26,23,20,17,14,10,6,3,0,-2,-4,-7,-9,-10,-12,-14,-15,-17,-20,-23,-26,-29,-32,-36,-40,-43,-47,-50,-53,-56,-58,-60,-62,-63,-64,-64,-63,-62,-59,-55,-49,-41,-30,-17,-4,6,15,22,27,31,33,34,35,36,36}, \
{30,30,30,30,30,30,30,29,29,29,29,29,29,29,29,28,27,26,24,21,18,15,11,7,3,0,-3,-6,-9,-11,-12,-14,-15,-16,-17,-19,-21,-23,-26,-29,-32,-35,-39,-42,-45,-48,-51,-53,-55,-56,-57,-57,-56,-55,-53,-49,-44,-38,-31,-23,-14,-6,0,7,13,17,21,24,26,27,29,29,30}, \
{25,25,26,26,26,25,25,25,25,25,25,25,25,26,25,25,24,23,21,19,16,12,8,4,0,-3,-7,-10,-13,-15,-16,-17,-18,-19,-20,-21,-22,-23,-25,-28,-31,-34,-37,-40,-43,-46,-48,-49,-50,-51,-51,-50,-48,-45,-42,-37,-32,-26,-19,-13,-7,-1,3,7,11,14,17,19,21,23,24,25,25}, \
{21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,20,18,16,13,9,5,1,-3,-7,-11,-14,-17,-18,-20,-21,-21,-22,-22,-22,-23,-23,-25,-27,-29,-32,-35,-37,-40,-42,-44,-45,-45,-45,-44,-42,-40,-36,-32,-27,-22,-17,-12,-7,-3,0,3,7,9,12,14,16,18,19,20,21,21}, \
{18,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,18,17,16,14,10,7,2,-1,-6,-10,-14,-17,-19,-21,-22,-23,-24,-24,-24,-24,-23,-23,-23,-24,-26,-28,-30,-33,-35,-37,-38,-39,-39,-38,-36,-34,-31,-28,-24,-19,-15,-10,-6,-3,0,1,4,6,8,10,12,14,15,16,17,18,18}, \
{16,16,17,17,17,17,17,17,17,17,17,16,16,16,16,16,16,15,13,11,8,4,0,-4,-9,-13,-16,-19,-21,-23,-24,-25,-25,-25,-25,-24,-23,-21,-20,-20,-21,-22,-24,-26,-28,-30,-31,-32,-31,-30,-29,-27,-24,-21,-17,-13,-9,-6,-3,-1,0,2,4,5,7,9,10,12,13,14,15,16,16}, \
{14,14,14,15,15,15,15,15,15,15,14,14,14,14,14,14,13,12,11,9,5,2,-2,-6,-11,-15,-18,-21,-23,-24,-25,-25,-25,-25,-24,-22,-21,-18,-16,-15,-15,-15,-17,-19,-21,-22,-24,-24,-24,-23,-22,-20,-18,-15,-12,-9,-5,-3,-1,0,1,2,4,5,6,8,9,10,11,12,13,14,14}, \
{12,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,11,10,9,6,3,0,-4,-8,-12,-16,-19,-21,-23,-24,-24,-24,-24,-23,-22,-20,-17,-15,-12,-10,-9,-9,-10,-12,-13,-15,-17,-17,-18,-17,-16,-15,-13,-11,-8,-5,-3,-1,0,1,1,2,3,4,6,7,8,9,10,11,12,12,12}, \
{11,11,11,11,11,12,12,12,12,12,11,11,11,11,11,10,10,9,7,5,2,-1,-5,-9,-13,-17,-20,-22,-23,-23,-23,-23,-22,-20,-18,-16,-14,-11,-9,-6,-5,-4,-5,-6,-8,-9,-11,-12,-12,-12,-12,-11,-9,-8,-6,-3,-1,0,0,1,1,2,3,4,5,6,7,8,9,10,11,11,11}, \
{10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,9,9,7,6,3,0,-3,-6,-10,-14,-17,-20,-21,-22,-22,-22,-21,-19,-17,-15,-13,-10,-8,-6,-4,-2,-2,-2,-2,-4,-5,-7,-8,-8,-9,-8,-8,-7,-5,-4,-2,0,0,1,1,1,2,2,3,4,5,6,7,8,9,10,10,10},
{9,9,9,9,9,9,9,10,10,9,9,9,9,9,9,8,8,6,5,2,0,-4,-7,-11,-15,-17,-19,-21,-21,-21,-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,0,0,-1,-2,-4,-5,-5,-6,-6,-5,-5,-4,-3,-1,0,0,1,1,1,1,2,3,3,5,6,7,8,8,9,9,9}, \
{9,9,9,9,9,9,9,9,9,9,9,9,8,8,8,8,7,5,4,1,-1,-5,-8,-12,-15,-17,-19,-20,-20,-19,-18,-16,-14,-11,-9,-7,-5,-4,-2,-1,0,0,1,1,0,0,-2,-3,-3,-4,-4,-4,-3,-3,-2,-1,0,0,0,0,0,1,1,2,3,4,5,6,7,8,8,9,9}, \
{9,9,9,8,8,8,9,9,9,9,9,8,8,8,8,7,6,5,3,0,-2,-5,-9,-12,-15,-17,-18,-19,-19,-18,-16,-14,-12,-9,-7,-5,-4,-2,-1,0,0,1,1,1,1,0,0,-1,-2,-2,-3,-3,-2,-2,-1,-1,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,8,9}, \
{8,8,8,8,8,8,9,9,9,9,9,9,8,8,8,7,6,4,2,0,-3,-6,-9,-12,-15,-17,-18,-18,-17,-16,-14,-12,-10,-8,-6,-4,-2,-1,0,0,1,2,2,2,2,1,0,0,-1,-1,-1,-2,-2,-1,-1,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,8}, \
{8,8,8,8,9,9,9,9,9,9,9,9,9,8,8,7,5,3,1,-1,-4,-7,-10,-13,-15,-16,-17,-17,-16,-15,-13,-11,-9,-6,-5,-3,-2,0,0,0,1,2,2,2,2,1,1,0,0,0,-1,-1,-1,-1,-1,0,0,0,0,-1,-1,-1,-1,-1,0,0,1,3,4,5,7,7,8}, \
{8,8,9,9,9,9,10,10,10,10,10,10,10,9,8,7,5,3,0,-2,-5,-8,-11,-13,-15,-16,-16,-16,-15,-13,-12,-10,-8,-6,-4,-2,-1,0,0,1,2,2,3,3,2,2,1,0,0,0,0,0,0,0,0,0,0,-1,-1,-2,-2,-2,-2,-2,-1,0,0,1,3,4,6,7,8}, \
{7,8,9,9,9,10,10,11,11,11,11,11,10,10,9,7,5,3,0,-2,-6,-9,-11,-13,-15,-16,-16,-15,-14,-13,-11,-9,-7,-5,-3,-2,0,0,1,1,2,3,3,3,3,2,2,1,1,0,0,0,0,0,0,0,-1,-1,-2,-3,-3,-4,-4,-4,-3,-2,-1,0,1,3,5,6,7}, \
{6,8,9,9,10,11,11,12,12,12,12,12,11,11,9,7,5,2,0,-3,-7,-10,-12,-14,-15,-16,-15,-15,-13,-12,-10,-8,-7,-5,-3,-1,0,0,1,2,2,3,3,4,3,3,3,2,2,1,1,1,0,0,0,0,-1,-2,-3,-4,-4,-5,-5,-5,-5,-4,-2,-1,0,2,3,5,6}, \
{6,7,8,10,11,12,12,13,13,14,14,13,13,11,10,8,5,2,0,-4,-8,-11,-13,-15,-16,-16,-16,-15,-13,-12,-10,-8,-6,-5,-3,-1,0,0,1,2,3,3,4,4,4,4,4,3,3,3,2,2,1,1,0,0,-1,-2,-3,-5,-6,-7,-7,-7,-6,-5,-4,-3,-1,0,2,4,6}, \
{5,7,8,10,11,12,13,14,15,15,15,14,14,12,11,8,5,2,-1,-5,-9,-12,-14,-16,-17,-17,-16,-15,-14,-12,-11,-9,-7,-5,-3,-1,0,0,1,2,3,4,4,5,5,5,5,5,5,4,4,3,3,2,1,0,-1,-2,-4,-6,-7,-8,-8,-8,-8,-7,-6,-4,-2,0,1,3,5}, \
{4,6,8,10,12,13,14,15,16,16,16,16,15,13,11,9,5,2,-2,-6,-10,-13,-16,-17,-18,-18,-17,-16,-15,-13,-11,-9,-7,-5,-4,-2,0,0,1,3,3,4,5,6,6,7,7,7,7,7,6,5,4,3,2,0,-1,-3,-5,-7,-8,-9,-10,-10,-10,-9,-7,-5,-4,-1,0,2,4}, \
{4,6,8,10,12,14,15,16,17,18,18,17,16,15,12,9,5,1,-3,-8,-12,-15,-18,-19,-20,-20,-19,-18,-16,-15,-13,-11,-8,-6,-4,-2,-1,0,1,3,4,5,6,7,8,9,9,9,9,9,9,8,7,5,3,1,-1,-3,-6,-8,-10,-11,-12,-12,-11,-10,-9,-7,-5,-2,0,1,4}, \
{4,6,8,11,13,15,16,18,19,19,19,19,18,16,13,10,5,0,-5,-10,-15,-18,-21,-22,-23,-22,-22,-20,-18,-17,-14,-12,-10,-8,-5,-3,-1,0,1,3,5,6,8,9,10,11,12,12,13,12,12,11,9,7,5,2,0,-3,-6,-9,-11,-12,-13,-13,-12,-11,-10,-8,-6,-3,-1,1,4}, \
{3,6,9,11,14,16,17,19,20,21,21,21,19,17,14,10,4,-1,-8,-14,-19,-22,-25,-26,-26,-26,-25,-23,-21,-19,-17,-14,-12,-9,-7,-4,-2,0,1,3,5,7,9,11,13,14,15,16,16,16,16,15,13,10,7,4,0,-3,-7,-10,-12,-14,-15,-14,-14,-12,-11,-9,-6,-4,-1,1,3}, \
{4,6,9,12,14,17,19,21,22,23,23,23,21,19,15,9,2,-5,-13,-20,-25,-28,-30,-31,-31,-30,-29,-27,-25,-22,-20,-17,-14,-11,-9,-6,-3,0,1,4,6,9,11,13,15,17,19,20,21,21,21,20,18,15,11,6,2,-2,-7,-11,-13,-15,-16,-16,-15,-13,-11,-9,-7,-4,-1,1,4}, \
{4,7,10,13,15,18,20,22,24,25,25,25,23,20,15,7,-2,-12,-22,-29,-34,-37,-38,-38,-37,-36,-34,-31,-29,-26,-23,-20,-17,-13,-10,-7,-4,-1,2,5,8,11,13,16,18,21,23,24,26,26,26,26,24,21,17,12,5,0,-6,-10,-14,-16,-16,-16,-15,-14,-12,-10,-7,-4,-1,1,4}, \
{4,7,10,13,16,19,22,24,26,27,27,26,24,19,11,-1,-15,-28,-37,-43,-46,-47,-47,-45,-44,-41,-39,-36,-32,-29,-26,-22,-19,-15,-11,-8,-4,-1,2,5,9,12,15,19,22,24,27,29,31,33,33,33,32,30,26,21,14,6,0,-6,-11,-14,-15,-16,-15,-14,-12,-9,-7,-4,-1,1,4}, \
{6,9,12,15,18,21,23,25,27,28,27,24,17,4,-14,-34,-49,-56,-60,-60,-60,-58,-56,-53,-50,-47,-43,-40,-36,-32,-28,-25,-21,-17,-13,-9,-5,-1,2,6,10,14,17,21,24,28,31,34,37,39,41,42,43,43,41,38,33,25,17,8,0,-4,-8,-10,-10,-10,-8,-7,-4,-2,0,3,6}, \
{22,24,26,28,30,32,33,31,23,-18,-81,-96,-99,-98,-95,-93,-89,-86,-82,-78,-74,-70,-66,-62,-57,-53,-49,-44,-40,-36,-32,-27,-23,-19,-14,-10,-6,-1,2,6,10,15,19,23,27,31,35,38,42,45,49,52,55,57,60,61,63,63,62,61,57,53,47,40,33,28,23,21,19,19,19,20,22},
{168,173,178,176,171,166,161,156,151,146,141,136,131,126,121,116,111,106,101,-96,-91,-86,-81,-76,-71,-66,-61,-56,-51,-46,-41,-36,-31,-26,-21,-16,-11,-6,-1,3,8,13,18,23,28,33,38,43,48,53,58,63,68,73,78,83,88,93,98,103,108,113,118,123,128,133,138,143,148,153,158,163,168}};

float
AP_Declination::get_declination(float lat, float lon)
{
	int16_t decSW, decSE, decNW, decNE, lonmin, latmin;
	uint8_t latmin_index,lonmin_index;
	float decmin, decmax;

	// Constrain to valid inputs
	lat = constrain(lat, -90, 90);
	lon = constrain(lon, -180, 180);

	latmin = floor(lat/5)*5;
	lonmin = floor(lon/5)*5;

	latmin_index= (90+latmin)/5;
	lonmin_index= (180+lonmin)/5;

	decSW = (int16_t)pgm_read_byte_far(&dec_tbl[latmin_index][lonmin_index]);
	decSE = (int16_t)pgm_read_byte_far(&dec_tbl[latmin_index][lonmin_index+1]);
	decNE = (int16_t)pgm_read_byte_far(&dec_tbl[latmin_index+1][lonmin_index+1]);
	decNW = (int16_t)pgm_read_byte_far(&dec_tbl[latmin_index+1][lonmin_index]);

	/* approximate declination within the grid using bilinear interpolation */
	decmin = (lon - lonmin) / 5 * (decSE - decSW) + decSW;
	decmax = (lon - lonmin) / 5 * (decNE - decNW) + decNW;
	return   (lat - latmin) / 5 * (decmax - decmin) + decmin;
}
