﻿<%@ Page Language="vb" AutoEventWireup="false" CodeBehind="AutoLogin.aspx.vb" Inherits="VBASPNETAutoLogin.AutoLogin" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>自动登录页面</title>

</head>
<body>
    <form id="form1" runat="server">
    <div>
     <table cellspacing="0" cellpadding="1" border="0" id="Login1" style="border-collapse:collapse;">

	<tr>

		<td><table cellpadding="0" border="0">

			<tr>

				<td align="center" colspan="2">登录</td>

			</tr><tr>
                

				<td align="right"><label for="UserName">用户名:</label></td><td><asp:TextBox ID="btnUserName" runat="server"> </asp:TextBox></td>

			</tr><tr>
			
                

				<td align="right"><label for="Password">密码:</label></td><td>
                            <asp:TextBox ID="btnPassword" runat="server" TextMode="Password" ></asp:TextBox></td>


			</tr><tr>
				<td align="right" colspan="2"><asp:Button ID="Login1" runat="server" 
                        Text="自动登录"  /></td>
			</tr>

		</table>
		
		</td>

	</tr>

</table>
</div>
    </form>
</body>
</html>