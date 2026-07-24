# ダブルタップ LANGUAGE_1 + トラックボールデッドゾーンの実装計画

## 概要

`&clt 3 LANGUAGE_1` の誤動作問題に対して、ZMKの **tap-dance** ビヘイビアを使い、ダブルタップでのみ `LANGUAGE_1` が発動する仕組みに変更する。合わせてトラックボールのデッドゾーンを導入する。

---

## 案の評価

### tap-dance + &mo 3 の動作

```
td_l3_lang1: tap_dance_layer3_lang1 {
    compatible = "zmk,behavior-tap-dance";
    #binding-cells = <0>;
    tapping-term-ms = <200>;
    bindings = <&mo 3>, <&kp LANGUAGE_1>;
};
```

| 操作 | 動作 | 説明 |
|---|---|---|
| **ホールド（長押し）** | `&mo 3` ホールド → **レイヤー3有効** ✅ | tapping-term経過後にレイヤー3が発動 |
| **ホールド中に他キー押下** | `&mo 3` が**即座に解決** → レイヤー3有効 ✅ | 他キーの割り込みでtap-danceが即座に確定するため、体感遅延なし |
| **シングルタップ** | `&mo 3` タップ → **実質的にno-op** ✅ | 一瞬レイヤー3が有効化→すぐ解除。LANGUAGE_1は**出ない** |
| **ダブルタップ** | `&kp LANGUAGE_1` → **日本語入力ON** ✅ | 素早く2回タップしたときのみIME切替 |

### メリット

- ✅ **誤動作を構造的に排除**: シングルタップではLANGUAGE_1が出ないため、ホールドのつもりがタップになっても問題なし
- ✅ **レイヤー0に日本語入力ONを維持**: ダブルタップで同じ物理キーからLANGUAGE_1を送信可能
- ✅ **レイヤー3のホールドが確実**: 他のキーを押した瞬間に即座にレイヤー3として確定
- ✅ **他のキー（Space, BS, Enter等）に影響なし**: 変更はこのキーのみ

### 注意点

> [!NOTE]
> **ホールド時の初期遅延**: tap-danceはダブルタップ判定のために `tapping-term-ms`（200ms）待つ仕組みです。ただし、**レイヤー3ホールド中に別のキーを押した瞬間に即座に確定**するため、実用上の体感遅延はほぼありません（レイヤー3に入って最初に押すキーが即座に反映される）。

> [!NOTE]
> **ダブルタップの速度**: 200ms以内に2回タップする必要があります。慣れが必要ですが、意図的な操作なので問題ないはずです。

---

## Proposed Changes

### 1. keymap ビヘイビア + キーマップ変更

#### [MODIFY] [keymap.keymap](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/keymap.keymap)

**① behaviors ブロックに tap-dance を追加（20行目付近に追記）:**

```dts
// ダブルタップで LANGUAGE_1、ホールドで Layer 3
// シングルタップは &mo 3 → 実質 no-op（誤入力防止）
td_l3_lang1: tap_dance_layer3_lang1 {
    compatible = "zmk,behavior-tap-dance";
    #binding-cells = <0>;
    tapping-term-ms = <200>;
    bindings = <&mo 3>, <&kp LANGUAGE_1>;
};
```

**② Layer 0 のキー変更（71行目）:**

```diff
-&clt 3 LANGUAGE_1
+&td_l3_lang1
```

---

### 2. トラックボール デッドゾーン追加

#### [MODIFY] [west.yml](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/west.yml)

yamaryu211 リモートと threshold モジュールを追加:

```diff
 manifest:
   remotes:
     - name: zmkfirmware
       url-base: https://github.com/zmkfirmware
     - name: sekigon-gonnoc
       url-base: https://github.com/sekigon-gonnoc
+    - name: yamaryu211
+      url-base: https://github.com/yamaryu211
   projects:
     ...existing projects...
+    - name: zmk-input-processor-temp-layer-threshold
+      remote: yamaryu211
+      revision: main
```

#### [MODIFY] [torabo_tsuki_lp.dtsi](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp.dtsi)

既存の `zip_auto_mouse_layer` の直後に threshold 版を追加:

```dts
// Auto Mouse Layer with threshold (デッドゾーン付き)
// トラックボール軽触ではレイヤー6に切り替わらない
zip_auto_mouse_layer_threshold: zip_auto_mouse_layer_threshold {
    compatible = "zmk,input-processor-temp-layer-threshold";
    #input-processor-cells = <2>;
    threshold = <15>;
    require-prior-idle-ms = <150>;
    excluded-positions = <18 19 21 22 23 32 33 34 35 36 38 40 41 42 44 45 46 47 48 49 50 51 52 55 64>;
};
```

#### [MODIFY] [torabo_tsuki_lp_left.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.overlay)

```diff
-    input-processors = <&zip_xy_transform (INPUT_TRANSFORM_X_INVERT | INPUT_TRANSFORM_Y_INVERT)>, <&zip_auto_mouse_layer 6 60000>;
+    input-processors = <&zip_xy_transform (INPUT_TRANSFORM_X_INVERT | INPUT_TRANSFORM_Y_INVERT)>, <&zip_auto_mouse_layer_threshold 6 60000>;
```

#### [MODIFY] [torabo_tsuki_lp_right.overlay](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.overlay)

```diff
-    input-processors = <&zip_xy_transform (INPUT_TRANSFORM_X_INVERT | INPUT_TRANSFORM_Y_INVERT)>, <&zip_auto_mouse_layer 6 60000>;
+    input-processors = <&zip_xy_transform (INPUT_TRANSFORM_X_INVERT | INPUT_TRANSFORM_Y_INVERT)>, <&zip_auto_mouse_layer_threshold 6 60000>;
```

#### [MODIFY] [torabo_tsuki_lp_left.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_left.conf) + [torabo_tsuki_lp_right.conf](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/boards/shields/torabo_tsuki_lp/torabo_tsuki_lp_right.conf)

```diff
+CONFIG_ZMK_INPUT_PROCESSOR_TEMP_LAYER_THRESHOLD=y
```

---

## Open Questions

> [!IMPORTANT]
> 1. **`tapping-term-ms = <200>` で良いですか？** ダブルタップの受付窓口です。小さすぎると素早くタップしないとIME切替できず、大きすぎるとホールド開始が遅れます（ただし他キー押下で即確定するため体感影響は小さい）。200msがバランス良いと思いますが調整可能です。
> 2. **`threshold = <15>` で良いですか？** トラックボールのデッドゾーンの累積移動量閾値です。大きいほどしっかり動かさないとレイヤー6が発動しません。小さすぎると効果が薄く、大きすぎると意図的な操作でもレイヤー切替が遅く感じます。15から開始して調整するのを推奨します。

## Verification Plan

### ビルド確認
- GitHub Actions でファームウェアビルドが成功することを確認

### 手動確認（ファームウェア書き込み後）
1. キーをホールド → レイヤー3に確実に移行できること
2. キーをホールドしつつ他のキーを押す → 即座にレイヤー3のキーが入力されること
3. キーをシングルタップ → LANGUAGE_1が出力**されない**こと
4. キーをダブルタップ → LANGUAGE_1が出力されIMEが切り替わること
5. トラックボールを軽く触れる → レイヤー6が起動**しない**こと
6. トラックボールを意図的に動かす → レイヤー6が正常に起動すること
