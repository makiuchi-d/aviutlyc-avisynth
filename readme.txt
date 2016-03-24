-----------------------------------------------------------------------
      Convert AviUtlYC Fix  for AviSynth 2.5  ver 1.00  by MakKi
-----------------------------------------------------------------------

【機能】

  warpsharp.dllのAviUtl内部形式変換関数の変換式修正版です。
  AviUtl(0.98以降)と全く同じ変換を行ないます。
  SIMD最適化されていないので遅いです。
  少々色が違っても気にならないという方にはおすすめできません。

【関数】

  ConvertToAviUtlYCFix(clip)       # YUY2,RGB24,RGB32からの変換
  ConvertYUY2ToAviUtlYCFix(clip)   # YUY2からの変換 (後方互換)

  ConvertAviUtlYCToYUY2Fix(clip)   # YUY2への変換
  ConvertAviUtlYCToRGB24Fix(clip)  # RGB24への変換
  ConvertAviUtlYCToRGB32Fix(clip)  # RGB32への変換

  それぞれの変換式について次のページで解説しています。
  興味のある方はご覧ください。
  http://mksoft.hp.infoseek.co.jp/doc/aviutlyc.html

【注意】

  このプログラムはフリーソフトウェアです。
  このプログラムによって損害を被った場合でも、作者は責任を負いません。
  このプログラムはGNU General Public Licenseの元で公開されています。

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA,
    or visit http://www.gnu.org/copyleft/gpl.html .

【謝辞】

  このプログラムはwarpsharp.dllのアイディアを元にしています。
   (http://www.geocities.co.jp/SiliconValley-PaloAlto/2382/)
  YUY2とAviUtl内部形式の相互変換式はゴミ置き場を参考にしました。
   (http://www.geocities.jp/mosd7500/)

  warpsharp.dllの作者様ならびにN099様に感謝します。

【配布元】

  MakKi's SoftWare
  http://mksoft.hp.infoseek.co.jp/

【更新履歴】

  2009/03/29   ver 1.00  ソースを0から書き直し
                         RGBとの相互変換をサポート
  2006/04/10   ver 0.01  公開


mailto:makki_d210@yahoo.co.jp