<?php
/*********************************************
  Wastepaper Uploader v1.5
  http://utu.under.jp/
  PHP���פ���򸵤�W3C�˽�򤷤�XHTML���ܻؤ��٤�
  ������ץȵڤ�HTMLʸ����¤�������ޤ�����
  �빽Ŭ����XHTML���⤷��ޤ���W3C�����ͤ�������ġ�
  ------------------����ˡ-------------------
  ���֥ǥ��쥯�ȥ��755
  img��imgs�ǥ��쥯�ȥ��777
  upload.php��sum.php��755
  up.log��last.cgi��count.txt��666  
  ------------------Based--------------------
  source by PHP���פ�� v2.2
  http://php.s3.to
  
  source by ����ݤ�
  http://zurubon.virtualave.net/

  �⤷��ΰ٤�.htaccess ��CGI�ػ�SSI�ػ�Indexɽ���ػߡ�
  Options -ExecCGI -Includes -Indexes
  .txt�Ǥ⡢��Ȥ�HTML����ɽ��������㤦�Τ����
  -------------------------------------------
 *********************************************/
if(phpversion()>="4.1.0"){//PHP4.1.0�ʹ��б�
  $_GET = array_map("_clean", $_GET);
  $_POST = array_map("_clean", $_POST);
  //$_POST = array_map("_clean", $_GET);
  extract($_GET);
  extract($_POST);
  extract($_COOKIE);
  extract($_SERVER);
  $upfile_type=_clean($_FILES['up']['type']);
  $upfile_size=$_FILES["upfile"]["size"];//˿��ǵ��Ť�������
  $upfile_name=_clean($_FILES["upfile"]["name"]);
  $upfile=$_FILES["upfile"]["tmp_name"];
}

  $title	= 'Wastepaper Uploader';	//�����ȥ�
  $home		= 'http://utu.under.jp/';	//�ۡ���
  $logfile	= "up.log";	//���ե�����̾���ѹ��������
  $updir 	= "./img/";	//���å��ѥǥ��쥯�ȥ�
  $upsdir	= "./imgs/";	//����ͥ�����¸�ǥ��쥯�ȥ�
  $prefix	= '';		//��Ƭ���up001.txt,up002.jpg�ʤ�up��
  $logmax	= 50;		//log��¸�ԡʤ���ʾ�ϸŤ��Τ�������
  $commax	= 250;		//��������������¡ʥХ��ȡ����ѤϤ���Ⱦʬ��
  $limitk	= 100;	//���åץ������¡�KB ����Х��ȡ�
  $page_def	= 10;		//��ڡ�����ɽ���Կ�
  $admin	= "12345";	//��������ѥ�
  $auto_link	= 1;		//�����Ȥμ�ư��󥯡�Yes=1;No=0);
  $denylist	= array('192.168.0.1','sex.com','annony');	//�����������ݥۥ���
  $arrowext	= array('zip','lzh','rar','avi','mpg','wmv','mp3','wma','swf','txt','bmp','jpg','png','gif');	//���ĳ�ĥ�� ��ʸ���ʤ���ʳ��ϥ��顼

  $count_start	= "2004/10/24";	//�����󥿳�����
  $count_file	= "count.txt";  //�����󥿥ե�����ʶ��ե������666��

  $last_file	= "last.cgi";	//Ϣ³��������ѥե�����ʶ��ե������666��
  $last_time	= 0;		//Ʊ��IP�����Ϣ³��Ƶ��Ĥ���ֳ֡�ʬ�ˡ�0��̵���¡�

  /* ����ɽ���ʴĶ�����ˤν������ (ɽ���ʤ�Checked ɽ�����ʤ��ʤ��) */
  $f_act  = 'checked="checked"';	//ACT�ʺ����󥯡�
  $f_com  = 'checked="checked"';	//������
  $f_size = 'checked="checked"';	//�ե����륵����
  $f_mime = '';				//MIME������
  $f_date = 'checked="checked"';	//���դ�
  $f_orig = 'checked="checked"';	//���ե�����̾

if($act=="envset"){
  $cookval = @implode(",", array($acte,$come,$sizee,$mimee,$datee,$orige)); 
  setcookie ("upcook", $cookval,time()+365*24*3600);
}
function _clean($str) {
  $str = htmlspecialchars($str);
  if (get_magic_quotes_gpc()) $str = stripslashes($str);
  return $str;
}
/* ��������إå��� */
?>
<?php echo '<?xml version="1.0" encoding="euc-jp"?>'."\n" ?>
<!DOCTYPE html
   PUBLIC "-//W3C//DTD XHTML 1.1//EN"
   "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=euc-jp" media="all" />
<title><?php echo "$title" ?></title>
<link href="style.css" type="text/css" rel="stylesheet" />
</head>
<body>

<h1><?php echo "$title" ?></h1>
<p>���ˤʤ�ͥ��ʤɡ�(������)��</p>

<?php
/* �إå��������ޤ� */
$foot = <<<FOOT

<h2>���ɽ��</h2>
<p id="link"><a href="http://php.s3.to/">��å�PHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a></p>

</body>
</html>
FOOT;

function FormatByte($size){//�Х��ȤΥե����ޥåȡ�B��kB��
  if($size == 0)			$format = "";
  else if($size <= 1024)		$format = $size."B";
  else if($size <= (1024*1024))		$format = sprintf ("%dkB",($size/1024));
  else if($size <= (10*1024*1024))	$format = sprintf ("%.2fMB",($size/(1024*1024)));
  else					$format = $size."B";

  return $format;
}
function paging($page, $total){//�ڡ�����󥯺���
  global $PHP_SELF,$page_def;

    for ($j = 1; $j * $page_def < $total+$page_def; $j++) {
      if($page == $j){//��ɽ�����Ƥ���Τώ؎ݎ����ʤ�
        $next .= "[<strong>$j</strong>]";
      }else{
        $next .= sprintf("[<a href=\"%s?page=%d\">%d</a>]", $PHP_SELF,$j,$j);//¾�ώ؎ݎ�
      }
    }
    if($page=="all") return sprintf ("Page: %s [ALL]",$next,$PHP_SELF);
    else return sprintf ("Page: %s [<a href=\"%s?page=all\">ALL</a>]",$next,$PHP_SELF);
}
function error($mes1=""){//���ä顼�Ҏ���������
  global $foot;

  echo $mes1;
  echo $foot;
  exit;
}
/* start */
$limitb = $limitk * 1024;
$host = @gethostbyaddr($REMOTE_ADDR);
if(!$upcook) $upcook=@implode(",",array($f_act,$f_com,$f_size,$f_mime,$f_date,$f_orig));
list($c_act,$c_com,$c_size,$c_mime,$c_date,$c_orig)=explode(",",$upcook);

/* ������������ */
if(is_array($denylist)){
  while(list(,$line)=each($denylist)){
    if(strstr($host, $line)) error('<h2>���顼��å�����</h2>
<p class="error">������������:�����������¤�����ޤ���</p>');
  }
}
/* ����¹� */
if($delid && $delpass!=""){
  $old = file($logfile);
  $find = false;
  for($i=0; $i<count($old); $i++){
    list($did,$dext,,,,,,$dpwd,)=explode("\t",$old[$i]);
    if($delid==$did){
      $find = true;
      $del_ext = $dext;
      $del_pwd = rtrim($dpwd);
    }else{
      $new[] = $old[$i];
    }
  }
  if(!$find) error('<h2>���顼��å�����</h2>
<p class="error">������顼:�����ե����뤬���Ĥ���ޤ���</p>');
  if($delpass == $admin || substr(md5($delpass), 2, 7) == $del_pwd){
    if(file_exists($updir.$prefix.$delid.".$del_ext")) unlink($updir.$prefix.$delid.".$del_ext");
    if(file_exists($upsdir.$prefix.$delid.".$del_ext")) unlink($upsdir.$prefix.$delid.".$del_ext");
    $fp = fopen($logfile, "w");
    flock($fp, 2);
    fputs($fp, @implode("",$new));  
    fclose($fp);
  }else{
    error('<h2>���顼��å�����</h2>
<p class="error">������顼:�ѥ���ɤ��㤤�ޤ�</p>');
  }
}
/* ����ե����� */
if($del){
  error("<h2>����ե�����</h2>
<form method=\"post\" enctype=\"multipart/form-data\" action=\"$PHP_SELF\">
<p>����ѥ�����<br />
<input type=\"hidden\" name=\"delid\" value=\"".htmlspecialchars($del)."\" />
<input type=\"password\" size=\"12\" name=\"delpass\" class=\"box\" tabindex=\"1\" accesskey=\"1\" />
<input type=\"submit\" value=\"���\" tabindex=\"2\" accesskey=\"2\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">���</a></p>
");
}
/* �Ķ�����ե����� */
if($act=="env"){
  echo "<h2>�Ķ�����</h2>
<form method=\"get\" action=\"$PHP_SELF\">
<p><input type=\"hidden\" name=\"act\" value=\"envset\" /></p>
<h3>ɽ������</h3>
<ul>
<li><input type=\"checkbox\" name=\"acte\" value=\"checked\" tabindex=\"1\" accesskey=\"1\" $c_act />ACT</li>
<li><input type=\"checkbox\" name=\"come\" value=\"checked\" tabindex=\"2\" accesskey=\"2\" $c_com />COMMENT</li>
<li><input type=\"checkbox\" name=\"sizee\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_size />SIZE</li>
<li><input type=\"checkbox\" name=\"mimee\" value=\"checked\" tabindex=\"4\" accesskey=\"4\" $c_mime />MIME</li>
<li><input type=\"checkbox\" name=\"datee\" value=\"checked\" tabindex=\"5\" accesskey=\"5\" $c_date />DATE</li>
<li><input type=\"checkbox\" name=\"orige\" value=\"checked\" tabindex=\"6\" accesskey=\"6\" $c_orig />ORIG</li>
</ul>
<p>�嵭������ˤ�ˬ�䤹�뤳�Ȥ��Ǥ��ޤ��ʢ�cookie��</p>
<p><input type=\"submit\" value=\"��Ͽ\" tabindex=\"7\" accesskey=\"7\" /><input type=\"reset\" value=\"�����᤹\" tabindex=\"8\" accesskey=\"8\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">���</a></p>

";
echo $foot;
exit;
}
$lines = file($logfile);
/* ���ץ��ɽ񤭹��߽��� */
if(file_exists($upfile) && $com && $upfile_size > 0){
  if(strlen($com) > $commax) error('<h2>���顼��å�����</h2>
<p class="error">��ƥ��顼:�����Ȥ�Ĺ�����ޤ�</p>
');
  if($upfile_size > $limitb)        error('<h2>���顼��å�����</h2>
<p class="error">��ƥ��顼:�ե����뤬�ǥ������ޤ�</p>');
  /* Ϣ³������� */
  if($last_time > 0){
    $now = time();
    $last = @fopen($last_file, "r+") or die('<h2>���顼��å�����</h2>
<p class="error">Ϣ³����ѥե����� $last_file ��������Ƥ�������</p>');
    $lsize = fgets($last, 1024);
    list($ltime, $lip) = explode("\t", $lsize);
    if($host == $lip && $last_time*60 > ($now-$ltime)){
      error('<h2>���顼��å�����</h2>
<p class="error">Ϣ³���������:���֤��֤��Ƥ��ľ���Ƥ�������</p>');
    }
    rewind($last);
    fputs($last, "$now\t$host\t");
    fclose($last);
  }
  /* ��ĥ�Ҥȿ��ե�����̾ */
  $pos = strrpos($upfile_name,".");	//��ĥ�Ҽ���
  $ext = substr($upfile_name,$pos+1,strlen($upfile_name)-$pos);
  $ext = strtolower($ext);//��ʸ����
  if(!in_array($ext, $arrowext))
    error('<h2>���顼��å�����</h2>
<p class="error">��ĥ�ҥ��顼:���γ�ĥ�ҥե�����ϥ��åץ��ɤǤ��ޤ���</p>');
  /* ���ݳ�ĥ�Ҥ�txt���Ѵ�
  for($i=0; $i<count($denyext); $i++){
    if(strstr($ext,$denyext[$i])) $ext = 'txt';
  }
  */
  list($id,) = explode("\t", $lines[0]);//No����
  $id = sprintf("%03d", ++$id);		//���󥯥�
  $newname = $prefix.$id.".".$ext;

  /* ����ž�� */
  move_uploaded_file($upfile, $updir.$newname);//3.0.16����ΥС�������PHP 3�ޤ��� 4.0.2 ��
  //copy($upfile, $updir.$newname);
  chmod($updir.$newname, 0604);

  /* MIME������ */
  if(!$upfile_type) $upfile_type = "text/plain";//�ǥե�MIME��text/plain

  /* ������¾ */
  $com = htmlspecialchars($com);	//�����Ѵ�
  if(get_magic_quotes_gpc()) $com = stripslashes($com);	//�����

  $now = gmdate("Y/m/d(D)H:i", time()+9*60*60);	//���դΥե����ޥå�
  $pwd = ($pass) ? substr(md5($pass), 2, 7) : "*";	//�ѥ��ú�����̵���ʤ�*��

  $dat = @implode("\t", array($id,$ext,$com,$host,$now,$upfile_size,$upfile_type,$pwd,$upfile_name,));

  if(count($lines) >= $logmax){		//�������С��ʤ�ǡ������
    for($d = count($lines)-1; $d >= $logmax-1; $d--){
      list($did,$dext,)=explode("\t", $lines[$d]);
      if(file_exists($updir.$prefix.$did.".".$dext)) {
        unlink($updir.$prefix.$did.".".$dext);
      }
    }
  }

  $fp = fopen ($logfile , "w");		//�񤭹��ߥ⡼�ɤǥ����ץ�
  flock($fp ,2);
  fputs ($fp, "$dat\n");		//��Ƭ�˽񤭹���
  for($i = 0; $i < $logmax-1; $i++)	//���ޤޤǤ�ʬ���ɵ�
    fputs($fp, $lines[$i]);
  fclose ($fp);
  reset($lines);
  $lines = file($logfile);		//����ʤ���
}
foreach($arrowext as $list) $arrow .= $list." ";
/* ��ƥե����� */
echo '<h2>���åץ��ɥե�����</h2>
<form method="post" enctype="multipart/form-data" action="'.$PHP_SELF.'" >
<p>Up File<strong>��'.$limitk.' KB�ޤǡ�</strong><br />
<input type="hidden" name="MAX_FILE_SIZE" value="'.$limitb.'" />
<input type="file" size="40" name="upfile" class="box" tabindex="1" accesskey="1" />
 Del Pass : <input type="password" size="10" name="pass" maxlength="10" class="box" tabindex="2" accesskey="2" /></p>
<p>Comment�ʢ�������̵�����ϥ����/URI�ϥ����ȥ�󥯡�<br />
<input type="text" size="45" name="com" value="" class="box" tabindex="3" accesskey="3" />
<input type="submit" value="Up/Reload" tabindex="4" accesskey="4" />
<input type="reset" value="Cancel" tabindex="5" accesskey="5" /></p>
<p>UP��ǽ��ĥ�ҡ�'.$arrow.'</p>
</form>
';
/* ������ */
echo "<p class=\"uline\">$count_start ���� ";
if(file_exists($count_file)){
  $fp = fopen($count_file,"r+");//�ɤ߽񤭥⡼�ɤǥ����ץ�
  $count = fgets($fp, 64);	//64�Х���orEOF�ޤǼ�����������ȥ��å�
  $count++;
  fseek($fp, 0);			//�ݥ��󥿤���Ƭ�ˡ���å����ƽ񤭹���
  flock($fp,2);
  fputs($fp, $count);
  fclose($fp);			//�ե�������Ĥ���
  echo $count;			//������ɽ��
}
/* �⡼�ɥ�� */
echo '
�ʤ����ˤ�����٥�1��D����Ƶ������</p>
<p class="uline"><a href="'.$home.'">HOME</a> | <a href="'.$PHP_SELF.'?act=env">�Ķ�����</a> | <a href="'.$PHP_SELF.'?">�����</a> | <a href="sam.php">��������</a></p>

<h2>�ե��������</h2>
<p class="uline">';
/* �����ϰ��� */
$st = ($page) ? ($page - 1) * $page_def : 0;
if(!$page) $page = 1;
if($page == "all"){
  $st = 0;
  $page_def = count($lines);
}
echo paging($page, count($lines));//�ڡ������
//�ᥤ��إå�
echo "</p>\n";
echo "<table summary=\"files\">\n<tr>";
if($c_act) echo "<th abbr=\"delete\" scope=\"col\">DEL</th>";
echo "<th abbr=\"name\" scope=\"col\">NAME</th>";
if($c_com) echo "<th abbr=\"comment\" scope=\"col\">COMMENT</th>";
if($c_size) echo "<th abbr=\"size\" scope=\"col\">SIZE</th>";
if($c_mime) echo "<th abbr=\"mime\" scope=\"col\">MIME</th>";
if($c_date) echo "<th abbr=\"date\" scope=\"col\">DATE</th>";
if($c_orig) echo "<th abbr=\"original name\" scope=\"col\">ORIG</th>";
echo "</tr>\n";
//�ᥤ��ɽ��
for($i = $st; $i < $st+$page_def; $i++){
  if($lines[$i]=="") continue;
  list($id,$ext,$com,$host,$now,$size,$mtype,$pas,$orig,)=explode("\t",$lines[$i]);
  $fsize = FormatByte($size);
  if($auto_link) $com = ereg_replace("(https?|ftp|news)(://[[:alnum:]\+\$\;\?\.%,!#~*/:@&=_-]+)","<a href=\"\\1\\2\">\\1\\2</a>",$com);

  $filename = $prefix.$id.".$ext";
  $target = $updir.$filename;

  
  echo "<tr><!--$host-->\n";//�ۥ���ɽ��
  if($c_act) echo "<td class=\"del\"><a href=\"$PHP_SELF?del=$id\">D</a></td>";
  echo "<td>[<a href=\"$target\">$filename</a>]</td>";
  if($c_com) echo "<td>$com</td>";
  if($c_size) echo "<td class=\"size\">$fsize</td>";
  if($c_mime) echo "<td>$mtype</td>";
  if($c_date) echo "<td>$now</td>\n";
  if($c_orig) echo "<td class=\"orig\">\n$orig</td>\n";
  echo "</tr>\n";
  }

echo "</table>\n<p class=\"tline\">";
echo paging($page,count($lines));
echo "</p>\n";
echo $foot;
?>

