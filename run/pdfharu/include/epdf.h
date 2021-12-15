#ifndef FEATURE_EPDF
#define FEATURE_EPDF
/*
	Easy PDF Version v3.0
*/
#ifdef __cplusplus
extern "C" {
#endif

/*----- 权限限制-----------------------*/
#define EPDF_ENABLE_READ         0				// 可读取
#define EPDF_ENABLE_PRINT        4				// 可打印
#define EPDF_ENABLE_EDIT_ALL     8				// 可编辑
#define EPDF_ENABLE_COPY         16				// 可复制
#define EPDF_ENABLE_EDIT         32				// 可编辑


/*----- 语言支持-----------------------*/

// 以下语言使用的是内置字体方案
#define EPDF_EN		"EN"		// 英文
#define EPDF_CN		"CN"		// 中文
#define EPDF_KO		"KO"		// 韩文
#define EPDF_JP		"JP"		// 日文
#define EPDF_GE		"GE"		// 德文

#define EPDF_FR		"FR"		// 法文
#define EPDF_SP		"SP"		// 西班牙
#define EPDF_TU		"TU"		// 土耳其
#define EPDF_IT		"IT"		// 意大利
#define EPDF_PO		"PO"		// 葡萄牙

#define EPDF_SW		"SW"		// 瑞典
#define EPDF_PL		"PL"		// 波兰
#define EPDF_DA		"DA"		// 丹麦
#define EPDF_CZ		"CZ"		// 捷克
#define EPDF_IR		"IR"		// 爱尔兰

#define EPDF_HU		"HU"		// 匈牙利 
#define EPDF_DU		"DU"		// 荷兰语 
#define EPDF_LA		"LA"		// 拉丁语
#define EPDF_RO		"RO"		// 罗马
#define EPDF_SU		"SU"		// 芬兰语

#define EPDF_SE		"SE"		// 塞尔维亚
#define EPDF_CR		"CR"		// 克罗地亚
#define EPDF_NO		"NO"		// 挪威

// 以下语言使用的是内嵌字体方案
#define EPDF_RU		"RU"		// 俄文
#define EPDF_AR		"AR"		// 阿拉伯
#define EPDF_VI		"VI"		// 越南
#define EPDF_TH		"TH"		// 泰文
#define EPDF_PE		"PE"		// 波斯

#define EPDF_HI		"HI"		// 印度
#define EPDF_UK		"UK"		// 乌克兰
#define EPDF_GR		"GR"		// 希腊
#define EPDF_BE		"BE"		// 孟加拉
#define EPDF_UN		"UN"		// UNICODE全语言

typedef enum _LangInternal{
	EPDF_LANG_EN,
	EPDF_LANG_CN,
	EPDF_LANG_KO,
	EPDF_LANG_JP,
	EPDF_LANG_GE,

	EPDF_LANG_FR,
	EPDF_LANG_SP,
	EPDF_LANG_TU,
	EPDF_LANG_IT,
	EPDF_LANG_PO,

	EPDF_LANG_SW,
	EPDF_LANG_PL,
	EPDF_LANG_DA,
	EPDF_LANG_CZ,
	EPDF_LANG_IR,

	EPDF_LANG_HU,
	EPDF_LANG_DU,

	EPDF_LANG_RU,
	EPDF_LANG_AR,
	EPDF_LANG_VI,
	EPDF_LANG_TH,
	EPDF_LANG_PE,

	EPDF_LANG_HI,
	EPDF_LANG_UK,
	EPDF_LANG_GR,
    EPDF_LANG_BE,
    EPDF_LANG_UN,
} EPDF_LangInternal;

/*----- 标准字体 -----------------------*/
/*
	注: PDF 标准字体
*/
typedef enum _FontInternal {
	FONT_NONE = 0,
	FONT_COURIER,
	FONT_COURIER_BOLD,
	FONT_COURIER_OBLIQUE,
	FONT_COURIER_BOLD_OBLIQUE,
	FONT_HELVETICA,
	FONT_HELVETICA_BOLD,
	FONT_HELVETICA_OBLIQUE,
	FONT_HELVETICA_BOLD_OBLIQUE,
	FONT_TIMES_ROMAN,
	FONT_TIMES_BOLD,
	FONT_TIMES_ITALIC,
	FONT_TIMES_BOLD_ITALIC,
	FONT_SYMBOL,
	FONT_ZAPF_DINGBATS
} EPDF_FontInternal;

// 文字对齐方式
typedef enum _TextAlignment{
    ALIGN_LEFT = 0,
    ALIGN_RIGHT,
    ALIGN_CENTER,
    ALIGN_JUSTIFY
} EPDF_TextAlignment;

// 文字样式
typedef enum _TextStyle{
	TEXT_NORMAL = 0,		// 正常
	TEXT_BOLD,				// 粗体 
	TEXT_ITALIC,			// 斜体 
	TEXT_BOLD_AND_ITALIC
} EPDF_TextStyle;

// 预定义页大小
typedef enum _PageSize{
    PAGE_SIZE_LETTER = 0,
    PAGE_SIZE_LEGAL,
    PAGE_SIZE_A3,			// A3 纸
    PAGE_SIZE_A4,			// A4 纸
    PAGE_SIZE_A5,
    PAGE_SIZE_B4,
    PAGE_SIZE_B5,
    PAGE_SIZE_EXECUTIVE,
    PAGE_SIZE_US4x6,
    PAGE_SIZE_US4x8,
    PAGE_SIZE_US5x7,
    PAGE_SIZE_COMM10,
    PAGE_SIZE_EOF
} EPDF_PageSize;

// 
typedef enum _PageDir{
    PAGE_PORTRAIT = 0,
    PAGE_LANDSCAPE
} EPDF_PageDir;


// 矩形结构
typedef struct {
	float x, y;
	float w, h;
} EPDF_Rect;

/*
	创建一个新的PDF文档
	file_name : 文档文名
	mode : 权限标志
*/
int epdf_new(char *file_name, int mode);

/*
	保存PDF文档，并释放内存
	pdf : epdf_new 返回值 
*/
int epdf_close(int pdf);

/*
	添加一个新的空白页
*/
int epdf_page(int pdf, float width, float height);

/*
	添加一个新的预定义大小的页
*/
int epdf_page2(int pdf, EPDF_PageSize size, EPDF_PageDir dir);

/*
    设置页大小方向
 */
int epdf_pageset(int page, EPDF_PageSize size, EPDF_PageDir dir);

/*
    获取页的宽
*/
float epdf_pagewidth(int page);

/*
    获取页的高
*/
float epdf_pageheight(int page);

/*
	添加一个目录索引项
*/
int epdf_list(int pdf, int parent, char * title, int opened);

/*
	将一个索引项与一个页连接
*/
int epdf_link(int list, int page);

/*
	计算指定字号文字宽度
*/
float epdf_textwidth(int page, float font_size, char * language, char * text);

/*
	在指定位置显示字符串
*/
int epdf_textshow(int page, float x, float y, float font_size, char * language, char * text);
int epdf_textrect(int page, float left,float top,float right,float bottom,float font_size, char *language, char *text,EPDF_TextAlignment align);


/*
	设置指定页的文字的样式
*/
int epdf_textstyle(int page, EPDF_TextStyle style);
float epdf_textdesent(int page, char *language, float font_size);

/*
	设置某语言使用PDF标准字体
*/
int epdf_textfont(int pdf, char *language, EPDF_FontInternal font);


/*
	设置字体目录路径
*/
int epdf_textpath(char * dir);

/*
	设置某语言使用文件字体
*/
int epdf_texttruetype(int pdf, char *language, char * truetype);

/*
	在指定位置显示图片
	img_file: 当前支持 PNG JPG 格式图片
*/
int epdf_imageshow(int page, float x, float y, float w, float h, char * img_file);

/*
	创建注释
*/
int epdf_annot(int page, EPDF_Rect rect, char * text, int type, int opened);

/*
	创建连接
*/
int epdf_rlink(int src_page, EPDF_Rect rect, int dst_page);


/*----- 绘图相关 -----------------------*/
int epdf_moveto(int page, float x, float y);

//图形绘制
int epdf_lineto(int page, float x, float y);
int epdf_rectangle(int page, float x, float y, float w, float h);
int epdf_arc(int page, float x, float y, float ray, float ang1, float ang2);
int epdf_circle(int page, float x, float y, float ray);
int epdf_ellipse(int page, float x,float y, float xray, float yray);
//
int epdf_stroke(int page);
int epdf_fill(int page);
int epdf_fillstroke(int page);

//线宽
int epdf_setlinewidth(int page, float width);
//虚线
int epdf_setlinedash(int page, short dash[], int dash_size, int phase);


//三元色模式
int epdf_setrgbstroke(int page, float r, float g, float b);
int epdf_setrgbfill(int page, float r, float g, float b);
//印刷四色模式
int epdf_setcmykstroke(int page, float c, float m, float y, float k);
int epdf_setcmykfill(int page, float c, float m, float y, float k);
//灰度模式
int epdf_setgraystroke(int page, float gray);
int epdf_setgrayfill(int page, float gray);

//设置行距
int epdf_settextleading(int page, float value);

#ifdef __cplusplus
}
#endif

#endif

