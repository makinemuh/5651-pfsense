<?php
/* $Id$ */
/*
	guiconfig.inc
	by Scott Ullrich, Copyright 2004, All rights reserved.
	originally based on of m0n0wall (http://m0n0.ch/wall)

	Copyright (C) 2003-2004 Manuel Kasper <mk@neon1.net>.
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

/* make sure nothing is cached */
if (!$omit_nocacheheaders) {
	header("Expires: 0");
	header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
	header("Cache-Control: no-store, no-cache, must-revalidate");
	header("Cache-Control: post-check=0, pre-check=0", false);
	header("Pragma: no-cache");
}

/* Include authentication routines */
/* THIS MUST BE ABOVE ALL OTHER CODE */
require_once("auth.inc");

/* parse the configuration and include all configuration functions */
require_once("config.inc");
require_once("functions.inc");

/*
 *   if user has selected a custom template, use it.
 *   otherwise default to pfsense tempalte
 */
if($config['theme'] <> "")
        $g['theme'] = $config['theme'];
else
        $g['theme'] = "avardar";

$apple_ua = array("iPhone","iPod");
foreach($apple_ua as $useragent)
	if(strstr($_SERVER['HTTP_USER_AGENT'], $useragent))
		$g['theme'] = "avardar";

$d_landirty_path = $g['varrun_path'] . "/lan.conf.dirty";
$d_pppoeuserdirty_path = $g['varrun_path'] . "/vpn-pppoe-users-edit.dirty";
$d_hostsdirty_path = $g['varrun_path'] . "/hosts.dirty";
$d_natconfdirty_path = $g['varrun_path'] . "/nat.conf.dirty";
$d_filterconfdirty_path = $g['varrun_path'] . "/filter.conf.dirty";
$d_ipsecconfdirty_path = $g['varrun_path'] . "/ipsec.conf.dirty";
$d_shaperconfdirty_path = $g['varrun_path'] . "/shaper.conf.dirty";
$d_pptpuserdirty_path = $g['varrun_path'] . "/pptpd.user.dirty";
$d_dnsmasqdirty_path = $g['varrun_path'] . "/dnsmasq.dirty";
$d_staticmapsdirty_path = $g['varrun_path'] . "/staticmaps.dirty";
$d_staticroutesdirty_path = $g['varrun_path'] . "/staticroutes.dirty";
$d_aliasesdirty_path = $g['varrun_path'] . "/aliases.dirty";
$d_proxyarpdirty_path = $g['varrun_path'] . "/proxyarp.dirty";
$d_fwupenabled_path = $g['varrun_path'] . "/fwup.enabled";
$d_firmwarelock_path = $g['varrun_path'] . "/firmware.lock";
$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";
$d_passthrumacsdirty_path = $g['varrun_path'] . "/passthrumacs.dirty";
$d_allowedipsdirty_path = $g['varrun_path'] . "/allowedips.dirty";
$d_ovpnclidirty_path = $g['varrun_path'] . "/ovpnclient.dirty";
$d_vipconfdirty_path = $g['varrun_path'] . "/vip.conf.dirty";
$d_vsconfdirty_path = $g['varrun_path'] . "/vs.conf.dirty";
$d_shaperconfdirty_path = $g['varrun_path'] . "/shaper.conf.dirty";

/* OpenVPN Directories */
$d_ovpnsrvdirty_path = "/tmp/ovpn-srv.dirty";
$d_ovpncrldirty_path = "/tmp/ovpn-crl.dirty";
$d_ovpnclidirty_path = "/tmp/ovpn-cli.dirty";
/* used by progress bar */
$lastseen = "-1";

if (file_exists($d_firmwarelock_path)) {
	if (!$d_isfwfile) {
		header("Location: system_firmware.php");
		exit;
	} else {
		return;
	}
}

/* some well knows ports */
$wkports = array(
	5999 => "CVSup",	
	53 => "DNS",
	21 => "FTP",
	3000 => "HBCI",
	80 => "HTTP",
	443 => "HTTPS",
	5190 => "ICQ",
	113 => "IDENT/AUTH",
	143 => "IMAP",
	993 => "IMAP/S",
	4500 => "IPsec NAT-T",
	500 => "ISAKMP",
	1701 => "L2TP",
	389 => "LDAP",
	1755 => "MMS/TCP",
	7000 => "MMS/UDP",
	445 => "MS DS",
	3389 => "MS RDP",
	1512 => "MS WINS",
	1863 => "MSN",
	119 => "NNTP",
	123 => "NTP",
	138 => "NetBIOS-DGM",
	137 => "NetBIOS-NS",
	139 => "NetBIOS-SSN",
	1194 => "OpenVPN",
	110 => "POP3",
	995 => "POP3/S",
	1723 => "PPTP",	
	1812 => "RADIUS",
	1813 => "RADIUS accounting",
	5004 => "RTP",
	5060 => "SIP",
	25 => "SMTP",
	465 => "SMTP/S",
	161 => "SNMP",
	162 => "SNMP-Trap",
	22 => "SSH",
	3478 => "STUN",
	23 => "Telnet",
	69 => "TFTP",
	5900 => "VNC");

$specialnets = array("wanip" => "WAN address", "lanip" => "LAN address", "lan" => "LAN net", "pptp" => "PPTP clients");

for ($i = 1; isset($config['interfaces']['opt' . $i]); $i++) {
	$specialnets['opt' . $i] = $config['interfaces']['opt' . $i]['descr'] . " net";
}

$medias = array("auto" => "autoselect", "100full" => "100BASE-TX full-duplex",
	"100half" => "100BASE-TX half-duplex", "10full" => "10BASE-T full-duplex",
	"10half" => "10BASE-T half-duplex");

/* platforms that support firmware updating */
$fwupplatforms = array('pfSense', 'net45xx', 'net48xx', 'generic-pc', 'embedded', 'wrap', 'nanobsd');

/* IPsec defines */
$my_identifier_list = array('myaddress' => 'My IP address',
				'address' => 'IP address',
				'fqdn' => 'Domain name',
				'user_fqdn' => 'User FQDN',
				'dyn_dns' => 'Dynamic DNS');

$p1_ealgos = array('des' => 'DES', '3des' => '3DES', 'blowfish' => 'Blowfish',
		'cast128' => 'CAST128','rijndael' => 'Rijndael (AES)', 'aes 256' => 'AES-256');
$p2_ealgos = array('des' => 'DES', '3des' => '3DES', 'blowfish' => 'Blowfish',
		'cast128' => 'CAST128', 'rijndael' => 'Rijndael (AES)', 'aes 256' => 'AES-256');

$p1_halgos = array('sha1' => 'SHA1', 'md5' => 'MD5');
$p1_authentication_methods = array('pre_shared_key' => 'Pre-shared key', 'rsasig' => 'RSA signature');
$p2_halgos = array('hmac_sha1' => 'SHA1', 'hmac_md5' => 'MD5');
$p2_protos = array('esp' => 'ESP', 'ah' => 'AH');
$p2_pfskeygroups = array('0' => 'off', '1' => '1', '2' => '2', '5' => '5');

function do_input_validation($postdata, $reqdfields, $reqdfieldsn, $input_errors) {

	/* check for bad control characters */
	foreach ($postdata as $pn => $pd) {
		if (is_string($pd) && preg_match("/[\\x00-\\x08\\x0b\\x0c\\x0e-\\x1f]/", $pd)) {
			$input_errors[] = "Alan '" . $pn . "' geçersiz karakterler içeriyor.";
		}
	}

	for ($i = 0; $i < count($reqdfields); $i++) {
		if ($_POST[$reqdfields[$i]] == "") {
			$input_errors[] = "The field '" . $reqdfieldsn[$i] . "' is required.";
		}
	}
}

function print_input_errors($input_errors) {
	global $g;
	echo "<p><table border=\"0\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">\n";
	echo "<tr><td bgcolor=\"#990000\" width=\"36\" align=\"center\" valign=\"top\"><img src=\"./themes/".$g['theme']."/images/icons/icon_error.gif\" width=\"28\" height=\"32\"></td>\n";
	echo "<td bgcolor=\"#FFD9D1\" style=\"padding-left: 8px; padding-top: 6px\">";

	echo "<span class=\"errmsg\"><p>Hatalar tespit edildi:<ul>\n";
	foreach ($input_errors as $ierr) {
		echo "<li>" . htmlspecialchars($ierr) . "</li>\n";
	}
	echo "</ul></span>";

	echo "</td></tr></table></p>&nbsp;<br>";
}

function exec_rc_script($scriptname) {

	global $d_sysrebootreqd_path;

	$execoutput = "";
	$retval = "";
	exec($scriptname . " >/dev/null 2>&1", $execoutput, $retval);
	return $retval;
}

function exec_rc_script_async($scriptname) {

	global $d_sysrebootreqd_path;
	$execoutput = "";
	$retval = "";

	exec("nohup " . $scriptname . " >/dev/null 2>&1 &", $execoutput, $retval);
	return $retval;
}

function verify_gzip_file($fname) {

    $returnvar = mwexec("/usr/bin/gzip -t " . escapeshellarg($fname));
	if ($returnvar != 0)
		return 0;
	else
		return 1;
}

function print_info_box_np($msg) {
	global $g;
	echo "<table height=\"32\" width=\"100%\">\n";
	echo "  <tr>\n";
	echo "   <td>\n";
	echo "      <div style='background-color:#990000' id='redbox'>\n";
	echo "       <table width='100%'><tr><td width='8%'>\n";
	echo "        &nbsp;&nbsp;&nbsp;<img style='vertical-align:middle' src=\"/themes/".$g['theme']."/images/icons/icon_exclam.gif\" width=\"28\" height=\"32\">\n";
	echo "        </td>\n";
	echo "        <td width='70%'><font color='white'><b>{$msg}</b></font>\n";
	echo "        </td>";
	if(stristr($msg, "apply") == true) {
		echo "         <td>";
		echo "           <input name=\"apply\" type=\"submit\" class=\"formbtn\" id=\"apply\" value=\"Değişiklikleri Uygula\">\n";
		echo "         </td>";
	}
	echo "        </tr></table>\n";
	echo "       </div>\n";
	echo "    </td>\n";
	echo "</table>\n";
	echo "<script type=\"text/javascript\">\n";
	echo "NiftyCheck();\n";
	echo "Rounded(\"div#redbox\",\"all\",\"#FFF\",\"#990000\",\"smooth\");\n";
	echo "Rounded(\"td#blackbox\",\"all\",\"#FFF\",\"#000000\",\"smooth\");\n";
	echo "</script>\n";
	echo "\n<br>\n";
}

function print_info_box($msg) {
	echo "<p>";
	print_info_box_np($msg);
	echo "</p>";
}

function format_bytes($bytes) {
	if ($bytes >= 1073741824) {
		return sprintf("%.2f GB", $bytes/1073741824);
	} else if ($bytes >= 1048576) {
		return sprintf("%.2f MB", $bytes/1048576);
	} else if ($bytes >= 1024) {
		return sprintf("%.0f KB", $bytes/1024);
	} else {
		return sprintf("%d bytes", $bytes);
	}
}

function get_std_save_message($ok) {
	global $d_sysrebootreqd_path;

	return "Değişiklikler başarıyla uygulandı. Durumu <a href='status_filter_reload.php'>izleyebilirsiniz</a>.";
}

function pprint_address($adr) {
	global $specialnets;

	if (isset($adr['any'])) {
		$padr = "*";
	} else if ($adr['network']) {
		if (preg_match("/opt[0-999]ip/", $adr['network'])) {
			$padr = "Interface IP address";
		} else {
			$padr = $specialnets[$adr['network']];
		}
	} else {
		$padr = $adr['address'];
	}

	if (isset($adr['not']))
		$padr = "! " . $padr;

	return $padr;
}

function pprint_port($port) {
	global $wkports;

	$pport = "";

	if (!$port)
		return "*";
	else {
		$srcport = explode("-", $port);
		if ((!$srcport[1]) || ($srcport[0] == $srcport[1])) {
			$pport = $srcport[0];
			if ($wkports[$srcport[0]]) {
				$pport .= " (" . $wkports[$srcport[0]] . ")";
			}
		} else
			$pport .= $srcport[0] . " - " . $srcport[1];
	}

	return $pport;
}

function captiveportal_users_sort() {
	global $g, $config;

	function cpusercmp($a, $b) {
		return strcasecmp($a['name'], $b['name']);
	}

	usort($config['captiveportal']['user'], "cpusercmp");
}

/* sort by interface only, retain the original order of rules that apply to
   the same interface */
function filter_rules_sort() {
	global $config;

	/* mark each rule with the sequence number (to retain the order while sorting) */
	for ($i = 0; isset($config['filter']['rule'][$i]); $i++)
		$config['filter']['rule'][$i]['seq'] = $i;

	function filtercmp($a, $b) {
		if ($a['interface'] == $b['interface'])
			return $a['seq'] - $b['seq'];
		else
			return -strcmp($a['interface'], $b['interface']);
	}

	usort($config['filter']['rule'], "filtercmp");

	/* strip the sequence numbers again */
	for ($i = 0; isset($config['filter']['rule'][$i]); $i++)
		unset($config['filter']['rule'][$i]['seq']);
}

function nat_rules_sort() {
	global $config;

	function natcmp($a, $b) {
		if ($a['external-address'] == $b['external-address']) {
			if ($a['protocol'] == $b['protocol']) {
				if ($a['external-port'] == $b['external-port']) {
					return 0;
				} else {
					return ($a['external-port'] - $b['external-port']);
				}
			} else {
				return strcmp($a['protocol'], $b['protocol']);
			}
		} else if (!$a['external-address'])
			return 1;
		else if (!$b['external-address'])
			return -1;
		else
			return ipcmp($a['external-address'], $b['external-address']);
	}

	usort($config['nat']['rule'], "natcmp");
}

function nat_1to1_rules_sort() {
	global $g, $config;

	function nat1to1cmp($a, $b) {
		return ipcmp($a['external'], $b['external']);
	}

	usort($config['nat']['onetoone'], "nat1to1cmp");
}

function nat_server_rules_sort() {
	global $g, $config;

	function natservercmp($a, $b) {
		return ipcmp($a['ipaddr'], $b['ipaddr']);
	}

	usort($config['nat']['servernat'], "natservercmp");
}

function nat_out_rules_sort() {
	global $g, $config;

	function natoutcmp($a, $b) {
		return strcmp($a['source']['network'], $b['source']['network']);
	}

	usort($config['nat']['advancedoutbound']['rule'], "natoutcmp");
}

function pptpd_users_sort() {
	global $g, $config;

	function usercmp($a, $b) {
		return strcasecmp($a['name'], $b['name']);
	}

	usort($config['pptpd']['user'], "usercmp");
}

function pppoe_users_sort() {
	global $g, $config;

	function usercmp($a, $b) {
		return strcasecmp($a['name'], $b['name']);
	}

	usort($config['pppoe']['user'], "usercmp");
}

function staticroutes_sort() {
	global $g, $config;

	function staticroutecmp($a, $b) {
		return strcmp($a['network'], $b['network']);
	}

	usort($config['staticroutes']['route'], "staticroutecmp");
}

function hosts_sort() {
	global $g, $config;

	function hostcmp($a, $b) {
		return strcasecmp($a['host'], $b['host']);
	}

	usort($config['dnsmasq']['hosts'], "hostcmp");
}

function staticmaps_sort($if) {
	global $g, $config;

	function staticmapcmp($a, $b) {
		return ipcmp($a['ipaddr'], $b['ipaddr']);
	}

	usort($config['dhcpd'][$if]['staticmap'], "staticmapcmp");
}

function aliases_sort() {
	global $g, $config;

	function aliascmp($a, $b) {
		return strcmp($a['name'], $b['name']);
	}

	usort($config['aliases']['alias'], "aliascmp");
}

function schedule_sort(){
	global $g, $config;

	function schedulecmp($a, $b) {
		return strcmp($a['name'], $b['name']);
	}

	usort($config['schedules']['schedule'], "schedulecmp");

}

function ipsec_mobilekey_sort() {
	global $g, $config;

	function mobilekeycmp($a, $b) {
		return strcmp($a['ident'][0], $b['ident'][0]);
	}

	usort($config['ipsec']['mobilekey'], "mobilekeycmp");
}

function proxyarp_sort() {
	global $g, $config;

	function proxyarpcmp($a, $b) {
		if (isset($a['network']))
			list($ast,$asn) = explode("/", $a['network']);
		else if (isset($a['range'])) {
			$ast = $a['range']['from'];
			$asn = 32;
		}
		if (isset($b['network']))
			list($bst,$bsn) = explode("/", $b['network']);
		else if (isset($b['range'])) {
			$bst = $b['range']['from'];
			$bsn = 32;
		}
		if (ipcmp($ast, $bst) == 0)
			return ($asn - $bsn);
		else
			return ipcmp($ast, $bst);
	}

	usort($config['proxyarp']['proxyarpnet'], "proxyarpcmp");
}

function passthrumacs_sort() {
	global $g, $config;

	function passthrumacscmp($a, $b) {
		return strcmp($a['mac'], $b['mac']);
	}

	usort($config['captiveportal']['passthrumac'],"passthrumacscmp");
}

function cpelements_sort() {
	global $g, $config;

	function cpelementscmp($a, $b) {
		return strcasecmp($a['name'], $b['name']);
	}

	usort($config['captiveportal']['element'],"cpelementscmp");
}

function allowedips_sort() {
	global $g, $config;

	function allowedipscmp($a, $b) {
		return strcmp($a['ip'], $b['ip']);
	}

	usort($config['captiveportal']['allowedip'],"allowedipscmp");
}

function wol_sort() {
	global $g, $config;

	function wolcmp($a, $b) {
		return strcmp($a['descr'], $b['descr']);
	}

	usort($config['wol']['wolentry'], "wolcmp");
}

function sort_rule_by_queue_priority() {
	global $g, $config;
	global $queues;

	function rqpcmp($a, $b) {
		global $queues;

		$ra = $queues[$a['outqueue']];
		$rb = $queues[$b['outqueue']];

		if ($ra == $rb)
			return 0;
		/* reverse sort - we want higher priority first */
		return ($ra < $rb) ? 1 : -1;
	}

	foreach($config['shaper']['queue'] as $q) {
		$queues[$q['name']] = $q['priority'];
	}

	usort($config['shaper']['rule'], "rqpcmp");
}
function gentitle($pgname) {
	global $config;
	return $config['system']['hostname'] . "." . $config['system']['domain'] . " - " . $pgname;
}

/* update the changedesc and changecount(er) variables */
function update_changedesc($update) {
	global $changedesc;
	global $changecount;

	$changedesc .= " {$update}";
	$changecount++;
}

function dump_clog($logfile, $tail, $withorig = true, $grepfor = "", $grepinvert = "") {
	global $g, $config;
    $sor = isset($config['syslog']['reverse']) ? "-r" : "";
    $logarr = "";
	$grepline = "  ";
	if(is_array($grepfor))
		foreach($grepfor as $agrep)
			$grepline .= " | grep \"$agrep\"";
	if(is_array($grepinvert))
		foreach($grepinvert as $agrep)
			$grepline .= " | grep -v \"$agrep\"";
	exec("/usr/sbin/clog {$logfile}{$grepline}| grep -v \"CLOG\" | grep -v \"\033\" | /usr/bin/tail {$sor} -n {$tail}", $logarr);
    /* comment out debug code
    echo "<!-- /usr/sbin/clog {$logfile}{$grepline}| grep -v \"CLOG\" | grep -v \"\033\" | /usr/bin/tail {$sor} -n {$tail} -->";
    */
    foreach ($logarr as $logent) {
            $logent = preg_split("/\s+/", $logent, 6);
            echo "<tr valign=\"top\">\n";
            if ($withorig) {
            		$entry_date_time = htmlspecialchars(join(" ", array_slice($logent, 0, 3)));
                    echo "<td class=\"listlr\" nowrap>" . $entry_date_time  . "</td>\n";
                    echo "<td class=\"listr\">" . htmlspecialchars($logent[4] . " " . $logent[5]) . "</td>\n";
            } else {
                    echo "<td class=\"listlr\" colspan=\"2\">" . htmlspecialchars($logent[5]) . "</td>\n";
            }
            echo "</tr>\n";
    }
}

function return_clog($logfile, $tail, $grepfor = "", $grepinvert = "", $grepreverse = false) {
	global $g, $config;
	$sor = isset($config['syslog']['reverse']) ? "-r" : "";
	$logarr = "";
	$grepline = "  ";
	if(is_array($grepfor))
		foreach($grepfor as $agrep)
			$grepline .= " | grep \"$agrep\"";
	if(is_array($grepinvert))
		foreach($grepinvert as $agrep)
			$grepline .= " | grep -v \"$agrep\"";
	/* comment out debug code
	echo "<!-- /usr/sbin/clog {$logfile}{$grepline}| grep -v \"CLOG\" | grep -v \"\033\" | /usr/bin/tail {$sor} -n {$tail} -->";
	*/
	exec("/usr/sbin/clog {$logfile}{$grepline}| grep -v \"CLOG\" | grep -v \"\033\" | /usr/bin/tail {$sor} -n {$tail}", $logarr);
	return($logarr);
}

/* Check if variable has changed, update and log if it has
 * returns true if var changed
 * varname = variable name in plain text
 * orig = original value
 * new = new value
 */
function update_if_changed($varname, & $orig, $new) {
	if (is_array($orig) && is_array($new)) {
		$a_diff = array_diff($orig, $new);
		foreach ($a_diff as $diff) {
			update_changedesc("removed {$varname}: \"{$diff}\"");
		}
		$a_diff = array_diff($new, $orig);
		foreach ($a_diff as $diff) {
			update_changedesc("added {$varname}: \"{$diff}\"");
		}
		$orig = $new;
		return true;

	} else {
		if ($orig != $new) {
			update_changedesc("{$varname}: \"{$orig}\" -> \"{$new}\"");
			$orig = $new;
			return true;
		}
	}
	return false;
}

function address_to_pconfig($adr, &$padr, &$pmask, &$pnot, &$pbeginport, &$pendport) {

        if (isset($adr['any']))
                $padr = "any";
        else if ($adr['network'])
                $padr = $adr['network'];
        else if ($adr['address']) {
                list($padr, $pmask) = explode("/", $adr['address']);
                if (!$pmask)
                        $pmask = 32;
        }

        if (isset($adr['not']))
                $pnot = 1;
        else
                $pnot = 0;

        if ($adr['port']) {
                list($pbeginport, $pendport) = explode("-", $adr['port']);
                if (!$pendport)
                        $pendport = $pbeginport;
        } else {
                if(alias_expand($pbeginport) <> "" || alias_expand($pendport) <> "") {
                        /* Item is a port alias */
                } else {
                        $pbeginport = "any";
                        $pendport = "any";
                }
        }
}

function pconfig_to_address(&$adr, $padr, $pmask, $pnot=false, $pbeginport=0, $pendport=0) {

        $adr = array();

        if ($padr == "any")
                $adr['any'] = true;
        else if (is_specialnet($padr))
                $adr['network'] = $padr;
        else {
                $adr['address'] = $padr;
                if ($pmask != 32)
                        $adr['address'] .= "/" . $pmask;
        }

        if ($pnot)
                $adr['not'] = true;
        else
                unset($adr['not']);

        if (($pbeginport != 0) && ($pbeginport != "any")) {
                if ($pbeginport != $pendport)
                        $adr['port'] = $pbeginport . "-" . $pendport;
                else
                        $adr['port'] = $pbeginport;
        }

        if(alias_expand($pbeginport)) {
                $adr['port'] = $pbeginport;
        }
}

function is_specialnet($net) {
        global $specialsrcdst;

        if (in_array($net, $specialsrcdst) || strstr($net, "opt"))
                return true;
        else
                return false;
}

function ipsec_ca_sort() {
        global $g, $config;

        function ipseccacmp($a, $b) {
                return strcmp($a['ident'], $b['ident']);
        }

        usort($config['ipsec']['cacert'], "ipseccacmp");
}


?>
