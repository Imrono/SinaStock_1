using git for version control

git reset --hard  #clear staged & modified files, not care about "ignore files"
git reset . / git reset HEAD  #clear staged files

00代表 正常
03代表 停牌
04代表 sz新股
05代表 sh新股

+		url_StockHistory	0x003ef4ec "http://vip.stock.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/600820.phtml?year=2015&judu=1"	wchar_t [150]



克隆版本库的时候，所使用的远程主机自动被Git命名为origin。如果想用其他的主机名，需要用git clone命令的-o选项指定。
$ git clone -o jQuery git@github.com:Imrono/SinaStock_1.git
$ git remote
jQuery
git remote add命令用于添加远程主机
git remote add origin git@github.com:Imrono/SinaStock_1.git