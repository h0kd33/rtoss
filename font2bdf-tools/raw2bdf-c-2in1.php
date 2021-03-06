<?php

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle, $fsize);
$byteArray = unpack("C*",$contents); $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//print_r($byteArray);

$w=@$_SERVER['argv'][2] ? @$_SERVER['argv'][2] : 16;
$h=@$_SERVER['argv'][3] ? @$_SERVER['argv'][3] : 16;
$offset=@$_SERVER['argv'][4] ? @$_SERVER['argv'][4] : 0;
$skip=@$_SERVER['argv'][5] ? @$_SERVER['argv'][5] : 0;
$chrs=@$_SERVER['argv'][6] ? @$_SERVER['argv'][6] : 256;

$tchrs=$chrs/2;
$fp=fopen(str_replace('.','-',@$_SERVER['argv'][1]).'.bdf','wb') or die('Cannot write file');
$fa=ceil($h*0.75);
$fd=$h-$fa;
$hdr= <<<HEADER
STARTFONT 2.1
FONT ${w}x$h
SIZE $h 75 75
FONTBOUNDINGBOX $w $h 0 -$fd
STARTPROPERTIES 4
FONT_ASCENT $fa
FONT_DESCENT $fd
SPACING "C"
DEFAULT_CHAR 32
ENDPROPERTIES
CHARS $tchrs

HEADER;
fwrite($fp,$hdr);

$swidth = $w*90;
$bwidth=ceil($w/16);
for ($char=0; $char<$chrs; $char+=2)
{
	fwrite($fp,"STARTCHAR ".($char/2)."\n");
	fwrite($fp,"ENCODING ".($char/2)."\n");
	fwrite($fp,"SWIDTH $swidth 0\n");
	fwrite($fp,"DWIDTH $w 0\n");
	fwrite($fp,"BBX $w $h 0 -$fd\n");
	fwrite($fp,"BITMAP\n");

	for ($y=0; $y<$h*$bwidth; $y++)
	{
		$v='';
		$v.=sprintf("%02X",$byteArray[$offset+$char*$skip+$char*($bwidth*$h)+$y]);
		$v.=sprintf("%02X\n",$byteArray[$offset+($char+1)*$skip+($char+1)*($bwidth*$h)+$y]);
		fwrite($fp, $v);
	}

	fwrite($fp,"ENDCHAR\n");
}

fwrite($fp,"ENDFONT\n");
fclose($fp);
