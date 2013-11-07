from fontTools.ttLib import TTFont

ft=TTFont()
ft.importXML('mynew.xml')
ft.save('test.ttf')
