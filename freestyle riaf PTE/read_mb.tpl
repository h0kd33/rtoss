<!--&READ-->
<title>{$fsub}</title><body>

<!--&IF($main,'<!--&MAIN/-->','')-->
<!--&IF($post,'<!--&POST/-->','')-->

</body>
<!--/&READ-->

<!--&POST-->
<form method=post action="{$dir_path}bbs.php">
<input type=hidden name=key value="{$key}">
���O<br>
<input type=text value="{$cname}" name=nick size=20 maxlength=40><br>
E-Mail<br>
<input type=text value="{$cmail}" name=mail size=15><br>
<textarea name=content cols=75 rows=3 class=input></textarea><br>
<input type=password value="{$cpass}" name=delk size=8 maxlength=8><br>
����<br>
�ȉ��̓���̨���ނ�ر���Ă��������<br>
<input type=text value="x" name=url size=8><br>
<input type=submit value="���e"><input type=reset value="ؾ��">
</form>
<!--/&POST-->

<!--&MAIN-->
<a href="{$dir_path}i/">��</a><a href="{$dir_path}read.php?key={$key}&ua=i">�S</a><a href="{$dir_path}read.php?key={$key}&ls={$read_def}&ua=i">�V</a><a href="#saigo">��</a><br>
<font size=+1 color="#FF0000">��{$fsub}</font>
<hr>
<!--&IF($first,'<!--&FIRST/-->','')-->
<!--&FOREACH($res,'RES')-->

<a name=saigo></a><hr>
<!--&IF($prev,'<a href="{$dir_path}read.php?key={$key}&st={$prev_st}&to={$prev}&ua=i">�O</a>','')-->
<!--&IF($next,'<a href="{$dir_path}read.php?key={$key}&st={$num}&to={$next}&ua=i">��</a>','')-->
<br><a href="{$dir_path}i/">��</a> <!--&IF($stopped,'','<a href="{$dir_path}read.php?key={$key}&mode=post&ua=i">���X</a>')--><br>
<!--/&MAIN-->

<!--&FIRST-->
[1] <font color="#006600"><!--&IF($femail,'<a href="mailto:{$femail}">{$fname}</a>','{$fname}')--></font> ({$fdate})<br>
{$fcom}
<!--/&FIRST-->

<!--&RESSHOW-->
<hr>[{$num}] <font color="#006600"><!--&IF($email,'<a href="mailto:{$email}">{$name}</a>','{$name}')--></font> ({$date})<br>
{$com}
<!--/&RESSHOW-->

<!--&RES-->
<!--&IF($show,'<!--&RESSHOW/-->','')-->
<!--/&RES-->
