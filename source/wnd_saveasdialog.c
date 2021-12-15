/*******************************************************************************
* Copyright(c)2015，大豪信息技术(威海)有限公司
*
* All rights reserved
*
* 文件名称：  wnd_saveasdialog.c
* 摘    要：  SaveAsDialog实现，实现一个存储窗口
*             
*
* 当前版本：  v1.0.0(初始版本)
* 作    者：  
* 完成日期：  2017.11.24
*
* 取代版本： 
* 原 作 者：  
* 完成日期：
*******************************************************************************/

#include "wnd_saveasdialog.h"
#include "wnd_dialog.h"
#include "wnd_button.h"
#include "wnd_global.h"

#include "app_frmfilebrowse.h"
#include "app_text.h"
#include "global_index.h"
#include "string.h"
#include "app_curve.h"
#include "hangulime.h"
#include "input_china_pinyin.h"
#include "wnd_frmscroll.h"

#define KEY_ENON                53	//英韩切换
#define KEY_COUNT		        56+1  //按键的数目 +EN/ON
#define MAX_FILETYPE_NUM        3   //文件类型图标最大显示个数
#define SAVE_AS_MAX_INPUT       35  //能输入的最大字符长度


//特殊按键的编码，不在ASCII的范围(0 - 128)内即可  
#define KEY_CODE_BACKSPACE      0xf0                     
#define KEY_CODE_ENTER          0xf1
#define KEY_CODE_SHIFT          0xf2
#define KEY_CODE_ESC            0xf3
#define KEY_CODE_UP             0xf4
#define KEY_CODE_DOWN           0xf5
#define KEY_CODE_LEFT           0xf6
#define KEY_CODE_RIGHT          0xf7
#define KEY_CODE_ENON           0xf8

//特殊按键在按键组中的索引
#define KEY_BACKSPACE_INDEX     13
#define KEY_ENTER_INDEX         38
#define KEY_SHIFT_INDEX         39
#define KEY_UP_INDEX            49
#define KEY_ESC_INDEX           51
#define KEY_SPACE_INDEX         52
#define KEY_LEFT_INDEX          53+1
#define KEY_DOWN_INDEX          54+1
#define KEY_RIGHT_INDEX         55+1

#define MAXCHINESENUM	21							//每次可显示的待候选的最大中文字符数

//_save_as_dialog 声明
typedef struct _save_as_dialog
{
    Dialog super;                           //基类 模态控件

    Button* FileNameLbl;                    //文件名称
    Button* FileFormatLbl;                  //文件格式
    Button* PromptInfo;                     //提示信息
    unsigned short InputBuffer[512];        //输入缓冲区 修改为宽
    Button* TextInput;                      //输入框，此处以Button实现，本质都是一样的
    Button* Keys[KEY_COUNT];                //所有键盘上的按钮
    Button* SaveToUSBCheckBox;              //保存到U盘checkbox按钮
    Button* FileTypeList[MAX_FILETYPE_NUM]; //文件类型图标
    GUIFONT* BlackFont;                     //默认是黑色字体
    GUIFONT* WhiteFont;                     //白色字体
    GUIFONT* RedFont;                       //红色字体，警告字体
    GUIFONT* CheckedFont;                   //选中字体，默认是蓝色
    
    unsigned int CursorIndex;               //光标位置
    unsigned int ClickedKey;                //键盘消失的时候用户点击的按钮 只有 ESC 和ENTER
    unsigned int IsUppercase;               //键盘当前是否出于大写模式

    char* NewFileName;                      //新的文件名(全路径)
    char* OriginalFileName;                 //原始文件名(全路径)
    char* FileName;
    char* PathName;
    char* FileExtension;
    char* FileType[MAX_FILETYPE_NUM];       //文件类型
    int num;                                //文件类型个数
    int* selectUsbFlag;                     //保存到usb

	int LangueType;							//输入法当前的语种 0 英文 1 韩文（默认英文）2 中文
	int currLanguageSet;                    //系统设置的语言
    char cPinyin[PINYIN_STORE_MAXLEN];      //拼音存储buf
    Button* PinYinBtn;                      //拼音输入框
    WordList *pMainWordSet;                 //汉字列表
    Button* ChineseName[MAXCHINESENUM];     //中文选字框
    Button* ButtonPagePrev;                 //前一页按钮
    Button* ButtonPageNext;                 //下一页按钮
    unsigned char ucKnobFlg;                //
    int iCurrSelectdChinese;                //选择的汉字序号
    BTN_SCROLL *pIMEScroll;                 //滚轮空间
} SaveAsDialog;

//按钮宽度和高度，高度都是一样的，一共有三种宽度
#define NORMAL_KEY_WIDTH        45                                      //普通按钮的宽度  
#define SPECIAL_KEY_WIDTH       68                                      //特殊按钮宽度 shift enter esc backspace等
#define SPACE_KEY_WIDTH         357                                     //空格宽度      
#define KEY_HEIGHT              35                                      //按钮高度
#define KEY_WIDTH_GAP           7                                       //按钮水平间隔
#define KEY_HEIGHT_GAP          15                                      //按钮垂直间隔
#define HOR_OFFSET(_x)          ((NORMAL_KEY_WIDTH+KEY_WIDTH_GAP) *_x)  //计算横轴上的偏移，_x是按钮位于的位置

//键盘第一行按钮起始坐标
#define LINE1_X                 40
#define LINE1_Y                 221

//键盘第二行按钮起始坐标
#define LINE2_X                 70
#define LINE2_Y                 (LINE1_Y + KEY_HEIGHT + KEY_HEIGHT_GAP)

//键盘第三行按钮起始坐标
#define LINE3_X                 93
#define LINE3_Y                 (LINE2_Y + KEY_HEIGHT + KEY_HEIGHT_GAP)

//键盘第四行按钮起始坐标 由于存在特殊按钮和普通按钮，需要两个起始位置
#define LINE4_X1                70
#define LINE4_X2                (LINE4_X1 + SPECIAL_KEY_WIDTH + KEY_WIDTH_GAP)
#define LINE4_Y                 (LINE3_Y + KEY_HEIGHT + KEY_HEIGHT_GAP)

//键盘第五行按钮起始坐标
#define LINE5_X1                70
#define LINE5_X2                (LINE5_X1 + SPECIAL_KEY_WIDTH + KEY_WIDTH_GAP)
#define LINE5_X3                (LINE5_X2 + SPACE_KEY_WIDTH + KEY_WIDTH_GAP)
#define LINE5_Y                 (LINE4_Y + KEY_HEIGHT + KEY_HEIGHT_GAP)

//每个按钮的坐标描述信息
typedef struct _geometry
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
} Geometry;

//按键位置表，用于创建按键
static Geometry keyGemoetryMap[] = 
{
    //第一行按钮坐标
    {LINE1_X + HOR_OFFSET( 0), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // `
    {LINE1_X + HOR_OFFSET( 1), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 1
    {LINE1_X + HOR_OFFSET( 2), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 2
    {LINE1_X + HOR_OFFSET( 3), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 3
    {LINE1_X + HOR_OFFSET( 4), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 4
    {LINE1_X + HOR_OFFSET( 5), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 5
    {LINE1_X + HOR_OFFSET( 6), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 6
    {LINE1_X + HOR_OFFSET( 7), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 7
    {LINE1_X + HOR_OFFSET( 8), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 8
    {LINE1_X + HOR_OFFSET( 9), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 9
    {LINE1_X + HOR_OFFSET(10), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // 0
    {LINE1_X + HOR_OFFSET(11), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // -
    {LINE1_X + HOR_OFFSET(12), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // +
    {LINE1_X + HOR_OFFSET(13), LINE1_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // backspace 该按钮较宽
    
    //第二行按钮坐标
    {LINE2_X + HOR_OFFSET( 0), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // q
    {LINE2_X + HOR_OFFSET( 1), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // w
    {LINE2_X + HOR_OFFSET( 2), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // e
    {LINE2_X + HOR_OFFSET( 3), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // r
    {LINE2_X + HOR_OFFSET( 4), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // t
    {LINE2_X + HOR_OFFSET( 5), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // y
    {LINE2_X + HOR_OFFSET( 6), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // u
    {LINE2_X + HOR_OFFSET( 7), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // i 
    {LINE2_X + HOR_OFFSET( 8), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // o
    {LINE2_X + HOR_OFFSET( 9), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // p
    {LINE2_X + HOR_OFFSET(10), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // [
    {LINE2_X + HOR_OFFSET(11), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // ]
    {LINE2_X + HOR_OFFSET(12), LINE2_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // '\'

    //第三行按钮坐标
    {LINE3_X + HOR_OFFSET( 0), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // a
    {LINE3_X + HOR_OFFSET( 1), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // s
    {LINE3_X + HOR_OFFSET( 2), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // d
    {LINE3_X + HOR_OFFSET( 3), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // f
    {LINE3_X + HOR_OFFSET( 4), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // g
    {LINE3_X + HOR_OFFSET( 5), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // h
    {LINE3_X + HOR_OFFSET( 6), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // j 
    {LINE3_X + HOR_OFFSET( 7), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // k
    {LINE3_X + HOR_OFFSET( 8), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // l
    {LINE3_X + HOR_OFFSET( 9), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // ;
    {LINE3_X + HOR_OFFSET(10), LINE3_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},     // '
    {LINE3_X + HOR_OFFSET(11), LINE3_Y, SPECIAL_KEY_WIDTH + 6, KEY_HEIGHT},// enter 该按钮较宽
    
    //第四行按钮坐标
    {LINE4_X1 + HOR_OFFSET( 0), LINE4_Y, SPECIAL_KEY_WIDTH, KEY_HEIGHT},   // shift 该按钮较宽
    {LINE4_X2 + HOR_OFFSET( 0), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // z
    {LINE4_X2 + HOR_OFFSET( 1), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // x
    {LINE4_X2 + HOR_OFFSET( 2), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // c
    {LINE4_X2 + HOR_OFFSET( 3), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // v
    {LINE4_X2 + HOR_OFFSET( 4), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // b
    {LINE4_X2 + HOR_OFFSET( 5), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // n
    {LINE4_X2 + HOR_OFFSET( 6), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // m
    {LINE4_X2 + HOR_OFFSET( 7), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // ,
    {LINE4_X2 + HOR_OFFSET( 8), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // .
    {LINE4_X2 + HOR_OFFSET( 9), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // up
    {LINE4_X2 + HOR_OFFSET(10), LINE4_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // /

    //第五行按钮坐标
    {LINE5_X1 + HOR_OFFSET( 0), LINE5_Y, SPECIAL_KEY_WIDTH, KEY_HEIGHT},   // esc
    {LINE5_X2 + HOR_OFFSET( 0), LINE5_Y, SPACE_KEY_WIDTH, KEY_HEIGHT},     // space
    {LINE5_X3 + HOR_OFFSET( 0), LINE5_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // en/on
    {LINE5_X3 + HOR_OFFSET( 1), LINE5_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // left
    {LINE5_X3 + HOR_OFFSET( 2), LINE5_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // down
    {LINE5_X3 + HOR_OFFSET( 3), LINE5_Y, NORMAL_KEY_WIDTH, KEY_HEIGHT},    // right
};

//小写字母文本表 +"" 占位
static char* lowerKeyTextMap[] = 
{
    "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\\",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\'", "Enter",
    "Shift", "z", "x", "c", "v", "b", "n", "m", ",", ".", "", "/", 
    "Esc", " ","" ,"", "", "",
};

//大写字母文本表 +"" 占位
static char* upperKeyTextMap[] = 
{
    "~", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "|",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "Enter",
    "Shift", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "", "?", 
    "Esc", "","", "", "", "",
};

//按键编码表 +"" 占位
static int lowerCharCodeMap[] = 
{
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', KEY_CODE_BACKSPACE,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', KEY_CODE_ENTER,
    KEY_CODE_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', KEY_CODE_UP, '/', 
    KEY_CODE_ESC, ' ',KEY_CODE_ENON, KEY_CODE_LEFT, KEY_CODE_DOWN, KEY_CODE_RIGHT,
};
// +"" 占位
static int upperCharCodeMap[] = 
{
    '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', KEY_CODE_BACKSPACE,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', KEY_CODE_ENTER,
    KEY_CODE_SHIFT, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', KEY_CODE_UP, '?', 
    KEY_CODE_ESC, ' ',KEY_CODE_ENON, KEY_CODE_LEFT, KEY_CODE_DOWN, KEY_CODE_RIGHT,
};

//韩文字符
static char *GcKeyName2[KEY_COUNT][2] = {
{"`", "~"},        {"1", "!"}, {"2", "@"}, {"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"}, {"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},  {"-", "_"},   {"=", "+"}, {"", ""},
{"ㅂ", "ㅃ"},        {"ㅈ", "ㅉ"}, {"ㄷ", "ㄸ"}, {"ㄱ", "ㄲ"}, {"ㅅ", "ㅆ"}, {"ㅛ", "ㅛ"}, {"ㅕ", "ㅕ"}, {"ㅑ", "ㅑ"}, {"ㅐ", "ㅒ"}, {"ㅔ", "ㅖ"}, {"[", "{"},  {"]", "}"}, {"\\", "|"},
{"ㅁ", "ㅁ"},        {"ㄴ", "ㄴ"}, {"ㅇ", "ㅇ"}, {"ㄹ", "ㄹ"}, {"ㅎ", "ㅎ"}, {"ㅗ", "ㅗ"}, {"ㅓ", "ㅓ"}, {"ㅏ", "ㅏ"}, {"ㅣ", "ㅣ"}, {";", ":"}, {"'", "\""}, {"Enter", "Enter"},
{"Shift", "Shift"},{"ㅋ", "ㅋ"}, {"ㅌ", "ㅌ"}, {"ㅊ", "ㅊ"}, {"ㅍ", "ㅍ"}, {"ㅠ", "ㅠ"}, {"ㅜ", "ㅜ"}, {"ㅡ", "ㅡ"}, {",", "<"}, {".", ">"}, {"", ""},    {"/", "?"},
{"Esc", "Esc"},    {"", ""},   {"", ""},   {"", ""},   {"", ""}
};


//函数前置申明
static void SaveAsDialog_Show(SaveAsDialog* _this);
static void SaveAsDialog_Destruct(SaveAsDialog* _this);
static void SaveAsDialog_ChangeCase(SaveAsDialog* _this);
static void SaveAsDialog_AddChar(SaveAsDialog* _this, int c);
static void SaveAsDialog_DelChar(SaveAsDialog* _this);
static void SaveAsDialog_MoveCursorLeft(SaveAsDialog* _this);
static void SaveAsDialog_MoveCursorRight(SaveAsDialog* _this);
static void SaveAsDialog_InitInputText(SaveAsDialog* _this);
static void SaveAsDialog_UpdateInputText(SaveAsDialog* _this);
static void SaveAsDialog_InitFileNameList(SaveAsDialog* _this);
static void SaveAsDialog_InitFileTypeList(SaveAsDialog* _this, char* fileType[], int num, unsigned int defaultOption);
static void SaveAsDialog_InitDialogBackground(SaveAsDialog* _this, int num);
static void SaveAsDialog_ProcessKeyCode(SaveAsDialog* _this, int code);
static void SaveAsDialog_InitParameter(SaveAsDialog* _this, char* orignalName, char* newFileName,
                                char* fileType[], int num, unsigned int defaultOption, int* selectUsb);
static void SaveAsDialog_SaveParameter(SaveAsDialog* _this);
static void SaveAsDialog_ChangeLangueInput(SaveAsDialog * _this);
//ɾ�������ַ�
static void SaveAsDialog_DelChineseChar(SaveAsDialog* _this);
//static void SaveAsDialog_UpdatePromptInfo(SaveAsDialog* _this, char* text);

//按键消息处理函数
static int KeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int KeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//checkBox点击响应函数
static int CheckBoxSaveToUSBDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CheckBoxSaveToUSBUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//中文选择按键处理
static int ChineseNameDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int ChineseNameUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//中文上页按键处理
static int ButtonPagePrevDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int ButtonPagePrevUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//中文下页按键处理
static int ButtonPageNextDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int ButtonPageNextUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//文件类型列表相应函数
static int CheckBoxFileTypeListDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int CheckBoxFileTypeListUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//按键按下响应函数
static int SaveAsKeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
static int SaveAsKeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen);
//功能函数如下:
static int FindCharFromString(unsigned short* cString, unsigned short c);
//增加字符到字符串中
static int AddCharToString(unsigned short* cString, int c);
//插入拼音字符
static void InsertPinYinChar(SaveAsDialog* _this, char cChar);
//删除拼音字符
static void DeletePinYinChar(SaveAsDialog* _this);
//显示拼音
static int DisplayPinYin(SaveAsDialog* _this);
//拼音转中文
static int PinYinToChinese(SaveAsDialog* _this);
//显示文字列表
static int DisplayWordList(SaveAsDialog* _this);
//初始化滚轮
static int SaveAsDialog_InitScroll(SaveAsDialog* _this, BTN_SCROLL *pScroll);
//��ʾ�������İ�����Ӧ����
static int SaveAsDialog_ShowScrollBtnDown(SaveAsDialog* _this, int index, int *pCurr, BTN_SCROLL *pScroll);
//��ʾ�������ĵ�����Ӧ����
static int SaveAsDialog_ShowScrollBtnUp(SaveAsDialog* _this, int iCurr, BTN_SCROLL *pScroll);
//��ʾ��ť�İ�����Ӧ����
static int ShowBtn_Down(SaveAsDialog* _this, int iIndex);
//��ʾ��ť�ĵ�����Ӧ����
static int ShowBtn_Up(SaveAsDialog* _this, int iIndex);
//增加韩文字符
static int AddHanCharToString(unsigned short*cString,unsigned short usChar);
//从字符串中删除字符
static int DelCharFromString(unsigned short* cString);
//删除指定字符,flag:0,字符'|', 1:其他字符
static int DelChar(unsigned short* cString, int flag);
//交换字符串中字符的位置(direction:0,左移，1:右移)
static void SwapCharPos(unsigned short* Cstring, int direction);
//保存全路径文件名
static void SaveFilePathAndName(SaveAsDialog* _this);
//保存文件路径名
//static void SaveFilePathName(SaveAsDialog* _this, char* pathName);
//获取初始文件路径名
//static void GetInitFilePathName(SaveAsDialog* _this, char* originalName);
//获取初始的文件名
static void GetInitFileName(SaveAsDialog* _this, char* originalName);
//获取初始的文件名后缀
static void GetInitFileExtension(SaveAsDialog* _this, char* originalName);
//判断文件名是否存在，0:不存在，1:存在
static int IsFileExist(char* pathName);
//删除隐藏文件
//static void DeleteHideFile(SaveAsDialog* _this);
//显示checkbox
static void DisplayCheckBoxChecked(SaveAsDialog* _this, Button* checkBox);
//设置checkbox的文字字体
static void SetCheckBoxTextFont(SaveAsDialog* _this, Button* checkBox);

static void SaveAsDialog_Construct(SaveAsDialog* _this, int x, int y, int w, int h)
{
	//字符缓存初始化
	memset(_this->InputBuffer, 0, sizeof(_this->InputBuffer));
    //先构造基类，并设置相关参数
    Dialog* dialog = (Dialog*)_this;
    Dialog_Construct(dialog, x, y, w, h);
    //Dialog_SetWinOpacity(dialog, 40);

    //基类多态函数的实现重写
    dialog->Show = (DialogShow)SaveAsDialog_Show;
    dialog->Destruct = (DialogDestruct)SaveAsDialog_Destruct;

    //内部GUI资源构造
    if (isFont24())
	{
		_this->BlackFont = CreateFont(FNTFILE_BIG, 24, 24, 0x0, COLOR_TRANS);
	    _this->CheckedFont = CreateFont(FNTFILE_BIG, 24, 24, 0x003F94FF, COLOR_TRANS);
	    _this->WhiteFont = CreateFont(FNTFILE_BIG, 24, 24, 0x00FFFFFF, COLOR_TRANS);
	    _this->RedFont = CreateFont(FNTFILE_BIG, 24, 24, 0x00E81853, COLOR_TRANS);
	}
	else
	{
		_this->BlackFont = CreateFont(FNTFILE_STD, 16, 16, 0x0, COLOR_TRANS);
	    _this->CheckedFont = CreateFont(FNTFILE_STD, 16, 16, 0x003F94FF, COLOR_TRANS);
	    _this->WhiteFont = CreateFont(FNTFILE_STD, 16, 16, 0x00FFFFFF, COLOR_TRANS);
	    _this->RedFont = CreateFont(FNTFILE_STD, 16, 16, 0x00E81853, COLOR_TRANS);
	}
        
    //提示信息
    _this->PromptInfo = CreateButton(20, 90, 480, 24);
    Button_SetBackground(_this->PromptInfo, BmpFileDirectory"bg_prompt.bmp");
    Button_SetFont(_this->PromptInfo, _this->RedFont);
    //输入框
    _this->TextInput = CreateButton(20, 49, 466, 41);
    Button_SetLeftPadding(_this->TextInput, 3);
    Button_SetBackground(_this->TextInput, BmpFileDirectory"bg_input.bmp");    
    Button_SetFont(_this->TextInput, _this->WhiteFont);
	
    //保存到U盘checkbox
    _this->SaveToUSBCheckBox = CreateCheckBox(20, 115, 141, 41, 50);
    _this->SaveToUSBCheckBox->Checkable = 1;
    Button_SetFont(_this->SaveToUSBCheckBox, _this->BlackFont);
    Button_SetText(_this->SaveToUSBCheckBox, TransString("OTDR_SAVE_TO_USB"));
    Button_SetBackground(_this->SaveToUSBCheckBox, BmpFileDirectory"btn_saveToUSB_unpress.bmp");
    Button_SetPressedBackground(_this->SaveToUSBCheckBox, BmpFileDirectory"btn_saveToUSB_press.bmp");
    Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->SaveToUSBCheckBox);
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->SaveToUSBCheckBox, CheckBoxSaveToUSBDown, _this, 0);  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->SaveToUSBCheckBox, CheckBoxSaveToUSBUp, _this, 0);

    //创建拼音按钮
    _this->PinYinBtn = CreateButton(4, 98 +97, 792, 24);
    Button_SetFont(_this->PinYinBtn, _this->BlackFont);
    Button_SetText(_this->PinYinBtn, TransString(" "));
    Button_SetBackground(_this->PinYinBtn, BmpFileDirectory"frm_ime_bg_pinyin1.bmp");
    //创建中文选字按钮
    int i;
    int j;
    for(i=0, j = 44; i < MAXCHINESENUM; i++, j+=34)
	{
		_this->ChineseName[i] = CreateButton(j, 98 + 60, 32, 33);	
        Button_SetFont(_this->ChineseName[i], _this->BlackFont);
        Button_SetText(_this->ChineseName[i], TransString(" "));
        Button_SetAlign(_this->ChineseName[i], GUILABEL_ALIGN_CENTER);
        Button_SetBackground(_this->ChineseName[i], BmpFileDirectory"btn_pinyin_unpress1.bmp");
        Button_SetPressedBackground(_this->ChineseName[i], BmpFileDirectory"btn_pinyin_press1.bmp");
        Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->ChineseName[i]);
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->ChineseName[i], ChineseNameDown, _this, i);  
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->ChineseName[i], ChineseNameUp, _this, i);
	}
    //创建中文选字上一页按钮
    _this->ButtonPagePrev = CreateButton(4, 98 + 60, 38, 33);
    Button_SetText(_this->ButtonPagePrev, TransString(" "));
    Button_SetBackground(_this->ButtonPagePrev, BmpFileDirectory"btn_pageup_unpress1.bmp");
    Button_SetPressedBackground(_this->ButtonPagePrev, BmpFileDirectory"btn_pageup_press1.bmp");
    Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->ButtonPagePrev);
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->ButtonPagePrev, ButtonPagePrevDown, _this, 0);  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->ButtonPagePrev, ButtonPagePrevUp, _this, 0);
    //创建中文选字下一页按钮
    _this->ButtonPageNext = CreateButton(758, 98 + 60, 38, 33);
    Button_SetText(_this->ButtonPageNext, TransString(" "));
    Button_SetBackground(_this->ButtonPageNext, BmpFileDirectory"btn_pagedown_unpress1.bmp");
    Button_SetPressedBackground(_this->ButtonPageNext, BmpFileDirectory"btn_pagedown_press1.bmp");
    Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->ButtonPageNext);
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->ButtonPageNext, ButtonPageNextDown, _this, 0);  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->ButtonPageNext, ButtonPageNextUp, _this, 0);
    //文件类型
    for(i = 0; i < MAX_FILETYPE_NUM; ++i)
    {
        _this->FileTypeList[i] = CreateCheckBox(559, 50 + 32 * i, 100, 25, 50);
        Button_SetFont(_this->FileTypeList[i], _this->BlackFont);
        Button_SetBackground(_this->FileTypeList[i], BmpFileDirectory"fileType_unpress.bmp");
        Button_SetPressedBackground(_this->FileTypeList[i], BmpFileDirectory"fileType_press.bmp");
        Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->FileTypeList[i]);
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->FileTypeList[i], CheckBoxFileTypeListDown, _this, i);  
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->FileTypeList[i], CheckBoxFileTypeListUp, _this, i);
    }
    //文件名
    _this->FileNameLbl = CreateButton(20, 20, 200, 24);
    Button_SetFont(_this->FileNameLbl, _this->BlackFont);
    Button_SetText(_this->FileNameLbl, TransString("CURVEINFO_LBL_FILENAME"));
    //文件格式
    _this->FileFormatLbl = CreateButton(535, 20, 200, 24);
    Button_SetFont(_this->FileFormatLbl, _this->BlackFont);
    Button_SetText(_this->FileFormatLbl, TransString("CURVEINFO_LBL_FILE_FORMAT"));
    for (i = 0; i < KEY_COUNT; ++i)
    {
        _this->Keys[i] = CreateButton(
            keyGemoetryMap[i].x, keyGemoetryMap[i].y, 
            keyGemoetryMap[i].w, keyGemoetryMap[i].h
            );
        Button_SetText(_this->Keys[i],TransString(lowerKeyTextMap[i]));
		if(i == KEY_ENON)
		{
			Button_SetText(_this->Keys[i], TransString("EN"));
		}
        Button_SetAlign(_this->Keys[i], GUILABEL_ALIGN_CENTER);
        Button_SetLayer(_this->Keys[i], DIALOG_GUI_LAYER);
        Button_SetFont(_this->Keys[i], _this->BlackFont);
		Button_SetCode(_this->Keys[i], i);
        //Button_SetCode(_this->Keys[i], lowerCharCodeMap[i]);
        Button_SetBackground(_this->Keys[i], NORMAL_KEY_BMP);
        Button_SetPressedBackground(_this->Keys[i], NORMAL_KEY_PRESSED_BMP);

        Dialog_AddWindowComp(dialog, OBJTYP_GUIBUTTON, sizeof(Button), _this->Keys[i]);
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_DOWN, _this->Keys[i], KeyDown, _this, i);  
        Dialog_LoginMsgHandler(dialog, GUIMESSAGE_TCH_UP, _this->Keys[i], KeyUp, _this, i);  
    }

    //注册物理按键响应函数
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_KEY_DOWN, _this, SaveAsKeyDown, _this, 0);  
    Dialog_LoginMsgHandler(dialog, GUIMESSAGE_KEY_UP, _this, SaveAsKeyUp, _this, 0);  
    //Shift按钮要能处于Checked状态
    Button_SetCheckable(_this->Keys[KEY_SHIFT_INDEX], 1);

    //Enter Esc Shift按钮使用特殊背景
    Button_SetBackground(_this->Keys[KEY_ENTER_INDEX], ENTER_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_ENTER_INDEX], ENTER_KEY_PRESSED_BMP);
    Button_SetBackground(_this->Keys[KEY_SHIFT_INDEX], SPECIAL_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_SHIFT_INDEX], SPECIAL_KEY_PRESSED_BMP);
    Button_SetBackground(_this->Keys[KEY_ESC_INDEX], SPECIAL_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_ESC_INDEX], SPECIAL_KEY_PRESSED_BMP);

    //退格 按钮使用Space专有背景图片
    Button_SetBackground(_this->Keys[KEY_BACKSPACE_INDEX], BACK_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_BACKSPACE_INDEX], BACK_KEY_PRESSED_BMP);

    //Space 按钮使用Space专有背景图片
    Button_SetBackground(_this->Keys[KEY_SPACE_INDEX], SPACE_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_SPACE_INDEX], SPACE_KEY_PRESSED_BMP);

    //Up 按钮使用Up专有背景图片
    Button_SetBackground(_this->Keys[KEY_UP_INDEX], UP_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_UP_INDEX], UP_KEY_PRESSED_BMP);

    //Down 按钮使用Down专有背景图片
    Button_SetBackground(_this->Keys[KEY_DOWN_INDEX], DOWN_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_DOWN_INDEX], DOWN_KEY_PRESSED_BMP);

    //Left 按钮使用Left专有背景图片
    Button_SetBackground(_this->Keys[KEY_LEFT_INDEX], LEFT_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_LEFT_INDEX], LEFT_KEY_PRESSED_BMP);

    //Right 按钮使用Right专有背景图片
    Button_SetBackground(_this->Keys[KEY_RIGHT_INDEX], RIGHT_KEY_BMP);
    Button_SetPressedBackground(_this->Keys[KEY_RIGHT_INDEX], RIGHT_KEY_PRESSED_BMP);

    //对话框其他参数设置
    //_this->CursorIndex = 0;
    _this->ClickedKey = SAVE_AS_DIALOG_KEY_NONE;
    _this->IsUppercase = 0; 
	_this->LangueType = 0;
    _this->currLanguageSet = GetCurrLanguageSet();
    memset(_this->cPinyin, 0, sizeof(_this->cPinyin));
    _this->pMainWordSet = NULL;
    _this->ucKnobFlg = 0;
    _this->iCurrSelectdChinese = 0;

    if(LANG_CHINASIM == _this->currLanguageSet)
    {
        Button_SetText(_this->Keys[KEY_ENON], TransString(""));
        Button_SetBackground(_this->Keys[KEY_ENON], EN_KEY_BMP);
        Button_SetPressedBackground(_this->Keys[KEY_ENON], EN_KEY_PRESS_BMP);
    }
    
    
    //定义滚轮btn
    BTN_SCROLL *pScrollObj = GuiMemAlloc(sizeof(BTN_SCROLL)*(MAXCHINESENUM+KEY_COUNT+2));
    if (pScrollObj)
	{
		SaveAsDialog_InitScroll(_this, pScrollObj);
        _this->pIMEScroll = pScrollObj;
    }
    for(i = 0; i < MAX_FILETYPE_NUM; ++i)
    {
        _this->FileType[i] = (char*)malloc(128);
    }
}

static void SaveAsDialog_Destruct(SaveAsDialog* _this)
{   
    //先释放自身申请的资源
    DestroyFont(&_this->BlackFont);
    DestroyFont(&_this->WhiteFont);
    DestroyFont(&_this->CheckedFont);
    DestroyFont(&_this->RedFont);
    DestroyButton(_this->TextInput);
    DestroyButton(_this->SaveToUSBCheckBox);
    DestroyButton(_this->PromptInfo);
    DestroyButton(_this->FileNameLbl);
    DestroyButton(_this->FileFormatLbl);
    DestroyButton(_this->PinYinBtn);
    DestroyButton(_this->ButtonPagePrev);
    DestroyButton(_this->ButtonPageNext);

    int i;    
	for (i = 0; i < KEY_COUNT; i++)
	{	
		DestroyButton(_this->Keys[i]);
	}	
    for(i = 0; i < MAX_FILETYPE_NUM; ++i)
    {
        DestroyButton(_this->FileTypeList[i]);
        free(_this->FileType[i]);
        _this->FileType[i] = NULL;
    }
    for(i = 0; i < MAXCHINESENUM; ++i)
    {
        DestroyButton(_this->ChineseName[i]);
    }
    //最后析构基类
    Dialog_Destruct((Dialog*)_this);
	//free(_this->PathName);
    free(_this->FileName);
    free(_this->FileExtension);

    //_this->PathName = NULL;
    _this->FileName = NULL;
    _this->FileExtension = NULL;
    memset(_this->cPinyin, 0, sizeof(_this->cPinyin));
    _this->iCurrSelectdChinese = 0;
    DestroyScroll(&(_this->pIMEScroll));
}

//设置是否显示中文按钮（iFlag：0不显示，1显示）
static void IsChineseBtnDisplay(SaveAsDialog* _this, int iFlag)
{
    int i = 0;

    for(i = 0; i < MAXCHINESENUM; ++i)
    {
        if(iFlag)
        {
            Button_Show(_this->ChineseName[i]);
        }
        Button_SetEnabled(_this->ChineseName[i], 0);
    }

    Button_SetEnabled(_this->PinYinBtn, iFlag);
    Button_SetEnabled(_this->ButtonPagePrev, iFlag);
    Button_SetEnabled(_this->ButtonPageNext, iFlag);
    
    if(iFlag)
    {
        Button_Show(_this->PinYinBtn);
        Button_Show(_this->ButtonPagePrev);
        Button_Show(_this->ButtonPageNext);
    }
}

static void SaveAsDialog_Show(SaveAsDialog* _this)
{   
    //先显示基类(完全重写的话也可以不显示)
    Dialog_Show((Dialog*)_this);
    Button_Show(_this->SaveToUSBCheckBox);
    Button_Show(_this->PromptInfo);
    Button_Show(_this->TextInput);
    Button_Show(_this->FileNameLbl);
    
    if(_this->num)
        Button_Show(_this->FileFormatLbl);
    int i;    
    //显示自身内部的控件元素
    if((_this->currLanguageSet != LANG_KOREAN)&& (_this->currLanguageSet != LANG_CHINASIM))
	{
    	Button_SetEnabled(_this->Keys[KEY_ENON], 0);
        Button_SetFont(_this->Keys[KEY_ENON], getGlobalFnt(EN_FONT_GRAY));
        IsChineseBtnDisplay(_this, 0);
	}
    
    for (i = 0; i < KEY_COUNT; i++)
	{
        Button_Show(_this->Keys[i]);
	}
    for(i = 0; i < _this->num; ++i)
    {
        if(_this->num)
            Button_Show(_this->FileTypeList[i]);
    }
    
    if ((_this->currLanguageSet == LANG_CHINASIM) || (_this->currLanguageSet == LANG_KOREAN))
    {
        (_this->LangueType == 2) ? IsChineseBtnDisplay(_this, 1)
                                 : IsChineseBtnDisplay(_this, 0);
    }
    
    SetPowerEnable(2, 1);//不显示状态栏
}

//执行SaveAsDialog
static unsigned int SaveAsDialog_Exec(SaveAsDialog* _this)
{
    Dialog_Exec((Dialog*)_this);
    return _this->ClickedKey;
}

static void SaveAsDialog_ChangeCase(SaveAsDialog* _this)
{
    static char** text[] = {lowerKeyTextMap, upperKeyTextMap};
    //static int* code[] = {lowerCharCodeMap, upperCharCodeMap};

    _this->IsUppercase  =  _this->IsUppercase ? 0 : 1;
    int i = 0;
    for (i = 0; i < KEY_COUNT; ++i)
    {
    	if(i == KEY_ENON)
    	{
			continue;
		}
		if((_this->LangueType == 0) || (_this->LangueType == 2))
		{
        	Button_SetText(_this->Keys[i], TransString(text[_this->IsUppercase][i]));
		}
		else if(_this->LangueType == 1)
		{
			Button_SetText(_this->Keys[i], TransString(GcKeyName2[i][_this->IsUppercase]));
		}
		Button_Show(_this->Keys[i]);
    }
	
}


static void InsertPinYinChar(SaveAsDialog* _this, char cChar)
{
    int iPinYinLen = 0;

	iPinYinLen = strlen(_this->cPinyin);
	if(iPinYinLen < PINYIN_VALID_MAXLEN)
	{
		iPinYinLen = strlen(_this->cPinyin);
		_this->cPinyin[iPinYinLen] = cChar;
		_this->cPinyin[iPinYinLen+1] = '\0';
	}
}

static int DisplayPinYin(SaveAsDialog* _this)
{
    int iReturn = 0;	

    Button_SetText(_this->PinYinBtn, TransString(_this->cPinyin));
    Button_Show(_this->PinYinBtn);

	return iReturn;	
}


static int PinYinToChinese(SaveAsDialog* _this)
{
	int iReturn = 0;	

	if (NULL != _this->pMainWordSet)
    {
        if (_this->pMainWordSet->iWordCount)
        {
            GuiMemFree(_this->pMainWordSet->pWordCode);
        }
        GuiMemFree(_this->pMainWordSet);
    }
    _this->pMainWordSet = QueryWordPinyin(_this->cPinyin, MAXCHINESENUM);

	return iReturn;	
}

static int DisplayWordList(SaveAsDialog* _this)
{
	int iReturn = 0;
	int iTemp = 0;
	GUICHAR Word[2] = {0, 0};
    if (NULL == _this->pMainWordSet)
    {
    	for (iTemp = 0; iTemp < MAXCHINESENUM; iTemp++)
	    {
            Button_SetText(_this->ChineseName[iTemp], TransString(""));
            Button_Show(_this->ChineseName[iTemp]);
            Button_SetEnabled(_this->ChineseName[iTemp], 0);
	    }
        return -1;
    }

    if (_this->pMainWordSet->iViewLen > MAXCHINESENUM)
    {
        return -2;
    }

    for (iTemp = _this->pMainWordSet->iHeadPos; 
         iTemp < _this->pMainWordSet->iTailPos; 
         iTemp++)
    {
        Word[0] = _this->pMainWordSet->pWordCode[iTemp];
        int iHeadPos = iTemp - _this->pMainWordSet->iHeadPos;
        GUICHAR* strChineseName = _this->ChineseName[iHeadPos]->Text->pLblText->pTextData;
        StringCpy1(strChineseName, Word);
        SetLabelText(strChineseName, _this->ChineseName[iHeadPos]->Text);
        Button_SetEnabled(_this->ChineseName[iHeadPos], 1);
        Button_Show(_this->ChineseName[iHeadPos]);
    }

    for (iTemp = _this->pMainWordSet->iTailPos - _this->pMainWordSet->iHeadPos;
         iTemp < MAXCHINESENUM;
         iTemp++)
    {
		Button_SetText(_this->ChineseName[iTemp], TransString(""));
        Button_Show(_this->ChineseName[iTemp]);
        Button_SetEnabled(_this->ChineseName[iTemp], 0);
    }
         
	return iReturn;
}

//初始化滚轮
static int SaveAsDialog_InitScroll(SaveAsDialog* _this, BTN_SCROLL *pScroll)
{
    int i = 0;
	if (NULL == pScroll)
	{
		return -1;
	}
	pScroll[0].pScrollBtn = _this->ButtonPagePrev->Background1;
	pScroll[0].pScrollLbl = NULL;
	pScroll[0].pScrollBtn_Down = ButtonPagePrevDown;
	pScroll[0].pScrollBtn_Up = ButtonPagePrevUp;
	for(i=1; i<=MAXCHINESENUM; i++)
	{
		pScroll[i].pScrollBtn = _this->ChineseName[i]->Background1;
		pScroll[i].pScrollLbl = _this->ChineseName[i]->Text;
		pScroll[i].pScrollBtn_Down = ChineseNameDown;
		pScroll[i].pScrollBtn_Up = ChineseNameUp;		
	}	
	pScroll[0].pScrollBtn = _this->ButtonPageNext->Background1;
	pScroll[0].pScrollLbl = NULL;
	pScroll[0].pScrollBtn_Down = ButtonPageNextDown;
	pScroll[0].pScrollBtn_Up = ButtonPageNextUp;	
	for(i=(MAXCHINESENUM+2); i<KEY_COUNT; i++)
	{
		pScroll[i].pScrollBtn = _this->Keys[i]->Background1;
		pScroll[i].pScrollLbl = _this->Keys[i]->Text;
		pScroll[i].pScrollBtn_Down = KeyDown;
		pScroll[i].pScrollBtn_Up = KeyUp;		
	}
	return 0;
}

/***
  * ���ܣ�
        ��ʾ���̰�ť����
  * ������
        1.GUIPICTURE *pBtnFx:   ��Ҫ��ʾ�ļ��̰�ťͼ�ο�
        2.GUILABEL *pLblFx:     ��Ҫ��ʾ�ļ��̰�ť��ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
static int ShowBtn_Down(SaveAsDialog* _this, int iIndex)
{
    //�����־������ֵ����
    int iReturn = 0;
	if(iIndex == 0)
	{
	    Button_SetBackground(_this->ButtonPagePrev, BmpFileDirectory"btn_pageup_press1.bmp");
        Button_Show(_this->ButtonPagePrev);
	}
	else if((iIndex>=1) && (iIndex<=MAXCHINESENUM))
	{
        Button_SetBackground(_this->ChineseName[iIndex-1], BmpFileDirectory"btn_pinyin_press1.bmp");
        Button_SetFont(_this->ChineseName[iIndex-1], getGlobalFnt(EN_FONT_WHITE));
        Button_Show(_this->ChineseName[iIndex-1]);
        Button_SetFont(_this->ChineseName[iIndex-1], getGlobalFnt(EN_FONT_BLACK));
	}
	else if(iIndex == (MAXCHINESENUM+1))
	{
        Button_SetBackground(_this->ButtonPageNext, BmpFileDirectory"btn_pagedown_press1.bmp");
        Button_Show(_this->ButtonPageNext);
	}
    
    SyncCurrFbmap();
    return iReturn;
}

/***
  * ���ܣ�
        ��ʾ���̰�ť����
  * ������
        1.GUIPICTURE *pBtnFx:   ��Ҫ��ʾ�ļ��̰�ťͼ�ο�
        2.GUILABEL *pLblFx:     ��Ҫ��ʾ�ļ��̰�ť��ǩ
  * ���أ�
        �ɹ������㣬ʧ�ܷ��ط���ֵ
  * ��ע��
***/
static int ShowBtn_Up(SaveAsDialog* _this, int iIndex)
{
    //�����־������ֵ����
    int iReturn = 0;
	if(iIndex == 0)
	{
        Button_SetBackground(_this->ButtonPagePrev, BmpFileDirectory"btn_pageup_unpress1.bmp");
        Button_Show(_this->ButtonPagePrev);
	}
	else if((iIndex>=1) && (iIndex<=MAXCHINESENUM))
	{
        Button_SetBackground(_this->ChineseName[iIndex-1], BmpFileDirectory"btn_pinyin_unpress1.bmp");
        Button_Show(_this->ChineseName[iIndex-1]);
	}
	else if(iIndex == (MAXCHINESENUM+1))
	{
        Button_SetBackground(_this->ButtonPageNext, BmpFileDirectory"btn_pagedown_unpress1.bmp");
        Button_Show(_this->ButtonPageNext);
	}
    
    SyncCurrFbmap();
    return iReturn;
}


int SaveAsDialog_ShowScrollBtnDown(SaveAsDialog* _this, int index, int *pCurr, BTN_SCROLL *pScroll)
{
	ShowBtn_Down(_this, index);
	*pCurr = index;
	return 0;
}

int SaveAsDialog_ShowScrollBtnUp(SaveAsDialog* _this, int iCurr, BTN_SCROLL *pScroll)
{
	if (pScroll == NULL)
	{
		return -1;
	}

	ShowBtn_Up(_this, iCurr);
    
	return 0;
}

static void SaveAsDialog_AddChar(SaveAsDialog* _this, int c)
{
    //文件名中不能包含以下特殊字符，遇到这些字符直接返回，不进行输入
    //if (strchr("\\/:*?<>|;&`\'()\"", c))
    //    return;

	//默认最大长度 35
	if(StringStrlen(_this->InputBuffer) > SAVE_AS_MAX_INPUT)
		return;

	//韩文可显字符
	if((_this->LangueType == 1) &&
		    ((c>=14 && c<=23) ||
		   	 (c>=27 && c<=35) ||
		     (c>=40 && c<=46)))
	{	
		GUICHAR *code = TransString(GcKeyName2[c][_this->IsUppercase]);
		_this->CursorIndex = AddHanCharToString(_this->InputBuffer, *code);
		GuiMemFree(code);
	}
	else if((_this->LangueType == 2) &&
            ((c>=14 && c<=23) ||
		    (c>=27 && c<=35) ||
		    (c>=40 && c<=46)))//chinese
    {
        char cChar = _this->IsUppercase ? upperCharCodeMap[c] : lowerCharCodeMap[c];
        InsertPinYinChar(_this, cChar);
        DisplayPinYin(_this);
        PinYinToChinese(_this);
        DisplayWordList(_this);
        _this->CursorIndex = StringStrlen(_this->InputBuffer) - 1;
    }
	else
    {
		int code = _this->IsUppercase ? upperCharCodeMap[c] : lowerCharCodeMap[c];
		
		if ((strchr("/\\:*?<>|\"", code)) || (StringStrlen(_this->InputBuffer) > SAVE_AS_MAX_INPUT))
        return;
    	//增加输入字符到光标位置
   		 _this->CursorIndex = AddCharToString(_this->InputBuffer, code);
	}
    //SaveAsDialog_UpdatePromptInfo(_this, " ");
    //更新显示
    SaveAsDialog_UpdateInputText(_this);
}

static void SaveAsDialog_DelChar(SaveAsDialog* _this)
{
    //删除光标位置的字符
    if(_this->CursorIndex)
    {
        int index = DelCharFromString(_this->InputBuffer);
        if(index >= 0)
        {
            _this->CursorIndex = index;
        }
    }
    //SaveAsDialog_UpdatePromptInfo(_this, " ");
    //更新显示
    SaveAsDialog_UpdateInputText(_this);
}

static void SaveAsDialog_MoveCursorLeft(SaveAsDialog* _this)
{
    //向左移动光标
    if((_this->CursorIndex < StringStrlen(_this->InputBuffer)) && (_this->CursorIndex > 0))//防止初始化时边界值出现bug
    {
        _this->CursorIndex--;
        SwapCharPos(_this->InputBuffer, 0);
    }
    //更新显示
    SaveAsDialog_UpdateInputText(_this);    
}

static void SaveAsDialog_MoveCursorRight(SaveAsDialog* _this)
{
    //向右移动光标
    if(_this->CursorIndex < StringStrlen(_this->InputBuffer) - 1)//防止初始化时边界值出现bug
    {
        _this->CursorIndex++;
        SwapCharPos(_this->InputBuffer, 1);
    }    
    //更新显示
    SaveAsDialog_UpdateInputText(_this);    
}

static void SaveAsDialog_InitInputText(SaveAsDialog* _this)
{
    if(_this == NULL)
        return;
    //strcpy(_this->InputBuffer, _this->FileName);
	GUICHAR *temp = TransString(_this->FileName);
	StringCpy1(_this->InputBuffer, temp);
	GuiMemFree(temp);
    int len = StringStrlen(_this->InputBuffer);
    _this->InputBuffer[len] = (unsigned short)'|';
    _this->InputBuffer[len + 1] = (unsigned short)'\0';
    Button_SetText(_this->TextInput, GetUnicode(_this->InputBuffer));
    //SaveAsDialog_UpdatePromptInfo(_this, "");    
    _this->CursorIndex = len; 
}

static void SaveAsDialog_UpdateInputText(SaveAsDialog* _this)
{
	GUICHAR *strText = GetUnicode(_this->InputBuffer);
	Button_SetText(_this->TextInput, strText);
    Button_Show(_this->TextInput);
}

static void SaveAsDialog_InitFileNameList(SaveAsDialog* _this)
{
    if(_this == NULL)
        return;
    //_this->PathName = (char*)malloc(128);
    _this->FileName = (char*)malloc(F_NAME_MAX);
    _this->FileExtension = (char*)malloc(40);
    GetInitFileName(_this, _this->OriginalFileName);
    GetInitFileExtension(_this, _this->OriginalFileName);
}

static void SetCheckBoxTextFont(SaveAsDialog* _this, Button* checkBox)
{
    if(checkBox == NULL)
        return;
    if(checkBox->Checked)
        Button_SetFont(checkBox, _this->CheckedFont);
    else
        Button_SetFont(checkBox, _this->BlackFont);
    checkBox->Checkable = 1;
}

static void SaveAsDialog_InitFileTypeList(SaveAsDialog* _this, char* fileType[], int num, unsigned int defaultOption)
{
    if(fileType == NULL && defaultOption > num)
        return;
    int i;
    if(num)
    {
        for(i = 0; i < num; ++i)
        {
            //_this->FileType[i] = fileType[i];
            strcpy(_this->FileType[i], fileType[i]);
            Button_SetText(_this->FileTypeList[i], TransString(_this->FileType[i]));
            _this->FileTypeList[i]->Checked = (i == defaultOption - 1) ? 1 : 0;
            SetCheckBoxTextFont(_this, _this->FileTypeList[i]);
        }
        memset(_this->FileExtension, 0, strlen(_this->FileExtension) + 1);
        strcpy(_this->FileExtension, _this->FileType[defaultOption - 1]);
    }
}

static void SaveAsDialog_InitDialogBackground(SaveAsDialog* _this, int num)
{
    if(num)
        Dialog_SetBackground((Dialog *)_this, BmpFileDirectory"bg_SaveAsDialog1.bmp");
    else
        Dialog_SetBackground((Dialog *)_this, BmpFileDirectory"bg_SaveAsDialog.bmp");
}

static void SaveAsDialog_ProcessKeyCode(SaveAsDialog* _this, int code)
{
    LOG(LOG_INFO, "---SaveAsDialog_ProcessKeyCode code %d\n", code);
    switch (code)
    {
    //删除一个字符
    case KEY_BACKSPACE_INDEX:
        _this->LangueType == 2 ? SaveAsDialog_DelChineseChar(_this)
                               : SaveAsDialog_DelChar(_this);
        break;

    //回车按钮处理
    case KEY_ENTER_INDEX:
        //将结果保存到输出参数中
        //TODO:
        SaveAsDialog_SaveParameter(_this);
        _this->ClickedKey = SAVE_AS_DIALOG_KEY_ENTER;
        Dialog_Close((Dialog*)_this);
        break;

    //Esc 按钮处理
    case KEY_ESC_INDEX:
        //防止下次进入还是之前的文件名
        memset(_this->InputBuffer, 0, sizeof(_this->InputBuffer));
        _this->ClickedKey = SAVE_AS_DIALOG_KEY_ESC;
        Dialog_Close((Dialog*)_this);
        break;

    //Shift 进行键盘切换
    case KEY_SHIFT_INDEX:
        SaveAsDialog_ChangeCase(_this);
        break;

    //向左移动光标
    case KEY_LEFT_INDEX:
        SaveAsDialog_MoveCursorLeft(_this);
        break;

    //向右移动光标
    case KEY_RIGHT_INDEX:
        SaveAsDialog_MoveCursorRight(_this);
        break;

    //上下按钮暂时无功能
    case KEY_UP_INDEX:
    case KEY_DOWN_INDEX:
        //do nothing
        break;
    
	//英韩切换
	case KEY_ENON:
		SaveAsDialog_ChangeLangueInput(_this);
		break;
    //可见字符，输入到输入框中
    default:
        SaveAsDialog_AddChar(_this, code);
        break;
    }
}

//初始化传入的文件参数
static void SaveAsDialog_InitParameter(SaveAsDialog* _this, char* orignalName, char* newFileName,
                                char* fileType[], int num, unsigned int defaultOption, int* selectUsb)
{
    _this->OriginalFileName = orignalName;
    _this->NewFileName = newFileName;
    _this->selectUsbFlag = selectUsb;
    _this->num = num;
    SaveAsDialog_InitFileNameList(_this);
    SaveAsDialog_InitInputText(_this);
    SaveAsDialog_InitFileTypeList(_this, fileType, num, defaultOption);
    SaveAsDialog_InitDialogBackground(_this, num);
}

static void SaveAsDialog_SaveParameter(SaveAsDialog* _this)
{
    if(_this == NULL)
        return;
    SaveFilePathAndName(_this);
    //防止下次进入还是之前的文件名
    memset(_this->InputBuffer, 0, sizeof(_this->InputBuffer));
    LOG(LOG_INFO, "_this->NewFileName is %s\n", _this->NewFileName);
}

static int KeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    Button* key = _this->Keys[iOutLen];
    Button_Down(key);
    SyncCurrFbmap();
	//仅当韩文模式下 53 即可触发，
	if(iOutLen == KEY_ENON)
	{
	    if(_this->LangueType == 0)
        {
            if(_this->currLanguageSet == LANG_KOREAN)
            {
                _this->LangueType = 1;
			    Button_SetText(key, TransString("KR"));
            }
            else if(_this->currLanguageSet == LANG_CHINASIM)
            {
                _this->LangueType = 2;
                Button_SetBackground(key, CN_KEY_BMP);
                Button_SetPressedBackground(key, CN_KEY_PRESS_BMP);
            }
        } 
        else
        {
            _this->LangueType = 0;
            if(_this->currLanguageSet == LANG_CHINASIM)
            {
                Button_SetBackground(key, EN_KEY_BMP);
                Button_SetPressedBackground(key, EN_KEY_PRESS_BMP);
            }
            else
            {
                Button_SetText(key, TransString("EN"));
            }
        }

        SaveAsDialog_Show(_this);
	}
    return 0;
}

static int KeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{    
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    Button* key = _this->Keys[iOutLen];
    Button_Up(key);
    SaveAsDialog_ProcessKeyCode(_this, key->Code);
    SyncCurrFbmap();
    return 0;   
}

static int CheckBoxSaveToUSBDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    return 0;
}

static int CheckBoxSaveToUSBUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    Button* checkBox = _this->SaveToUSBCheckBox;
    if(checkBox->Checked)
    {
        checkBox->Checked = 0;
        *(_this->selectUsbFlag) = 0;
    }
    else
    {
        if(IsFileExist(MntUsbDirectory))
        {
            checkBox->Checked = 1;
            //SaveAsDialog_UpdatePromptInfo(_this);
            Button_SetText(_this->PromptInfo, TransString(" "));
            *(_this->selectUsbFlag) = 1;
        }
        else
        {
            checkBox->Checked = 0;
            Button_SetText(_this->PromptInfo, TransString("DIALOG_NO_USB"));            
            *(_this->selectUsbFlag) = 0;
        }
        DisplayPicture(_this->PromptInfo->Background1);
        DisplayLabel(_this->PromptInfo->Text);
    }
    DisplayCheckBoxChecked(_this, checkBox);
    SyncCurrFbmap();
    return 0;   
}

//中文选字按下处理
static int ChineseNameDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iReturn = 0;	
    int iTemp = iOutLen;	
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
	if(_this->ucKnobFlg)
	{
		_this->ucKnobFlg = 0;
		SaveAsDialog_ShowScrollBtnDown(_this, iTemp, &(_this->iCurrSelectdChinese), _this->pIMEScroll);	
		iTemp -=1;
	}
	else
	{
		SaveAsDialog_ShowScrollBtnDown(_this, iTemp+1, &(_this->iCurrSelectdChinese), _this->pIMEScroll);
	}	

	if(NULL == _this->ChineseName[iTemp]->Text->pLblText->pTextData)
	{
		return -1;
	}
	else
	{
		if(_this->ChineseName[iTemp]->Text->pLblText->pTextData[0] == (GUICHAR)'\0')
		{
			return -2;
		}
	}

    return iReturn;
}

//中文选字按键抬起处理
static int ChineseNameUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    int iReturn = 0;	
    int iTemp = iOutLen;	
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
	if(_this->ucKnobFlg)
	{
		_this->ucKnobFlg = 0;
		SaveAsDialog_ShowScrollBtnUp(_this, iTemp, _this->pIMEScroll);
		iTemp -=1;
	}
	else
	{
		SaveAsDialog_ShowScrollBtnUp(_this, iTemp+1, _this->pIMEScroll);
	}	
	if(NULL == _this->ChineseName[iTemp]->Text->pLblText->pTextData)
	{
		return -1;
	}
	else
	{
		if(_this->ChineseName[iTemp]->Text->pLblText->pTextData[0] == (GUICHAR)'\0')
		{
			return -2;
		}
	}
	AddCharToString(_this->InputBuffer, _this->ChineseName[iTemp]->Text->pLblText->pTextData[0]);
	SaveAsDialog_UpdateInputText(_this);
    _this->CursorIndex = StringStrlen(_this->InputBuffer) - 1;
	memset(_this->cPinyin, 0, sizeof(_this->cPinyin));
	DisplayPinYin(_this); 
    SyncCurrFbmap();
    return iReturn;
}


static int WordListTurnPage(SaveAsDialog* _this, int iTurnFlag)
{
	int iReturn = 0;	

    if (NULL == _this->pMainWordSet)
    {
        return -1;
    }

    if (0 == iTurnFlag)//上一页处理
    {
        if (0 == _this->pMainWordSet->iHeadPos)
        {
            return iReturn;
        }

        _this->pMainWordSet->iHeadPos -= _this->pMainWordSet->iViewLen;
        if (_this->pMainWordSet->iHeadPos < 0)
        {
            _this->pMainWordSet->iHeadPos = 0;
        }
    }
    else
    {
        if (_this->pMainWordSet->iTailPos == _this->pMainWordSet->iWordCount)
        {
            return iReturn;
        }

        _this->pMainWordSet->iHeadPos += _this->pMainWordSet->iViewLen;
        if (_this->pMainWordSet->iHeadPos >= _this->pMainWordSet->iWordCount)
        {
            _this->pMainWordSet->iHeadPos -= _this->pMainWordSet->iViewLen;
        }
    }    
    _this->pMainWordSet->iTailPos = _this->pMainWordSet->iHeadPos + _this->pMainWordSet->iViewLen;
    if (_this->pMainWordSet->iTailPos > _this->pMainWordSet->iWordCount)
    {
        _this->pMainWordSet->iTailPos = _this->pMainWordSet->iWordCount;
    }

    DisplayWordList(_this);

    return iReturn;	
}

//上一页按键按下
static int ButtonPagePrevDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    SaveAsDialog_ShowScrollBtnDown(_this, 0, &(_this->iCurrSelectdChinese), _this->pIMEScroll);
    SyncCurrFbmap();    
    return 0;
}

//上一页按键弹起
static int ButtonPagePrevUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    SaveAsDialog_ShowScrollBtnUp(_this, 0, _this->pIMEScroll);
	WordListTurnPage(_this, 0);
    SyncCurrFbmap();
    return 0;   
}

//下一页按键按下
static int ButtonPageNextDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    SaveAsDialog_ShowScrollBtnDown(_this, MAXCHINESENUM+1, &(_this->iCurrSelectdChinese), _this->pIMEScroll);
    SyncCurrFbmap();
    return 0;
}

//下一页按键弹起
static int ButtonPageNextUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    SaveAsDialog_ShowScrollBtnUp(_this, MAXCHINESENUM+1, _this->pIMEScroll);
	WordListTurnPage(_this, 1);
	SyncCurrFbmap();
    
    return 0;   
}

static int SaveAsKeyDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;

    uiValue = (unsigned int)pInArg;
    
	switch (uiValue)
    {
	case KEYCODE_ESC:
		break;
	case KEYCODE_HOME:
		break;
	default:
		break;
	}
	
	return iReturn;

}

static int SaveAsKeyUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    //错误标志、返回值定义
	int iReturn = 0;
    unsigned int uiValue;
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    uiValue = (unsigned int)pInArg;
    LOG(LOG_INFO, "----SaveAsKeyUp----uiValue %d\n", uiValue);
    switch (uiValue)
    {
	case KEYCODE_ESC:
        SaveAsDialog_ProcessKeyCode(_this, KEY_ESC_INDEX);
		break;
	case KEYCODE_HOME:
	#ifdef SAVE_SCREEN
		ScreenShot();
	#endif
		break;
    case KEYCODE_LEFT:
        SaveAsDialog_ProcessKeyCode(_this, KEY_LEFT_INDEX);
        SyncCurrFbmap();
        break;
    case KEYCODE_RIGHT:
        SaveAsDialog_ProcessKeyCode(_this, KEY_RIGHT_INDEX);
        SyncCurrFbmap();
        break;
    case KEYCODE_ENTER:
        SaveAsDialog_ProcessKeyCode(_this, KEY_ENTER_INDEX);
        break;
	default:
		break;
	}
	
	return iReturn;
}

//文件类型列表相应函数
static int CheckBoxFileTypeListDown(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    return 0;
}

static int CheckBoxFileTypeListUp(void *pInArg, int iInLen, void *pOutArg, int iOutLen)
{
    SaveAsDialog* _this = (SaveAsDialog*)pOutArg;
    if(_this->num == 0 || iOutLen >= _this->num)
        return -1;
    int i;
    for(i = 0; i < _this->num; ++i)
    {
        if(i == iOutLen)
        {
            _this->FileTypeList[i]->Checked = 1;
            memset(_this->FileExtension, 0, strlen(_this->FileExtension) + 1);
            strcpy(_this->FileExtension, _this->FileType[i]);
            DisplayPicture(_this->FileTypeList[i]->Background2);
        }
        else
        {
            _this->FileTypeList[i]->Checked = 0;
            DisplayPicture(_this->FileTypeList[i]->Background1);
        }
        SetCheckBoxTextFont(_this, _this->FileTypeList[i]);
        DisplayLabel(_this->FileTypeList[i]->Text);
    }
    SyncCurrFbmap();
    return 0;   
}

static void DisplayCheckBoxChecked(SaveAsDialog* _this, Button* checkBox)
{
    if((_this == NULL) || (checkBox == NULL))
        return;
    if(checkBox->Checked)
    {
        DisplayPicture(checkBox->Background2);
        Button_SetFont(checkBox, _this->CheckedFont);
    }
    else
    {
        DisplayPicture(checkBox->Background1);
        Button_SetFont(checkBox, _this->BlackFont);
    }
    DisplayLabel(checkBox->Text);
}

//在字符串中查找指定的字符，返回字符在字符串中的位置
static int FindCharFromString(unsigned short* cString, unsigned short c)
{
    if(cString == NULL)
        return -1;
    int iPos = -2;
	int len = StringStrlen(cString);

	for(iPos = 0; iPos < len; iPos++)
	{
		if(cString[iPos] == c)
		{
			return iPos;
		}
	}
    //char* str = strchr(cString, c);
    //iPos = (str == NULL) ? -2 : (strlen(cString) - strlen(str));
    return iPos;
}

//在光标位置添加字符
static int AddCharToString(unsigned short* cString, int c)
{
    if(cString == NULL)
        return -1;
    int len = StringStrlen(cString);
    int i;
    if(len == 0)
    {
        cString[0] = c;
        cString[1] = (unsigned short)'|';
        cString[2] = (unsigned short)'\0';
        return 1;
    }
    int iPos = FindCharFromString(cString, (unsigned short)'|');
	
    unsigned short buf[512] = {};
    StringCpy1(buf, cString);
    for(i = iPos; i < len; ++i)
    {
        cString[i + 1] = buf[i];
    }
    cString[iPos] = c;
    cString[len + 1] = (unsigned short)'\0';
    return (iPos + 1);
}

//在光标位置删除字符
static int DelCharFromString(unsigned short* cString)
{
    if(cString == NULL)
        return -1;
    int len = StringStrlen(cString);
    if(len == 0)
    {
        cString[0] = (unsigned short)'|';
        cString[1] = (unsigned short)'\0';
        return 1;
    }
    int iPos = DelChar(cString, 1);
    return (iPos - 1);
}

//交换字符串中字符的位置
static void SwapCharPos(unsigned short* cString, int direction)
{
    if(cString == NULL)
        return;
    int len = StringStrlen(cString);
    int i;
    for(i = 0; i < len; ++i)
    {
        if(cString[i] == (unsigned short)'|')
        {
            int temp = 0;
            temp = cString[i];
            if(direction)
            {
                cString[i] = cString[i + 1];
                cString[i + 1] = temp;
            }
            else
            {
                cString[i] = cString[i-1];
                cString[i - 1] = temp;
            }
            break;
        }
    }
}

//删除指定字符
static int DelChar(unsigned short* cString, int flag)
{
    if(cString == NULL)
        return 0;
    int iPos = FindCharFromString(cString, (unsigned short)'|');
    int len = StringStrlen(cString);
    int i;
    for(i = iPos; i < len -1; ++i)
    {
        cString[i] = cString[i + 1];
    }
    if(flag && iPos)
    {
        cString[iPos - 1] = (unsigned short)'|';
    }
    cString[len - 1] = (unsigned short)'\0';
    return iPos;
}

//保存文件名(路径+文件名+后缀名)
static void SaveFilePathAndName(SaveAsDialog* _this)
{
    if(_this == NULL)
        return;
    //2、文件名
    char fileName[F_NAME_MAX] = {};	
	//去除光标
    DelChar(_this->InputBuffer, 0);
	//去除末端空格
	StringUcs2TrimSpace(_this->InputBuffer);
	GetUcs2ToUtf8(_this->InputBuffer,fileName);

    LOG(LOG_INFO, "\n  saver loading!!!!!\n");
    //3、组合
    memset(_this->NewFileName, 0, strlen(_this->NewFileName) + 1);
    sprintf(_this->NewFileName, "%s%s", fileName, _this->FileExtension);
}
#if 0
//比较两个字符串，不区分大小写
int StrIcmp(const char* str1, const char* str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int num = 'a' - 'A';
    while(*str1)
    {
        char c1 = *str1;
        char c2 = *str2;
        if(c1 >= 'A' && c1 <= 'Z')
        {
            c1 += num;
        }
        if(c2 >= 'A' && c2 <= 'Z')
        {
            c2 += num;
        }
        if(c1 != c2)
        {
            return (c1 - c2);
        }
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

//获取初始的文件路径名
static void GetInitFilePathName(SaveAsDialog* _this, char* originalName)
{
    if(originalName == NULL)
        return;
    char pathName[128] = {};
    char* buf = strrchr(originalName, '/');
    if(buf)
    {
        memset(pathName, 0, strlen(pathName));
        memcpy(pathName, originalName, strlen(originalName) - strlen(buf) + 1);
        pathName[strlen(pathName)] = '\0';
        printf("pathName is %s\n", pathName);
        strcpy(_this->PathName, pathName);
    }
}
#endif
//获取初始的文件名
static void GetInitFileName(SaveAsDialog* _this, char* originalName)
{
    if(originalName == NULL)
        return;
    char fileName[F_NAME_MAX] = {};
    memset(fileName, 0, strlen(fileName));
    char* buf = strrchr(originalName, '.');
    if(buf)
    {
        memcpy(fileName, originalName, strlen(originalName) - strlen(buf));
        fileName[strlen(fileName)] = '\0';
        LOG(LOG_INFO, "fileName is %s\n", fileName);
        strcpy(_this->FileName, fileName);
    }
}

//获取初始的文件名后缀
static void GetInitFileExtension(SaveAsDialog* _this, char* originalName)
{
    if(originalName == NULL)
        return;
    char fileExtension[128] = {};
    memset(fileExtension, 0, strlen(fileExtension));
    char* buf = strrchr(originalName, '.');
    if(buf)
    {
        strcpy(fileExtension, buf);
        LOG(LOG_INFO, "fileExtension is %s\n", fileExtension);
        strcpy(_this->FileExtension, fileExtension);
    }
}
#if 0
//保存文件路径名
static void SaveFilePathName(SaveAsDialog* _this, char* pathName)
{
    if(_this == NULL)
        return;
    if(_this->SaveToUSBCheckBox->Checked)
    {
        strcpy(pathName, "/mnt/usb/");
    }
    else
    {
        strcpy(pathName, _this->PathName);
    }
}
#endif
//判断文件名是否存在，0:不存在，1:存在
static int IsFileExist(char* pathName)
{
    int iReturn = 0;

    if(pathName == NULL)
        return 0;
    if((access(pathName, F_OK)) != -1)
    {
        iReturn = 1;
    }
    
    return iReturn;
}
/***
  * 功能：
        弹出带有键盘的保存文件的dialog
  * 参数：
        1.originalName  : 原始文件名(传入参数)
        2.newName       : 新的文件名(传出参数)
        3.fileType[]    : 文件类型列表(传入参数)
        4.num           : 文件类型列表个数(传入参数)
        5.defaultOption : 默认文件类型(传入参数)(暂时可输入1, 2, 3)
        6.selectUsb     : 是否保存到usb(传出参数)
  * 返回：
        执行完成返回用户点击的按钮,输出最新的文件名
  * 备注：
***/
int ShowSaveAsDialog(char* originalName, char* newName, char* fileType[], int num, unsigned int defaultOption, int* selectUsb)
{
    *selectUsb = 0;
    SaveAsDialog dialog;
    SaveAsDialog_Construct(&dialog, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    SaveAsDialog_InitParameter(&dialog, originalName, newName, fileType, num, defaultOption, selectUsb);
       
    return (int)SaveAsDialog_Exec(&dialog);
}

//输入法切换
static void SaveAsDialog_ChangeLangueInput(SaveAsDialog * _this)
{
	int i = 0;
	if((_this->LangueType == 0) || (_this->LangueType == 2))
	{
		for (i = 0; i < KEY_COUNT; i++)
		{	
			if(i == KEY_ENON)
			{
				continue;
			}
			if(_this->IsUppercase == 0)
			{
				Button_SetText(_this->Keys[i], TransString(lowerKeyTextMap[i]));
            }
			else
			{
				Button_SetText(_this->Keys[i], TransString(upperKeyTextMap[i]));
			}
        	Button_Show(_this->Keys[i]);
		}
	}
	else if(_this->LangueType == 1)
	{
		for (i = 0; i < KEY_COUNT; i++)
		{	
			if(i == KEY_ENON)
			{
				continue;
			}
			Button_SetText(_this->Keys[i], TransString(GcKeyName2[i][_this->IsUppercase]));
        	Button_Show(_this->Keys[i]);
		}
	}
}

static void DeletePinYinChar(SaveAsDialog* _this)
{
	int iPinYinLen = 0;

	iPinYinLen = strlen(_this->cPinyin);
	
	if(iPinYinLen != 0)
	{
		iPinYinLen = strlen(_this->cPinyin);
		_this->cPinyin[iPinYinLen-1] = '\0';
	}
	else
	{
		SaveAsDialog_DelChar(_this);
	}

}

//删除中文文字
static void SaveAsDialog_DelChineseChar(SaveAsDialog* _this)
{
    DeletePinYinChar(_this);
	DisplayPinYin(_this);
	PinYinToChinese(_this);
	DisplayWordList(_this);	
	SaveAsDialog_UpdateInputText(_this);
}

//韩文 字符 拼接
static int AddHanCharToString(unsigned short *cString,unsigned short usChar)
{
	if(cString == NULL)
        return -1;
	
	int i = 0;
	int iInStrLength = 1;	
	int iLenUniBuff = 0;
	
	i = StringStrlen(cString);
	//去除光标
	iLenUniBuff = StringStrlen(cString) - 1;

    int iPos = FindCharFromString(cString, (unsigned short)'|');

	//韩文光标 0尾部 1首部 2中部
	int posIsHead = 0;
	if(isJamo(usChar))
	{
		//韩文光标在首部 
		//韩文输入时，头插插时， '|'AB >> C'|'AB	
		if((iPos == 0) && (i > 1))
		{
			posIsHead = 1;
		}
		else if(iPos != i-1)
		{
			posIsHead = 2;
		}
		unsigned short inputunicode[2] = { 0 };
		unsigned short outunicode[2] = { 0 };
		
		//取上一个元素 即光标的 前一个
		if(iPos > 0)
		{
			inputunicode[0] = cString[iPos-1];
		}
		else
		{
			inputunicode[0] = 0;
		}
		inputunicode[1] = usChar;
			
		int ret = hangulIME(inputunicode, outunicode);
		
		//把显示框中最后一个字符删除，替代成刚刚组合出来的最新的字符
		if (ret == 1)
        {	
			if(posIsHead == 1)		//    '|'A    B'|'A
           	{
           		for(; i>iPos; i--)
				{
					cString[i] = cString[i-1];
				}
				iPos++;
				cString[iPos-1] = outunicode[0];
			}
			else			
			{
            	if(iPos > 0)//	A'|'		@'|'    或      A'|'B      @'|'B
            	{            		
            		cString[iPos-1] = outunicode[0];					
				}
				else			  //	'|'         A'|'
				{
					iPos++;
					cString[iPos] = cString[iPos-1];				
					cString[iPos-1] = outunicode[0];	
				}
			}
        }
		//把显示框中最后一个字符删除，显示框上追加输出来的两个字符
        else if (ret == 2)//如果返回值为2，返回了2个字符
        {
            if(posIsHead == 2)   //		A'|'B     AC'|'B
            {
            	for(; i>iPos; i--)
				{
					cString[i] = cString[i-1];
				}
				iPos++;
				
				cString[iPos-2] = outunicode[0];
				cString[iPos-1] = outunicode[1];	
            }
           	else				//       A'|'      AB'|'
           	{
				iPos++;
				cString[iPos] = cString[iPos-1];
				
				cString[iPos-2] = outunicode[0];
				cString[iPos-1] = outunicode[1];	
           	}
        }
        else//这种情况下，也就是说无法进行组合，显示框最后一个字符可能不是韩语字符
        {
            //所以只要往显示框追加新按键的字符就行了
			if(posIsHead == 0)
			{
				iPos++;
				cString[iPos] = cString[iPos-1];
				
            	cString[iPos-1] = usChar;
			}
			else if(posIsHead == 1)
			{
				for(; i>iPos; i--)
				{
					cString[i] = cString[i-1];
				}
				iPos++;
				
				cString[iPos-1] = usChar;
			}
			else
			{
				for(; i>iPos; i--)
				{
					cString[i] = cString[i-1];
				}
				iPos++;
				
				cString[iPos-1] = usChar;	
			}  
        }
	}

	iInStrLength = StringStrlen(cString);	
	cString[iInStrLength] = (unsigned short)'\0';
	
	return iPos;
}
