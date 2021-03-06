
/*************************************************************  Stock Messages */ 
/* stock ready message for device */
/* device types classes */
__root const char OKAYmsg[] @ ".far.rodata" = {"OK"};
__root const char ERRORmsg[] @ ".far.rodata" = {"ERROR"};
__root const char WSTATECONNECTEDmsg[] @ ".far.rodata" = {"WSTATE=CONNECTED"};
__root const char IPmsg[] @ ".far.rodata" = {"IP:"};

/*  SNAP'S RESPONSE TO DEVICE status message */
__root const char Powered[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x01,0xFE,0x03,0x00}; /* 02,02,00,53,01,ff,03*/
__root const char AdaptorReady[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x04,0xFB,0x03,0x00};/* 02,02,00,53,04,fc,03*/
__root const char ConfiguringAdaptor[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x05,0xFA,0x03,0x00};/* 02,02,00,53,05,fb,03*/
__root const char ACKMessage[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x55,0x06,0xf7,0x03,0x00};/* 02,02,00,55,06,f8,03*/
__root const char ResendMessage[] @ ".far.rodata" = {0x02,0x10,0xFc,0x00,0x53,0x07,0xf8,0x03,0x00}; /* 02,02,00,53,07,f9,03*/

/*  dummy response messages */
/*  Reset and provisioning GS1011 messages */
__root const char YouThereMessage[] @ ".far.rodata" = {"AT\r\n"};
__root const char ShutOffEchoMessage[] @ ".far.rodata" = {"ATE0\r"};
__root const char ForceFactoryResetMessage[] @ ".far.rodata" = {"AT&F\r"};
__root const char ClearW0Message[] @ ".far.rodata" ={"AT&W0\r"};
__root const char ClearW1Message[] @ ".far.rodata" ={"AT&W1\r"};
__root const char ClearY0Message[] @ ".far.rodata" ={"AT&Y0\r"};
__root const char ResetAdaptorMessage[] @ ".far.rodata" ={"AT+RESET\r\n"};
__root const char DisassociateMessage[] @ ".far.rodata" = {"AT+WD\r"};
__root const char SetInitialIPAddressMessage[] @ ".far.rodata" = {"AT+NSET=192.168.1.1,255.255.255.0,192.168.1.1\r"};
__root const char SetWM2Message[] @ ".far.rodata" = {"AT+WM=2\r"};
__root const char SetSerialNumberasAccessPointHeader[] @ ".far.rodata" = {"AT+WA="};
__root const char SetWEBPROVMessage[] @ ".far.rodata" = {"AT+WEBPROV=,,,2,,1\r"};
__root const char SWReset_Response[] @ ".far.rodata" = {"RESET"};

/* put device serial number between these messages*/
__root const char SetSerialNumberasAccessPointTail[] @ ".far.rodata" = {",,11\r"};
__root const char SetDHCPSRVRMessage[] @ ".far.rodata" = {"AT+DHCPSRVR=1\r"};
/* end put device serial number between these messages*/

/*  post provisioning response */
__root const char NWCONN_Response[] @ ".far.rodata" = {"NEWCONN"};

/*  website response */
__root const char Good_Response[] @ ".far.rodata" = {"good"};
__root const char goodmsg[] @ ".far.rodata" = {"good"};
__root const char updatemsg[] @ ".far.rodata" = {"update"};

/*  setup gs101 for update messages  */
__root const char DNSLOOKUP_Message[] @ ".far.rodata" = {"AT+DNSLOOKUP=mars.bwgrp.com\r"};
__root const char ConnectionType_CONF11_message[] @ ".far.rodata" = {"AT+HTTPCONF=11,mars.bwgrp.com\r"};
__root const char KeepAlive_CONF3_Message[] @ ".far.rodata" ={"AT+HTTPCONF=3, keep-alive\r"};
__root const char HTTPOPEN_FULL_MESSAGE[] @ ".far.rodata" ={"AT+HTTPOPEN=54.225.101.110\r"};   /*add ip Address*/

__root const char Dummy_update[] @ ".far.rodata" ={"AT+HTTPSEND=0,1,100,/account/api/setdevicelog/p1/0000001234512555/p2/12345/p3/D/p4/S/p5/S/p6/D/p7/00078901/p8/345678/p9/000789/p10/21221/p11/32222/p12/31111/p13/0/p14/0/p15/0\r"};
__root const char Cigar_update[] @ ".far.rodata" ={"AT+HTTPSEND=0,1,100,/account/api/setdevicelog/p1/0000051122334455/p2/999/p3/2703/p4/0/p5/688/p6/0/p7/1/p8/680/p9/900/p10/0/p11/700/p12/0/p13/0\r"};

/*  get network status message */
__root const char GetNSTATMessage[] @ ".far.rodata" = {"AT+NSTAT=?\r"};
/* setup GS1011 to connect to WIFI */
__root const char SetNDHCPMessage[] @ ".far.rodata" = {"AT+NDHCP=1\r"};
__root const char GetProfileMessage[] @ ".far.rodata" = {"AT&V\r"};

/*  send update message headers */
__root const char  pheader[] @ ".far.rodata" = {"/P"};
__root const char SendtoWebsiteHeader[] @ ".far.rodata" ={"AT+HTTPSEND="};   /*add CID, and status*/
__root const char SendtoWebsiteHeader1[] @ ".far.rodata" = {",1,100,/account/api/setdevicelog"};   /*add CID, and status*/
__root const char TestingSerialNumber[] @ ".far.rodata" = {"A0UA0000051122334456"};   /*add CID, and status*/
