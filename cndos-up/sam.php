<?php echo '<?xml version="1.0" encoding="euc-jp"?>'."\n" ?>
<?php
/***
* Reconstruction by Wastepaper Basket
* ����ͥ��륫�å������ʲ���������by ToR

* ��PHP���ץ�����GD��ɬ�פǤ���̵�����Ǥϥ���ʤȤ���¿��
* $_GET�����Ѥ��Ƥޤ����Ť��С�������PHP�Ǥ�$_GET��$HTTP_GET_VARS $_SERVER��$HTTP_SERVER_VERS
**/

$title     = 'Wastepaper Basket';	//�����ȥ�
$upphp     = 'upload.php';		//���åץ���
$img_dir   = "./img/";			//���������ǥ��쥯�ȥ�
$thumb_dir = "./imgs/";			//����ͥ�����¸�ǥ��쥯�ȥ�
$ext       = ".+\.png$|.+\.jpe?g$";	//��ĥ�ҡ�GIF��GD�Ύʎގ����ގ��ݤˤ�äƤ�̵��
$W         = 120;			//���ϲ�����
$H         = 120;			//���ϲ����⤵
$cols      = 2;				//1�Ԥ�ɽ�����������
$page_def  = 4;			//1�ڡ�����ɽ�����������

if ($_GET["cmd"]=="min" && isset($_GET["pic"])) {
  $src = $img_dir.$_GET["pic"];

  // ���������ȹ⤵�ȥ����פ����
  $size = GetImageSize($src);
  switch ($size[2]) {
    case 1 : $im_in = ImageCreateFromGIF($src);  break;
    case 2 : $im_in = ImageCreateFromJPEG($src); break;
    case 3 : $im_in = ImageCreateFromPNG($src);  break;
  }
  // �ɤ߹��ߥ��顼��
  if (!$im_in) {
    $im_in = ImageCreate($W,$H);
    $bgc = ImageColorAllocate($im_in, 0xff, 0xff, 0xff);
    $tc  = ImageColorAllocate($im_in, 0,0x80,0xff);
    ImageFilledRectangle($im_in, 0, 0, $W, $H, $bgc);
    ImageString($im_in,1,5,30,"Error loading {$_GET["pic"]}",$tc);
    ImagePNG($im_in);
    exit;
   }
  // �ꥵ����
  if ($size[0] > $W || $size[1] > $H) {
    $key_w = $W / $size[0];
    $key_h = $H / $size[1];
    ($key_w < $key_h) ? $keys = $key_w : $keys = $key_h;

    $out_w = $size[0] * $keys;
    $out_h = $size[1] * $keys;
  } else {
    $out_w = $size[0];
    $out_h = $size[1];
  }
  // ���ϲ����ʥ���ͥ���ˤΥ��᡼�������
  $im_out = ImageCreateTrueColor($out_w, $out_h);
  // ��������Ĳ��Ȥ� ���ԡ����ޤ���
  ImageCopyResampled($im_out, $im_in, 0, 0, 0, 0, $out_w, $out_h, $size[0], $size[1]);

  // �����ǥ��顼���Ф����ϲ����Ԥ��֤������Ƥ���������(GD2.0�ʲ�
  //$im_out = ImageCreate($out_w, $out_h);
  //ImageCopyResized($im_out, $im_in, 0, 0, 0, 0, $out_w, $out_h, $size[0], $size[1]);

  // ����ͥ��������֥饦���˽��ϡ���¸
  switch ($size[2]) {
  case 1 : if (function_exists('ImageGIF')) { ImageGIF($im_out); ImageGIF($im_out, $thumb_dir.$_GET["pic"]); } break;
  case 2 : ImageJPEG($im_out);ImageJPEG($im_out, $thumb_dir.$_GET["pic"]); break;
  case 3 : ImagePNG($im_out); ImagePNG($im_out, $thumb_dir.$_GET["pic"]);  break;
  }
  // �����������᡼�����˴�
  ImageDestroy($im_in);
  ImageDestroy($im_out);
  exit;
}
// �ǥ��쥯�ȥ����������������
$d = dir($img_dir);
while ($ent = $d->read()) {
  if (eregi($ext, $ent)) {
    $files[] = $ent;
  }
}
$d->close();
// ������
@natsort($files);
$files2 = @array_reverse($files);
//�إå�HTML
echo <<<HEAD
<!DOCTYPE html
   PUBLIC "-//W3C//DTD XHTML 1.1//EN"
   "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=euc-jp" />
<title>$title</title>
<link href="style.css" type="text/css" rel="stylesheet" media="all" />
</head>
<body>

<h1>$title</h1>
<p>���ˤʤ�ͥ��ʤɡ�(������)��</p>

<h2>��������</h2>
<table summary="����ͥ������">
<tr>

HEAD;

//print_r($files);
$maxs = count($files)-1;
$start = $_GET['start'];
$ends = $start+$page_def-1;
$counter = 0;
while (list($line, $filename) = @each($files2)) {
  if (($line >= $start) && ($line <= $ends)) {
    $image = rawurlencode($filename);
    // ����ͥ��뤬������ώ��юȎ��٤ؤΎ؎ݎ�������ʳ��ώ��юȎ���ɽ��������
    if (file_exists($thumb_dir.$image)) $piclink = $thumb_dir.$image;
    else $piclink = $_SERVER["PHP_SELF"]."?cmd=min&pic=".$image;
    $counter++;
    if (((($counter) % $cols) == 1)&&(($counter) != 1)) {
	echo "</tr>\n<tr>\n";
    }//�ᥤ��HTML
    echo <<<EOD
<td class="img"><a href="$img_dir$image"><img src="$piclink" alt="$filename" title="$filename" /><br />$filename</a></td>

EOD;
  }
}
echo "</tr>\n</table>\n";

//�͎ߎ����ޥ��
echo '<p class="tline"><a href="'.$upphp.'?">���</a> Page:';
if ($_GET["start"] > 0) {
  $prevstart = $_GET["start"] - $page_def;
  echo " <a href=\"$_SERVER[PHP_SELF]?start=$prevstart\">&lt;&lt;����</a>";
}
if ($ends < $maxs) {
  $nextstart = $ends+1;
  echo " <a href=\"$_SERVER[PHP_SELF]?start=$nextstart\">����&gt;&gt;</a>";
}
echo '</p>';

echo '

<h2>���ɽ��</h2>
<p id="link"><a href="http://php.s3.to/">��å�PHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a></p>

</body>
</html>
';
?>