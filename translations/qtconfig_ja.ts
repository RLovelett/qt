<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="ja_JP">
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+88"/>
        <source>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Appearance&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;Use this tab to customize the appearance of your Qt applications.&lt;/p&gt;&lt;p&gt;You can select the default GUI Style from the drop down list and customize the colors.&lt;/p&gt;&lt;p&gt;Any GUI Style plugins in your plugin path will automatically be added to the list of built-in Qt styles. (See the Library Paths tab for information on adding new plugin paths.)&lt;/p&gt;&lt;p&gt;When you choose 3-D Effects and Window Background colors, the Qt Configuration program will automatically generate a palette for you. To customize colors further, press the Tune Palette button to open the advanced palette editor.&lt;p&gt;The Preview Window shows what the selected Style and colors look like.</source>
        <translation>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;外観&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;このタブでは Qt アプリケーションの外観をカスタマイズします。&lt;/p&gt;&lt;p&gt;ドロップダウンリストからデフォルトの GUI スタイルを選択して、その色をカスタマイズできます。&lt;/p&gt;&lt;p&gt;プラグインパスにある全てのプラグインが自動的にQt のビルトインスタイルのリストに追加されます。 (See the Library Paths tab for information on adding new plugin paths.)&lt;/p&gt;&lt;p&gt;3D 効果とウィンドウの背景色を選択した場合、Qt 設定ツールは自動的にパレットを生成します。さらに色をカスタマイズするには、パレットの調整ボタンを押して拡張パレットエディタを開いてください。&lt;p&gt;プレビューウィンドウでは選択したスタイルと色がどのように見えるかを確認できます。</translation>
    </message>
    <message>
        <location line="+16"/>
        <source>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Fonts&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;Use this tab to select the default font for your Qt applications. The selected font is shown (initially as &apos;Sample Text&apos;) in the line edit below the Family, Style and Point Size drop down lists.&lt;/p&gt;&lt;p&gt;Qt has a powerful font substitution feature that allows you to specify a list of substitute fonts.  Substitute fonts are used when a font cannot be loaded, or if the specified font doesn&apos;t have a particular character.&lt;p&gt;For example, if you select the font Lucida, which doesn&apos;t have Korean characters, but need to show some Korean text using the Mincho font family you can do so by adding Mincho to the list. Once Mincho is added, any Korean characters that are not found in the Lucida font will be taken from the Mincho font.  Because the font substitutions are lists, you can also select multiple families, such as Song Ti (for use with Chinese text).</source>
        <translation>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;フォント&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;このタブでは Qt アプリケーションのデフォルトフォントの設定を行います。選択したフォントはファミリー、スタイル、ポイントサイズのドロップダウンリストの下にあるラインエディタに(初期値は「テキストのサンプル」で)表示されます。&lt;/p&gt;&lt;p&gt;Qt には強力なフォント置換機能があり、置換するフォントのリストを指定することができます。置換するフォントはフォントが読み込めない場合や指定したフォントが特定の文字を持たない場合に使用されます。&lt;/p&gt;&lt;p&gt;例えば、漢字を持たない Lucida フォントを選択した場合に、リストに明朝体のフォントを追加すれば漢字からなる文字列を明朝体で表示することができます。明朝体のフォントが追加してある場合、Lucida フォントに含まれない全ての漢字が明朝体のフォントから用いられます。フォントの置換機能では、ハングル等の複数のファミリーを指定することができます。</translation>
    </message>
    <message>
        <location line="+19"/>
        <source>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Interface&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;Use this tab to customize the feel of your Qt applications.&lt;/p&gt;&lt;p&gt;If the Resolve Symlinks checkbox is checked Qt will follow symlinks when handling URLs. For example, in the file dialog, if this setting is turned on and /usr/tmp is a symlink to /var/tmp, entering the /usr/tmp directory will cause the file dialog to change to /var/tmp.  With this setting turned off, symlinks are not resolved or followed.&lt;/p&gt;&lt;p&gt;The Global Strut setting is useful for people who require a minimum size for all widgets (e.g. when using a touch panel or for users who are visually impaired).  Leaving the Global Strut width and height at 0 will disable the Global Strut feature&lt;/p&gt;&lt;p&gt;XIM (Extended Input Methods) are used for entering characters in languages that have large character sets, for example, Chinese and Japanese.</source>
        <translation>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;インターフェース&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;このタブでは Qt アプリケーションの操作感をカスタマイズします。&lt;/p&gt;&lt;p&gt;「URL でのシンボリックリンクを解決」チェックボックスがチェックされている場合、Qt が URL を処理する際にシンボリックリンクを追跡します。例えば、この設定がオンで /usr/tmp が /var/tmp へのシンボリックリンクの場合、ファイルダイアログで /usr/tmp を表示しようとすると /var/tmp へ移動します。この設定がオフの場合、シンボリックリンクの解決や追跡は行われません。&lt;/p&gt;&lt;p&gt;「グローバルな設定」は全てのウィジェットに最小サイズが必要な場合(例えば、タッチパネルを使用していたり、目の不自由な方々など)に有用です。幅の最小値と高さの最小値を0にするとこの機能が無効になります。&lt;/p&gt;&lt;p&gt;XIM (インプットメソッド) は中国語や日本語のような、多数の文字を持つ言語で文字の入力に用いられています。</translation>
    </message>
    <message>
        <location line="+18"/>
        <source>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Printer&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;Use this tab to configure the way Qt generates output for the printer.You can specify if Qt should try to embed fonts into its generated output.If you enable font embedding, the resulting postscript will be more portable and will more accurately reflect the visual output on the screen; however the resulting postscript file size will be bigger.&lt;p&gt;When using font embedding you can select additional directories where Qt should search for embeddable font files.  By default, the X server font path is used.</source>
        <translation>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;プリンタ&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;このタブでは Qt が印刷する際に生成する出力の設定を行います。Qt が生成された印刷物にフォントを埋め込もうとするかどうかを指定できます。フォントを埋め込む場合、生成された PostScript の可搬性が増し、その見た目は画面上の表示をより正確に反映したものとなります。その反面、生成された PostScript ファイルのサイズが大きくなります。&lt;p&gt;フォントを埋め込む場合、Qt が埋め込むフォントファイルを検索するディレクトリを追加で選択することができます。デフォルトでは X サーバーのフォントパスが使われます。</translation>
    </message>
    <message>
        <location line="+13"/>
        <source>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Phonon&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;Use this tab to configure the Phonon GStreamer multimedia backend. &lt;p&gt;It is reccommended to leave all settings on &quot;Auto&quot; to let Phonon determine your settings automatically.</source>
        <translation>&lt;p&gt;&lt;b&gt;&lt;font size+=2&gt;Phonon&lt;/font&gt;&lt;/b&gt;&lt;/p&gt;&lt;hr&gt;&lt;p&gt;このタブでは Phonon の GStreamer マルチメディアバックエンドの設定を行います。&lt;p&gt;全ての設定を &quot;自動&quot; にして Phonon に自動的に設定させることを推奨します。</translation>
    </message>
    <message>
        <location line="+83"/>
        <source>Desktop Settings (Default)</source>
        <translation>デスクトップの設定(デフォルト)</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Choose style and palette based on your desktop settings.</source>
        <translation>デスクトップの設定に応じてスタイルとパレットを選択します。</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.ui"/>
        <source>On The Spot</source>
        <translation>On The Spot</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+16"/>
        <source>Unknown</source>
        <translation>不明</translation>
    </message>
    <message>
        <location line="+166"/>
        <location line="+1"/>
        <location line="+43"/>
        <location line="+1"/>
        <source>Auto (default)</source>
        <translation>自動(デフォルト)</translation>
    </message>
    <message>
        <location line="-43"/>
        <source>Choose audio output automatically.</source>
        <translation>オーディオ出力を自動的に選択します。</translation>
    </message>
    <message>
        <location line="+1"/>
        <location line="+1"/>
        <source>aRts</source>
        <translation>aRts</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Experimental aRts support for GStreamer.</source>
        <translation>Gstreamer での aRts のサポート(Experimental)。</translation>
    </message>
    <message>
        <location line="+36"/>
        <source>Phonon GStreamer backend not available.</source>
        <translation>Phonon の GStreamer バックエンドが見つかりません。</translation>
    </message>
    <message>
        <location line="+5"/>
        <source>Choose render method automatically</source>
        <translation>描画メソッドを自動的に選択します</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <source>X11</source>
        <translation>X11</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use X11 Overlays</source>
        <translation>X11 のオーバーレイを使用</translation>
    </message>
    <message>
        <location line="+3"/>
        <location line="+1"/>
        <source>OpenGL</source>
        <translation>OpenGL</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use OpenGL if available</source>
        <translation>OpenGL が使用可能な場合は使用</translation>
    </message>
    <message>
        <location line="+2"/>
        <location line="+1"/>
        <source>Software</source>
        <translation>ソフトウェア</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Use simple software rendering</source>
        <translation>ソフトウェアでの描画を使用</translation>
    </message>
    <message>
        <location line="+27"/>
        <source>No changes to be saved.</source>
        <translation>内容が変更されていません。</translation>
    </message>
    <message>
        <location line="+4"/>
        <source>Saving changes...</source>
        <translation>保存中…</translation>
    </message>
    <message>
        <location line="+111"/>
        <source>Saved changes.</source>
        <translation>変更を保存しました。</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.ui"/>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="-61"/>
        <source>Over The Spot</source>
        <translation>Over The Spot</translation>
    </message>
    <message>
        <location/>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+2"/>
        <source>Off The Spot</source>
        <translation>Off The Spot</translation>
    </message>
    <message>
        <location/>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+2"/>
        <source>Root</source>
        <translation>Root</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+324"/>
        <source>Select a Directory</source>
        <translation>ディレクトリの選択</translation>
    </message>
    <message>
        <location line="+15"/>
        <source>&lt;h3&gt;%1&lt;/h3&gt;&lt;br/&gt;Version %2&lt;br/&gt;&lt;br/&gt;Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).</source>
        <translation>&lt;h3&gt;%1&lt;/h3&gt;&lt;br/&gt;バージョン %2&lt;br/&gt;&lt;br/&gt;Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.ui"/>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+3"/>
        <location line="+1"/>
        <location line="+7"/>
        <source>Qt Configuration</source>
        <translation>Qt 設定ツール</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.cpp" line="+21"/>
        <source>Save Changes</source>
        <translation>変更の保存</translation>
    </message>
    <message>
        <location line="+1"/>
        <source>Save changes to settings?</source>
        <translation>変更内容を設定に保存しますか？</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/mainwindow.ui"/>
        <source>Appearance</source>
        <translation>外観</translation>
    </message>
    <message>
        <location/>
        <source>GUI Style</source>
        <translation>GUI スタイル</translation>
    </message>
    <message>
        <location/>
        <source>Select GUI &amp;Style:</source>
        <translation>GUI スタイルの選択(&amp;S):</translation>
    </message>
    <message>
        <location/>
        <source>Preview</source>
        <translation>プレビュー</translation>
    </message>
    <message>
        <location/>
        <source>Select &amp;Palette:</source>
        <translation>パレットの選択(&amp;P):</translation>
    </message>
    <message>
        <location/>
        <source>Active Palette</source>
        <translation>アクティブなパレット</translation>
    </message>
    <message>
        <location/>
        <source>Inactive Palette</source>
        <translation>アクティブではないパレット</translation>
    </message>
    <message>
        <location/>
        <source>Disabled Palette</source>
        <translation>無効時のパレット</translation>
    </message>
    <message>
        <location/>
        <source>Build Palette</source>
        <translation>パレットの設定</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Button Background:</source>
        <translation>ボタンのバックグラウンド(&amp;B):</translation>
    </message>
    <message>
        <location/>
        <source>Window Back&amp;ground:</source>
        <translation>ウィンドウのバックグラウンド(&amp;G):</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Tune Palette...</source>
        <translation>パレットの調整(&amp;T)...</translation>
    </message>
    <message>
        <location/>
        <source>Please use the KDE Control Center to set the palette.</source>
        <translation>KDE のシステム設定でパレットの設定をしてください。</translation>
    </message>
    <message>
        <location/>
        <source>Fonts</source>
        <translation>フォント</translation>
    </message>
    <message>
        <location/>
        <source>Default Font</source>
        <translation>デフォルトのフォント</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Style:</source>
        <translation>スタイル(&amp;S):</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Point Size:</source>
        <translation>ポイントサイズ(&amp;P):</translation>
    </message>
    <message>
        <location/>
        <source>F&amp;amily:</source>
        <translation>ファミリー(&amp;F):</translation>
    </message>
    <message>
        <location/>
        <source>Sample Text</source>
        <translation>テキストのサンプル</translation>
    </message>
    <message>
        <location/>
        <source>Font Substitution</source>
        <translation>フォントの置換</translation>
    </message>
    <message>
        <location/>
        <source>S&amp;elect or Enter a Family:</source>
        <translation>ファミリーを選択するか入力してください(&amp;E):</translation>
    </message>
    <message>
        <location/>
        <source>Current Substitutions:</source>
        <translation>現在の置換定義:</translation>
    </message>
    <message>
        <location/>
        <source>Up</source>
        <translation>上へ</translation>
    </message>
    <message>
        <location/>
        <source>Down</source>
        <translation>下へ</translation>
    </message>
    <message>
        <location/>
        <source>Remove</source>
        <translation>削除</translation>
    </message>
    <message>
        <location/>
        <source>Select s&amp;ubstitute Family:</source>
        <translation>置換するファミリーの選択(&amp;U):</translation>
    </message>
    <message>
        <location/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location/>
        <source>Interface</source>
        <translation>インターフェース</translation>
    </message>
    <message>
        <location/>
        <source>Feel Settings</source>
        <translation>一般的な設定</translation>
    </message>
    <message>
        <location/>
        <source> ms</source>
        <translation>ミリ秒</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Double Click Interval:</source>
        <translation>ダブルクリックの間隔(&amp;D):</translation>
    </message>
    <message>
        <location/>
        <source>No blinking</source>
        <translation>点滅しない</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Cursor Flash Time:</source>
        <translation>カーソルの点滅の間隔(&amp;C):</translation>
    </message>
    <message>
        <location/>
        <source> lines</source>
        <translation>行</translation>
    </message>
    <message>
        <location/>
        <source>Wheel &amp;Scroll Lines:</source>
        <translation>マウスホイールでのスクロール行数(&amp;S):</translation>
    </message>
    <message>
        <location/>
        <source>Resolve symlinks in URLs</source>
        <translation>URL でのシンボリックリンクを解決</translation>
    </message>
    <message>
        <location/>
        <source>GUI Effects</source>
        <translation>GUI のエフェクト</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Enable</source>
        <translation>有効(&amp;E)</translation>
    </message>
    <message>
        <location/>
        <source>Alt+E</source>
        <translation>Alt+E</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Menu Effect:</source>
        <translation>メニューのエフェクト(&amp;E):</translation>
    </message>
    <message>
        <location/>
        <source>C&amp;omboBox Effect:</source>
        <translation>コンボボックスのエフェクト(&amp;O):</translation>
    </message>
    <message>
        <location/>
        <source>&amp;ToolTip Effect:</source>
        <translation>ツールチップのエフェクト(&amp;T):</translation>
    </message>
    <message>
        <location/>
        <source>Tool&amp;Box Effect:</source>
        <translation>ツールボックスのエフェクト(&amp;B):</translation>
    </message>
    <message>
        <location/>
        <source>Disable</source>
        <translation>無効</translation>
    </message>
    <message>
        <location/>
        <source>Animate</source>
        <translation>アニメーション</translation>
    </message>
    <message>
        <location/>
        <source>Fade</source>
        <translation>フェード</translation>
    </message>
    <message>
        <location/>
        <source>Global Strut</source>
        <translation>グローバルな設定</translation>
    </message>
    <message>
        <location/>
        <source>Minimum &amp;Width:</source>
        <translation>幅の最小値(&amp;W):</translation>
    </message>
    <message>
        <location/>
        <source>Minimum Hei&amp;ght:</source>
        <translation>高さの最小値(&amp;G):</translation>
    </message>
    <message>
        <location/>
        <source> pixels</source>
        <translation>ピクセル</translation>
    </message>
    <message>
        <location/>
        <source>Enhanced support for languages written right-to-left</source>
        <translation>右から左に書かれる言語のサポートの強化</translation>
    </message>
    <message>
        <location/>
        <source>XIM Input Style:</source>
        <translation>XIM のインプットスタイル:</translation>
    </message>
    <message>
        <location/>
        <source>Default Input Method:</source>
        <translation>デフォルトのインプットメソッド:</translation>
    </message>
    <message>
        <location/>
        <source>Printer</source>
        <translation>プリンタ</translation>
    </message>
    <message>
        <location/>
        <source>Enable Font embedding</source>
        <translation>フォントを埋め込む</translation>
    </message>
    <message>
        <location/>
        <source>Font Paths</source>
        <translation>フォントのパス</translation>
    </message>
    <message>
        <location/>
        <source>Browse...</source>
        <translation>参照...</translation>
    </message>
    <message>
        <location/>
        <source>Press the &lt;b&gt;Browse&lt;/b&gt; button or enter a directory and press Enter to add them to the list.</source>
        <translation>&lt;b&gt;ブラウズ&lt;/b&gt;ボタンを押すかディレクトリを入力し、エンターキーか追加ボタンでリストに追加してください。</translation>
    </message>
    <message>
        <location/>
        <source>Phonon</source>
        <translation>Phonon</translation>
    </message>
    <message>
        <location/>
        <source>About Phonon</source>
        <translation>Phonon について</translation>
    </message>
    <message>
        <location/>
        <source>Current Version:</source>
        <translation>現在のバージョン:</translation>
    </message>
    <message>
        <location/>
        <source>Not available</source>
        <translation>使用不可能</translation>
    </message>
    <message>
        <location/>
        <source>Website:</source>
        <translation>ウェブサイト:</translation>
    </message>
    <message>
        <location/>
        <source>&lt;a href=&quot;http://phonon.kde.org&quot;&gt;http://phonon.kde.org/&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;http://phonon.kde.org&quot;&gt;http://phonon.kde.org/&lt;/a&gt;</translation>
    </message>
    <message>
        <location/>
        <source>About GStreamer</source>
        <translation>GStreamer について</translation>
    </message>
    <message>
        <location/>
        <source>&lt;a href=&quot;http://gstreamer.freedesktop.org/&quot;&gt;http://gstreamer.freedesktop.org/&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;http://gstreamer.freedesktop.org/&quot;&gt;http://gstreamer.freedesktop.org/&lt;/a&gt;</translation>
    </message>
    <message>
        <location/>
        <source>GStreamer backend settings</source>
        <translation>GStreamer のバックエンドの設定</translation>
    </message>
    <message>
        <location/>
        <source>Preferred audio sink:</source>
        <translation>優先するオーディオシンク:</translation>
    </message>
    <message>
        <location/>
        <source>Preferred render method:</source>
        <translation>優先する描画方法:</translation>
    </message>
    <message>
        <location/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-style:italic;&quot;&gt;Note: changes to these settings may prevent applications from starting up correctly.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-style:italic;&quot;&gt;注: これらの設定の変更によりアプリケーションが正常に起動しなくなる場合があります。&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location/>
        <source>&amp;File</source>
        <translation>ファイル(&amp;F)</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Help</source>
        <translation>ヘルプ(&amp;H)</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Save</source>
        <translation>保存(&amp;S)</translation>
    </message>
    <message>
        <location/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location/>
        <source>Ctrl+S</source>
        <translation>Ctrl+S</translation>
    </message>
    <message>
        <location/>
        <source>E&amp;xit</source>
        <translation>終了(&amp;X)</translation>
    </message>
    <message>
        <location/>
        <source>Exit</source>
        <translation>終了</translation>
    </message>
    <message>
        <location/>
        <source>Ctrl+Q</source>
        <translation>Ctrl+Q</translation>
    </message>
    <message>
        <location/>
        <source>&amp;About</source>
        <translation>Qt 設定ツールについて(&amp;A)</translation>
    </message>
    <message>
        <location/>
        <source>About</source>
        <translation>Qt 設定ツールについて</translation>
    </message>
    <message>
        <location/>
        <source>About &amp;Qt</source>
        <translation>&amp;Qt について</translation>
    </message>
    <message>
        <location/>
        <source>About Qt</source>
        <translation>Qt について</translation>
    </message>
</context>
<context>
    <name>PaletteEditorAdvanced</name>
    <message>
        <location filename="../tools/qtconfig/paletteeditoradvanced.ui"/>
        <source>Tune Palette</source>
        <translation>パレットの調整</translation>
    </message>
    <message>
        <location/>
        <source>Select &amp;Palette:</source>
        <translation>パレットの選択(&amp;P):</translation>
    </message>
    <message>
        <location/>
        <source>Active Palette</source>
        <translation>アクティブなパレット</translation>
    </message>
    <message>
        <location/>
        <source>Inactive Palette</source>
        <translation>アクティブではないパレット</translation>
    </message>
    <message>
        <location/>
        <source>Disabled Palette</source>
        <translation>無効時のパレット</translation>
    </message>
    <message>
        <location/>
        <source>Auto</source>
        <translation>自動</translation>
    </message>
    <message>
        <location/>
        <source>Build inactive palette from active</source>
        <translation>アクティブなパレットからアクティブではないパレットを構築</translation>
    </message>
    <message>
        <location/>
        <source>Build disabled palette from active</source>
        <translation>アクティブなパレットから無効時のパレットを構築</translation>
    </message>
    <message>
        <location/>
        <source>Central color &amp;roles</source>
        <translation>中央部の色の役割(&amp;R)</translation>
    </message>
    <message>
        <location/>
        <source>Choose central color role</source>
        <translation>中央部の色の役割を選択してください</translation>
    </message>
    <message>
        <location/>
        <source>&lt;b&gt;Select a color role.&lt;/b&gt;&lt;p&gt;Available central roles are: &lt;ul&gt; &lt;li&gt;Window - general background color.&lt;/li&gt; &lt;li&gt;WindowText - general foreground color. &lt;/li&gt; &lt;li&gt;Base - used as background color for e.g. text entry widgets, usually white or another light color. &lt;/li&gt; &lt;li&gt;Text - the foreground color used with Base. Usually this is the same as WindowText, in what case it must provide good contrast both with Window and Base. &lt;/li&gt; &lt;li&gt;Button - general button background color, where buttons need a background different from Window, as in the Macintosh style. &lt;/li&gt; &lt;li&gt;ButtonText - a foreground color used with the Button color. &lt;/li&gt; &lt;li&gt;Highlight - a color to indicate a selected or highlighted item. &lt;/li&gt; &lt;li&gt;HighlightedText - a text color that contrasts to Highlight. &lt;/li&gt; &lt;li&gt;BrightText - a text color that is very different from WindowText and contrasts well with e.g. black. &lt;/li&gt; &lt;/ul&gt; &lt;/p&gt;</source>
        <translation>&lt;b&gt;色の役割の選択。&lt;/b&gt;&lt;p&gt;選択可能な中央部の役割: &lt;ul&gt; &lt;li&gt;ウィンドウ - 一般的な背景色。&lt;/li&gt; &lt;li&gt;ウィンドウテキスト - 一般的な前景色。 &lt;/li&gt; &lt;li&gt;ベース - テキスト入力ウィジェット等の背景色。通常は白などの明るい色。 &lt;/li&gt; &lt;li&gt;テキスト - ベースと共に用いられる前景色。通常はウィンドウテキストと同じだが、ウィンドウとベースに対してよりよいコントラストが必要な場合に使用される。 &lt;/li&gt; &lt;li&gt;ボタン - 一般的なボタンの背景色。Macintosh スタイルのようにウィンドウと異なる背景色を使用する場合に使用される。 &lt;/li&gt; &lt;li&gt;ボタンテキスト - ボタンと共に用いられる前景色 &lt;/li&gt; &lt;li&gt;ハイライト - アイテムが選択されていたり、ハイライトされていることを示すために使用される色。 &lt;/li&gt; &lt;li&gt;ハイライトのテキスト - ハイライト時のテキストの色。 &lt;/li&gt; &lt;li&gt;明るいテキスト - 黒などの背景色に対して使用されるテキストの色。ウィンドウテキストの反対の色。 &lt;/li&gt; &lt;/ul&gt; &lt;/p&gt;</translation>
    </message>
    <message>
        <location/>
        <source>Window</source>
        <translation>ウィンドウ</translation>
    </message>
    <message>
        <location/>
        <source>WindowText</source>
        <translation>ウィンドウテキスト</translation>
    </message>
    <message>
        <location/>
        <source>Base</source>
        <translation>ベース</translation>
    </message>
    <message>
        <location/>
        <source>AlternateBase</source>
        <translation>その他のベース</translation>
    </message>
    <message>
        <location/>
        <source>ToolTipBase</source>
        <translation>ツールチップのベース</translation>
    </message>
    <message>
        <location/>
        <source>ToolTipText</source>
        <translation>ツールチップのテキスト</translation>
    </message>
    <message>
        <location/>
        <source>Text</source>
        <translation>テキスト</translation>
    </message>
    <message>
        <location/>
        <source>Button</source>
        <translation>ボタン</translation>
    </message>
    <message>
        <location/>
        <source>ButtonText</source>
        <translation>ボタンのテキスト</translation>
    </message>
    <message>
        <location/>
        <source>BrightText</source>
        <translation>明るいテキスト</translation>
    </message>
    <message>
        <location/>
        <source>Highlight</source>
        <translation>ハイライト</translation>
    </message>
    <message>
        <location/>
        <source>HighlightedText</source>
        <translation>ハイライトされたテキスト</translation>
    </message>
    <message>
        <location/>
        <source>Link</source>
        <translation>リンク</translation>
    </message>
    <message>
        <location/>
        <source>LinkVisited</source>
        <translation>訪問済みのリンク</translation>
    </message>
    <message>
        <location/>
        <source>&amp;Select Color:</source>
        <translation>色の選択(&amp;S):</translation>
    </message>
    <message>
        <location/>
        <source>3-D shadow &amp;effects</source>
        <translation>3Dの影の効果(&amp;E)</translation>
    </message>
    <message>
        <location/>
        <source>Generate shadings</source>
        <translation>影を生成する</translation>
    </message>
    <message>
        <location/>
        <source>Check to let 3D-effect colors be calculated from button-color.</source>
        <translation>3D効果の色をボタンの色から計算するにはチェックしてください。</translation>
    </message>
    <message>
        <location/>
        <source>Build &amp;from button color</source>
        <translation>ボタンの色から構築する(&amp;F)</translation>
    </message>
    <message>
        <location/>
        <source>Choose 3D-effect color role</source>
        <translation>3D効果の色の役割の選択</translation>
    </message>
    <message>
        <location/>
        <source>&lt;b&gt;Select a color role.&lt;/b&gt;&lt;p&gt;Available effect roles are: &lt;ul&gt; &lt;li&gt;Light - lighter than Button color. &lt;/li&gt; &lt;li&gt;Midlight - between Button and Light. &lt;/li&gt; &lt;li&gt;Mid - between Button and Dark. &lt;/li&gt; &lt;li&gt;Dark - darker than Button. &lt;/li&gt; &lt;li&gt;Shadow - a very dark color. &lt;/li&gt; &lt;/ul&gt;</source>
        <translation>&lt;b&gt;色の役割の選択。&lt;/b&gt;&lt;p&gt;選択可能な中央部の役割: &lt;ul&gt; &lt;li&gt;ライト - ボタンの色より明るい。 &lt;/li&gt; &lt;li&gt;ミッドライト - ボタンとライトの間。 &lt;/li&gt; &lt;li&gt;ミッド - ボタンとダークの間。 &lt;/li&gt; &lt;li&gt;ダーク - ボタンより暗い。 &lt;/li&gt; &lt;li&gt;シャドウ - とても暗い色。 &lt;/li&gt; &lt;/ul&gt;</translation>
    </message>
    <message>
        <location/>
        <source>Light</source>
        <translation>ライト</translation>
    </message>
    <message>
        <location/>
        <source>Midlight</source>
        <translation>ミッドライト</translation>
    </message>
    <message>
        <location/>
        <source>Mid</source>
        <translation>ミッド</translation>
    </message>
    <message>
        <location/>
        <source>Dark</source>
        <translation>ダーク</translation>
    </message>
    <message>
        <location/>
        <source>Shadow</source>
        <translation>シャドウ</translation>
    </message>
    <message>
        <location/>
        <source>Select Co&amp;lor:</source>
        <translation>色の選択(&amp;L):</translation>
    </message>
    <message>
        <location filename="../tools/qtconfig/paletteeditoradvanced.cpp" line="+54"/>
        <location line="+6"/>
        <source>Choose a color</source>
        <translation>色を選択してください</translation>
    </message>
    <message>
        <location line="-5"/>
        <source>Choose a color for the selected central color role.</source>
        <translation>選択された中央部の色の役割に使用する色を選択してください。</translation>
    </message>
    <message>
        <location line="+6"/>
        <source>Choose a color for the selected effect color role.</source>
        <translation>選択の効果の色の役割に使用する色を選択してください。</translation>
    </message>
</context>
<context>
    <name>PreviewFrame</name>
    <message>
        <location filename="../tools/qtconfig/previewframe.cpp" line="+82"/>
        <source>Desktop settings will only take effect after an application restart.</source>
        <translation>デスクトップの設定はアプリケーションの再起動後に反映されます。</translation>
    </message>
</context>
<context>
    <name>PreviewWidget</name>
    <message>
        <location filename="../tools/qtconfig/previewwidget.ui"/>
        <source>Preview Window</source>
        <translation>プレビューウィンドウ</translation>
    </message>
    <message>
        <location/>
        <source>GroupBox</source>
        <translation>グループボックス</translation>
    </message>
    <message>
        <location/>
        <source>RadioButton1</source>
        <translation>ラジオボタン1</translation>
    </message>
    <message>
        <location/>
        <source>RadioButton2</source>
        <translation>ラジオボタン2</translation>
    </message>
    <message>
        <location/>
        <source>RadioButton3</source>
        <translation>ラジオボタン3</translation>
    </message>
    <message>
        <location/>
        <source>GroupBox2</source>
        <translation>グループボックス2</translation>
    </message>
    <message>
        <location/>
        <source>CheckBox1</source>
        <translation>チェックボックス1</translation>
    </message>
    <message>
        <location/>
        <source>CheckBox2</source>
        <translation>チェックボックス2</translation>
    </message>
    <message>
        <location/>
        <source>LineEdit</source>
        <translation>ラインエディタ</translation>
    </message>
    <message>
        <location/>
        <source>ComboBox</source>
        <translation>コンボボックス</translation>
    </message>
    <message>
        <location/>
        <source>PushButton</source>
        <translation>プッシュボタン</translation>
    </message>
    <message>
        <location/>
        <source>&lt;p&gt;&lt;a href=&quot;http://qt.nokia.com&quot;&gt;http://qt.nokia.com&lt;/a&gt;&lt;/p&gt;
&lt;p&gt;&lt;a href=&quot;http://www.kde.org&quot;&gt;http://www.kde.org&lt;/a&gt;&lt;/p&gt;</source>
        <translation></translation>
    </message>
</context>
</TS>
