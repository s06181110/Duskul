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

# Git hubのサイトでプルリクエストを作成

# 5.checkoutでmasterブランチに戻る

# 6.他の人の変更をmasterブランチに反映する
git pull origin master

# 7.作業ブランチに入る
git checkout ブランチ名 

# 自分が作ったブランチにいることを確認してから
# 8.masterの内容を自分のブランチに反映する
git merge master

#---- 作業する ----#

# あとは 4 ~ 8の繰り返し
```