<?php
function gzip($file,$level=3) {
	$fh = @fopen($file, 'r+b');
	if (flock($fh, LOCK_EX)) {
		$buffer = fread($fh, filesize($file));
		$gzdata = "\x1f\x8b\x08\x00\x00\x00\x00\x00";  // ��XGZip�з����Y
		$Size = strlen($buffer);
		$Crc = crc32($buffer);
		$Contents = gzcompress($buffer, $level);
		$Contents = substr($Contents, 0, strlen($Contents) - 4);
		$gzdata .= $Contents;
		$gzdata .= pack('V', $Crc); // ��X�ˬd�X
		$gzdata .= pack('V', $Size); // ��X���ɤj�p
		rewind($fh);
		fwrite($fh, $gzdata);
		fflush($fh);
		ftruncate($fh, ftell($fh));
		fclose($fh);
	}
}

function gunzip($file) {
	$file = @gzopen($file, 'rb', 0);
	if ($file) {
		 $data = '';
		 while (!gzeof($file)) {
			$data .= gzread($file, 1024);
		 }
		 gzclose($file);
	}
	return $data;
}

function ungzlog($file) {
	$logs=explode("\n",gunzip($file));
	$lcnt=count($logs);
	for($i=0;$i<$lcnt;$i++) $logs[$i].="\n";
	return $logs;
}


?>