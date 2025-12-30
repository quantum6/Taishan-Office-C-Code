#ifndef __PROXY_CONFIG_OFFICE_H__
#define __PROXY_CONFIG_OFFICE_H__

#include "fw_config.h"

#define PROXY_STARTER            APPLICATION_TIO

#define KEY_FILE_PLUGIN_JAR      "OfficePlugin.jar"

#define CLASS_MAIN               "tso/plugin/office/OfficePlugin"
#define CLASS_COMMAND            CLASS_MAIN

#define GET_INSTANCE_SIGNATURE   "()Ltso/plugin/office/OfficePluginable;"

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

static const JniFunctionPair oJniFunctions[] =
{
/*
这里不实现，由调用方（如libtsoffice.so）实现。
    { (char*)"init",                        init               },
    { (char*)"getPluginVersion",            getPluginVersion   },
    { (char*)"registListener",              registListener     },
    { (char*)"unRegistListener",            unRegistListener   },
*/
    { (char*)"createDocument",              JNI_FUNCTION_Str_Z     },
    { (char*)"openDocument",                JNI_FUNCTION_StrZ_Z    },
    { (char*)"openDocumentRemote",          JNI_FUNCTION_StrZ_Z    },
    { (char*)"saveAs",                      JNI_FUNCTION_Str_Z     },

    { (char*)"saveURL",                     JNI_FUNCTION_StrStr_Z  },
    { (char*)"print",                       JNI_FUNCTION_V_Z       },
    { (char*)"printRevision",               JNI_FUNCTION_S_Z       },
    { (char*)"close",                       JNI_FUNCTION_V_Z       },

    { (char*)"enableProtect",               JNI_FUNCTION_Z_Z       },
    { (char*)"setToolbarAllVisible",        JNI_FUNCTION_Z_Z       },
    { (char*)"enableCopy",                  JNI_FUNCTION_Z_Z       },
    { (char*)"enableCut",                   JNI_FUNCTION_Z_Z       },

    { (char*)"enableRevision",              JNI_FUNCTION_Z_Z       },
    { (char*)"setZoomRatio",                JNI_FUNCTION_F_Z       },
    { (char*)"getZoomRatio",                JNI_FUNCTION_V_F       },
    { (char*)"setCompositeEnable",          JNI_FUNCTION_StrZ_Z    },

    { (char*)"setDocumentId",               JNI_FUNCTION_Str_Z     },
    { (char*)"getDocumentId",               JNI_FUNCTION_V_Str     },
    { (char*)"setDocumentType",             JNI_FUNCTION_Str_Z     },
    { (char*)"getDocumentType",             JNI_FUNCTION_V_Str     },

    { (char*)"setMetaData",                 JNI_FUNCTION_StrStr_Z  },
    { (char*)"getMetaData",                 JNI_FUNCTION_Str_Str   },
    { (char*)"insertPicture",               JNI_FUNCTION_StrFFFF_Z },
    { (char*)"getText",                     JNI_FUNCTION_V_Str     },

    { (char*)"backspace",                   JNI_FUNCTION_V_Z       },
    { (char*)"insertText",                  JNI_FUNCTION_Str_Z     },
    { (char*)"insertTable",                 JNI_FUNCTION_StrII_Z   },
    { (char*)"removeTable",                 JNI_FUNCTION_Str_Z     },

    { (char*)"setRowHeight",                JNI_FUNCTION_StrIF_Z   },
    { (char*)"setColumnWidth",              JNI_FUNCTION_StrIF_Z   },
    { (char*)"setCellProtected",            JNI_FUNCTION_StrIIZ_Z  },
    { (char*)"cursorToCell",                JNI_FUNCTION_StrII_Z   },

    { (char*)"enableRevisionAcceptCommand", JNI_FUNCTION_Z_Z       },
    { (char*)"enableRevisionRejectCommand", JNI_FUNCTION_Z_Z       },
    { (char*)"showRevision",                JNI_FUNCTION_S_Z       },
    { (char*)"setUserName",                 JNI_FUNCTION_Str_Z     },

    { (char*)"getUserName",                 JNI_FUNCTION_V_Str     },
    { (char*)"showRevisionAuthor",          JNI_FUNCTION_StrZ_Z    },
    { (char*)"acceptAllChanges",            JNI_FUNCTION_V_Z       },
    { (char*)"rejectAllChanges",            JNI_FUNCTION_V_Z       },

    { (char*)"filterCertainAuthor",         JNI_FUNCTION_StrZ_Z    },
    { (char*)"insertDocumentField",         JNI_FUNCTION_Str_Z     },
    { (char*)"setDocumentField",            JNI_FUNCTION_StrStr_Z  },
    { (char*)"getAllDocumentField",         JNI_FUNCTION_V_List    },

    { (char*)"deleteDocumentField",         JNI_FUNCTION_Str_Z     },
    { (char*)"showDocumentField",           JNI_FUNCTION_StrZ_Z    },
    { (char*)"getDocumentFieldValue",       JNI_FUNCTION_Str_Str   },
    { (char*)"enableDocumentField",         JNI_FUNCTION_StrZ_Z    },

    { (char*)"insertDocument",              JNI_FUNCTION_StrStr_Z  },
    { (char*)"cursorToDocumentField",       JNI_FUNCTION_StrS_Z    },

    { (char*)"beginMark",                   JNI_FUNCTION_V_Z       },
    { (char*)"stopMark",                    JNI_FUNCTION_V_Z       },
    { (char*)"replaceTemplate",             JNI_FUNCTION_StrStr_Z  },
    { (char*)"addToolBar",                  JNI_FUNCTION_StrIStr_Z },

    { (char*)"getAllBookMarks",             JNI_FUNCTION_V_List    },
    { (char*)"setBookMarkValue",            JNI_FUNCTION_StrStr_Z  },
    { (char*)"setBookMarkFile",             JNI_FUNCTION_StrStr_Z  },
    { (char*)"setBookMarkPicture",          JNI_FUNCTION_StrStr_Z  },

    { (char*)"connectWindowsShare",         JNI_FUNCTION_Str4_Z    },
    { (char*)"saveToWindowsShare",          JNI_FUNCTION_Str_Z     },
    
    { (char*)"enableEdit",                  JNI_FUNCTION_Z_Z       },
    { (char*)"enableSave",                  JNI_FUNCTION_Z_Z       },
    { (char*)"showPrintDialog",             JNI_FUNCTION_V_Z       }
};

#endif
