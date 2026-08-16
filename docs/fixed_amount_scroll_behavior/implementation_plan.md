# 固定量スクロール カスタムビヘイビアの実装

## 背景・目的

ZMK v0.3 の標準 `&msc` ビヘイビアは「押している間、連続的にスクロールする」動作です。
ユーザーの要望は「ボタンを1回押すと、**指定した固定量（例: 10）だけスクロール**する（ワンショット）」ビヘイビアです。

ZMKには該当するビルトイン機能がないため、**カスタムCビヘイビア `zmk,behavior-scroll-fixed`** を新規作成します。

## 設計方針

### 動作概要
- キーを **押した瞬間** に、設定した固定量のスクロールHIDレポートを1回だけ送信
- キーを **離した時** は何もしない（連続スクロールしない）
- 縦スクロール・横スクロールの両方に対応

### 使い方（キーマップでの記述例）

```dts
/ {
    behaviors {
        // 上スクロール（10単位）
        scrl_up: scroll_up {
            compatible = "zmk,behavior-scroll-fixed";
            #binding-cells = <0>;
            scroll-y = <10>;   // 正 = 上方向
        };

        // 下スクロール（10単位）
        scrl_dn: scroll_down {
            compatible = "zmk,behavior-scroll-fixed";
            #binding-cells = <0>;
            scroll-y = <(-10)>; // 負 = 下方向
        };
    };
};
```

キーマップでは `&scrl_up` / `&scrl_dn` として使用します。

---

## 提案する変更

### カスタムビヘイビア

#### [NEW] [behavior_scroll_fixed.c](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/src/behavior_scroll_fixed.c)

カスタムビヘイビアドライバ本体。キー押下時に `zmk_hid_mouse_scroll_update()` + `zmk_endpoints_send_mouse_report()` を呼び出し、指定量のスクロールHIDレポートを1回送信します。

主要ロジック:
1. DTS プロパティ `scroll-y` / `scroll-x` から固定スクロール量を読み込み
2. `on_keymap_binding_pressed` でスクロール値をHIDレポートにセットして送信
3. 送信後にスクロール値をクリア（0にリセット）して再送信（繰り返し防止）
4. `on_keymap_binding_released` では何もしない

#### [NEW] [zmk,behavior-scroll-fixed.yaml](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/dts/bindings/behaviors/zmk,behavior-scroll-fixed.yaml)

デバイスツリーバインディング定義。以下のプロパティを定義:
- `scroll-y` (int, default 0): 縦スクロール量。正=上、負=下
- `scroll-x` (int, default 0): 横スクロール量。正=右、負=左

---

### ビルド設定

#### [MODIFY] [CMakeLists.txt](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/CMakeLists.txt)

新しいソースファイル `src/behavior_scroll_fixed.c` をビルド対象に追加。

#### [MODIFY] [module.yml](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/zephyr/module.yml)

カスタム DTS バインディングのパスとして `dts_root: .` を追加。

---

### キーマップ

#### [MODIFY] [keymap.keymap](file:///c:/Users/spica/Desktop/zmk-keyboard-torabo-tsuki-lp/config/keymap.keymap)

`behaviors` セクションに `scrl_up` / `scrl_dn` などのインスタンスを定義。

---

## 検証計画

### ビルド確認
- GitHub Actions でファームウェアがエラーなくビルドできることを確認

### 手動テスト
- 実機でキーを1回タップ → 指定量だけスクロールすることを確認
- キーを長押し → 1回分のスクロールしか発生しないことを確認
- 上下スクロールの両方向が正しく動作することを確認
