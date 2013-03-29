u-Battery
===========

u-battery(自作モバイルブースター)を使用した際の事故等の責任は負いません
自己責任において組み立て、使用してください

組み立てのポイントなど
----------------------

###ダイオードのパターンミス
D2 D3 D4 はこのURLの部品です <http://www.digikey.jp/product-detail/ja/PMEG3050EP,115/568-6753-1-ND/2676793>
基板のパターンと部品サイズが合ってないので、スズメッキ線等を組み合わせて実装してください。

D3 D4はスルーホールが近くにあるのでそのスルーホールにハンダ付けすると楽です。

D5 D6もサイズが合ってないのでD2と同様にスズメッキ線と組み合わせて実装してください。

###チップ部品のサイズ
チップ抵抗、チップコンデンサは基本1608サイズ(日本式表記)です。
アメリカだと1608を0603と表記するので間違えないように

###温度センサ
IC3はバッテリー温度を監視するセンサを実装するパターンです。
秋月のCMOS温度センサーS-8120Cを実装できます。
<http://akizukidenshi.com/catalog/g/gI-04574/>

自分はこの温度センサの実装はしていません。センサの信号を読み取るプログラムも作ってないですw

温度センサは勝手に実装しても構いません。ただし、温度センサの信号を読み取るプログラムを追加しなければいけません。

###R5について
電流監視用の抵抗ですがIC1のOPアンプが注文ミスにより実装不可能となったため、R5はそのままジャンパでショートさせてください。

###実装しなくていい部品
OPアンプ周辺の抵抗、コンデンサは実装する必要はないです。
lpc1114のクロック回路も実装不要です。

* R3 R7 R8 R9 R10 R11 R51
* C3 C27 C28 C31
* Y1

###その他
* **partslist.txt**にどこにどの部品を実装すればいいか書かれています
* チップICの実装はフラックスを使った方がいい。というか使うべき
* R35からR44までの抵抗ですが471Ωから500Ωぐらいかな
* 当然ですが背の低い部品、小さい部品からハンダ付けしていかないと後で面倒なことになるので注意
* 完成イメージは **IMG_20130329_181824.jpg**と **IMG_20130329_230819.jpg**です
* あとは **assembly-guide.jpg**をみてください

###回路について
充電回路はマイコン(LPC1114)からON/OFFの制御ができます。
これをONにしたらバッテリーが充電されるよう設計していますが、バッテリを接続した状態でONにしたことがないので一応注意してください。ONにしなければ全く問題ないです。

OPアンプはサイズのミスで実装できないですが、基板のパターンに合う(SOP-8サイズ)のOPアンプを各自で買ってきて実装するのは問題ないです。
TLV272と似た仕様のOPアンプなら大丈夫だと思う(ちょっと自信ない)
その場合、OPアンプからのアナログ電圧を読みとるプログラムが書かれていないのでプログラムを自分で変更する必要があります。

完成イメージのSJ2にはチップコンデンサが実装されていますが、これは実装しなくても問題ないです
同じく完成イメージのC22も実装されていませんが、これはC23と同様に電解コンデンサを90°に曲げて実装してください。実装しなくても動いてるけどねw

LEDアレイの抵抗R35からR44は画像だといろんな値の抵抗が実装されていますが、これは抵抗の数が不足したので使えそうな抵抗を寄せ集めて実装したためです。471Ωから500Ωぐらいの抵抗なら問題ないです。

###電源投入手順とか電源OFF時の挙動とか
この装置は電源スイッチがありません。さらにバッテリーをつないだだけでは電源は入りません。電源を入れるには15VをDCinに接続してください。15VをDCinに接続したときに、バッテリーが接続されていてある程度の電圧があれば、15VをDCinがら抜いてもバッテリーの電力で動作し続けます。

電源OFFはバッテリーの電圧が設定値を下回ったら自動で電源が落とされます。電源が落ちているときも多少の電流が流れているので、念のためバッテリーのコネクタは基板から外しておいてください。

この辺はあまりイケてない仕様なのでバージョン2を作る機会があれば改善したい。

###バッテリーも買うなら
バッテリーを買わなくてもただの5V DC-DCコンバータとして使ってもOKです。
もしバッテリーを買うならバッテリーの発熱、発火、爆発には十分注意してください。
充電器も買ってください、基板に実装されている充電回路は動かしていのでまだ信頼性が低いです。

バッテリーと充電器を買うならこのアダプタも買った方がいいです。
<http://www.hobbyking.com/hobbyking/store/__18099__HXT_4MM_to_Banana_Plug_Charge_Lead_Adapter.html>
充電器にはこのアダプタが付属していないのでないと充電が面倒です。

バッテリーはリチウムポリマー電池で。3セル直列の11.1Vのものを使用してください。容量は自由です。

###使用している主なパーツのデータシート
#####リチウムポリマーバッテリー
*Turnigy 5000mAh 3S 25C Lipo Pack* <http://www.hobbyking.com/hobbyking/store/uh_viewItem.asp?idProduct=9183>

#####充電器
*Turnigy Accucel-6 50W 6A Balancer/Charger w/ accessories* <http://www.hobbyking.com/hobbyking/store/__7028__Turnigy_Accucel_6_50W_6A_Balancer_Charger_w_accessories.html>

#####MainController
*LPC1114* <http://ics.nxp.com/support/documents/microcontrollers/pdf/user.manual.lpc11xx.lpc11cxx.jp.pdf>

#####DC-DC Converter
*BD9876* IntFET sync 3A <http://rohmfs.rohm.com/jp/products/databook/datasheet/ic/power/switching_regulator/bd9876efj-j.pdf>

#####BatteryCharger
*MAX1873SEEE+* <http://datasheets.maxim-ic.com/jp/ds/MAX1873_jp.pdf>

#####バッテリー監視IC
*BQ29312A*
<http://www.tij.co.jp/jp/lit/ds/symlink/bq29312a.pdf>
<http://www.tij.co.jp/jp/lit/an/slua375/slua375.pdf>

#####Inductor
*NS12565T150MN* 15uH 4.33A
<http://www.digikey.jp/product-detail/ja/NS12565T150MN/587-2745-1-ND/2573939>

#####Pch-MOSFET
*SI4435DDY* <http://www.digikey.jp/product-detail/ja/SI4435DDY-T1-GE3/SI4435DDY-T1-GE3CT-ND/1978995>

#####Nch-MOSFET
*BSH105,215* <http://www.digikey.jp/product-detail/ja/BSH105,215/568-6215-1-ND/2531502>

#####OPアンプ
*TLV272* <http://www.tij.co.jp/product/jp/TLV272>

###メモ
開発時のメモです

    DSG/CHG meams BQ29312A Pin State L/H. It's not FET State
                                Disconnect DCin
     +--------------------------------------------------------------------------------------------------------+
     v          Use             Connect DCin                                                                  |
    BAT  :Full ----> BAT  :Low ---------------> BAT check mode ---------------------> BAT  :Charging -------->|
    DCin :0v         DCin :0v                   BAT  :unknown    |  BAT need          DCin :15v               |
    5Vout:on         5Vout:off                  DCin :15v        |   to charge        5Vout:on                |
    DSG  :Low        DSG  :Low                  5Vout:off        |                    DSG  :-(Low)Recommend   |
    CHG  :-          CHG  :-                    DSG  :Low        |                    CHG  :Low               |
                      |                         CHG  :-          |                     |                      |
                      | reach BAT Low limit      ^               |                     | BAT is full          |
                      v                          |               |                     v                      |
                     BAT save mode or Power down |               +------------------> BAT  :Full -------------+
                     DCin  0v                    |                  BAT doesn't       DCin :15V
                     5Vout off                   |                   need to charge   5Vout:on
                     DSG   Hi                    |                                    DSG  :-(Low)Recommend
                     CHG   -                     |                                    CHG  :Hi
                     REG   0v                    |
                     Host MCU is down            |
                      |                          |
                      +--------------------------+
                               Connect DCin

    DCinがつながっているならCHGはBATの状態に応じて決めなければならない BAT Full: Hi, BAT Low: Low
    DCinが繋がってないならCHGの状態はどっちでもいい

    DSGは基本LowにするHiにするとシステム全体が落ちる。なので、BATが最低電圧に到達した時のみHiにする
    全体が落ちているときはDCinを繋ぐと復旧する。それ以外の方法では復旧できない。
    復旧直後はBATチェックモードに入りDSGをLowにした上でBATの状態を調べる
    Hi=FET:off Low=FET:on

プログラムは完璧に上の図の通りには動作してないです。充電関係が上の図と違うはず。

解決すべき問題点や基板の設計ミスなどがたくさんあるので。気が向いたらバージョン2も作ってみたい。

組み立てに関して伝えなきゃいけないことは大体書いたと思います。もし質問等ありましたら、@_stroboまでお願いします