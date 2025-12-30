
#ifndef __NPPLUGIN_CONFIG_OFFICE_H__
#define __NPPLUGIN_CONFIG_OFFICE_H__

#define NPPLUGIN_PRODUCT_NAME                 "taishan-office"
#define NPPLUGIN_PRODUCT_DESCRIPTION          "taishan-office"

#define NPPLUGIN_PRODUCT_MIME "application/taishan-office:tio:Taishan Office mimetype"

#define KEY_FILE_PLUGIN_PROXY    PROXY_NAME_OFFICE

/*

消息名 定义
OnStartApp      应用程序已启动
OnCloseApp      应用程序即将关闭
OnCreate        新文档已创建
OnLoadFinished  已装入文档
OnNew           新文档已创建并可见。异步发送
OnLoad          新文档已装入并可见。异步发送
OnSaveAs        将要用新文件名保存文档
OnSaveAsDone    已用新文件名保存文档
OnSave          将要保存文档
OnSaveDone      文档已保存
OnPrepareUnload 文档即将被删除
OnUnload        文档已删除
OnFocus         文档已被激活
OnUnfocus       文档已被关闭
OnPrint         即将打印文档
OnModifyChange  文档状态已发生改变
*/

static const char* saPluginJsFunctions[] =
{	
    //功能由插件实现。
    (char*)"init",
    (char*)"getPluginVersion",
    (char*)"registListener",
    (char*)"unRegistListener",

    //功能由代理实现
    (char*)"createDocument",
    (char*)"openDocument",
    (char*)"openDocumentRemote",
    (char*)"saveAs",

    (char*)"saveURL",
    (char*)"print",
    (char*)"close",
    (char*)"printRevision",

    (char*)"enableProtect",
    (char*)"setToolbarAllVisible",
    (char*)"enableCopy",
    (char*)"enableCut",

    (char*)"enableRevision",
    (char*)"setZoomRatio",
    (char*)"getZoomRatio",
    (char*)"setCompositeEnable",

    (char*)"setDocumentId",
    (char*)"getDocumentId",
    (char*)"setDocumentType",
    (char*)"getDocumentType",

    (char*)"setMetaData",
    (char*)"getMetaData",
    (char*)"insertPicture",
    (char*)"getText",

    (char*)"backspace",
    (char*)"insertText",
    (char*)"insertTable",
    (char*)"removeTable",

    (char*)"setRowHeight",
    (char*)"setColumnWidth",
    (char*)"setCellProtected",
    (char*)"cursorToCell",

    (char*)"enableRevisionAcceptCommand",
    (char*)"enableRevisionRejectCommand",
    (char*)"showRevision",
    (char*)"setUserName",

    (char*)"getUserName",
    (char*)"showRevisionAuthor",
    (char*)"acceptAllChanges",
    (char*)"rejectAllChanges",

    (char*)"filterCertainAuthor",
    (char*)"insertDocumentField",
    (char*)"setDocumentField",
    (char*)"getAllDocumentField",

    (char*)"deleteDocumentField",
    (char*)"showDocumentField",
    (char*)"getDocumentFieldValue",
    (char*)"enableDocumentField",

    (char*)"insertDocument",
    (char*)"cursorToDocumentField",

    (char*)"beginMark",
    (char*)"stopMark",
    (char*)"replaceTemplate",
    (char*)"addToolBar",

    (char*)"getAllBookMarks",
    (char*)"setBookMarkValue",
    (char*)"setBookMarkFile",
    (char*)"setBookMarkPicture",

    (char*)"connectWindowsShare",
    (char*)"saveToWindowsShare",
    
    (char*)"enableEdit",
    (char*)"enableSave",

    (char*)"showPrintDialog"

};

#endif // __NPPLUGIN_CONFIG_OFFICE_H__
