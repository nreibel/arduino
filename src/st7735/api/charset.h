#ifndef SRC_ST7735_API_CHARSET_H_
#define SRC_ST7735_API_CHARSET_H_

#include "st7735_cfg.h"
#include "types.h"

#define ST7735_CHARSET_WIDTH  5
#define ST7735_CHARSET_HEIGHT 7

static const __flash uint8_t s_st7735_charset[][ST7735_CHARSET_WIDTH] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // (space)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x00, 0x40, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x09, 0x09, 0x09, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // "\"
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x08, 0x08, 0x2A, 0x1C, 0x08}  // Tilda
};

#if ST7735_EXTENDED_ASCII

#define ST7735_EXT_ASCII_START_IDX 0xB0

typedef enum {
    E_EURO = 128, // € (Euro sign)
    E_SBQUO = 130, // ‚ (Single low-9 quotation mark)
    E_FNOF = 131, // ƒ (Latin small letter f with hook)
    E_BDQUO = 132, // „ (Double low-9 quotation mark)
    E_HELLIP = 133, // … (Horizontal ellipsis)
    E_DAGGER = 134, // † (Dagger)
    E_DDAGGER = 135, // ‡ (Double dagger)
    E_CIRC = 136, // ˆ (Modifier letter circumflex accent)
    E_PERMIL = 137, // ‰ (Per mille sign)
    E_SCARON_U = 138, // Š (Latin capital letter S with caron)
    E_LSAQUO = 139, // ‹ (Single left-pointing angle quotation)
    E_OELIG_U = 140, // Œ (Latin capital ligature OE)
    E_ZCARON_U = 142, // Ž (Latin capital letter Z with caron)
    E_LSQUO = 145, // ‘ (Left single quotation mark)
    E_RSQUO = 146, // ’ (Right single quotation mark)
    E_LDQUO = 147, // “ (Left double quotation mark)
    E_RDQUO = 148, // ” (Right double quotation mark)
    E_BULL = 149, // • (Bullet)
    E_NDASH = 150, // – (En dash)
    E_MDASH = 151, // — (Em dash)
    E_TILDE = 152, // ˜ (Small tilde)
    E_TRADE = 153, // ™ (Trade mark sign)
    E_SCARON_L = 154, // š (Latin small letter S with caron)
    E_RSAQUO = 155, // › (Single right-pointing angle quotation mark)
    E_OELIG = 156, // œ (Latin small ligature oe)
    E_ZCARON = 158, // ž (Latin small letter z with caron)
    E_YUML_U = 159, // Ÿ (Latin capital letter Y with diaeresis)
    E_NBSP = 160, //   (Non-breaking space)
    E_IEXCL = 161, // ¡ (Inverted exclamation mark)
    E_CENT = 162, // ¢ (Cent sign)
    E_POUND = 163, // £ (Pound sign)
    E_CURREN = 164, // ¤ (Currency sign)
    E_YEN = 165, // ¥ (Yen sign)
    E_BRVBAR = 166, // ¦ (Pipe, Broken vertical bar)
    E_SECT = 167, // § (Section sign)
    E_UML = 168, // ¨ (Spacing diaeresis - umlaut)
    E_COPY = 169, // © (Copyright sign)
    E_ORDF = 170, // ª (Feminine ordinal indicator)
    E_LAQUO = 171, // « (Left double angle quotes)
    E_NOT = 172, // ¬ (Not sign)
    E_SHY = 173, // ­ (Soft hyphen)
    E_REG = 174, // ® (Registered trade mark sign)
    E_MACR = 175, // ¯ (Spacing macron - overline)
    E_DEG = 176, // ° (Degree sign)
    E_PLUSMN = 177, // ± (Plus-or-minus sign)
    E_SUP2 = 178, // ² (Superscript two - squared)
    E_SUP3 = 179, // ³ (Superscript three - cubed)
    E_ACUTE = 180, // ´ (Acute accent - spacing acute)
    E_MICRO = 181, // µ (Micro sign)
    E_PARA = 182, // ¶ (Pilcrow sign - paragraph sign)
    E_MIDDOT = 183, // · (Middle dot - Georgian comma)
    E_CEDIL = 184, // ¸ (Spacing cedilla)
    E_SUP1 = 185, // ¹ (Superscript one)
    E_ORDM = 186, // º (Masculine ordinal indicator)
    E_RAQUO = 187, // » (Right double angle quotes)
    E_FRAC14 = 188, // ¼ (Fraction one quarter)
    E_FRAC12 = 189, // ½ (Fraction one half)
    E_FRAC34 = 190, // ¾ (Fraction three quarters)
    E_IQUEST = 191, // ¿ (Inverted question mark)
    E_AGRAVE_U = 192, // À (Latin capital letter A with grave)
    E_AACUTE_U = 193, // Á (Latin capital letter A with acute)
    E_ACIRC_U = 194, // Â (Latin capital letter A with circumflex)
    E_ATILDE_U = 195, // Ã (Latin capital letter A with tilde)
    E_AUML_U = 196, // Ä (Latin capital letter A with diaeresis)
    E_ARING_U = 197, // Å (Latin capital letter A with ring above)
    E_AELIG_U = 198, // Æ (Latin capital letter AE)
    E_CCEDIL_U = 199, // Ç (Latin capital letter C with cedilla)
    E_EGRAVE_U = 200, // È (Latin capital letter E with grave)
    E_EACUTE_U = 201, // É (Latin capital letter E with acute)
    E_ECIRC_U = 202, // Ê (Latin capital letter E with circumflex)
    E_EUML_U = 203, // Ë (Latin capital letter E with diaeresis)
    E_IGRAVE_U = 204, // Ì (Latin capital letter I with grave)
    E_IACUTE_U = 205, // Í (Latin capital letter I with acute)
    E_ICIRC_U = 206, // Î (Latin capital letter I with circumflex)
    E_IUML_U = 207, // Ï (Latin capital letter I with diaeresis)
    E_ETH_U = 208, // Ð (Latin capital letter ETH)
    E_NTILDE_U = 209, // Ñ (Latin capital letter N with tilde)
    E_OGRAVE_U = 210, // Ò (Latin capital letter O with grave)
    E_OACUTE_U = 211, // Ó (Latin capital letter O with acute)
    E_OCIRC_U = 212, // Ô (Latin capital letter O with circumflex)
    E_OTILDE_U = 213, // Õ (Latin capital letter O with tilde)
    E_OUML_U = 214, // Ö (Latin capital letter O with diaeresis)
    E_TIMES = 215, // × (Multiplication sign)
    E_OSLASH_U = 216, // Ø (Latin capital letter O with slash)
    E_UGRAVE_U = 217, // Ù (Latin capital letter U with grave)
    E_UACUTE_U = 218, // Ú (Latin capital letter U with acute)
    E_UCIRC_U = 219, // Û (Latin capital letter U with circumflex)
    E_UUML_U = 220, // Ü (Latin capital letter U with diaeresis)
    E_YACUTE_U = 221, // Ý (Latin capital letter Y with acute)
    E_THORN_U = 222, // Þ (Latin capital letter THORN)
    E_SZLIG = 223, // ß (Latin small letter sharp s - ess-zed)
    E_AGRAVE = 224, // à (Latin small letter a with grave)
    E_AACUTE = 225, // á (Latin small letter a with acute)
    E_ACIRC = 226, // â (Latin small letter a with circumflex)
    E_ATILDE = 227, // ã (Latin small letter a with tilde)
    E_AUML = 228, // ä (Latin small letter a with diaeresis)
    E_ARING = 229, // å (Latin small letter a with ring above)
    E_AELIG = 230, // æ (Latin small letter ae)
    E_CCEDIL = 231, // ç (Latin small letter c with cedilla)
    E_EGRAVE = 232, // è (Latin small letter e with grave)
    E_EACUTE = 233, // é (Latin small letter e with acute)
    E_ECIRC = 234, // ê (Latin small letter e with circumflex)
    E_EUML = 235, // ë (Latin small letter e with diaeresis)
    E_IGRAVE = 236, // ì (Latin small letter i with grave)
    E_IACUTE = 237, // í (Latin small letter i with acute)
    E_ICIRC = 238, // î (Latin small letter i with circumflex)
    E_IUML = 239, // ï (Latin small letter i with diaeresis)
    E_ETH = 240, // ð (Latin small letter eth)
    E_NTILDE = 241, // ñ (Latin small letter n with tilde)
    E_OGRAVE = 242, // ò (Latin small letter o with grave)
    E_OACUTE = 243, // ó (Latin small letter o with acute)
    E_OCIRC = 244, // ô (Latin small letter o with circumflex)
    E_OTILDE = 245, // õ (Latin small letter o with tilde)
    E_OUML = 246, // ö (Latin small letter o with diaeresis)
    E_DIVIDE = 247, // ÷ (Division sign)
    E_OSLASH = 248, // ø (Latin small letter o with slash)
    E_UGRAVE = 249, // ù (Latin small letter u with grave)
    E_UACUTE = 250, // ú (Latin small letter u with acute)
    E_UCIRC = 251, // û (Latin small letter u with circumflex)
    E_UUML = 252, // ü (Latin small letter u with diaeresis)
    E_YACUTE = 253, // ý (Latin small letter y with acute)
    E_THORN = 254, // þ (Latin small letter thorn)
    E_YUML = 255, // ÿ (Latin small letter y with diaeresis)
} iso_8859_1_e;

static const __flash uint8_t s_st7735_charset_ext[][ST7735_CHARSET_WIDTH] = {
    [E_DEG - ST7735_EXT_ASCII_START_IDX]  = {0x00, 0x06, 0x09, 0x09, 0x06}, // °
};
#endif // ST7735_EXTENDED_ASCII

#endif // SRC_ST7735_API_CHARSET_H_
