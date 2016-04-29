<?php
echo "Hello";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Cache-Control" CONTENT="no-cache, must-revalidate">
<meta HTTP-EQUIV="pragma" CONTENT="no-cache"> 
<title>positive</title>
<link href="css/global.css" type="text/css" rel="stylesheet">
<!--[if IE 6]>
<script src="js/DD_belatedPNG.js" type="text/javascript"></script>
<script type="text/javascript">
DD_belatedPNG.fix('*');
document.execCommand("BackgroundImageCache", false, true);
</script> 
<![endif]-->
<style>
#msg img { margin-right:10px;*position: relative;*top: -50%;*left: 0;vertical-align: middle;border: none;}
</style>
<script type="text/javascript" src="js/jquery-1.7.2.js"></script>
<script type="text/javascript">
<!--
$(document).ready(function(){
    $("#username").focus();

    $("#Submit").click(function () {
        if ($("#username").val().length == 0)
        {
            $("#msg").html("<img src='images/th.png'>«Î ‰»Î”√ªß√˚£°");
            $("#username").focus();
            return false;
        }
        if ($("#password").val().length == 0)
        {
            $("#msg").html("<img src='images/th.png'>«Î ‰»Î√‹¬Î£°");
            $("#password").focus();
            return false;
        }

    document.loginForm.submit();
    return false;
    });

    $(document).keydown(function (event) {
        if (event.keyCode == 13) $("#Submit").click();
    });
});
//-->
</script>
<META name=GENERATOR content="MSHTML 8.00.7600.16671"></HEAD>
<BODY style="BACKGROUND: url(images/loginbg.jpg) #e5f1fc repeat-x 50% top;overflow-y: hidden;">
<div style="margin:0 auto; clear:both; background: url(images/logbg1.png) no-repeat top; height:600px; text-align:center; padding-top:170px;">
<div style="width:522px; height:321px; margin:0 auto;background: url(images/loginbg2.jpg) no-repeat top; clear:both;">
  <table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">
    <form action="" method="post" name="loginForm" id="loginForm" target="_self">
      <tr>
        <td height="80">&nbsp;</td>
      </tr>
      <tr>
        <td width="364" height="35" align="right" style="padding-left:5px;"></td>
        </tr>
      <tr>
        <td height="35" align="right" style="padding-left:180px;"><table width="100%" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td height="40" align="right" style="font-size:16px; color:#022f68; font-weight:bold;">’ ∫≈£∫</td>
              <td align="left"><input type="text" name="username" id="username" style="height:28px; width:250px;" /></td>
            </tr>
            <tr>
              <td height="25" align="right">&nbsp;</td>
              <td align="left" id="msg" style="color:#F00;"><?php echo $error; ?>
            <?=$_SESSION['userid']?></td>
            </tr>
            <tr>
              <td height="43" align="right"><span style="font-size:16px; color:#022f68; font-weight:bold;">√‹¬Î£∫</span></td>
              <td align="left"><input type="password" name="password" id="password" style="height:28px; width:250px;" />
                  <input name="blue_base" type="hidden" value="<?=$blue_base?>" />
                  <input name="blue_sql" type="hidden" value="<?=$blue_sql?>" />
                  <input name="blue_data" type="hidden" value="<?=$blue_data?>" />
                  <input name="maxsite" type="hidden" value="<?=$maxsite?>" /></td>
            </tr>
            <tr>
              <td height="60" align="right">&nbsp;</td>
              <td align="left" valign="bottom"><input type="button" id="Submit" name="Submit" value="µ«¬Ω" class="submitstyle"></td>
            </tr>
        </table></td>
        </tr>
    </form>
  </table>
</div>
  </div>
<div id="footer"></div>

</BODY></HTML>