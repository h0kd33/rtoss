<?php
if (phpversion()>="4.1.0"){
  extract($_SERVER);
  extract($_REQUEST);
}
/***********************************
  * DL�J�E���^         by ToR
  *
  * http://php.s3.to/
  * 2001/10/23
  **********************************
  * DL�J�E���^�ł�
  * �t�@�C����
  * ID|�J�E���g|�ꏊ|�o�^��|�^�C�g��|�Q��URL|���L�^���܂�
  * �Ǘ����[�h��JavaScript���g�p���Ă��܂��B
  * �w�胊���N���ȊO��DL���Ȃ��悤�ɏo���܂�
  * http://www.proxy2.de��top_dl���Q�l�ɂ��܂���
  *
  * 2002/08/19 �F�؋����B�����ύX�s�A�����o�O�C��
  * 2002/09/13 ROOT��߁B�����߽�ł���
  *
  * �t�@�C���̏ꏊ�Ɖ���y�[�W�́Adl.php����̑����߽���A�t��URL�������܂�
  */
define(LOGFILE, "dllog.txt");		/* ���O�t�@�C���� */

define(ADMINPASS, "0123");		/* �Ǘ��p�X */
define(SHOW_DEF,15);			/* �����N�͍ŏ������\���H */

define(BAR, "aqua.gif");		/* �o�[�̉摜 */
define(HEIGHT, 10);			/* �摜�̍��� */

define(REFCHECK, 0);			/* �����N���`�F�b�N����H����ꍇ�� */
$okurl = array('http://php.s3.to');	/* ���̃��X�g�ȊO�����DL�͕s�� */

/* �ւ������� */
function head(){
  global $PHP_SELF;
  $this_day = gmdate("Y/m/d(D) H:i:s", time()+9*3600);//���ݎ���
  ?>
<html>
<head>
<title>Top Downloads</title>
<style type=text/css>
td { font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
a:hover { color: #ff0000; text-decoration: underline }
select {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
input {  font-size: 8pt}
tt {  font-family: Verdana, Arial; font-size: 13pt; font-weight: bold;color:f08020;}
a {  text-decoration: none}
</style>
<script language="Javascript">
<!--
  function ViewDetail(target,selObj) {
    eval(target+".location='<? echo $PHP_SELF;?>?job=detail&detail="+selObj.options[selObj.selectedIndex].value+"'");
  }
// -->
</script>
</head>
<body bgcolor="FFFFDD" text="#000000" link="#000066" vlink="#000066" alink="#000066">
<br>
<form method="post" action="<? echo $PHP_SELF;?>">
<table width="560" border="0" cellspacing="0" cellpadding="2" height="55" align="center">
  <tr> 
    <td> <tt>���b�cPHP! Downloads </tt></td>
    <td align="right"> <img src="point.gif" width="9" height="9"><a href="<? echo $PHP_SELF;?>?job=admin">Administration</a></td>
  </tr>
  <tr>
    <td><img src="point.gif" width="9" height="9"><b>Date: <? echo $this_day;?></b></td>
    <td align="right">
       <select name="files" onChange="ViewDetail('self',this)">
         <option selected>show details</option>
<?php
}
/* �t�b�^���� */
function foot() {
  ?>
 <table border=0 width="560" align="center" cellspacing="1" cellpadding="3">
  <tr> 
    <td valign="top"><a href="http://php.s3.to/" target=_new">PHP Downloads</a></td>
      <td align="right">
       <select name="option">
         <option value="total" selected>Total</option>
         <option value="perDay">Per day</option>
       </select>
       <select name="top">
         <option value="10" selected>Top 10</option>
         <option value="15">Top 15</option>
         <option value="20">Top 20</option>
         <option value="30">Top 30</option>
         <option value="all">all</option>
       </select>
       <input type="submit" value="Submit">
    </td>
  </tr>
</table>
</form>
</body>
</html>
<?php
  exit;
}
/* �J�E���g�A�b�v��DL */
function count_dl($id){

  $lines = file(LOGFILE);	//�t�@�C���ǂݍ���
  $find = FALSE;		//�t���O
  for($i=0; $i<count($lines); $i++){
    list($file_id,$count,$location,$stamp,$descript,$ref) = explode("|",$lines[$i]);
    if($id == $file_id){	//����������J�E���g�A�b�v
      $find=TRUE;
      $count++;
      $lines[$i] = "$file_id|$count|$location|$stamp|$descript|$ref|\n";//��������
      $redirect = $location;	//�t�@�C���ʒu����
      break;
    }
  }
  if($find){//�^�[�Q�b�g������������DL
    //���O�X�V
    $fp = fopen(LOGFILE, "w");
    flock($fp, LOCK_EX);
    fputs($fp, implode("", $lines));
    fclose($fp);
    if(eregi("^(f|ht)tp:\/\/[._a-z0-9-]+",$redirect)){
      header("Location: $redirect");//URL�̏ꍇLocation
    }else{//���[�J���̏ꍇ�_�C�A���O
      header("Content-Type: application/octet-stream; name=".basename($redirect));
      header("Content-Disposition: attachment; filename=".basename($redirect));
      readfile($redirect);//�ǂݍ���
    }
  }else{
    die("�Ώۃt�@�C����������܂���I");
  }
  exit;
}
/* �����L���O�\�� */
function show_table($top,$option){

  head();//�w�b�_�\��
  //�y�[�W����
  $lines = file(LOGFILE);
  $now = time();

  if($top == all) $top = count($lines);
  else $top = (preg_match("/^(\d+)/",$top)) ? $top : SHOW_DEF;
  //�\�[�g�p�ɓǂݍ���
  for($i=0; $i<count($lines); $i++){
    list($file_id,$num,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
    if($option=="perDay"){
      $dl_perday = ($now-$stamp>86400) ? sprintf("%.2f",($num*86400/($now-$stamp))) : 0;
      $num_arr[$descript] = $dl_perday;	//�����N�p
      $name_arr[$file_id] = $descript;	//�E��I��p
      $ref_arr[$descript] = $refurl;	//�Q�Ɨp
      $total += $dl_perday;			//���v�J�E���g�v�Z
    }else{
      $num_arr[$descript] = $num;	//�����N�p
      $name_arr[$file_id] = $descript;	//�E��I��p
      $ref_arr[$descript] = $refurl;	//�Q�Ɨp
      $total += $num;			//���v�J�E���g�v�Z
    }
  }
  arsort($num_arr);			//�����̑傫����
  asort($name_arr);			//���O�̏�������
  //�E�㕔���쐬
  while(list($fileid,$desc)=each($name_arr)){
    echo "        <option value=\"$fileid\">$desc</option>\n";
  }
  //�e�[�u���w�b�_
  echo "      </select>\n    </td>\n  </tr>\n</table>\n";
  echo "<table bgcolor=\"F7F7F7\" border=0 cellpadding=3 cellspacing=2 width=560 align=center>\n"; 
  echo "  <tr><td colspan=4 bgcolor=\"A8C1F1\"><font color=\"#000099\"><b>Top $top Downloads</b></font></td></tr>\n";
  //���v����������
  if($total > 0){
    $flag = FALSE;	//��ʃt���O
    while(list($title,$value)=each($num_arr)){
      if ($show < $top) {//��ʉ���
        $tmp = $rank;						//�ȑO�̐�
        $rank = $value;						//�V������
        if($tmp != $rank) $num = $show + 1;			//�Ⴄ�Ȃ烉���N�A�v
        if(!$flag) $top_pos = $value;				//��ʃf�[�^
        $per = sprintf("%.1f %%",($value *100 / $total));	//���t�H�[�}�b�g
        $img_width = ($value * 310 / $top_pos);			//�o�[������
        if($ref_arr[$title]) $title = "<a href=\"$ref_arr[$title]\">$title</a>";
        echo "<tr><td align=center><b>$num</b></td><td>$title</td><td nowrap><img src=\"".BAR."\" width=$img_width height=".HEIGHT."> $value </td><td>$per</td></tr>\n";
        $show++;						//�\���J�E���g
        $flag = TRUE;
      }else{
        break;
      }
    }
  }
  foot();//�t�b�^�\��
}
/* �ڍ׃f�[�^�\�� */
function show_detail($id){

  head();//�w�b�_�\��
  $now = time();
  //�E�㕔��
  $lines = file(LOGFILE);
  for($i=0; $i<count($lines); $i++){
    list($file_id,$num,$location,$stamp,$descript,) = explode("|",$lines[$i]);
    $name_arr[$file_id] = $descript;
    $total += $num;
  }
  asort($name_arr);
  while(list($fileid,$desc)=each($name_arr)){
    echo "        <option value=\"$fileid\">$desc</option>\n";
  }
  //�e�[�u���w�b�_
  echo "      </select>\n    </td>\n  </tr>\n</table>\n";
  echo "<table bgcolor=\"F7F7F7\" border=0 cellpadding=3 cellspacing=2 width=560 align=center>\n"; 
  echo "  <tr><td colspan=4 bgcolor=\"A8C1F1\"><font color=\"#000099\"><b>Top $top Downloads</b></font></td></tr>\n";
  //���ږ��̔z��
  $details = array('�t�@�C����','�^�C�g��','�o�^��','�o�ߓ���','����������DL��','���vDL��','�S�̂̊���','����y�[�W');
  reset($lines);//�ꉞ���Z�b�g
  for($i=0; $i<count($lines); $i++){
    list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
    if($id == $file_id){
      $file_date = date("Y/m/d(D) H:i:s", $stamp);	//�f�[�^�o�^��
      $total_days = (int)(($now-$stamp)/86400);		//�o�ߓ���
      //����������DL��
      $dl_perday = ($now-$stamp>86400) ? sprintf("%.2f",($count*86400/($now-$stamp))) : 0;
      $percent = sprintf("%.2f%%",($count*100/$total));//�S�̂ɂ����銄��
      $link = "<a href=\"$refurl\">$refurl</a>";
      break;
    }
  }
  //�f�[�^�̔z��
  $vals = array($file_id,$descript,$file_date,$total_days,$dl_perday,$count,$percent,$link);
  //�Ή������ĕ\��
  for($c=0; $c<count($details); $c++){
    echo "  <tr>\n    <td width=35% bgcolor=ced5ff>$details[$c]</td>\n    <td bgcolor=e0e8ff>$vals[$c]</td>\n  </tr>\n";
  }
  
  echo "<tr><td colspan=2 align=left bgcolor=A8C1F1><a href=\"$GLOBALS[PHP_SELF]\">&lt;&lt;Back</a></td></tr>";
  echo "</table>\n";
  foot();//�t�b�^�\��
}
/* �Ǘ���ʕ\���@*/
function show_track(){
  global $PHP_SELF;

  $jump = $PHP_SELF."?job=admin&admin=".md5(ADMINPASS);

  ?>
<html><head>
<meta http-equiv="Expires" content="Tue, 01 Jan 1980 1:00:00 GMT">
<meta http-equiv="Pragma" content="no-cache">
<title>Administration Mode</title>
<style type="text/css">
<!--
td {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
a {  text-decoration: none; color: #000000}
a:hover { color: #000099; text-decoration: underline }
.input {  font-size: 8pt}
tt {  font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13pt; font-weight: bold}
//-->
</style>
<script language="Javascript">
<!--
function check_char(strg) {
 var found = 0;
 for (char_pos=0; char_pos<strg.length; char_pos++) {
    if (strg.charAt(char_pos) == "|" || strg.charAt(char_pos) == "&" || strg.charAt(char_pos) == "=" || strg.charAt(char_pos) == '"' || strg.charAt(char_pos) == "'") {
       found = 1;
       break;
    }
 }
 return (found == 1) ? false : true;
}
function set_value(entry, old_value) {
 var new_value = window.prompt(entry+" �̒l��ύX���܂�",old_value)
 var found = 0;
 if (new_value != "" && new_value != null) {
    for (char_pos=0; char_pos<new_value.length; char_pos++) {
       if (!(new_value.charAt(char_pos) >= '0' && new_value.charAt(char_pos) <= '9')) {
          found = 1;
       }
    }
    if (found == 1) {
       alert ("��������͂��Ă��������I");
    }
    else if (new_value == old_value) {
       alert ("�ύX����Ă܂���I");
    }
    else { 
       window.location.href = "<? echo $jump;?>&act=set_value&id="+entry+"&new="+new_value
    }
 }
}
function ChangeEntry(entry, old_name, feature) {
 var new_name = window.prompt(entry+"�@�̐V���� "+feature+" ����͂��Ă�������",old_name)
 var found = 1;
 if (new_name != "" && new_name != null) {
    if (check_char(new_name)) {
         found = 0;
    }
    if (found == 1) {
       alert ("Error! "+entry+" �ɋ�����ĂȂ��������܂܂�Ă��܂�!");
    }
    else if (new_name == old_name) {
       alert ("�ύX����Ă܂���I");
    }
    else if (feature == "����") {
       window.location.href = "<? echo $jump;?>&act=ren_entry&id="+entry+"&new="+new_name
    }
    else if (feature == "FileID") {
       window.location.href = "<? echo $jump;?>&act=ren_id&id="+entry+"&new="+new_name
    }
    else if (feature == "URL") {
       window.location.href = "<? echo $jump;?>&act=set_loc&id="+entry+"&new="+new_name
    }
    else if (feature == "����y�[�W") {
       window.location.href = "<? echo $jump;?>&act=set_ref&id="+entry+"&new="+new_name
    }
 }
}
function del_entry(entry) {
 if (window.confirm("�{���ɍ폜���Ă���낵���ł����H("+entry+")")) {
    window.location.href = "<? echo $jump;?>&act=del_entry&id="+entry
 }
}
function check_form() {
 var found = 1;
 des_field=document.form.description.value;
 id_field=document.form.new_id.value;
 if (des_field == "" || id_field == "") {
    alert ("ID�ƃ^�C�g���𖄂߂Ă��������I");
    return false;
 }
 if (check_char(des_field) && check_char(id_field)) {
    found = 0;
 }
 if (found == 1) {
    alert ("Error! ������Ă��Ȃ������ł�");
    return false;
 }
 if (document.form.new_loc.value.indexOf(".")==-1) {
    alert("�t�@�C���̏ꏊ������������܂���I");
    document.form.new_loc.focus();
    return false;
 }
}
// -->
</script>
</head>
<body bgcolor="#FFFFFF">
 <table width="560" border="0" align="center">
  <tr>
    <td height="40"><tt>Administration Mode</tt></td>
    <td height="40" align="right" valign="bottom"><img src="point.gif" width="9" height="9"><a href="<? echo $PHP_SELF;?>?">Back to Top Downloads</a></td>
  </tr>
 </table>
 <table bgcolor="#F7F7F7" border=1 cellspacing=0 cellpadding=3 width="560" align="center">
    <tr bgcolor="#A8E1A8"> 
      <td><b>����</b></td>
      <td><b>�t�@�C��ID</b></td>
      <td><b>DL��</b></td>
      <td><b>�ꏊ</b></td>
      <td><b>����y�[�W</b></td>
      <td><b>�폜</b></td>
    </tr>
<?php
$lines = file(LOGFILE);
$totalfiles = count($lines);
for($i=0; $i<count($lines); $i++){
  list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
  if($location=="") $location = "-";
  if($refurl=="") $refurl="-";
  print "    <tr>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$descript','����')\">$descript</a></td>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$file_id','FileID')\">$file_id</a></td>\n";
  print "      <td><a href=\"javascript:set_value('$file_id','$count')\">$count</a></td>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$location','URL')\">$location</a></td>\n"; 
  print "      <td><a href=\"javascript:ChangeEntry('$file_id','$refurl','����y�[�W')\">$refurl</a><td><a href=\"javascript:del_entry('$file_id')\">�폜</a></td>\n    </tr>\n";
  $total+=$count;
}
echo '
    <tr> 
      <td><b><font color="#336600">&gt;&gt;Total Files:</font></b> <font color="#FF0000">'.$totalfiles.'</font></td>
      <td colspan="4"><b><font color="#336600">&gt;&gt;Total Downloads:</font></b> <font color="#FF0000">'.$total.'</font></td>
    </tr>
  </table>
  <form method="post" action="'.$PHP_SELF.'" name="form">
    <table width="560" border="0" align="center">
      <tr>
';?>
        <td>�t�@�C���̏ꏊ�F </td>
        <td><input type="text" name="new_loc" size="42"><small>dl.php����̃p�X��URL</small>
          <input type="hidden" name="job" value="admin">
          <input type="hidden" name="admin" value="<? echo md5(ADMINPASS);?>">
      </tr>
      <tr> 
        <td>�t�@�C�� ID�F </td>
        <td><input type="text" name="new_id" size="42"></td>
      </tr>
      <tr> 
        <td>�\���^�C�g���F </td>
        <td><input type="text" name="description" size="42"></td>
      </tr>
      <tr> 
        <td>����y�[�WURL�F </td>
        <td><input type="text" name="new_ref" size="42">
          <input type="submit" name="submit" value="Add Entry" onclick="return check_form()" class="input"></td>
      </tr>
    </table>
  </form>
<br><center>�����N�̎d���@dl.php?dl=[�t�@�C��ID]</center>
</body>
</html>
<?
exit;
}
/* �ǉ��E�ύX�E�폜 */
function manage($manage,$id,$new){
  $lines = file(LOGFILE);//���O�ǂݍ���
  $find = FALSE;//�t���O

  for($i=0; $i<count($lines); $i++){
    //�f�[�^����
    list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
    if($id == $file_id){//����ID��������
      switch($manage){	//���[�h����
      case 'del_entry':	//�폜
        $lines[$i] = "";
        $find = TRUE;
        break;
      case 'ren_entry':	//�����ύX
        $new = chop($new);
        if(get_magic_quotes_gpc()) $new = stripslashes($new);
        $lines[$i] = "$file_id|$count|$location|$stamp|$new|$refurl|\n";
        $find = TRUE;
        break;
      case 'ren_id':	//ID�ύX
        $new = preg_replace("/\s*/","",$new);
        $lines[$i] = "$new|$count|$location|$stamp|$descript|$refurl|\n";
        $find = TRUE;
        break;
      case 'set_value':	//�J�E���g�ύX
        $lines[$i] = "$file_id|$new|$location|$stamp|$descript|$refurl|\n";
        $find = TRUE;
        break;
      case 'set_loc':	//�ꏊ�ύX
        $new = preg_replace("/\s*/","",$new);
        $lines[$i] = "$file_id|$count|$new|$stamp|$descript|$refurl|\n";
        $find = TRUE;
        break;
      case 'set_ref':	//�Q��URL�ύX
        $lines[$i] = "$file_id|$count|$location|$stamp|$descript|$new|\n";
        $find = TRUE;
        break;
      }
    }
  }
  if($find){//���O�X�V
    $fp = fopen(LOGFILE, "w");
    flock($fp, LOCK_EX);
    fputs($fp, implode("", $lines));
    fclose($fp);
  }
}
/* �V�K�ǉ� */
function add_data($url,$id,$desc,$ref){
  //���`
  $id = str_replace("|","",$id);
  $id = str_replace("'","",$id);
  $id = chop(str_replace('"','',$id));
  $url = chop($url);
  if(strstr($url,".") && !empty($id)) {
    $lines = file(LOGFILE);
    $find = FALSE;
    //����ID���o�^����Ă��Ȃ���
    for($i=1; $i<count($lines); $i++){
      list($file_id,) = explode("|",$lines[$i]);
      if($id == $file_id){
        $find = TRUE;
        break;
      }
    }
    if(!$find){//���O��������
      $desc = str_replace("|","",$desc);
      $desc = str_replace("'","",$desc);
      $desc = chop(str_replace('"','',$desc));
      if(get_magic_quotes_gpc()) $desc = stripslashes($desc);
      $ref = str_replace("|","",$ref);
      $ref = str_replace("'","",$ref);
      $ref = chop(str_replace('"','',$ref));
      $stamp = time();
      $newline = "$id|0|$url|$stamp|$desc|$ref|\n";//�t�H�[�}�b�g
      $fp = fopen(LOGFILE, "a");
      fputs($fp, $newline);
      fclose($fp);
    }
  }else{
    die("URL��ID���s���ł�");
  }
}
/* �Ǘ����[�h���O�C�� */
function validation($admin){

  if($admin && ($admin != md5(ADMINPASS)))
    $err = "<font color=red>�p�X���[�h���Ⴂ�܂��I</font>";
  if(empty($admin) || $err){
    ?>
      <html>
        <head><title>PHP Downloads</title>
        <meta http-equiv="Content-Type" content="text/html; charset=Shift_JIS">
<style type="text/css">
<!--
td {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
tt {  font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13pt; font-weight: bold}
-->
</style>
</head>
<body bgcolor="FFFFDD">
<form method="post" action="<? echo $PHP_SELF;?>">
 <table width="560" border="0" align="center">
  <tr>
    <td height="40"><tt>Administration Mode</tt></td>
  </tr>
 </table>
 <table width="560" border="0" cellspacing="0" cellpadding="3" align="center">
   <tr bgcolor="A8E1E1">
     <td><b>�p�X���[�h����͂��Ă�������</b></td>
   </tr>
   <tr bgcolor="F7F7F7"> 
     <td> 
       <input type="password" name="admin_submit">
       <input type="submit" value="Submit">
       <input type="hidden" name="job" value="admin">
     </td>
   </tr>
   <tr bgcolor="F7F7F7"> 
     <td><? echo $err;?></td>
   </tr>
   <tr> 
     <td><br>
       <a href="http://php.s3.to" target="_blank">���b�cPHP!</a> 
       | <a href="<? echo $PHP_SELF;?>?">Back to Top Downloads</a></td>
   </tr>
 </table>
</form>
</body>
</html>
<?php
exit;
  }
  return true;
}
/*************main******************/
//�_�E�����[�h
if(!empty($dl)){
  if(REFCHECK){
    for($i=0; $i<count($okurl); $i++){
      if(!strstr(getenv("HTTP_REFERER"),$okurl[$i]))
        die("�w��URL�ȊO�����DL�ł��܂���");
    }
  }
  count_dl($dl);
  exit;
}
switch($job){
case 'detail':
  if($detail) show_detail($detail);
  break;
case 'admin':
  if($admin_submit) $admin = md5($admin_submit);
  validation($admin);
  if($act) manage($act,$id,$new);
  if($submit=="Add Entry") add_data($new_loc,$new_id,$description,$new_ref);
  show_track();
  break;
default:
  if(time()-filemtime(LOGFILE) > 3600) @copy(LOGFILE,LOGFILE.".bak");
  show_table($top,$option);
}
?>
