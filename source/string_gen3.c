/*
 * This file is part of the PokeBoxDS project.
 * Copyright (C) 2019 Jennifer Berringer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; even with the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "string_gen3.h"

#include "asset_manager.h"
#include "utf8.h"

/* For more info on the games' character encoding, see:
 * https://bulbapedia.bulbagarden.net/wiki/Character_encoding_in_Generation_III
 */
static const uint16_t charmap_latin[] = {
' ',  0xC0, 0xC1, 0xC2, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC,    0, 0xCE, 0xCF, 0xD2, 0xD3, 0xD4,
0,    0xD9, 0xDA, 0xDB, 0xD1, 0xDF, 0xE0, 0xE1, 0,    0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0,
0xEE, 0xEF, 0xF2, 0xF3, 0xF4,    0, 0xF9, 0xFA, 0xFB, 0xF1, 0xB0,    0,    0,  '&',  '+', 0,
0,    0,    0,    0,    0,    '=',  ';',  0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0x25AF,   0xBF,   0xA1, 0xE000, 0xE001, 0xE002, 0xE003,    0,
0,           0,   0xCD,    '%',    '(',    ')',      0,    0,
0,           0,      0,      0,      0,      0,      0,    0,
0xE2,        0,      0,      0,      0,      0,      0, 0xED,
0,           0,      0,      0,      0,      0,      0,    0,
0,      0x2B06, 0x2B07, 0x2B05, 0x27A1,      0,      0,    0,
0,    0,    0,    0,    '<',  '>',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '!',  '?',  '.',  '-',  0xB7,
0x2026, 0x201C, 0x201D, 0x2018, 0x2019, 0x2642, 0x2640, 0xE004,
',',    0x00D7,    '/',    'A',    'B',    'C',    'D',    'E',
'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',
'V',  'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',
'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  0x25B6,
':',  0xC4, 0xD6, 0xDC, 0xE4, 0xF6, 0xFC, 0,    0,    0,    0,    0,    0,    0,    0,    0};

static const uint16_t charmap_jp[] = {
// Hiragana: 00-4F (00 is CJK space)
0x3000, 0x3042, 0x3044, 0x3046, 0x3048, 0x304A, 0x304B, 0x304D,
0x304F, 0x3051, 0x3053, 0x3055, 0x3057, 0x3059, 0x305B, 0x305D,
0x305F, 0x3061, 0x3064, 0x3066, 0x3068, 0x306A, 0x306B, 0x306C,
0x306D, 0x306E, 0x306F, 0x3072, 0x3075, 0x3078, 0x307B, 0x307E,
0x307F, 0x3080, 0x3081, 0x3082, 0x3084, 0x3086, 0x3088, 0x3089,
0x308A, 0x308B, 0x308C, 0x308D, 0x308F, 0x3092, 0x3093, 0x3041,
0x3043, 0x3045, 0x3047, 0x3049, 0x3083, 0x3085, 0x3087, 0x304C,
0x304E, 0x3050, 0x3052, 0x3054, 0x3056, 0x3058, 0x305A, 0x305C,
0x305E, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x3070, 0x3073,
0x3076, 0x3079, 0x307C, 0x3071, 0x3074, 0x3077, 0x307A, 0x307D,
// Katakana: 50-9F (50 is Hiragana small-Tsu)
0x3063, 0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD,
0x30AF, 0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD,
0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC,
0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE,
0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9,
0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F2, 0x30F3, 0x30A1,
0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30AC,
0x30AE, 0x30B0, 0x30B2, 0x30B4, 0x30B6, 0x30B8, 0x30BA, 0x30BC,
0x30BE, 0x30C0, 0x30C2, 0x30C5, 0x30C7, 0x30C9, 0x30D0, 0x30D3,
0x30D6, 0x30D9, 0x30DC, 0x30D1, 0x30D4, 0x30D7, 0x30DA, 0x30DD,
// Fullwidth Numbers and symbols
0x30C3, 0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16,
0xFF17, 0xFF18, 0xFF19, 0xFF01, 0xFF1F, 0x3002, 0x30FC, 0x30FB,
0x2026, 0x300E, 0x300F, 0x300C, 0x300D, 0x2642, 0x2640, 0x5186,
// Fullwidth Latin alphabet
0xFF0C, 0x00D7, 0xFF0F, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25,
0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D,
0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35,
0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0xFF41, 0xFF42, 0xFF43,
0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B,
0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53,
0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0x25B6,
0xFF1A,   0xC4,   0xD6,   0xDC,   0xE4,   0xF6,   0xFC,      0,
     0,      0,      0,      0,      0,      0,      0,      0};

int decode_gen3_string(char *out, const uint8_t *str, int destLen, int len, uint16_t lang) {
	const uint16_t *map;
	int outLen;

	lang &= 0xF;
	if (lang == LANG_JAPANESE)
		map = charmap_jp;
	else
		map = charmap_latin;

	outLen = 0;
	for (int i = 0; i < len; i++) {
		uint8_t ch = str[i];
		uint16_t cp;
		if (ch == 0xFF)
			break;
		cp = map[ch];
		// Only French has double angle quotes in place of double tickmark quotes
		if (lang == LANG_FRENCH && (ch == 0xB1 || ch == 0xB2))
			cp = 0xAB + (ch - 0xB1);
		if (!cp)
			cp = '?';
		outLen += utf8_encode_one(out + outLen, cp, destLen - outLen);
	}
	out[outLen] = 0;
	return outLen;
}

int decode_gen3_string16(uint16_t *out, const uint8_t *str, int len, uint16_t lang) {
	const uint16_t *map;
	int outLen;

	lang &= 0xF;
	if (lang == LANG_JAPANESE)
		map = charmap_jp;
	else
		map = charmap_latin;

	outLen = 0;
	for (int i = 0; i < len; i++) {
		uint8_t ch = str[i];
		uint16_t cp;
		if (ch == 0xFF)
			break;
		cp = map[ch];
		// Only French has double angle quotes in place of double tickmark quotes
		if (lang == LANG_FRENCH && (ch == 0xB1 || ch == 0xB2))
			cp = 0xAB + (ch - 0xB1);
		if (!cp)
			cp = '?';
		out[outLen++] = cp;
	}
	out[outLen] = 0;
	return outLen;
}
