# ウォークスルー: ダブルタップ LANGUAGE_1 + トラックボールデッドゾーン

## 変更概要

レイヤー3キー（`&clt 3 LANGUAGE_1`）の「ホールドのつもりがタップになる」誤動作と、トラックボール軽触によるレイヤー6誤発動の2つの問題を解決しました。

---

## 変更内容

### 1. tap-dance によるダブルタップ LANGUAGE_1

#### [keymap.keymap](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/keymap.keymap)

**ビヘイビア追加** (L22-30): `td_l3_lang1` tap-dance ビヘイビアを定義

```dts
td_l3_lang1: tap_dance_layer3_lang1 {
    compatible = "zmk,behavior-tap-dance";
    #binding-cells = <0>;
    tapping-term-ms = <200>;
    bindings = <&mo 3>, <&kp LANGUAGE_1>;
};
```

**キー変更** (L81): Layer 0 のキーを変更

```diff
-&clt 3 LANGUAGE_1
+&td_l3_lang1
```

**動作:**
| 操作 | 結果 |
|---|---|
| ホールド（長押し） | レイヤー3有効 |
| ホールド中に他キー押下 | 即座にレイヤー3確定 |
| シングルタップ | no-op（LANGUAGE_1は出ない） |
| ダブルタップ | LANGUAGE_1（IME切替） |

---

### 2. トラックボール デッドゾーン

#### [west.yml](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/west.yml)
- `yamaryu211` リモート追加 (L7-8)
- `zmk-input-processor-temp-layer-threshold` モジュール追加 (L32-34)

#### [torabo_tsuki_lp.dtsi](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp.dtsi)
- `zip_auto_mouse_layer_threshold` プロセッサを追加 (L37-46)
- `threshold = <15>`: 累積移動量15以上でレイヤー6発動

#### [torabo_tsuki_lp_left.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.overlay) / [torabo_tsuki_lp_right.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.overlay)
- `zip_auto_mouse_layer` → `zip_auto_mouse_layer_threshold` に切替

#### [torabo_tsuki_lp_left.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.conf) / [torabo_tsuki_lp_right.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.conf)
- `CONFIG_ZMK_INPUT_PROCESSOR_TEMP_LAYER_THRESHOLD=y` 追加

---

## ビルド & 検証

### ビルド
- GitHub Actions でプッシュ後にファームウェアビルドが成功するか確認してください

### 手動検証（ファームウェア書き込み後）

| # | テスト項目 | 期待結果 |
|---|---|---|
| 1 | キーをホールド | レイヤー3に移行 |
| 2 | キーをホールドしつつ他キー押下 | 即座にレイヤー3のキーが入力される |
| 3 | キーをシングルタップ | LANGUAGE_1が出力**されない** |
| 4 | キーをダブルタップ | LANGUAGE_1が出力されIMEが切替わる |
| 5 | トラックボールを軽く触れる | レイヤー6が起動**しない** |
| 6 | トラックボールを意図的に動かす | レイヤー6が正常に起動 |

---

## 調整可能なパラメータ

| パラメータ | 現在の値 | ファイル | 説明 |
|---|---|---|---|
| `tapping-term-ms` (td_l3_lang1) | 200ms | keymap.keymap L28 | ダブルタップの受付窓口。大きくすると楽に2回タップできるが、ホールド開始が遅れる |
| `threshold` | 15 | torabo_tsuki_lp.dtsi L43 | デッドゾーン閾値。大きくすると軽触に強くなるが、意図的操作でもレイヤー切替が遅くなる |
