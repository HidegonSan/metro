# Metro: v0.0.1

[[日本語]](README.md) [[English]](README_EN.md)

静的型付けスクリプト言語です。強力で精度の良い型推論が特徴です。

## 使用方法

### リポジトリをクローンし、ビルドする

```bash
git clone --recursive git@github.com:bomkei/metro
pip install fbp
cd metro
fbp metro
```

### 実行

```bash
./metro [your script files]
```

## 開発に参加する

歓迎します！[これ](CONTRIBUTE.md)を読んでください。

## 実装

### インタプリタ

yacc や lex といった他のライブラリ、ツールは一切使用しておりません。
字句解析から構文木の評価まで、すべて C++ でオリジナルのソースコードが書かれています。

## メモリ安全性

ガベージコレクタがオブジェクトを管理してくれます。
