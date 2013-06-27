Attribute VB_Name = "GainSpanRoutines"
'*****************************************************************************************************
'         COMMANDS FOR GS1011
'*****************************************************************************************************
Public Sub StartTrans()
    strng = Chr(27) & "S" & CIDString
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub EndTrans()
    strng = Chr(27) & "E"
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub FactoryReset()
SNAPApp.GS1011Text = ""
SNAPApp.SetupNetworkStatusLabel.Caption = "Factory Resets GS1011!"
    strng = "AT&F" & vbCrLf
    Send_Output_Wait_Response (strng)
    strng = "AT&W0" & vbCrLf
    Send_Output_Wait_Response (strng)
    strng = "AT&W1" & vbCrLf
    Send_Output_Wait_Response (strng)
    strng = "AT&Y0" & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub ResetAdaptor()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Resetting GS1011!"
    strng = "AT+RESET" & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub ShutOffEcho()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Turning off echo!"
    strng = "ATE0" & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub turnOnEcho()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Turning on echo!"
    strng = "ATE1" & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub DisassociateWeb()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Clearing Web Settings!"
    strng = "AT+WD" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub GetProfiles()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Getting Profiles!"
    strng = "AT&V" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub GetNSTAT()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Netword Status!"
    strng = "AT+NSTAT=?" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SetupSendInitialIPAddress()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Default IP Address!"
    strng = "AT+NSET=192.168.1.1,255.255.255.0,192.168.1.1" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendNDHCP()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending NDHCP!"
    strng = "AT+NDHCP=1" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendWPAPSK()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending WPAPSK!"
    If atWorkFlag = True Then
    strng = "AT+WWPA=internet" & vbCrLf
    Else
    strng = "AT+WWPA=dili2012" & vbCrLf
    End If
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendWA()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Setting default network name!"
    If atWorkFlag = True Then
    strng = "AT+WA=Mars_Guest" & vbCrLf
    Else
    strng = "AT+WA=NewDocJCUNet2" & vbCrLf
    End If
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    tempstr = Buffer$
    SNAPApp.SetupNetworkStatusLabel.Caption = "Saving IP Address "
    'CopyOurIPAddress (tempstr)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendWA_Init()
Dim str As String
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Web Access name (serial number)!"
    'strng = "AT+WA=" & ChangeParameters.Text1(1).Text & ",,11" &VBCRLF
    
    strng = "AT+WA=" & ChangeParameters.Text1(1).Text & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    tempstr = Buffer$
    SNAPApp.SetupNetworkStatusLabel.Caption = "Saving IP Address "
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendDNSLookup()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Looking for mars.bwgrp.com IP Address!"
    If newWebSiteFlag = True Then
    strng = "AT+dnslookup=mars.bwgrp.com" & vbCrLf
    Else
    strng = "AT+dnslookup=mars.dev.onpbg.com" & vbCrLf
    End If
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    tempstr = ""
    tempstr = Buffer$
    SNAPApp.SetupNetworkStatusLabel.Caption = "Saving IP Address "
    CopyIPAddress (tempstr)
    SNAPApp.SetupNetworkStatusLabel.Caption = "Saving IP Address " & IPAddressString
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendHTTPConfWebsite()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Setting mars.bwgrp.com as the web database to connect to!"
    If newWebSiteFlag = True Then
    strng = "AT+HTTPCONF=11,mars.bwgrp.com" & vbCrLf
    Else
    strng = "AT+HTTPCONF=11,mars.dev.onpbg.com" & vbCrLf
    End If
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    tempstr = Buffer$
    SNAPApp.SetupNetworkStatusLabel.Caption = "Saving IP Address "
End Sub
Public Sub SendHTTPHeaderKeepAlive()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending keep alive (don't disconnect after send)!"
    strng = "at+httpconf=3, keep-alive"
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    strng = strng & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendHTTPHeaderUserAgent()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending User Agent!"
    strng = "AT+HTTPCONF=20,User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.1.9)Gecko/20100315 Firefox/3.5.9"
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    strng = strng & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendHTTPOpen()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Open mars.bwgrp.com!"
    strng = "AT+HTTPOPEN=" & IPAddressString & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
    tempstr = Buffer$
    GetCID (tempstr)
    
    GSPANReadyforDataFlag = True
End Sub
Public Sub setupWebsiteStatus()
Dim ctr As Integer
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending status to website!"
    ChangeParameters.CreateUpdateMessage
    strng = UpdateMessageString
    copyText1toSavedTexts
End Sub
Public Sub SendHTTPSend()
Dim str As String
    tmpstr = strng
    strng = "AT+HTTPSEND=" & CIDString & ",1,100" & "," & tmpstr & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng & vbCrLf
    SendingUpdateFlag = True
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text.Text = SNAPApp.GS1011Text.Text & Buffer$ & vbCrLf
    ChangeParameters.SendChangesButton.Visible = False
End Sub
Public Sub SendWM()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Web mode!"
    strng = "AT+wm=0" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendWM_2()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Web mode to limited!"
    strng = "AT+WM=2" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendHTTPHeaderHost()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Configure host!"
    strng = "at+httpconf=11," & IPAddressString
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    strng = strng & vbCrLf
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendBDATA()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending BDATA!"
    strng = "AT+BDATA=1" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendNSTCP()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending NSTCP!"
    strng = "AT+NSTCP=8010" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SendNCTCP()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending NCTCP!"
    strng = "AT+NCTCP=" & IPAddressString & ",80" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
    tempstr = Buffer$
    GetCID (tempstr)
End Sub
Public Sub setDHCPSRVR()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending DHCPSRVR!"
    strng = "AT+DHCPSRVR=1" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub SetProvisioning()
    SNAPApp.SetupNetworkStatusLabel.Caption = "Sending Provisioning!"
    strng = "AT+WEBPROV=,,,2,,1" & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    SendingUpdateFlag = True
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
'************************************************************** utilities
Sub CheckModeAP()
Dim i As Integer
    ModeIsAPflag = False
    For i = 1 To Len(parseString)
        If Mid(parseString, i, Len(Mode = AP)) = "MODE=AP" Then
            ModeIsAPflag = True
            Exit Sub
        End If
    Next i
End Sub

Public Sub DoEscSend()
setupWebsiteStatus
strng = Chr(27) & "S" & CIDString
strng = strng & "PUT HTTP/1.1" & vbCrLf & "User-Agent: curl/7.19.5 (i486-pc-linux-gnu) libcurl/7.19.5 OpenSSL/0.9.8g zlib/1.2.3.3" & vbCrLf & UpdateMessageString & vbCrLf & Chr(27) & "E"
SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
Send_Output_Wait_Response (strng)
SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf

End Sub
Public Sub SendHTTPGet()
    'SendHTTPOpen
    SNAPApp.SetupNetworkStatusLabel.Caption = "Doing GET!"
    strng = "AT+HTTPSEND=" & CIDString & ",1,10,," & vbCrLf
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & strng
    Send_Output_Wait_Response (strng)
    SNAPApp.GS1011Text = SNAPApp.GS1011Text & Buffer$ & vbCrLf
End Sub
Public Sub GetCID(strng As String)
Dim k, i As Integer
GotCIDFlag = False
parseString = strng
StartPointer = 0
EndPointer = 0
Check4ConnectFlag = False
Check4Disconnect
If Check4ConnectFlag = True Then
    CIDString = Mid(parseString, StartPointer, 1)
    GotCIDFlag = True
    Exit Sub
Else
Check4Error
If Check4ConnectFlag = True Then Exit Sub
For i = 1 To Len(parseString)
    If (Mid(parseString, i, 1) >= "0") And (Mid(parseString, i, 1) <= "9") Then
        CIDString = Mid(parseString, i, 1)
        GotCIDFlag = True
        Exit Sub
    End If
Next i
End If
End Sub
Sub Check4Connect()
Dim k, i As Integer
k = Len("CONNECT ")
Check4ConnectFlag = False
For i = 1 To Len(parseString)
    If (UCase(Mid(parseString, i, k)) = "CONNECT ") Then
        StartPointer = i + k
        Check4ConnectFlag = True
        Exit Sub
    End If
Next i
End Sub
Sub Check4Disconnect()
Dim k, i As Integer
k = Len("DISCONNECT ")
Check4ConnectFlag = False
For i = 1 To Len(parseString)
    If (UCase(Mid(parseString, i, k)) = "DISCONNECT ") Then
        StartPointer = i + k
        Check4ConnectFlag = True
        Exit Sub
    End If
Next i
End Sub
Sub Check4Error()
CompareString = "ERROR"
k = Len(CompareString)
Check4ConnectFlag = False
For i = 1 To Len(parseString)
    If (UCase(Mid(parseString, i, k)) = CompareString) Then
        StartPointer = i + k
        Check4ConnectFlag = True
        Exit Sub
    End If
Next i
End Sub
Public Sub CopyIPAddress(strng As String)
parseString = strng
StartPointer = 0
EndPointer = 0
CompareString = "IP:"
ComparePointer = 1
FindCharacter
If FoundCharacterPointer = 0 Then
    Exit Sub
Else
    StartPointer = FoundCharacterPointer + Len(CompareString)
End If
If StartPointer = 0 Then Exit Sub

CompareString = vbNewLine & "OK"
ComparePointer = StartPointer
FindCharacter
If FoundCharacterPointer = 0 Then
    Exit Sub
Else
    EndPointer = FoundCharacterPointer
End If
IPAddressString = Mid(parseString, StartPointer, EndPointer - StartPointer)
End Sub
Public Sub CopyOurIPAddress(strng As String)
parseString = strng
StartPointer = 0
EndPointer = 0
CompareString = "IP addr="
ComparePointer = 1
FindCharacter
If FoundCharacterPointer = 0 Then
    Exit Sub
Else
    StartPointer = FoundCharacterPointer + Len(CompareString)
End If
CompareString = "   "
ComparePointer = FoundCharacterPointer
FindCharacter
If FoundCharacterPointer = 0 Then
    Exit Sub
Else
    EndPointer = FoundCharacterPointer
End If

ourIPAddressString = Mid(parseString, StartPointer, (EndPointer) - StartPointer)
SNAPApp.OurIPAddressLabel.Caption = "GS1011 IP Address: " & ourIPAddressString
End Sub
Public Sub DetermineSendStringLength(strng As String)
Dim cntr As Integer

cntr = Len(strng)
tmplengthString = CStr(cntr)
If Len(tmplengthString) < 4 Then
    If Len(tmplengthString) = 3 Then
        StrLengthString = "0" & tmplengthString
    ElseIf Len(tmplengthString) = 2 Then
        StrLengthString = "00" & tmplengthString
    ElseIf Len(tmplengthString) = 1 Then
        StrLengthString = "000" & tmplengthString
    End If
Else
    StrLengthString = tmplengthString
End If
End Sub

