# プロジェクト設定

シリアルの送受信 (recv and send for serial)。`ser/` (コマンド) と `lib/` (ライブラリ)。

## コミットガイドライン

- コミットメッセージは英語で記述
- **件名は、Conventional Commits の形式で書く**: `type(scope): description`。scope (括弧を含む) は、省略できる。description は、英語で、命令形 (例: `fix(ser): Fix the checksum of the received data`)。
  - type は、次のどれか。
    - `feat`: 機能の追加
    - `fix`: バグの修正
    - `docs`: 文書とコメントだけの変更 (`*.md`、ソースのコメント)
    - `style`: 整形、規約の適用 (動作を変えない)
    - `refactor`: 動作を変えない、書き換え
    - `test`: テストだけの変更
    - `build`: ビルド (`Makefile`、`ser/Makefile`、`lib/Makefile`) の変更
    - `ci`: CI (`.github/`) の変更
    - `chore`: そのほかの雑務 (最初のコミット、`.gitignore`、`version.h` の更新など)
  - scope は、変更の場所: `ser` (`ser/` のコマンド)、`lib` (`lib/` のライブラリ)、`tests` (`tests/`)。複数にまたがるときは、省略する。
  - 互換性を壊す変更は、type (または scope) の直後に、感嘆符を付ける (Conventional Commits の仕様のとおり)。
  - 複数の type にまたがるときは、変更の中心になる type にする。
- 件名のあとに空行を 1 行入れて、本文を書く。本文はダッシュ/ハイフンで始まる箇条書き (例: `- Fix bug in parser`)。箇条書きの各行の間には、空行を入れない
- コミットメッセージに `Co-Authored-By` または Claude 帰属行を含めない
- **rebase したあとの push は、`git push --force-with-lease --force-if-includes origin <ブランチ名>` で行う** (`--force` は使わない。リモートに、手元に取り込んでいないコミットがあれば、push は失敗する)。ユーザーに依頼されたときだけ、push する。
