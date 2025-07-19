#pragma once

namespace core
{
	enum class MouseButton : char
	{
		Left = 1,
		Middle,
		Right,
		Extra1,
		Extra2
	};

	enum class Scancode : short
	{
		Unknown = 0,

		/**
		 *  \name Usage page 0x07
		 *
		 *  These values are from usage page 0x07 (USB keyboard page).
		 */
		/* @{ */

		A = 4,
		B = 5,
		C = 6,
		D = 7,
		E = 8,
		F = 9,
		G = 10,
		H = 11,
		I = 12,
		J = 13,
		K = 14,
		L = 15,
		M = 16,
		N = 17,
		O = 18,
		P = 19,
		Q = 20,
		R = 21,
		S = 22,
		T = 23,
		U = 24,
		V = 25,
		W = 26,
		X = 27,
		Y = 28,
		Z = 29,

		One = 30,
		Two = 31,
		Three = 32,
		Four = 33,
		Five = 34,
		Six = 35,
		Seven = 36,
		Eight = 37,
		Nine = 38,
		Zero = 39,

		Return = 40,
		Escape = 41,
		Backspace = 42,
		Tab = 43,
		Space = 44,

		Minus = 45,
		Equals = 46,
		LeftBracket = 47,
		RightBracket = 48,
		Backslash = 49, /**< Located at the lower left of the return
						 *   key on ISO keyboards and at the right end
						 *   of the QWERTY row on ANSI keyboards.
						 *   Produces REVERSE SOLIDUS (backslash) and
						 *   VERTICAL LINE in a US layout, REVERSE
						 *   SOLIDUS and VERTICAL LINE in a UK Mac
						 *   layout, NUMBER SIGN and TILDE in a UK
						 *   Windows layout, DOLLAR SIGN and POUND SIGN
						 *   in a Swiss German layout, NUMBER SIGN and
						 *   APOSTROPHE in a German layout, GRAVE
						 *   ACCENT and POUND SIGN in a French Mac
						 *   layout, and ASTERISK and MICRO SIGN in a
						 *   French Windows layout.
						 */
		NonUsHash = 50, /**< ISO USB keyboards actually use this code
						 *   instead of 49 for the same key, but all
						 *   OSes I've seen treat the two codes
						 *   identically. So, as an implementor, unless
						 *   your keyboard generates both of those
						 *   codes and your OS treats them differently,
						 *   you should generate Backslash
						 *   instead of this code. As a user, you
						 *   should not rely on this code because SDL
						 *   will never generate it with most (all?)
						 *   keyboards.
						 */
		Semicolon = 51,
		Apostrophe = 52,
		Grave = 53, /**< Located in the top left corner (on both ANSI
					 *   and ISO keyboards). Produces GRAVE ACCENT and
					 *   TILDE in a US Windows layout and in US and UK
					 *   Mac layouts on ANSI keyboards, GRAVE ACCENT
					 *   and NOT SIGN in a UK Windows layout, SECTION
					 *   SIGN and PLUS-MINUS SIGN in US and UK Mac
					 *   layouts on ISO keyboards, SECTION SIGN and
					 *   DEGREE SIGN in a Swiss German layout (Mac:
					 *   only on ISO keyboards), CIRCUMFLEX ACCENT and
					 *   DEGREE SIGN in a German layout (Mac: only on
					 *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
					 *   French Windows layout, COMMERCIAL AT and
					 *   NUMBER SIGN in a French Mac layout on ISO
					 *   keyboards, and LESS-THAN SIGN and GREATER-THAN
					 *   SIGN in a Swiss German, German, or French Mac
					 *   layout on ANSI keyboards.
					 */
		Comma = 54,
		Period = 55,
		Slash = 56,

		CapsLock = 57,

		F1 = 58,
		F2 = 59,
		F3 = 60,
		F4 = 61,
		F5 = 62,
		F6 = 63,
		F7 = 64,
		F8 = 65,
		F9 = 66,
		F10 = 67,
		F11 = 68,
		F12 = 69,

		PrintScreen = 70,
		ScrollLock = 71,
		Pause = 72,
		Insert = 73, /**< insert on PC, help on some Mac keyboards (but
						  does send code 73, not 117) */
		Home = 74,
		PageUp = 75,
		Delete = 76,
		End = 77,
		PageDown = 78,
		Right = 79,
		Left = 80,
		Down = 81,
		Up = 82,

		NumLockClear = 83, /**< num lock on PC, clear on Mac keyboards
							*/
		KeypadpDivide = 84,
		KeypadMultiply = 85,
		KeypadMinus = 86,
		KeypadPlus = 87,
		KeypadEnter = 88,
		KeypadOne = 89,
		KeypadTwo = 90,
		KeypadThree = 91,
		KeypadFour = 92,
		KeypadFive = 93,
		KeypadSix = 94,
		KeypadSeven = 95,
		KeypadEight = 96,
		KeypadNine = 97,
		KeypadZero = 98,
		KeypadPeriod = 99,

		NonUsBackslash = 100, /**< This is the additional key that ISO
							   *   keyboards have over ANSI ones,
							   *   located between left shift and Y.
							   *   Produces GRAVE ACCENT and TILDE in a
							   *   US or UK Mac layout, REVERSE SOLIDUS
							   *   (backslash) and VERTICAL LINE in a
							   *   US or UK Windows layout, and
							   *   LESS-THAN SIGN and GREATER-THAN SIGN
							   *   in a Swiss German, German, or French
							   *   layout. */
		Application = 101,	  /**< windows contextual menu, compose */
		Power = 102,		  /**< The USB document says this is a status flag,
							   *   not a physical key - but some Mac keyboards
							   *   do have a power key. */
		KeypadEquals = 103,
		F13 = 104,
		F14 = 105,
		F15 = 106,
		F16 = 107,
		F17 = 108,
		F18 = 109,
		F19 = 110,
		F20 = 111,
		F21 = 112,
		F22 = 113,
		F23 = 114,
		F24 = 115,
		Execute = 116,
		Help = 117, /**< AL Integrated Help Center */
		Menu = 118, /**< Menu (show menu) */
		Select = 119,
		Stop = 120,	 /**< AC Stop */
		Again = 121, /**< AC Redo/Repeat */
		Undo = 122,	 /**< AC Undo */
		Cut = 123,	 /**< AC Cut */
		Copy = 124,	 /**< AC Copy */
		Paste = 125, /**< AC Paste */
		Find = 126,	 /**< AC Find */
		Mute = 127,
		VolumeUp = 128,
		VolumeDown = 129,
		/* not sure whether there's a reason to enable these */
		/*     LockingCapsLock = 130,  */
		/*     LockingNumLock = 131, */
		/*     LockingScrollLock = 132, */
		KeypadComma = 133,
		KeypadEqualsAsFourHundred = 134,

		InternationalOne = 135, /**< used on Asian keyboards, see
									 footnotes in USB doc */
		InternationalTwo = 136,
		InternationalThree = 137, /**< Yen */
		InternationalFour = 138,
		InternationalFive = 139,
		InternationalSix = 140,
		InternationalSeven = 141,
		InternationalEight = 142,
		InternationalNine = 143,
		LangOne = 144,	 /**< Hangul/English toggle */
		LangTwo = 145,	 /**< Hanja conversion */
		LangThree = 146, /**< Katakana */
		LangFour = 147,	 /**< Hiragana */
		LangFive = 148,	 /**< Zenkaku/Hankaku */
		LangSix = 149,	 /**< reserved */
		LangSeven = 150, /**< reserved */
		LangEight = 151, /**< reserved */
		LangNine = 152,	 /**< reserved */

		AltErase = 153, /**< Erase-Eaze */
		SysReq = 154,
		Cancel = 155, /**< AC Cancel */
		Clear = 156,
		Prior = 157,
		ReturnTwo = 158,
		Separator = 159,
		Out = 160,
		Oper = 161,
		ClearAgain = 162,
		CrSel = 163,
		ExSel = 164,

		KeypadDoubleZero = 176,
		KeypadTripleZero = 177,
		ThousandsSeparator = 178,
		DecimalSeparator = 179,
		CurrencyUnit = 180,
		CurrencySubunit = 181,
		KeypadLeftParen = 182,
		KeypadRightParen = 183,
		KeypadLeftBrace = 184,
		KeypadRightBrace = 185,
		KeypadTab = 186,
		KeypadBackspace = 187,
		KeypadA = 188,
		KeypadB = 189,
		KeypadC = 190,
		KeypadD = 191,
		KeypadE = 192,
		KeypadF = 193,
		KeypadXor = 194,
		KeypadPower = 195,
		KeypadPercent = 196,
		KeypadLess = 197,
		KeypadGreater = 198,
		KeypadAmpersand = 199,
		KeypadDoubleAmpersand = 200,
		KeypadVerticalBar = 201,
		KeypadDoubleVerticalBar = 202,
		KeypadColon = 203,
		KeypadHash = 204,
		KeypadSpace = 205,
		KeypadAt = 206,
		KeypadExclam = 207,
		KeypadMemStore = 208,
		KeypadMemRecall = 209,
		KeypadMemClear = 210,
		KeypadMemAdd = 211,
		KeypadMemSubtract = 212,
		KeypadMemMultiply = 213,
		KeypadMemDivide = 214,
		KeypadPlusMinus = 215,
		KeypadClear = 216,
		KeypadClearEntry = 217,
		KeypadBinary = 218,
		KeypadOctal = 219,
		KeypadDecimal = 220,
		KeypadHexadecimal = 221,

		LeftCtrl = 224,
		LeftShift = 225,
		LeftAlt = 226, /**< alt, option */
		LeftGui = 227, /**< windows, command (apple), meta */
		RightCtrl = 228,
		RightShift = 229,
		RightAlt = 230, /**< alt gr, option */
		RightGui = 231, /**< windows, command (apple), meta */

		Mode = 257, /**< I'm not sure if this is really not covered
					 *   by any of the above, but since there's a
					 *   special KMOD_MODE for it I'm adding it here
					 */

		/* @} */ /* Usage page 0x07 */

		/**
		 *  \name Usage page 0x0C
		 *
		 *  These values are mapped from usage page 0x0C (USB consumer page).
		 *  See https://usb.org/sites/default/files/hut1_2.pdf
		 *
		 *  There are way more keys in the spec than we can represent in the
		 *  current scancode range, so pick the ones that commonly come up in
		 *  real world usage.
		 */
		/* @{ */

		AudioNext = 258,
		AudioPrev = 259,
		AudioStop = 260,
		AudioPlay = 261,
		AudioMute = 262,
		MediaSelect = 263,
		Www = 264, /**< AL Internet Browser */
		Mail = 265,
		Calculator = 266, /**< AL Calculator */
		Computer = 267,
		AcSearch = 268,	   /**< AC Search */
		AcHome = 269,	   /**< AC Home */
		AcBack = 270,	   /**< AC Back */
		AcForward = 271,   /**< AC Forward */
		AcStop = 272,	   /**< AC Stop */
		AcRefresh = 273,   /**< AC Refresh */
		AcBookmarks = 274, /**< AC Bookmarks */

		/* @} */ /* Usage page 0x0C */

		/**
		 *  \name Walther keys
		 *
		 *  These are values that Christian Walther added (for mac keyboard?).
		 */
		/* @{ */

		BrightnessDown = 275,
		BrightnessUp = 276,
		DisplaySwitch = 277, /**< display mirroring/dual display
								  switch, video mode switch */
		KbdIllumToggle = 278,
		KbdIllumDown = 279,
		KbdIllumUp = 280,
		Eject = 281,
		Sleep = 282, /**< SC System Sleep */

		AppOne = 283,
		AppTwo = 284,

		/* @} */ /* Walther keys */

		/**
		 *  \name Usage page 0x0C (additional media keys)
		 *
		 *  These values are mapped from usage page 0x0C (USB consumer page).
		 */
		/* @{ */

		AudioRewind = 285,
		AudioFastForward = 286,

		/* @} */ /* Usage page 0x0C (additional media keys) */

		/**
		 *  \name Mobile keys
		 *
		 *  These are values that are often used on mobile phones.
		 */
		/* @{ */

		SoftLeft = 287,	 /**< Usually situated below the display on phones and
							  used as a multi-function feature key for selecting
							  a software defined function shown on the bottom left
							  of the display. */
		SoftRight = 288, /**< Usually situated below the display on phones and
							  used as a multi-function feature key for selecting
							  a software defined function shown on the bottom right
							  of the display. */
		Call = 289,		 /**< Used for accepting phone calls. */
		EndCall = 290,	 /**< Used for rejecting phone calls. */

		/* @} */ /* Mobile keys */

		/* Add any other keys here. */
	};
}

#define es_maxScancodes 290
#define es_maxMouseButtons 5