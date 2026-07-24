# タスクリスト: ダブルタップ LANGUAGE_1 + トラックボールデッドゾーン

## tap-dance ビヘイビア追加
- [x] [keymap.keymap](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/keymap.keymap) - behaviors に `td_l3_lang1` 追加
- [x] [keymap.keymap](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/keymap.keymap) - Layer 0 の `&clt 3 LANGUAGE_1` → `&td_l3_lang1` 変更

## トラックボール デッドゾーン追加
- [x] [west.yml](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/west.yml) - yamaryu211 リモート + threshold モジュール追加
- [x] [torabo_tsuki_lp.dtsi](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp.dtsi) - threshold プロセッサ定義追加
- [x] [torabo_tsuki_lp_left.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.overlay) - threshold プロセッサに切替
- [x] [torabo_tsuki_lp_right.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.overlay) - threshold プロセッサに切替
- [x] [torabo_tsuki_lp_left.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.conf) - CONFIG追加
- [x] [torabo_tsuki_lp_right.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.conf) - CONFIG追加
