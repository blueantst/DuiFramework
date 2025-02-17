#pragma once
#include "style/style_declaration.h"
#include "style/style_selector.h"

namespace ui
{
	class StyleSheet;
	class StyleRule : public RefCounted<StyleRule>
	{
	public:
		StyleRule();
		StyleRule(StyleSelectorList* l, StyleDeclarationList* d);

		void AddSelector(StyleSelector* selector);

		void AddDeclaction(StyleDeclaration* declaration);

		void AddDeclaction(StyleProperty p, StyleValue* v);

		void MatchRules(View* v, StyleDeclarationList& l);

		void SetLineNumber(uint32 num);
		void SetStyleSheet(StyleSheet* sheet);
	private:
		friend class RefCounted < StyleRule > ;
		~StyleRule();
		scoped_ptr<StyleSelectorList> selectors_;
		scoped_ptr<StyleDeclarationList> declarations_;
		uint32 line_;
		scoped_refptr<StyleSheet> sheet_;
		DISALLOW_COPY_AND_ASSIGN(StyleRule);
	};
}