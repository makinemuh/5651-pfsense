<?php
/*
	vpn_pptp_users.php
	part of m0n0wall (http://m0n0.ch/wall)
	
	Copyright (C) 2003-2005 Manuel Kasper <mk@neon1.net>.
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

require("guiconfig.inc");

if (!is_array($config['pptpd']['user'])) {
	$config['pptpd']['user'] = array();
}
pptpd_users_sort();
$a_secret = &$config['pptpd']['user'];

if ($_POST) {

	$pconfig = $_POST;

	if ($_POST['apply']) {
		$retval = 0;
		config_lock();
		$retval = vpn_setup();
		config_unlock();
		$savemsg = get_std_save_message($retval);
		if ($retval == 0) {
			if (file_exists($d_pptpuserdirty_path))
				unlink($d_pptpuserdirty_path);
		}
	}
}

if ($_GET['act'] == "del") {
	if ($a_secret[$_GET['id']]) {
		unset($a_secret[$_GET['id']]);
		write_config();
		touch($d_pptpuserdirty_path);
		header("Location: vpn_pptp_users.php");
		exit;
	}
}

$pgtitle = "VPN: PPTP: Kullanıcılar";
include("head.inc");

?>

<body link="#0000CC" vlink="#0000CC" alink="#0000CC">
<?php include("fbegin.inc"); ?>
<p class="pgtitle"><?=$pgtitle?></p>
<form action="vpn_pptp_users.php" method="post">
<?php if ($savemsg) print_info_box($savemsg); ?>
<?php if (isset($config['pptpd']['radius']['enable']))
	print_info_box("UYARI: RADIUS şu anda aktiftir. Yerel kullanıcı veritabanı kullanılmayacaktır."); ?>
<?php if (file_exists($d_pptpuserdirty_path)): ?><p>
<?php print_info_box_np("PPTP kullanıcı listesi değiştirildi.<br>Değişikliklerin çalışması için uygula seçilmelidir.<br><b>Uyarı: Bütün PPTP bağlantıları sonlandırılacaktır!</b>");?><br>
<?php endif; ?>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr><td class="tabnavtbl">
<?php
	$tab_array = array();
	$tab_array[0] = array("Configuration", false, "vpn_pptp.php");
	$tab_array[1] = array("Users", true, "vpn_pptp_users.php");
	display_top_tabs($tab_array);
?>    </td></tr>
  <tr> 
	<td>
        <div id="mainarea">
              <table class="tabcont" width="100%" border="0" cellpadding="6" cellspacing="0">
                <tr> 
                  <td class="listhdrr">Kullanıcı Adı</td>
                  <td class="listhdr">IP adresi</td>
                  <td class="list">
			<table border="0" cellspacing="0" cellpadding="1">
			   <tr>
				<td width="17"></td>
				<td><a href="vpn_pptp_users_edit.php"><img src="./themes/<?= $g['theme']; ?>/images/icons/icon_plus.gif" title="add user" width="17" height="17" border="0"></a></td>
			   </tr>
			</table>
		  </td>
		</tr>
			  <?php $i = 0; foreach ($a_secret as $secretent): ?>
                <tr> 
                  <td class="listlr">
                    <?=htmlspecialchars($secretent['name']);?>
                  </td>
                  <td class="listr">
                    <?=htmlspecialchars($secretent['ip']);?>&nbsp;
                  </td>
                  <td class="list" nowrap> <a href="vpn_pptp_users_edit.php?id=<?=$i;?>"><img src="./themes/<?= $g['theme']; ?>/images/icons/icon_e.gif" title="edit user" width="17" height="17" border="0"></a>
                     &nbsp;<a href="vpn_pptp_users.php?act=del&id=<?=$i;?>" onclick="return confirm('Bu kullanıcıyı silmek istediğinizden emin misiniz?')"><img src="./themes/<?= $g['theme']; ?>/images/icons/icon_x.gif" title="kullanıcıyı sil" width="17" height="17" border="0"></a></td>
				</tr>
			  <?php $i++; endforeach; ?>
                <tr> 
                  <td class="list" colspan="2"></td>
                  <td class="list">
			<table border="0" cellspacing="0" cellpadding="1">
			   <tr>
				<td width="17"></td>
				<td><a href="vpn_pptp_users_edit.php"><img src="./themes/<?= $g['theme']; ?>/images/icons/icon_plus.gif" title="Kullanıcı ekle" width="17" height="17" border="0"></a></td>
			   </tr>
			</table>
		  </td>
		</tr>
              </table>
</div>
			</td>
	</tr>
</table>
</form>
<?php include("fend.inc"); ?>
</body>
</html>
