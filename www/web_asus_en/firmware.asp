<!--
Copyright 2001, Broadcom Corporation
All Rights Reserved.

This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
the contents of this file may not be disclosed to third parties, copied or
duplicated in any form, in whole or in part, without the prior written
permission of Broadcom Corporation.

$Id: firmware.asp,v 1.2 2002/04/18 23:22:46 mhuang Exp $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Firmware</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
</head>

<body>
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>

<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr>
    <td colspan="2" class="navbar"><map name="FPMap1">
      <area href="index.asp" shape="rect" coords="0, 0, 100, 20" alt="Basic">
      <area href="status.asp" shape="rect" coords="100, 0, 200, 20" alt="Status">
      <area href="filter.asp" shape="rect" coords="200, 0, 300, 20" alt="Filters">
      <area href="forward.asp" shape="rect" coords="300, 0, 400, 20" alt="Routing">
      <area href="wireless.asp" shape="rect" coords="400, 0, 500, 20" alt="Wireless">
      <area href="firmware.asp" shape="rect" coords="500, 0, 600, 20" alt="Firmware">
      <area href="internal.asp" shape="rect" coords="600, 0, 700, 20" alt="Internal">
    </map><img border="0" src="btn.gif" usemap="#FPMap1" alt="Navigation Bar"></td>
  </tr>
  <tr>
    <td colspan="2" class="edge"><img border="0" src="blur_new.jpg" alt=""></td>
  </tr>
  <tr>
    <td><img border="0" src="logo_new.gif" alt=""></td>
    <td width="100%" valign="top">
	<br>
	<span class="title">FIRMWARE</span><br>
	<span class="subtitle">This page allows you to upgrade the
	firmware.</span>
    </td>
  </tr>
</table>

<form method="post" action="upgrade.cgi" enctype="multipart/form-data">

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310">
	<a href="javascript:void(0);"
	onMouseOver="return overlib('Displays the current version of the router firmware.');"
	onMouseOut="return nd();">	
	Current Version:&nbsp;&nbsp;
	</a>
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><% nvram_get("os_version"); %></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310">
	<a href="javascript:void(0);"
	onMouseOver="return overlib('Selects the new firmware to upload to the router.');"
	onMouseOut="return nd();">	
	New Firmware:&nbsp;&nbsp;
	</a>
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input type="file" name="file"></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="310"></td>
    <td>&nbsp;&nbsp;</td>
    <td><input type="submit" value="Upgrade"></td>
  </tr>
</table>

</form>

<p class="label">&#169;2001 Broadcom Corporation. All rights reserved. 

</body>
</html>
