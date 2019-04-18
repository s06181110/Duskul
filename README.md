# プロジェクト演習 課題

## Gitの使い方

``` bash
# 0.初期設定 (これは最初だけやるやつ)
# main.cがあるフォルダに移動する
rm *
git init
git remote add origin https://github.com/s06181110/project_training.git
git pull origin master

# 1.他の人の変更を反映する
 # 非推奨
git pull 
 # 推奨
git fetch
git merge

# 2.ブランチを作成して入る
git branch branch_name
git checkout branch_name 

# 上の省略 ver
git checkout -b branch_name

# 3.変更を加えた後
git add .
git commit -m "変更内容(英語推奨)"
git push origin branch_name

# プルリクエストを作成
# この時右側のReviewersにレビューして欲しい人を指名しておく

# 4.checkoutでmasterブランチに戻る
# 最初(1)に戻る

# 5.自分の変更がマージされた後
# 自分がmasterにいることを確認する
git branch
> \*master
> develop

# 6.マージされなかった場合
# 変更を加えてpushする

# 作業ブランチを破棄する
git branch -D branch_name

# もし、マージされる前に破棄してやりなおす場合
git fetch origin
git checkout -b 破棄したブランチ origin/破棄したブランチ
