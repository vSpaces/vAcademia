#pragma once

namespace rmml {

/**
 * Класс стилей текстового элемента RMML
 */

class mlRMMLStyles :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable
{
//	bool mbCreateMediaCalled;
	JSString* htmlTag;
	class stlStyles{
		friend class mlRMMLStyles;
	public:
		class stlStyle: public mlStyle{
			friend class mlRMMLStyles;
			wchar_t* pwcTag;
		public:
			stlStyle(){ pwcTag=NULL; }
			~stlStyle(){
				if(pwcTag!=NULL) MP_DELETE_ARR( pwcTag);
			}
			stlStyle& operator=(const stlStyle& aStyle);
		};
		// вектор классов, упорядоченный по возрастанию названий тэгов
		// Первый - по умолчанию (pwcTag==NULL)
		MP_VECTOR<stlStyle*> mvStyles; 
		//
		stlStyles():
			MP_VECTOR_INIT(mvStyles)
		{ 
			stlStyle* pStyle = MP_NEW(stlStyle);
			mvStyles.push_back(pStyle); 
		}
		~stlStyles();
		stlStyle* GetStyle(const wchar_t* apwcTag=NULL);
		stlStyle* CreateStyle(JSString* ajssTag=NULL);
		int GetCount(){ return mvStyles.size(); }
		stlStyles& operator=(const stlStyles& aStyles);
	} mStyles;
	// ссылка на стиль из вектора mvStyles, 
	// соответствующая htmlTag
	stlStyles::stlStyle* mpStyle; 

	__forceinline bool ParseFontStyleValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle);
	__forceinline bool ParseFontWeightValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle);
	__forceinline bool ParseTextAlignValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle);
	__forceinline bool ParseTextDecorationValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle);
	__forceinline bool ParseAntialiasingValue(const wchar_t* apwcValue, int aiLen, stlStyles::stlStyle &aStyle);

public:
	mlRMMLStyles(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLStyles(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_value, // all styles, serialized to text value
		JSPROP_htmlTag, // current HTML tag name (following properties for)
		JSPROP_font, // example: 'italic small-caps bold 12pt serif'
		JSPROP_fontStyle, // 'normal' 'italic'
		JSPROP_fontWeight, // 'normal' 'bold'
		JSPROP_fontSize, // example: '14pt'
		JSPROP_lineHeight, // 'normal' '5pt' '10%'
		JSPROP_fontFamily, // example: 'Times New Roman'
		JSPROP_textAlign, // 'left' 'right' 'center' 'justify'
		JSPROP_textIndent, // example: '10px'
		JSPROP_textDecoration, // 'none' 'underline'
		JSPROP_color, // 'red' '#F00' '#FF0000' 'rgb 1.0 0.0 0.0'
		JSPROP_antialiasing // 'none' 'smooth' (по умолчанию - как установлено движком - smooth)
		
	};

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult SetValue(wchar_t* apwcValue);

// реализация  mlILoadable
MLILOADABLE_IMPL

public:
	mlStyle* GetStyle(const char* apszTag=NULL);

};

/*
	<xs:attributeGroup name="rmmlStyle">
		<xs:attributeGroup ref="styleFont"/>
		<xs:attribute name="textAlign" type="xs:string"/>
		<xs:attribute name="textIndent" type="xs:string"/>
		<xs:attribute name="textDecoration" type="xs:string"/>
		<xs:attribute name="color" type="xs:string"/>
	</xs:attributeGroup>
	<xs:attributeGroup name="styleFont">
		<xs:attribute name="font" type="xs:string"/>
		<xs:attribute name="fontStyle" type="xs:string"/>
		<xs:attribute name="fontWeight" type="xs:string"/>
		<xs:attribute name="fontSize" type="xs:string"/>
		<xs:attribute name="lineHeight" type="xs:string"/>
		<xs:attribute name="fontFamily" type="xs:string"/>
	</xs:attributeGroup>
*/

}
