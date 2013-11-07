
 -----------------------------------------------------------------------------
				ERISA コンバーター＆ PSD ライブラリ
 -----------------------------------------------------------------------------


■ 始めに
　この度は、「ERISA 画像コンバーター」をダウンロードくださいましてありがとうございます。
　本ソフトウェアは無料で配布しておりますが、著作権は Leshade Entis に帰属していますので無断での再配布はご遠慮ください。
　同梱の PSD ライブラリの利用につきましては、これを利用したソフトウェアの配布の際に何らかの著作権者情報を表示いただければ自由に利用してくださって構いません。


■ 特徴
　ERISA 画像コンバーターは、各種画像ファイルを読み込み、ERI 形式、あるいは指定されたフォーマットに変換して記録することができます。
　読み込みと書き出しには以下のフォーマットに対応しています。

	ERI, MEI  (Entis Rasterized Image format) *
	BMP  (Windows Bitmap) *
	AVI  (Windows AVI file)
	PSD  (Adobe Photoshop data file)
	PNG, TIFF, JPEG 等  (GDI+ を利用) *

　* 印のあるフォーマットは書き出しにも対応しているフォーマットです。
　MEI 形式は、アニメーション ERI 形式の拡張子で、フォーマット自体に違いはありません。（拡張子は ERI でも構いません）
　GDI+ は、WindowsXP 以降であれば問題なく利用できます。
　Windows98,Me, Windows2000 では gdiplus.dll を erisacvt.exe と同じディレクトリに置くことによって利用できるようになります。
　gdiplus.dll はマイクロソフトから無料で配布されています。

	http://www.microsoft.com/downloads/details.aspx?FamilyID=bfc0b436-9015-43e2-81a3-54938b6f4614

　ERI 形式や AVI 形式、あるいは GIF や PNG 形式ではアニメーションの読み込みに対応しています。（現在、GDI+ を使ったアニメーションの書き出しには対応していません）
　また、PSD ファイルでは、レイヤーに対応しています。（レイヤーの透明マスクにも対応していますので、PSD ファイルで見たままの画像を別のフォーマットに変換することができます）
　スクリプトを記述することによって、レイヤー名を列挙することもできます。
　更に、erisacvt では、αチャネルへの対応、更に、透明部分を除く不透明部分のみの自動切り出し、および切り出し結果の出力機能も有しています。
　この機能によって、PSD ファイルから画面パーツを切り出す処理などをほとんど自動化できます。（切り出した画像を並べて1つの画像にまとめることもできます）


■ 使い方
　erisacvt /? で書式が表示できます。
　erisacvt /help で処理ファイルの書式を表示できます。
　以下にコマンドラインの例を示します。

> erisacvt sample.psd sample.eri
　sample.psd ファイルを sample.eri ファイルに変換します。

> erisacvt sample.avi sample.mei
　sample.avi ファイルを sample.mei ファイルに変換します。

> erisacvt /mime:image/png sample.eri sample.png
　sample.eri を sample.png ファイルに変換します。

> erisacvt /l sample.xml /r report.xml /o output.txt
　sample.xml に記述された処理ファイルの内容にしたがって処理を行います。
　画像の切り出しが行われた場合には、report.xml に切り出された座標が出力されます。
　output.txt にはスクリプトが output() 関数で出力した結果が書き出されます。（普通のテキストエディタで開くには、改行コードは "\r\n" で出力するようにしたほうが良いでしょう）


■ 処理ファイル例
　単に src ディレクトリにあるファイルを dst ディレクトリに ERI 形式に変換して出力する処理は以下のようになります。

<?xml version="1.0" encoding="Shift_JIS"?>
<erisa>
	<env srcdir="src" dstdir="dst"/>
	<file src="*.*" dst=".\"/>
</erisa>


　各 PSD ファイルの各レイヤーを取り出して、それを横に並べた1枚の画像ファイルに変換して出力する処理は以下のようになります。

<?xml version="1.0" encoding="Shift_JIS"?>
<erisa>
	<env srcdir="src" dstdir="dst"/>
	<file src="*.psd" dst=".\">
		<arrange way="horz"/>
	</file>
</erisa>


　但し、この場合、PSD のレイヤー矩形情報が有効なので（レイヤー毎に切り出されたサイズで並べられるので）同じサイズの画像として並べたい場合には次のようにします。

<?xml version="1.0" encoding="Shift_JIS"?>
<erisa>
	<env srcdir="src" dstdir="dst" cut_align="1"/>
	<file src="*.psd" dst=".\">
		<cut/>
		<arrange way="horz"/>
	</file>
</erisa>

　この処理の違いは、実際に複数のレイヤーからなる PSD ファイルを変換してみれば分かるかと思います。
（因みに、ERI ファイルを Photoshop から読み込むには、同梱の bin フォルダ内の erichan.8bi ファイルを Photoshop がインストールしてあるディレクトリの "Plug-Ins\File Format" フォルダにコピーしてください）

　PSD ファイルの各レイヤーを連番で出力するのは次のような処理になります。

<?xml version="1.0" encoding="Shift_JIS"?>
<erisa>
	<env srcdir="src" dstdir="dst"/>
	<file src="*.psd">
		<write dst="*_%%:0,1"/>
	</file>
</erisa>

　こうすると、元の PSD ファイル名の後ろに _00, _01,... が付加された形式で連番ファイルが生成されます。

　因みに、これと同じ様な処理をスクリプトを使って実現すると次のようになります。

<?xml version="1.0" encoding="Shift_JIS"?>
<erisa>
	<env srcdir="src" dstdir="dst"/>
	<cotopha><!-- 詞葉スクリプト♪
		function sample_func( reference imgctx )
			;
			; ファイルタイトルの抽出
			string	file_title := imgctx.filename
			integer	title_len := file_title.Find( "." )
			if	title_len >= 0
				file_title := file_title.Left( title_len )
			endif
			;
			; ファイルタイトルの抽出は、あるいは
			;	Array	aParam
			;	if file_title.IsMatchUsage( "(<-.>*)", aParam ) == ""
			;		file_title := aParam[0]
			;	endif
			; でも構いません
			;
			; レイヤー毎の書き出し
			for layer_img | imgctx[i]
				ImageContext	ictxSub
				print( "レイヤー ", i, ' : "', layer_img.name, '"\n' )
				;
				; レイヤーの抽出
				ictxSub.merge( imgctx, i, 1 )
				;
				; merge 関数の代わりに
				;	ictxSub.size := imgctx.size
				;	ictxSub[0] := Image( layer_img )
				; としても同じです
				;
				ictxSub.save( file_title + "_" + string(i) )
			next
			imgctx.Remove()
		endfunc
	--></cotopha>
	<file src="*.psd" script="sample_func"/>
</erisa>

　無意味に煩雑のようにも見えるかもしれませんが、レイヤー名などから処理の内容を決定したりするような、自動処理を行う汎用的な処理ファイルを記述するのに、スクリプトが有効なことがあります。（また、切り出し座標などを任意の書式でファイルに出力しておいて、別のプログラムで処理させることも可能です）
　詞葉スクリプトの書式や仕様に関しては、EntisGLS に同梱されている「詞葉スクリプト仕様書.doc」を参照してください。


■ PSD 読み込みライブラリについて補足
　同梱のPSD 読み込みライブラリを利用するには、EntisGLS が必要となりますが、大したことはやっていないので、別のライブラリ用に簡単に書き換えられることでしょう。


■ 最後に
　ご意見ご質問などお気軽にお寄せください。

	E-mail : leshade@entis.jp
	URL    : http://www.entis.jp/

