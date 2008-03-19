<?php
require("./fav_settings.php");
require("./fav_strings.php");

  $Command1__varid=val($_POST,"id");
  $Command1__varcat=val($_POST,"newIsCat","0");
  $Command1__varprot=val($_POST,"newIsProtected","0");
  $Command1__varname=val($_POST,"newName");
  $Command1__varaddr=val($_POST,"newUrl");
  $Command1__varcatid=val($_POST,"catid",'0');

$MM_RedirectUrl=$homeURL.$SidebarSuffix1;

$Recordset1__MMColParam="1";
if (isset($_GET["id"])) $Recordset1__MMColParam=intval($_GET["id"]);

$conn=sqlite_popen($sqlite_file);
$qry="SELECT * FROM Fav WHERE id = ".sqlite_escape_string($Recordset1__MMColParam);
$rs=sqlite_query($conn,$qry);
$rcnt=sqlite_num_rows($rs);
$row = sqlite_fetch_array($rs);

if (isset($_POST["MM_insert"]) && $_POST["MM_insert"]=="form1"){
  $Command1_CommandText="UPDATE Fav SET cat = ".sqlite_escape_string($Command1__varcat)." , protected = ".sqlite_escape_string($Command1__varprot)." , catid = ".sqlite_escape_string($Command1__varcatid)." , name = '".sqlite_escape_string($Command1__varname)."' , addr = '".sqlite_escape_string($Command1__varaddr)."' WHERE id = ".sqlite_escape_string($Command1__varid);
  sqlite_exec($Command1_CommandText,$conn);

  header("Location: ".$MM_RedirectUrl);
  exit();
} 

echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>'.$MyFav_Mod_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css">';
if ($InSidebar){
echo '<STYLE type="text/css"><!--
body {font-size: 9pt;}
--></STYLE>';
}
echo '<script>
var orig_catid;
function detChk() {
	if (document.getElementById("newIsCat").checked) {
//		document.getElementById("newUrl").disabled=true;
//		document.getElementById("newUrl").value="";
		document.getElementById("newIsProtected").disabled=true;
		document.getElementById("catid").disabled=true;
		orig_catid=document.getElementById("catid").selectedIndex;
		document.getElementById("catid").selectedIndex="0";
	} else {
		document.getElementById("newIsProtected").disabled=false;
//		document.getElementById("newUrl").disabled=false;
		document.getElementById("catid").disabled=false;
		document.getElementById("catid").selectedIndex=orig_catid;
	}
}
function detSel() {
	if (document.getElementById("catid").selectedIndex != 0) {
		document.getElementById("newIsCat").disabled=true;
		document.getElementById("newIsCat").checked=false;
	} else {
		document.getElementById("newIsCat").disabled=false;
	}
}
</script>
</head>';

require("./fav_header.htm");
if (isset($_SESSION['isLogined']) && $rcnt==1) {
echo '<h2>'.$MyFav_Mod_Header.'</h2>
<form name="form1" action="'.$_SERVER['PHP_SELF'].$SidebarSuffix1.'" method="POST">
  '.$MyFav_AddMod_Name.'
  <input name="newName" type="text" value="'.($row["name"]).'" size="'.$TextboxWidth.'">
  <br>
  '.$MyFav_AddMod_URL.'
  <input name="newUrl" id="newUrl" type="text" size="'.$TextboxWidth.'" value="'.($row["addr"]).'">
  <br>
  '.$MyFav_AddMod_IsCategory.'
  <input name="newIsCat" id="newIsCat" onChange="detChk();" type="checkbox" value="1" '.($row['cat']?"checked":'').' '.(!$row["catid"]==0?"disabled":'').'>
  <br>
  '.$MyFav_AddMod_IsPasswordProtected.'
  <input name="newIsProtected" id="newIsProtected" type="checkbox" value="1" '.($row["protected"]?"checked":'').' '.($row['cat']?"disabled":'').'>
  <br>
  '.$MyFav_AddMod_ParentCategory.'
  <select name="catid" id="catid" onChange="detSel();" '.($row['cat']?"disabled":'').'>';

$qry="SELECT id,name FROM Fav WHERE cat = 1 ORDER BY ord,id";
$rs=sqlite_query($conn,$qry);
echo '    <option value="0">'.$MyFav_Cat_NoCategory.'</option>';
  while($row2 = sqlite_fetch_array($rs)) {
echo '    <option value="'.($row2["id"]).'" '.($row2["id"]==$row["catid"]?"selected":'').'>'.($row2["name"]).'</option>';
  } 
echo '  </select>
  <input type="submit" name="Submit" value="'.$MyFav_AddMod_Submit.'">
  <input type="hidden" name="MM_insert" value="form1">
  <input type="hidden" name="id" value="'.($row["id"]).'">
  '.$MyFav_BackHTML.'
</form>';
} elseif ($rcnt!=1) {
echo $MyFav_NotFound.'<br><center>'.$MyFav_BackHTML.'</center>';
} else {
echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
}
require("./fav_footer.htm");
?>
</body>
</html>
