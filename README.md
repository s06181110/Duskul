# プロジェクト演習 課題

## Gitの使い方

``` bash
# 0.初期設定 (これは最初だけやるやつ)
# main.cがあるフォルダに移動する
rm *
git init
git remote add origin https://github.com/s06181110/project_training.git

# 1.他の人の変更を反映する
git pull origin master

# 2.ブランチを作成する
git branch ブランチ名

# 3.作成したブランチに入る
git checkout ブランチ名 

# 上（2, 3）の省略 ver
git checkout -b ブランチ名

# 自分が作ったブランチにいることを確認する
git branch
master
* ブランチ名

#---- 作業する ----#

# 4.変更を加えた後
git add .
git commit -m "変更内容(英語推奨)"
git push origin ブランチ名

# 5.Git hubのサイトでプルリクエストを作成

# 6.他の人の変更をmasterブランチに反映する
git pull origin master

# あとは 4 ~ 6の繰り返し
```

# 知りたい人用にメモ
```bash
# 自分が変更したファイルを確認する
git status

# 変更した内容をみる
git diff

# みんなの変更履歴をみる
git log

# １つ前のバージョンに戻す(変更内容も全部取り消すから注意)
# HEAD^ をlogのIDにするとそこまで戻れる
git reset --hard HEAD^

# コミットの取り消し(変更内容は残る)
git reset HEAD



