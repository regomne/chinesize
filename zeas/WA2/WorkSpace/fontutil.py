import sys,ctypes,struct
from PIL import Image, ImageMath

mat2 = '\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00'
def createFont(FontSize, FontFace):
	hFont = ctypes.windll.gdi32.CreateFontA(FontSize, 0, 0, 0, 500, 0, 0, 0, 0x86,\
	                  0,                 0,                  3,       2             |0, FontFace.encode('gbk'));
	                 #OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,iQuality,VARIABLE_PITCH|FF_DONTCARE
	hDC = ctypes.windll.gdi32.CreateCompatibleDC(ctypes.windll.user32.GetDC(0))
	ctypes.windll.gdi32.SelectObject(hDC, hFont)
	tm = '\0'*0x38
	ctypes.windll.gdi32.GetTextMetricsA(hDC, tm)
	tmAscent = struct.unpack('I', tm[4:8])[0]
	return hDC, tmAscent

def applyOutline(a, o):
	w, h = a.size
	c = a.crop((o, o, w-o, h-o))
	t = Image.new('L',a.size)
	for n in ((o,0),(0,o),(o+o,o),(o,o+o)):
		t.paste(c,n)
		a = ImageMath.eval("convert(max(a,b),'L')",a=a,b=t)
	for n in ((0,0),(o+o,0),(0,o+o),(o+o,o+o)):
		t.paste(c,n)
		a = ImageMath.eval("convert(max(a,b/1.414),'L')",a=a,b=t)
	return a

def RenderFont(char, hdc, BlockSize, tmAscent, doubleBrightness, offx=0, offy=0):
    lpgm = '\0'*20
    char = ord(char)
    cbBuffer = ctypes.windll.gdi32.GetGlyphOutlineW(hdc, char, 6, lpgm, 0, 0, mat2)
    buffer = '\0'*cbBuffer+'\0'*16
    ctypes.windll.gdi32.GetGlyphOutlineW(hdc, char, 6, lpgm, cbBuffer, buffer, mat2)
    BoxX, BoxY, X, Y, CellIncX, CellIncY = struct.unpack('iiiihh', lpgm)
    Y = tmAscent - Y
    CharBlock = Image.fromstring('L', ((BoxX+3)&~3, BoxY), buffer.translate(doubleBrightness))
    AlphaBlock = Image.new('L', (BlockSize, BlockSize))
    ColorBlock = Image.new('L', (BlockSize, BlockSize), 255)
    AlphaBlock.paste(CharBlock, (X+offx, Y+offy))
    return Image.merge('RGBA', (ColorBlock,ColorBlock,ColorBlock,AlphaBlock))