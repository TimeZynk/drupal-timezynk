<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://localhost/NUnitAsp/web/dtd/xhtml1-transitional.dtd">
<HTML>
    <HEAD>
        <title>My Intelliplan</title>
        <meta charset="iso-8859-1" http-equiv="Content-Type" content="text/html">
        <meta content="Microsoft Visual Studio 7.0" name="GENERATOR">
        <meta content="C#" name="CODE_LANGUAGE">
        <meta content="JavaScript" name="vs_defaultClientScript">
        <meta content="http://schemas.microsoft.com/intellisense/ie5" name="vs_targetSchema">

        <LINK href="../css/Portal.css" type="text/css" rel="stylesheet">
        <link
href="../css/Style.css" type=text/css rel=STYLESHEET>
        <script language="javascript">
            var FListBox;
            var myFocus;

            function OpenExpense(AListBox, AUrl) {
                FListBox = AListBox;
                var myWindow;

                myWindow = window.open(AUrl, "NewAbstract", "width=600,height=400,status=false,resizable=yes,toolbar=no,status=no,scrollbars=yes");
                myWindow.focus();
            }

            function AddOptions(AArray, ACount) {
                // Clear List
                for (var I=0; I<=FListBox.options.length -1; I++)
                    FListBox.options[I] = null;
                for (var I=0; I<=FListBox.options.length -1; I++)
                    FListBox.options[I] = null;


                if (ACount > -1)
                {
                    for (var I=0; I<=ACount; I++)
                    {
                        FListBox.options[FListBox.options.length] = new Option(AArray[I], "");
                    }
                }
                // Add Default text
                FListBox.options[FListBox.options.length] = new Option("[Klicka här]", "");
            }


            function PostForm() {
                if (document.forms[0].DoPost.value == 'true')
                {
                    return true;
                }
                else
                {
                    document.forms[0].DoPost.value = 'true';
                    myFocus.focus();

                    var Index = -1;
                    I = 0;
                    Found = false;
                    IsEnb = true;

                    while (I < myFocus.form.length && Index == -1)
                    {
                        if (myFocus.form[I] == myFocus)
                            Index = I;
                        else
                            I++;
                    }

                    // Check if it's the last input
                    if (myFocus.form[(Index +1)] != null)
                    {
                        // Check if it's an Expense box
                        if (myFocus.form[(Index +1)].id == "Expense")
                            myFocus.form[(Index +3)].focus();
                        else
                        {
                            while (IsEnb)
                            {
                                Index++;
                                if (!myFocus.form[Index].disabled)
                                    IsEnb = false;
                            }
                            // Check if it's the last input
                            if (myFocus.form[Index] != null)
                                myFocus.form[Index].focus();
                        }
                    }
                    return false;
                }
            }

            function OnKeyDown() {
                myFocus = document.activeElement;


                if (event.keyCode == '13')
                    document.forms[0].DoPost.value = 'false';
                else
                    document.forms[0].DoPost.value = 'true';
            }

            function SetStatus(AText)
            {
                window.status = AText;
            }
        </script>
    </HEAD>
    <body ms_positioning="FlowLayout">
        <form name="TimeReport" method="post" action="TimeReport.aspx?DateInWeek=<? print $DateInWeek ?>" id="TimeReport" onkeydown="OnKeyDown();" onsubmit="return PostForm();">
<input type="hidden" name="__EVENTTARGET" value="" />
<input type="hidden" name="__EVENTARGUMENT" value="" />
<input type="hidden" name="__VIEWSTATE" value="dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPEpvaGFuIEhlYW5kZXI7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDw0NTs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjs+O2w8cDwgW1Zpc2EgYWxsYSB1cHBkcmFnXSA7MD47cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIExhZ2VyYXJiZXRhcmU7NTk4Mz47Pj47bDxpPDA+Oz4+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDQ+Oz4+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDM+Oz4+O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDN4cHMwYmg3JTJmWnY4NGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDN4cHMwYmg3JTJmWnY4NGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPFxlO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMFA5dzBjWE54WURNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDBQOXcwY1hOeFlETTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDBlSnh1S2h3ZDREczRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgwZUp4dUtod2Q0RHM0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjs+Pjt0PDtsPGk8MTM+O2k8MTU+Oz47bDx0PHQ8O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMDgvMTEgOzIwMTAtMTEtMDg+O3A8dGksIDA5LzExIDsyMDEwLTExLTA5PjtwPG9uLCAxMC8xMSA7MjAxMC0xMS0xMD47cDx0bywgMTEvMTEgOzIwMTAtMTEtMTE+O3A8ZnIsIDEyLzExIDsyMDEwLTExLTEyPjtwPGzDtiwgMTMvMTEgOzIwMTAtMTEtMTM+O3A8c8O2LCAxNC8xMSA7MjAxMC0xMS0xND47Pj47Pjs7Pjt0PHQ8O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+O2k8OD47aTw5PjtpPDEwPjtpPDExPjtpPDEyPjtpPDEzPjtpPDE0PjtpPDE1PjtpPDE2PjtpPDE3PjtpPDE4PjtpPDE5PjtpPDIwPjtpPDIxPjtpPDIyPjtpPDIzPjtpPDI0PjtpPDI1PjtpPDI2PjtpPDI3PjtpPDI4PjtpPDI5PjtpPDMwPjtpPDMxPjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIExhZ2VyYXJiZXRhcmU7NTk4Mz47cDwtLS07LTE+O3A8VsOlcmQgYXYgbsOkcnN0w6VlbmRlIFw+NWRhZ2FyO19BQ19Ww6VyZCBhdiBuw6Ryc3QuIFw+NWRncj47cDxWw6VyZCBhdiBuw6Ryc3TDpWVuZGUgXDw2IGRhZ2FyO19BQ19Ww6VyZCBhdiBuw6Ryc3QuIFw8NmRncj47cDxWw6VyZCBhdiBiYXJuIG1lZCB0aWxsZsOkbGxpZyBmw7Zyw6RsZHJhcGVubmluZztfQUNfVsOlcmQgYXYgYmFybiB0LmZwPjtwPFV0dGFnIGtvbXAvZmxleDtfQUNfVXR0YWcga29tcC9mbGV4PjtwPFV0dGFnIGV4dHJhIGZyaWRhZyBuYXRpb25hbGRhZ2VuO19BQ19VdHRhZyBleHRyYSBmcmlkYWcgPjtwPFV0dGFnIGFyYmV0c3RpZHNmw7Zya29ydG5pbmc7X0FDX1V0dC4gYXJiZXRzdGlkc2bDtnJrLj47cDxVdGJpbGRuaW5nIG1lZCBsw7ZuO19BQ19VdGJpbGRuaW5nIG0gbMO2bj47cDxUasOkbnN0bGVkaWcgbWVyIMOkbiA1IGRhZ2FyLCBhcmJldGFyZSBvY2ggdGptO19BQ19UasOkbnN0bGVkaWcgXD41IGRnciA+O3A8VGrDpG5zdGxlZGlnICBtaW5kcmUgw6RuIDYgZGFnYXIsIGFyYmV0YXJlIG9jaCB0am07X0FDX1Rqw6Ruc3RsZWRpZyBcPDYgZGdyID47cDxUasOkbnN0bGVkaWcgZGVsIGF2IGRhZywgdGptO19BQ19UamwuZGVsIGF2IGRhZywgdGptPjtwPFRqw6Ruc3RsZWRpZyBkZWwgYXYgZGFnLCBrb2xsO19BQ19UamwuIGRlbCBhdiBkYWcsIGFyYj47cDxTdHVkaWVsZWRpZ2hldCB1dGFuIGzDtm47X0FDX1N0dWRpZWxlZGlnIHV0YW4gbMO2bj47cDxTanVrZnLDpW52YXJvIHRqw6Ruc3RlbcOkbiBtw6VuYWRzYW5zdMOkbGxkO19BQ19TanVrIHRqbSAobcOlbi5sw7ZuKT47cDxTanVrZnLDpW52YXJvIGtvbGxla3RpdmFyZSBtw6VuYWRzYW5zdMOkbGxkO19BQ19TanVrIGFyYiAobcOlbi5sw7ZuKT47cDxTanVrZnLDpW52YXJvIHRpbWFuc3TDpGxsZDtfQUNfU2p1ayAodGltYXZsKT47cDxTZW4gYW5rb21zdDtfQUNfU2VuIGFua29tc3Q+O3A8U2VtZXN0ZXI7X0FDX1NlbWVzdGVyPjtwPFBlcm1pc3Npb247X0FDX1Blcm1pc3Npb24+O3A8UGFwcGFkYWdhciAoMTAgZGdyKTtfQUNfUGFwcGFkYWdhcj47cDxPZ2lsdGlnIGZyw6VudmFybztfQUNfT2dpbHRpZyBmcsOlbnZhcm8+O3A8T2JldCBzZW07X0FDX09iZXRhbGQgc2VtZXN0ZXI+O3A8T2JlbGFnZCB0aWQ7X0FDX09iZWxhZ2QgdGlkPjtwPEludHJvZHVrdGlvbiBtZWQgbMO2bjtfQUNfSW50cm9kdWt0aW9uIG0gbMO2bj47cDxIYXZhbmRlc2thcHNsZWRpZ2hldCBkYWdhdmRyYWc7X0FDX0hhdmFuZGVza2FwPjtwPEbDtnLDpGxkcmFsZWRpZyBtZXIgw6RuIDUgZGFnYXI7X0FDX0bDtnLDpGxkcmFsZWRpZyBcPjVkZ3I+O3A8RsO2csOkbGRyYWxlZGlnIG1pbmRyZSDDpG4gNiBkYWdhcjtfQUNfRsO2csOkbGRyYWxlZGlnIFw8NmRncj47cDxGYWNrbGlnIHV0YmlsZG5pbmcgdXRhbiBsw7ZuO19BQ19GYWNrbGlnIHV0Yi51dGFuIGzDtm4+O3A8S29uc3VsdGVuIHRhY2thciBuZWogdGlsbCB1cHBkcmFnO19BQ19BdmLDtmp0IHVwcGRyYWc+O3A8XGU7X0FDXz47Pj47Pjs7Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8VmVja2EgS2xhcjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDzDhG5kcmEgdmVja2E7bzxmPjs+Pjs+Ozs+Oz4+Oz4+O2w8T2xkUm93c1JlcGVhdGVyOl9jdGwwOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tCb3hEZWxldGU7T2xkUm93c1JlcGVhdGVyOl9jdGwxOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDE6Q2hlY2tCb3hEZWxldGU7T2xkUm93c1JlcGVhdGVyOl9jdGwyOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+" />

<script language="javascript" type="text/javascript">
<!--
    function __doPostBack(eventTarget, eventArgument) {
        var theform;
        if (window.navigator.appName.toLowerCase().indexOf("microsoft") > -1) {
            theform = document.TimeReport;
        }
        else {
            theform = document.forms["TimeReport"];
        }
        theform.__EVENTTARGET.value = eventTarget.split("$").join(":");
        theform.__EVENTARGUMENT.value = eventArgument;
        theform.submit();
    }
// -->
</script>
<script language=JavaScript>
function OpenPrintWindow() {
var l_PrintWindow;
l_PrintWindow = window.open('../TimeReport/PrintSettings.aspx', 'Print', 'width=400,height=540,status=false,resizable=yes,toolbar=no,status=no,scrollbars=yes')
l_PrintWindow.focus();
}</script>


    <script language=JavaScript>
function OpenHelpWindow() {
var myWindow;
myWindow = window.open('../Portal/Help.aspx?HelpId=TimeReport', 'Help', 'width=350,height=400,status=false,resizable=yes,toolbar=no,status=no,scrollbars=yes')
myWindow.focus();
}</script>



            <input id="xx" type="hidden" value="true" name="DoPost">
            <!-- TOP BAR START -->
            <table cellSpacing="0" cellPadding="0" width="100%" border="0">

                <tr>
                    <td id="TDMenu" width="100%">
<table width="100%" border="0" cellpadding="0" cellspacing="0" bgcolor="#ffffff">
    <tr>
        <td><img src="../Images/trans.gif" width="1" height="10" border="0"></td>
    </tr>
    <tr>
        <td width="40%" align="left" valign="middle">
            <table border="0" cellpadding="0" cellspacing="0">

                <tr>
                    <td valign="middle">
                        <img src="../Images/trans.gif" width="5" height="1" border="0">
                        <img id="_ctl24_ImageCustomer" src="../Customers/intelliplan_logo.gif" alt="" border="0" />
                    </td>
                    <td><img src="../Images/trans.gif" width="20" height="1" border="0"></td>
                    <td><font face="Verdana" size="2"><span id="_ctl24_LabeUserName"><?php print $username ?></span></font></td>
                </tr>

            </table>
        </td>
        <td width="60%" align="right" valign="middle">
            <table border="0" cellpadding="0" cellspacing="0">
                <tr>
                    <td>
                        <table border="0" cellpadding="0" cellspacing="0">
                            <tr id="_ctl24_MenuItems">
    <td>

<table cellspacing="0" cellpadding="0" border="0">
    <tr>
        <td align="middle">
            <a id="_ctl24__ctl1_HyperlinkImage" href="../Portal/Main.aspx?MId=MyPage"><img id="_ctl24__ctl1_MenuItemImage" src="../Images/Img_MyPageIco.gif" alt="" border="0" /></a>
        </td>
    </tr>
    <tr>
        <td><a id="_ctl24__ctl1_MenuItemHyperlink" class="MainMenuSmall" href="../Portal/Main.aspx?MId=MyPage">Min Sida</a></td>
    </tr>

</table>
</td>
    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>
    <td>
<table cellspacing="0" cellpadding="0" border="0">
    <tr>
        <td align="middle">
            <a id="_ctl24__ctl4_HyperlinkImage" href="../WebCV/WebCV.aspx?SequenceNr=1&amp;MId=WebCV" target="_blank"><img id="_ctl24__ctl4_MenuItemImage" src="../Images/Img_MyPageIco.gif" alt="" border="0" /></a>
        </td>
    </tr>

    <tr>
        <td><a id="_ctl24__ctl4_MenuItemHyperlink" class="MainMenuSmall" href="../WebCV/WebCV.aspx?SequenceNr=1&amp;MId=WebCV" target="_blank">CV</a></td>
    </tr>
</table>
</td>
    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>
    <td>
<table cellspacing="0" cellpadding="0" border="0">
    <tr>
        <td align="middle">

            <a id="_ctl24__ctl7_HyperlinkImage" href="TimeReport.aspx?MId=TReport"><img id="_ctl24__ctl7_MenuItemImage" src="../Images/Img_CalenderIco.gif" alt="" border="0" /></a>
        </td>
    </tr>
    <tr>
        <td><a id="_ctl24__ctl7_MenuItemHyperlink" class="MainMenuSmall" href="TimeReport.aspx?MId=TReport">Tidrapportering</a></td>
    </tr>
</table>
</td>
    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>

    <td>
<table cellspacing="0" cellpadding="0" border="0">
    <tr>
        <td align="middle">
            <a id="_ctl24__ctl10_HyperlinkImage" href="javascript:OpenPrintWindow();"><img id="_ctl24__ctl10_MenuItemImage" src="../Images/Img_PrinterIco.gif" alt="" border="0" /></a>
        </td>
    </tr>
    <tr>
        <td><a id="_ctl24__ctl10_MenuItemHyperlink" class="MainMenuSmall" href="javascript:OpenPrintWindow();">Skriv ut</a></td>

    </tr>
</table>

</td>
    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>
    <td>
<table cellspacing="0" cellpadding="0" border="0">
    <tr>
        <td align="middle">
            <a id="_ctl24__ctl13_HyperlinkImage" href="javascript:OpenHelpWindow();"><img id="_ctl24__ctl13_MenuItemImage" src="../Images/Img_Help.gif" alt="" border="0" /></a>
        </td>

    </tr>
    <tr>
        <td><a id="_ctl24__ctl13_MenuItemHyperlink" class="MainMenuSmall" href="javascript:OpenHelpWindow();">Hjälp</a></td>
    </tr>
</table>
</td>
    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>
    <td>
<table cellspacing="0" cellpadding="0" border="0">
    <tr>

        <td align="middle">
            <a id="_ctl24__ctl16_HyperlinkImage" href="../Portal/LogOut.aspx?MId=LogOut"><img id="_ctl24__ctl16_MenuItemImage" src="../Images/Img_LogOut.gif" alt="" border="0" /></a>
        </td>
    </tr>
    <tr>
        <td><a id="_ctl24__ctl16_MenuItemHyperlink" class="MainMenuSmall" href="../Portal/LogOut.aspx?MId=LogOut">Logga ut</a></td>
    </tr>
</table>
</td>

    <td><img src="../Images/trans.gif" width="10" height="1" border="0"></td>
</tr>

                        </table>
                    </td>
                    <td><img src="../Images/trans.gif" width="20" height="1" border="0"></td>
                    <td valign="middle">
                        <a href="http://www.intelliplan.se" target="_TOP">
                            <img id="_ctl24_ImageIPLogo" src="../Images/Img_IntelliplanLogoWhite.gif" alt="" border="0" /></a> <img src="../Images/trans.gif" width="5" height="1" border="0">

                    </td>
                </tr>
            </table>
        </td>
    </tr>
    <tr>
        <td><img src="../Images/trans.gif" width="1" height="4" border="0"></td>
    </tr>
</table>

</td>

                </tr>
                <tr>
                    <td width="100%"><span id="TopBar1"><table border="0" background="../Images/Img_Tonebanner.jpg" style="background-repeat: no-repeat;" bgcolor="#7aa1be" width="100%" cellpadding="0">
    <tr>
        <td valign="middle" width="8"><img src="../Images/Trans.gif" alt="" border="0" height="24" width="8" /></td>
        <td width="15" valign="middle"><img src="../Images/Img_CalenderIco.gif" alt="" border="0" /></td>
        <td width="5" valign="middle"><img src="../Images/Trans.gif" alt="" border="0" width="5" /></td>
        <td width="100%" valign="middle" class="TopBarBig"><span>Tidrapportering</span></td>

    </tr>
</table>
</span></td>
                </tr>
            </table>
            <IMG height="3" src="../Images/Trans.gif" width="1" border="0"><br>
            <!-- TOP BAR END -->
            <table style="BORDER-RIGHT: #aca899 1px solid; BORDER-TOP: #aca899 1px solid; BORDER-LEFT: #aca899 1px solid; BORDER-BOTTOM: #aca899 1px solid"
                cellSpacing="0" cellPadding="0" width="100%" border="0">
                <!-- WEEK NAVIGATION START -->
                <tr>

                    <td width="*">
                        <table style="BORDER-TOP: #ffffff 1px solid; BORDER-LEFT: #ffffff 1px solid" cellSpacing="0"
                            cellPadding="0" width="100%" border="0">
                            <tr>
                                <td width="5"><IMG height="24" src="../Images/Trans.gif" width="5" border="0"></td>
                                <!-- Previous week -->
                                <td width="13"><A href="TimeReport.aspx?DateInWeek=<?php print $previous_dateinweek?>" ><IMG height="13" src="../Images/Btn_ArrowLeft.gif" width="13" border="0"></A>
                                </td>
                                <td width="5"><IMG height="1" src="../Images/Trans.gif" width="5" border="0"></td>
                                <td class="WeeksNav" align="center" width="70"><b><span id="LabelWeek"><span id="Translatedlabel2">Vecka</span></span> <span id="LabelWeekNumber"><?php print $current_week ?></span></b></td>

                                <td width="5"><IMG height="1" src="../Images/Trans.gif" width="5" border="0"></td>
                                <!-- Next week -->
                                <td width="13"><A href="TimeReport.aspx?DateInWeek=<?php print $next_dateinweek?>" ><IMG height="13" src="../Images/Btn_ArrowRight.gif" width="13" border="0"></A>
                                </td>
                                <td width="40"><IMG height="1" src="../Images/Trans.gif" width="40" border="0"></td>
                                <td class="NavigationMedium" width="*"><select name="CustOrdersDropDown" onchange="__doPostBack('CustOrdersDropDown','')" language="javascript" id="CustOrdersDropDown" class="TimeReportListBox">
    <option selected="selected" value="0"> [Visa alla uppdrag] </option>
    <?php foreach($assignments as $assignment):?>
    <option value="<?php print $assignment->id ?>"><?php print $assignment->title ?></option>
    <?php endforeach; ?>

</select><IMG height="1" src="../Images/Trans.gif" width="20" border="0">
                                    <b><span id="Translatedlabel3">Ej klar-rapporterade veckor: </span>
                                      <?php
                                        $index = 0;
                                        foreach($unfinished_weeks as $week => $dateinweek):
                                          $prefix = "NotPrepWeeksRepeater__ctl$index";?>
                                            <a id="<?php print $prefix ?>_WeekNotPrep" class="WeeksNav" href="TimeReport.aspx?DateInWeek=<?php print $dateinweek ?>"><?php print substr($week,5,2)?></a>
                                            <span id="<?php print $prefix ?>0_LabelWeekNotPrepComma">,</span>
				      <?php endforeach;?>
                                    </b>
                                </td>
                            </tr>

                        </table>
                    </td>
                </tr>
                <!-- WEEK NAVIGATION END -->
                <tr>
                    <td width="100%" bgColor="#aca899"></td>
                </tr>
                <tr>
                    <td width="100%">

                        <table style="BORDER-TOP: #ffffff 1px solid; BORDER-LEFT: #ffffff 1px solid" cellSpacing="0"
                            cellPadding="0" width="100%" border="0">
                            <tr id="RowsSubject1">
    <td width="5"><IMG height="18" src="../Images/Trans.gif" width="5" border="0"></td>
    <td class="SubjectRow" width="68"><span id="Translatedlabel4">Dag klar</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="70"><span id="Translatedlabel5">Veckodag</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="*"><span id="Translatedlabel6">Uppdrag/Frånvaroorsak</span></td>

    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="50"><span id="Translatedlabel7">Från</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="50"><span id="Translatedlabel8">Till</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="40"><span id="Translatedlabel9">Rast</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>

    <td class="SubjectRow" width="47"><span id="Translatedlabel10">Timmar</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="50"><span id="Translatedlabel11">Övertid</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="120"><span id="Translatedlabel12">Övrigt</span></td>
    <td width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></td>
    <td class="SubjectRow" width="50"><span id="Translatedlabel13">Ta bort</span></td>

</tr>

                            <tr id="RowsSubject2">
    <td bgcolor="#aca899" colspan="20"></td>
</tr>

                            <!-- LIST ROWS START -->
                              <?php foreach ($reports as $idx => $report):
                                $id_prefix = "OldRowsRepeater__ctl$idx";
                                $name_prefix = "OldRowsRepeater:_ctl$idx";
                                $begintime = $report->get_begintime();
                                $endtime = $report->get_endtime();
                                $weekday = substr($begintime->format('D'), 0, 2);
                                ?>
                                    <tr>
                                        <td id="<?php print $id_prefix ?>_td0" colspan="20" bgcolor="#ffffff"><img src="../Images/trans.gif" width="1" height="3" border="0"></td>

                                    </tr>

                                    <tr>
                                        <td id="<?php print $id_prefix ?>_td1" width="5" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="5" height="18" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td2" width="68" class="TimeReportOldRow" valign="middle" bgcolor="#ffffff">
                                            <img id="<?php print $id_prefix ?>_ImageStatus" src="../Images/Trans.gif" alt="" border="0" />
                                            <input id="<?php print $id_prefix ?>_CheckboxDayDone" type="checkbox" name="<?php print $name_prefix ?>:CheckboxDayDone" <?php if($report->state == TZIntellitimeReport::STATE_REPORTED) { print 'checked'; }?>/></td>

                                        <td id="<?php print $id_prefix ?>_td3" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td4" width="70" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">

                                            <span id="<?php print $id_prefix ?>_LabelDate"><b><?php print $weekday; ?></b>, <?php print $begintime->format('d/m'); ?> </span></td>

                                        <td id="<?php print $id_prefix ?>_td5" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td6" width="*" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">
                                            <span id="<?php print $id_prefix ?>_LabelCustom"><?php print $report->title ?></span></td>

                                        <td id="<?php print $id_prefix ?>_td7" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td8" width="50" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">
                                            <input name="<?php print $name_prefix ?>:TextboxTimeFrom" type="text" value="<?php print $report->begin ?>" size="5" id="<?php print $id_prefix ?>_TextboxTimeFrom" class="TimeReportTextBox" /><input name="<?php print $name_prefix ?>:DateFromHidden" id="<?php print $id_prefix ?>_DateFromHidden" type="hidden" value="<?php print $report->begin ?>" /></td>

                                        <td id="<?php print $id_prefix ?>_td9" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td10" width="50" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">
                                            <input name="<?php print $name_prefix ?>:TextboxTimeTo" type="text" value="<?php print $report->end ?>" size="5" id="<?php print $id_prefix ?>_TextboxTimeTo" class="TimeReportTextBox" /><input name="<?php print $name_prefix ?>:DateToHidden" id="<?php print $id_prefix ?>_DateToHidden" type="hidden" value="<?php print $report->end ?>" /></td>

                                        <td id="<?php print $id_prefix ?>_td11" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td12" width="40" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">

                                            <input name="<?php print $name_prefix ?>:TextboxBreak" type="text" value="<?php print $report->break_duration_minutes ?>" size="3" id="<?php print $id_prefix ?>_TextboxBreak" class="TimeReportTextBox" /><input name="<?php print $name_prefix ?>:BreakHidden" id="<?php print $id_prefix ?>_BreakHidden" type="hidden" value="none" /></td>

                                        <td id="<?php print $id_prefix ?>_td13" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td14" width="47" class="TimeReportOldRow" valign="top" align="middle" bgcolor="#ffffff">
                                            <span id="<?php print $id_prefix ?>_LabelHours"><?php print $report->duration_hours ?></span></td>

                                        <td id="<?php print $id_prefix ?>_td15" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td16" width="50" class="TimeReportOldRow" align="center" valign="top" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="1" height="1" border="0">


                                            <input name="<?php print $name_prefix ?>:TextboxExplicitOvertime" type="text" value="0" size="3" id="<?php print $id_prefix ?>_TextboxExplicitOvertime" class="TimeReportTextBox" /><input name="<?php print $name_prefix ?>:OverTimeHidden" id="<?php print $id_prefix ?>_OverTimeHidden" type="hidden" value="none" /></td>

                                        <td id="<?php print $id_prefix ?>_td17" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td18" width="120" rowspan="2" class="TimeReportOldRow" valign="top" bgcolor="#ffffff">
                                            <select id="SelectExpenses" style="CURSOR: wait" name="Expense" size="3"  class="TimeReportListBox" style="width: 120px" onclick="OpenExpense(this, 'Expense.aspx?<?php print $report->id ?>');" onchange="OpenExpense(this, 'Expense.aspx?<?php print $report->id ?>');">

                                                <option>[Klicka här]</option>
                                            </select>
                                        </td>

                                        <td id="<?php print $id_prefix ?>_td19" width="8" rowspan="2" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="15" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td20" width="50" rowspan="2" class="TimeReportOldRow" valign="top" align="middle" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="1" height="1" border="0">
                                            <input id="<?php print $id_prefix ?>_CheckBoxDelete" type="checkbox" name="<?php print $name_prefix ?>:CheckBoxDelete" /></td>

                                    </tr>
                                    <tr>
                                        <td id="<?php print $id_prefix ?>_td21" width="5" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="5" height="1" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td22" width="45" class="TimeReportOldRow" valign="top" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="1" height="1" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td23" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="1" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td24" width="70" class="TimeReportOldRow" valign="middle" align="right" bgcolor="#ffffff">
                                            <?php if($comments):?>
                                            <span id="<?php print $id_prefix ?>_Translatedlabel14">Not:</span></td>
                                            <?php endif;?>

                                        <td id="<?php print $id_prefix ?>_td25" width="8" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/Trans.gif" width="8" height="1" border="0"></td>

                                        <td id="<?php print $id_prefix ?>_td26" width="*" colspan="2" class="TimeReportOldRow" bgcolor="#ffffff">
                                            <?php if($comments):?>
                                            <input name="<?php print $name_prefix ?>:TextboxNote" type="text" value="<?php print $report->comment ?>" size="40" id="<?php print $id_prefix ?>_TextboxNote" class="TimeReportListBox" /></td>
                                            <?php endif;?>

                                        <td id="<?php print $id_prefix ?>_Td27" width="*" colspan="2" class="TimeReportOldRow" bgcolor="#ffffff">
                                            <span id="<?php print $id_prefix ?>_AbsencePeriodFromLabel"></span></td>

                                        <td id="<?php print $id_prefix ?>_Td28" width="*" colspan="1" class="TimeReportOldRow" bgcolor="#ffffff">
                                            <span id="<?php print $id_prefix ?>_AbsencePeriodToLabel"></span></td>

                                        <td id="<?php print $id_prefix ?>_Td29" width="*" colspan="11" class="TimeReportOldRow" bgcolor="#ffffff"><img src="../Images/trans.gif" width="1" height="3" border="0"></asp:textbox></td>

                                    </tr>
                                    <tr>

                                        <td id="<?php print $id_prefix ?>_td30" bgcolor="#ffffff" colspan="20"><img src="../Images/trans.gif" width="1" height="3" border="0"></td>

                                    </tr>

                                    <tr>
                                        <td id="<?php print $id_prefix ?>_td34" colspan="20" bgcolor="#aca899"><img src="../Images/trans.gif" width="1" height="1" border="0"></td>

                                    </tr>
                              <?php endforeach;?>
                            <!-- LIST ROWS END -->
                            <!-- ADD NEW ROW START -->

                            <table id="AddRowPanel" cellpadding="0" cellspacing="0" border="0" width="100%"><tr><td>

                                <TR>
                                    <TD width="5"><IMG height="18" src="../Images/Trans.gif" width="5" border="0"></TD>
                                    <TD class="SubjectRow" width="123" colSpan="3"><B>
                                            <span id="Translatedlabel15">Lägg till ny rad</span></B></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="*">
                                        <span id="Translatedlabel16">Uppdrag/Frånvaroorsak</span></TD>

                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="50">
                                        <span id="Translatedlabel17">Från</span></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="50">
                                        <span id="Translatedlabel18">Till</span></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="40">

                                        <span id="Translatedlabel19">Rast</span></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="47"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="50">
                                        <span id="Translatedlabel20">Övertid</span></TD>
                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="120"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>

                                    <TD width="8"><IMG height="15" src="../Images/Img_Seperator.gif" width="8" border="0"></TD>
                                    <TD class="SubjectRow" width="50"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>
                                </TR>
                                <TR>
                                    <TD bgColor="#aca899" colSpan="20"><IMG height="1" src="../Images/trans.gif" width="1" border="0"></TD>
                                </TR>
                                <TR>
                                    <TD bgColor="#ffffff" colSpan="20"><IMG height="3" src="../Images/trans.gif" width="1" border="0"></TD>
                                </TR>

                                <TR>
                                    <TD class="TimeReportText" width="5"><IMG height="18" src="../Images/Trans.gif" width="5" border="0"></TD>
                                    <TD class="TimeReportText" align="right" width="123" colSpan="3">
                                        <select name="AddDateDropDown" id="AddDateDropDown" class="TimeReportListBox">
        <option value=""></option>
        <?php foreach ($daysthisweek as $date): ?>
        <option value="<?php print $date->format('Y-m-d') ?>"><?php print $date->format('D, d/m')?> </option>
        <?php endforeach; ?>

    </select></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="*">
                                        <select name="AddRowDropDown" id="AddRowDropDown" class="TimeReportListBox">
        <option value=""></option>
        <?php foreach($assignments as $assignment):?>
        <option value="<?php print $assignment->id ?>"><?php print $assignment->title ?></option>
        <?php endforeach;?>
        <option value="-1">---</option>
	<?php
	if(!empty($absence_codes)):
	  foreach($absence_codes as $code):?>
        <option value="<?php print '_AC_' . $code->id ?>"><?php print $code->title ?></option>
        <?php
          endforeach;
        endif;
        ?>
        <option value="_AC_"></option>
    </select></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="50">
                                        <input name="AddTimeFromTextBox" type="text" size="5" id="AddTimeFromTextBox" class="TimeReportTextBox" /></TD>

                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="50">
                                        <input name="AddTimeToTextBox" type="text" size="5" id="AddTimeToTextBox" class="TimeReportTextBox" /></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="40">
                                        <input name="AddBreakTextBox" type="text" size="3" id="AddBreakTextBox" class="TimeReportTextBox" /></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="47"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>

                                    <TD class="TimeReportText" width="50">
                                        <input name="AddExplicitOvertimeTextBox" type="text" size="3" id="AddExplicitOvertimeTextBox" class="TimeReportTextBox" /></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="120"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="50"><IMG height="1" src="../Images/Trans.gif" width="1" border="0"></TD>
                                </TR>
                                <TR>
                                    <TD class="TimeReportText" width="5"><IMG height="18" src="../Images/Trans.gif" width="5" border="0"></TD>

                                    <TD class="TimeReportText" align="right" width="123" colSpan="3">
                                        <?php if($comments):?><span id="Translatedlabel21">Not:</span><?php endif;?></TD>
                                    <TD class="TimeReportText" width="8"><IMG height="15" src="../Images/trans.gif" width="8" border="0"></TD>
                                    <TD class="TimeReportText" width="*" colSpan="2">
                                        <?php if($comments):?><input name="AddNoteTextBox" type="text" size="40" id="AddNoteTextBox" class="TimeReportListBox" /><?php endif;?></TD>
                                    <TD class="TimeReportText" width="*" colSpan="13">
                                        <input id="FullDayCheckBox" type="checkbox" name="FullDayCheckBox" />
                                        <span id="Translatedlabel23"> Heldag (frånvaro)</span></TD>

                                </TR>
                                <TR>
                                    <TD bgColor="#ffffff" colSpan="20"><IMG height="3" src="../Images/trans.gif" width="1" border="0"></TD>
                                </TR>
                                <tr id="TRSaveErrorAdd">
        <TD bgcolor="#ffffff" colspan="5"><IMG height="1" src="../Images/trans.gif" width="1" border="0"></TD>
        <TD bgcolor="#ffffff" colspan="12">
                                        <span id="AddErrorLabel" class="ErrorMedium"></span></TD>
        <TD bgcolor="#ffffff" colspan="3"><IMG height="1" src="../Images/trans.gif" width="1" border="0"></TD>

    </tr>

                                <TR>
                                    <TD bgColor="#aca899" colSpan="20"></TD>
                                </TR>

</td></tr></table>
                            <!-- ADD NEW ROW END -->
                            <tr>
                                <td colSpan="20">
                                    <table border="0">

                                        <tr>
                                            <td width="10"><IMG height="5" src="../Images/Trans.gif" width="10" border="0"></td>
                                            <td vAlign="middle" align="left" width="30%"><input type="submit" name="UpdateButton" value="Uppdatera" id="UpdateButton" /><input type="submit" name="DoneButton" value="Vecka Klar" id="DoneButton" /></td>
                                            <td width="5"><IMG height="30" src="../Images/Trans.gif" width="10" border="0"></td>
                                            <td class="SubjectRow" vAlign="middle" align="right" width="30%"><b><span id="Translatedlabel22">Summa timmar:</span></b></td>
                                            <td width="5"><IMG height="30" src="../Images/Trans.gif" width="5" border="0"></td>
                                            <td class="SubjectRow" align="left"><b><?php print $total_duration_hours ?></b></td>
                                        </tr>

                                    </table>
                                </td>
                            </tr>
                            <tr>
                                <td colSpan="20"><IMG height="3" src="../Images/trans.gif" width="1" border="0"></td>
                            </tr>
                        </table>
                    </td>
                </tr>

            </table>
            <table cellSpacing="0" cellPadding="0" border="0">
                <tr>
                    <td colSpan="20"><IMG height="20" src="../Images/trans.gif" width="1" border="0"></td>
                </tr>
                <tr>
                    <td><IMG height="1" src="../Images/Trans.gif" width="5" border="0"></td>
                    <td class="SubjectRow"><span id="Tl23">Glöm inte att klar-rapportera veckan.</span></td>

                </tr>
            </table>
            <br>
        </form>
    </body>
</HTML>
