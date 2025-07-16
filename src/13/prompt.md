#

- 計画書生成: <https://g.co/gemini/share/abe482192e71>
- ソース生成: <https://g.co/gemini/share/265c5d9339c1>

---

次の形式に従って各課題のあらまし (YAML) を作ろうと思います.  生成した YAML は独立させ, 各ソースコードを再現させる用途で使います.  再現率を高めるために次の指示に従って**第 12 回課題**についての YAML を生成してください.

- まず, `assignments--raw.md` の第 12 回課題を参照する
  - 視覚情報が必要な場合は, PDF を参照する
- 必要に応じてソースと提出画像を参考にする
  - 課題 1 か 2 かはファイル名 (.cpp) から判断する
- `CODING_STYLE.md` に書いてある情報は除く
- YAML 内にコメントは生成してはならない
  - `[cite_start]`, `[cite: 509]` などの引用識別子も生成してはならない

```yml
- l12:
  # 課題 1
  - a1:
    # 課題の説明.  Markdown 形式で記述すること.
    - description: -|
    # 画像の入力
    - input:
      - "aquarium.jpg"
    # 画像の出力
    - output:
      # 出力画像名
      - "aquarium.jpg"
      # または, `cv::imshow` で表示する場合は
      - "<WINDOW_SHOW>"
    # 課題の条件や, 使用する関数の指定があれば箇条書きで記述.
    - requirements:
      - "新しい変数 cv::Vec3b output_valを用いる。"
      - "手動でこの輪郭番号を指定する方法はNG"
    # 課題の要件を満たしているかどうかのチェックすべき項目.
    # 実装が正しいかを見るため, 各項目は視覚的に分かることを簡潔に
    - checklists:
      - "..."
      - "..."
      - "..."
    # 課題の意図を読めなかったり, この YAML の生成において, 特記事項があればこのフィールドを Markdown 形式で埋める
    - note: -|
  # 課題 2 – チャレンジ課題を含む
  - a2:
    # ...同様に...
```

---

`CODING_STYLE.md` に準拠して課題を解いてください.

- 基本的に `allowedFunctions` のみで実装
  - `assignments.yml` が不明瞭だったり, 実装が困難ならその旨を報告
- また, 02 から 12 までの各課題の C++ を出力
  - 各課題は…
    - 最初に 第 XXX 回 課題 X (`ファイル名`) のH2ヘッダーを付与
    - ファイル名はソースに含めてはならない
    - 学籍番号は `k24132`
    - 入力画像は `assets/` から.  出力画像は `output/` がベースパスです.  結合した状態で `define` 宣言して構いません.

---

```
cp ./src/02/hw/01/assets/dog.png ./src/13/generated/assets/dog.png
cp ./src/02/hw/02/assets/aquarium.jpg ./src/13/generated/assets/aquarium.jpg
cp ./src/03/hw/01/assets/sakura.jpg ./src/13/generated/assets/sakura.jpg
cp ./src/03/hw/02/assets/apple_tree.jpg ./src/13/generated/assets/apple_tree.jpg
cp ./src/04/hw/01/assets/image1.jpg ./src/13/generated/assets/image1.jpg
cp ./src/04/hw/02/assets/image2.jpg ./src/13/generated/assets/image2.jpg
cp ./src/05/hw/01/assets/apple_grayscale.jpg ./src/13/generated/assets/apple_grayscale.jpg
cp ./src/06/hw/01/assets/apple_tree.jpg ./src/13/generated/assets/apple_tree.jpg
cp ./src/07/hw/02/assets/issue_input1.jpg ./src/13/generated/assets/issue_input1.jpg
cp ./src/07/hw/02/assets/issue_input2.jpg ./src/13/generated/assets/issue_input2.jpg
cp ./src/08/hw/01/assets/papersample.jpg ./src/13/generated/assets/papersample.jpg
cp ./src/08/hw/02/assets/red_green_rectangle.jpg ./src/13/generated/assets/red_green_rectangle.jpg
cp ./src/09/hw/01/assets/sample.jpg ./src/13/generated/assets/sample.jpg
cp ./src/09/hw/02/assets/sample.jpg ./src/13/generated/assets/sample.jpg
cp ./src/09/hw/02/assets/sample_rotate.jpg ./src/13/generated/assets/sample_rotate.jpg
cp ./src/10/hw/01/assets/issue1.jpg ./src/13/generated/assets/issue1.jpg
cp ./src/10/hw/02/assets/challenge.jpg ./src/13/generated/assets/challenge.jpg
cp ./src/11/hw/01/assets/handinwhite.jpg ./src/13/generated/assets/handinwhite.jpg
cp ./src/11/hw/02/assets/fruit_image.jpg ./src/13/generated/assets/fruit_image.jpg
cp ./src/12/hw/01/assets/gazoDora.jpg ./src/13/generated/assets/gazoDora.jpg
cp ./src/12/hw/01/assets/green.jpg ./src/13/generated/assets/green.jpg
cp ./src/12/hw/01/assets/red.jpg ./src/13/generated/assets/red.jpg
cp ./src/12/hw/01/assets/star.jpg ./src/13/generated/assets/star.jpg
cp ./src/12/hw/01/assets/yellow.jpg ./src/13/generated/assets/yellow.jpg
cp ./src/12/hw/02/assets/gazoDora.jpg ./src/13/generated/assets/gazoDora.jpg
cp ./src/12/hw/02/assets/red.jpg ./src/13/generated/assets/red.jpg
```

---

```
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g <> -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a


/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./02_02_colors_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./03_01_negaposi_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./03_02_LUT_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./04_01_bin_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./04_02_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./05_01_filter_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./05_02_denoising_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./06_01_UP_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./06_02_gradient_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./07_02_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./07_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./08_01_ptile_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./08_02_bin_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./09_01_star_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./09_02_rectangle_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./10_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./10_k24132egg.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./11_01_handinwhite_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./11_02_fruit_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./12_01_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
/usr/bin/g++ -fcolor-diagnostics -fansi-escape-codes -g ./12_02_k24132.cpp -o ./out/a -std=c++17 `pkg-config --cflags --libs opencv4`; ./out/a
```
