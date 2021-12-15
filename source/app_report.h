/*******************************************************************************
* Copyright(c)2020，捷太科仪器(苏州)有限公司上海研发部
*
* All rights reserved
*
* 文件名称：  app_report.h
* 摘    要：  OTDR数据生成Html报告
*
* 当前版本：  v1.0.0
* 作    者：  
* 完成日期：  
*
* 取代版本：  
* 原 作 者：  
* 完成日期：  
*******************************************************************************/
#ifndef _APP_REPORT_H
#define _APP_REPORT_H
#include "guiwindow.h"

#define CREAT_REPORT

//html开始
#define HTML_START		\
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n\
<html>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n"

//html无边框表格标签
#define HTML_NOBORDER_TAB 	\
"<table width=\"100%%\" border=\"0\" cellpadding=\"1\" cellspacing=\"0\" bordercolor=\"#000000\">\n"

//html带边框表格标签
#define HTML_BORDER_TAB	\
"<table width=\"100%%\" border=\"1\" cellspacing=\"0\" cellpadding=\"1\" class=\"cellBorderFull\">\n"

//要用到的html其他标签定义
#define HTML_TAB_END		"</table>\n"//html表结束
#define HTML_TR   			"<tr>\n"	//html表格行开始
#define HTML_TR_END	    	"</tr>\n"	//html表格行结束
#define HTML_BORDER_TH 	  	"<th align=center width=\"%d%%\" class=\"headerBold\" height=\"15\">"
#define HTML_TH_END		 	"</th>\n"
#define HTML_BORDER_TD 	  	"<td align=center width=\"%d%%\" class=\"HaveBorderRight\" height=\"15\">"
#define HTML_NOBORDER_TD	"<td align=center width=\"%d%%\" height=\"15\">"
#define HTML_TD_END		  	"</td>\n"
#define HTML_Hn				"<h%d>%s</h%d>\n"
#define HTML_IMG 			"<img src=\"%s\"/>\n"

//生成<tr>标签起始
#define GENERATE_TR_TO_HTML(file)\
		fprintf(file, HTML_TR)
//生成</tr>标签尾
#define GENERATE_TR_END_TO_HTML(file)\
		fprintf(file, HTML_TR_END)
//生成<tab>标签 带边框
#define GENRATE_BORDER_TAB_TO_HTML(file)\
		fprintf(file, HTML_BORDER_TAB)
//生成<tab>标签 无边框
#define GENRATE_NOBORDER_TAB_TO_HTML(file)\
		fprintf(file, HTML_NOBORDER_TAB)
//生成</tab>标签 (表格结束)
#define GENRATE_TAB_END_TO_HTML(file)\
		fprintf(file, HTML_TAB_END)
//生成<img>标签 (图片)
#define GENERATE_IMG_TO_HTML(file, img_file)\
		fprintf(file, HTML_IMG, img_file)
//生成标题<h(1-6)...</h(1-6)>
#define GENERATE_TITLE_TO_HTML(file, level, title)\
		fprintf(file, HTML_Hn, level, title, level)
//生成表格表头<th>...</th>
#define GENERATE_BORDER_TH_TO_HTML(file, fmt, width, data)\
		fprintf(file, HTML_BORDER_TH fmt HTML_TH_END, width, data)
//生成带边框表格数据<td>...</td>
#define GENERATE_BORDER_TD_TO_HTML(file, fmt, width, data)\
		fprintf(file, HTML_BORDER_TD fmt HTML_TD_END, width, data)
//生成无边框表格数据<td>...</td>
#define GENERATE_NOBORDER_TD_TO_HTML(file, fmt, width, data)\
		fprintf(file, HTML_NOBORDER_TD fmt HTML_TD_END, width, data)

/*******************************************************************************
*								端面检查报告								   *
*******************************************************************************/
//html头以及css样式
#define HTML_HEAD_FIBER \
"<head>\n\
<style type=\"text/css\">\n\
	h1{font-family: Verdana, Arial, sans-serif;\n\
		font-size:18pt; font-weight:bold; text-align:left; border-bottom-color:#0066CC;\n\
		border-bottom-style:solid; border-bottom-width:2px; div.break {page-break-before:always}}\n\
	h2{font-family:Verdana, Arial, sans-serif; font-size:12pt; font-weight:bold; text-align:left;\n\
		border-bottom-width: 1px;border-bottom-style: solid;border-bottom-color: #999999;}\n\
	h4{font-family:Verdana, Arial, sans-serif; font-size: 12pt;font-weight: bold;text-align: left;}\n\
	th{font-family:Verdana, Arial, sans-serif;font-weight: normal;border-collapse:collapse;text-align: center;}\n\
	body{font-family:Verdana, Arial, sans-serif;font-size: 9pt;}\n\
	.cellBorderFull{border: 1px solid #000000;border-collapse:collapse;}\n\
	.headerBold{border:1px solid #000000; border-collapse:collapse; font-weight:bold; text-align:center; scope:col}\n\
	tr{vertical-align: center;}\n\
	.HaveBorderRight{border-right-width:1px; border-right-style:solid; border-right-color:#000000;}\n\
</style>\n\
<title>%s</title>\n\
</head>\n"

#define HTML_COLSPAN_TH     "<th align=\"center\" valign=\"center\" colspan=\"%d\" width=\"%d%%\" class=\"headerBold\">"	//表头跨列
#define HTML_ROWSPAN_TH		"<th align=\"center\" valign=\"center\" rowspan=\"%d\" width=\"%d%%\" class=\"headerBold\">"	//表头跨行
#define HTML_COLSPAN_TD     "<td align=\"center\" valign=\"center\" colspan=\"%d\" width=\"%d%%\" class=\"HaveBorderRight\">"	//表格跨列
#define HTML_ROWSPAN_TD		"<td align=\"center\" valign=\"center\" rowspan=\"%d\" width=\"%d%%\" class=\"HaveBorderRight\">"	//表格跨行
#define HTML_IMG_CENTER 	"<p style=\"text-align:%s\"><img src=\"%s\"/></p>\n"	//居中显示图片

//html 脚和结束符
#define HTML_FOOTER_FIBER \
"<p style=\"text-align:left\">\n\
%s________________________    %s %s<br /><br />\n\
<big>Copyright(c)2015-2020</big><br />\n\
</p>\n\
</body>\n\
</html>\n"
	
//生成html头
#define GENERATE_HTML_HEADER_FIBER_TO_HTML(file, title)\
			fprintf(file, HTML_START HTML_HEAD_FIBER, title)
//生成html脚
#define GENERATE_HTML_FOOTER_FIBER_TO_HTML(file, name, date, date1)\
				fprintf(file, HTML_FOOTER_FIBER, name, date, date1)

//生成<img>标签 (显示图片 可选择对齐方式-靠左 lift，靠右 right，居中 center)
#define GENERATE_IMG_ALIGN_TO_HTML(file, align, img_file)\
		fprintf(file, HTML_IMG_CENTER, align, img_file)

		
//生成表格表头<th>...</th> 跨多行
#define GENERATE_BORDER_ROWS_TH_TO_HTML(file, fmt, rows, width, data)\
		fprintf(file, HTML_ROWSPAN_TH fmt HTML_TH_END, rows, width, data)
//生成表格表头<th>...</th> 跨多列
#define GENERATE_BORDER_COLS_TH_TO_HTML(file, fmt, cols, width, data)\
		fprintf(file, HTML_COLSPAN_TH fmt HTML_TH_END, cols, width, data)
//生成带边框表格数据<td>...</td> 跨多行
#define GENERATE_BORDER_ROWS_TD_TO_HTML(file, fmt, rows, width, data)\
			fprintf(file, HTML_ROWSPAN_TD fmt HTML_TD_END, rows, width, data)
//生成带边框表格数据<td>...</td> 跨多列
#define GENERATE_BORDER_COLS_TD_TO_HTML(file, fmt, cols, width, data)\
			fprintf(file, HTML_COLSPAN_TD fmt HTML_TD_END, cols, width, data)

//生成无边框表格显示图片<td>...</td>
#define GENERATE_NOBORDER_IMAGE_TD_TO_HTML(file, width, img_file)\
			fprintf(file, HTML_NOBORDER_TD HTML_IMG HTML_TD_END, width, img_file)


//获得网页多国语言字符串
#define GetHtmlLabelText(iIndex, language) GetCurrLanguageUtf8Text(iIndex, language)


#endif //_APP_REPORT_H
