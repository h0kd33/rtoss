<!--&MAIN-->
<html><head><title>freeStyle</title>
<body>
{$tit} <br>
<hr>

<!--&IF($post,'<!--&POST/-->','')-->
<!--&IF($main,'<!--&MAINFRAME/-->','')-->

<br><div align=right><small><a href=http://php.s3.to target=_blank>freeStyle bbs</a> / <a href="http://riaf.org/f_style/">riaf</a> / PTE</small></div>
</center></body></html>
<!--/&MAIN-->

<!--&POST-->
<form method="post" action="{$dir_path}../bbs.php">
���O<br>
<input type="text" name="nick" size="20"><br>
���[��<br>
<input type="text" name="mail" size="30"><br>
�薼<br>
<input type="text" name="subject" maxlength="80" size="50"><br>
<textarea name="content" rows="5" class=input cols="70"></textarea><br>
����<br>
�ȉ��̓���̨���ނ�ر���Ă��������<br>
<input type=text value="x" name=url size=8><br>
<input type="submit" value="�V�K���e">
</form>
<!--/&POST-->

<!--&MAINFRAME-->
<a href="i.php?mode=post">�V�K���e</a><br>
<!--&FOREACH($head,'HEAD')-->

<hr>
<!--&IF($prev,'<a href="i.php?page={$prev}">���O��</a>�@','')-->
<!--&IF($next,'<a href="i.php?page={$next}">���ց�</a>','')-->
<!--/&MAINFRAME-->

<!--&HEAD-->
{$no} : <a href="{$dir_path}read.php?key={$key}&ls={$read_def}&ua=i">{$title}</a><br>
<!--/&HEAD-->
