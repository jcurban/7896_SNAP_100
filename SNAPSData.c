
/*************************************************************  Stock Messages */ 
/* stock ready message for device */
/* device types classes */
__root const char OKAYmsg[] @ ".far.rodata" = {"OK"};
__root const char ERRORmsg[] @ ".far.rodata" = {"ERROR"};
__root const char WSTATECONNECTEDmsg[] @ ".far.rodata" = {"WSTATE=CONNECTED"};
__root const char IPmsg[] @ ".far.rodata" = {"IP:"};

/*  DEVICE status message */
__root const char Powered[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x01,0xfe,0x03,0x00};
__root const char AdaptorReady[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x04,0xfB,0x03,0x00};
__root const char ConfiguringAdaptor[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x05,0xfA,0x03,0x00};

/*  Reset and provisioning GS1011 messages */
__root const char YouThereMessage[] @ ".far.rodata" = {"AT\r\n"};
__root const char ShutOffEchoMessage[] @ ".far.rodata" = {"ATE0\r\n"};
__root const char ForceFactoryResetMessage[] @ ".far.rodata" = {"AT&F\r"};
__root const char ClearW0Message[] @ ".far.rodata" ={"AT&W0\r"};
__root const char ClearW1Message[] @ ".far.rodata" ={"AT&W1\r"};
__root const char ClearY0Message[] @ ".far.rodata" ={"AT&Y0\r"};
__root const char ResetAdaptorMessage[] @ ".far.rodata" ={"AT+RESET\r\n"};
__root const char DisassociateMessage[] @ ".far.rodata" = {"AT+WD\r"};
__root const char SetInitialIPAddressMessage[] @ ".far.rodata" = {"AT+NSET=192.168.1.1,255.255.255.0,192.168.1.1\r"};
__root const char SetWM2Message[] @ ".far.rodata" = {"AT+WM=2\r"};
__root const char SetSerialNumberasAccessPointHeader[] @ ".far.rodata" = {"AT+WA="};
                                                /* put device serial number between these messages*/
__root const char SetSerialNumberasAccessPointTail[] @ ".far.rodata" = {",,11\r"};
__root const char SetDHCPSRVRMessage[] @ ".far.rodata" = {"AT+DHCPSRVR=1\r"};
__root const char SetWEBPROVMessage[] @ ".far.rodata" = {"AT+WEBPROV=,,,2,,1\r"};

/*  setup update messages  */
__root const char DNSLOOKUP_Message[] @ ".far.rodata" = {"AT+DNSLOOKUP=mars.bwgrp.com\r"};
__root const char ConnectionType_CONF11_message[] @ ".far.rodata" = {"AT+HTTPCONF=11,mars.bwgrp.com\r"};
__root const char KeepAlive_CONF3_Message[] @ ".far.rodata" ={"AT+HTTPCONF=3, keep-alive\r"};
__root const char HTTPOPEN_FULL_MESSAGE[] @ ".far.rodata" ={"AT+HTTPOPEN=54.225.101.110"};   /*add ip Address*/


/*  get network status message */
__root const char GetNSTATMessage[] @ ".far.rodata" = {"AT+NSTAT=?\r"};
/* setup GS1011 to connect to WIFI */
__root const char SetNDHCPMessage[] @ ".far.rodata" = {"AT+NDHCP=1\r"};
__root const char GetProfileMessage[] @ ".far.rodata" = {"AT&V\r"};

/*  send update message headers */
__root const char  pheader[] @ ".far.rodata" = {"/P"};
__root const char SendtoWebsiteHeader[] @ ".far.rodata" ={"AT+HTTPSEND="};   /*add CID, and status*/
__root const char SendtoWebsiteHeader1[] @ ".far.rodata" = {",1,100,/account/api/setdevicelog/"};   /*add CID, and status*/
