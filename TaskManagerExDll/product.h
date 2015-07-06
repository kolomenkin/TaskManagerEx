// Product version defines
#pragma once
#define TO_STR2(x)	#x
#define TO_STR(x)	TO_STR2(x)

#define RUSSIAN_RESOURCES

#define COPYRIGHT1					"Written by Zoltan Csizmadia (zoltan_csizmadia@yahoo.com), 2000"
#define COPYRIGHT2					"Improved and refactoried by Sergey Kolomenkin (kolomenkin@gmail.com), 2005-2015"


#define APPVERSION					2
#define APPREVISION					1
#define APPSUBREVISION				0
#define APPRELEASE					248

#define	PRODUCTNAME					"Task Manager Extension"

#define VERSIONCOPYRIGHT			"Copyright © 2000 - 2015, Zoltan Csizmadia and Sergey Kolomenkin\0"
#define VERSIONCOMMENTS				COPYRIGHT1 "\r\n\r\n" COPYRIGHT2 "\0"
#define VERSIONPRODUCTNAME			PRODUCTNAME "\0"
#define VERSIONINTERNALNAME			"TaskManagerEx\0"

#define VERSIONPRODUCTVERSIONSTR	TO_STR(APPVERSION) "." TO_STR(APPREVISION) "." TO_STR(APPSUBREVISION) " build " TO_STR(APPRELEASE) "\0"

// Module Info:
#define VERSIONFILEVERSIONSTR		TO_STR(MODVERSION) "." TO_STR(MODREVISION) "." TO_STR(MODSUBREVISION) " build " TO_STR(MODRELEASE) "\0"

// Russian:
#ifdef RUSSIAN_RESOURCES

#define COPYRIGHT1_RUS				"Автор: Zoltan Csizmadia (zoltan_csizmadia@yahoo.com), 2000"
#define COPYRIGHT2_RUS				"Улучшено и дополнено: Сергей Коломенкин (kolomenkin@gmail.com), 2005-2015"

#define VERSIONCOPYRIGHT_RUS		"Copyright © 2000 - 2015, Zoltan Csizmadia, Сергей Коломенкин\0"
#define VERSIONCOMMENTS_RUS			COPYRIGHT1_RUS "\r\n\r\n" COPYRIGHT2_RUS "\0"
#define VERSIONPRODUCTNAME_RUS		VERSIONPRODUCTNAME
#define VERSIONINTERNALNAME_RUS		VERSIONINTERNALNAME

#define VERSIONPRODUCTVERSIONSTR_RUS	TO_STR(APPVERSION) "." TO_STR(APPREVISION) "." TO_STR(APPSUBREVISION)  " сборка " TO_STR(APPRELEASE) "\0"

	// Module Info:
#define VERSIONFILEVERSIONSTR_RUS	TO_STR(MODVERSION) "." TO_STR(MODREVISION) "." TO_STR(MODSUBREVISION)  " сборка " TO_STR(MODRELEASE) "\0"
#define VERSIONORIGINALFILENAME_RUS	VERSIONORIGINALFILENAME

#endif

//#define VERSIONCOMPANYNAME			"\0"
//#define VERSIONTRADEMARKS				"\0"
//#define VERSIONPRIVATEBUILD			"\0"
//#define VERSIONSPECIALBUILD			"\0"
