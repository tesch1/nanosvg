// -*-  Mode: C++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: t -*-
/*
 * Adaptor for using tinyxml2 to parse xml for nanosvg.
 *
 * Adaped from https://github.com/poke1024/tove2d
 *
 * Copyright (c) 2018, Bernhard Liebl
 * Copyright (c) 2019 Michael Tesch tesch1@gmail.com
 *
 * Distributed under the MIT license. See LICENSE file for details.
 *
 * All rights reserved.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <iterator>
#include <cmath>

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "tinyxml2.h"
#define NANOSVG_IMPLEMENTATION
#define NANOSVG_NO_PARSE_FROM_FILE
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "nanosvg.h"

namespace bridge {

using std::pow;
using std::sqrt;

typedef void (*StartElementCallback)(void* ud, const char* el, const char** attr);
typedef void (*EndElementCallback)(void* ud, const char* el);
typedef void (*ContentCallback)(void* ud, const char* el);

struct equal_cstr {
	inline bool operator()(const char *a, const char *b) const {
		return strcmp(a, b) == 0;
	}
};

class NanoSVGVisitor : public tinyxml2::XMLVisitor
{
	using XMLDocument = tinyxml2::XMLDocument;
	using XMLElement = tinyxml2::XMLElement;
	using XMLAttribute = tinyxml2::XMLAttribute;
	using XMLText = tinyxml2::XMLText;

	StartElementCallback mStartElement;
	EndElementCallback mEndElement;
	ContentCallback mContent;
	int mDepth = 0;

	void *mUserData;
	const XMLDocument *mCurrentDocument;
	bool mSkipDefs;

	typedef std::map<std::string, const XMLElement *> ElementsMap;
	std::unique_ptr<ElementsMap> mElementsById;

	void gatherIds(const XMLElement *parent) {
		const XMLElement *child = parent->FirstChildElement();
		while (child) {
			gatherIds(child);

			const char *id = child->Attribute("id");
			if (id) {
				mElementsById->insert(ElementsMap::value_type(id, child));
			}
			child = child->NextSiblingElement();
		}
	}

	const XMLElement *lookupById(const char *id) {
		if (!mElementsById.get()) {
			assert(mCurrentDocument);
			mElementsById.reset(new ElementsMap);
			gatherIds(mCurrentDocument->RootElement());
		}

		const auto it = mElementsById->find(id);
		if (it != mElementsById->end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

public:
	NanoSVGVisitor(StartElementCallback startElement,
				   EndElementCallback endElement,
				   ContentCallback content,
				   void *userdata)
		: mStartElement(startElement),
		  mEndElement(endElement),
		  mContent(content),
		  mUserData(userdata),
		  mCurrentDocument(nullptr),
		  mSkipDefs(false)
	{
	}

	virtual bool Visit(const XMLText &text) {
		std::cout << "'" << text.Value() << "'\n";
		return true;
	}

	virtual bool VisitEnter(const XMLDocument &doc) {
		mCurrentDocument = doc.ToDocument();
		std::cerr << std::string(mDepth++, ' ')
				  << "DocEnter doc'"
				  << mCurrentDocument->RootElement()->Name()
				  << /*<< doc.Value() << */"'\n";

		// always handle <defs> tags first
		mSkipDefs = false;
#if 1
		const XMLElement *defs = doc.RootElement()->FirstChildElement("defs");
		while (defs) {
			defs->Accept(this);
			defs = defs->NextSiblingElement("defs");
		}
#endif
		mSkipDefs = true;

		return true;
	}

	virtual bool VisitExit(const XMLDocument &doc) {
		std::cerr << std::string(mDepth--, ' ') << "DocExit'" << doc.Value() << "'\n";
		mCurrentDocument = nullptr;
		return true;
	}

	virtual bool VisitEnter(const XMLElement &element, const XMLAttribute *firstAttribute) {
		std::cerr << std::string(mDepth++, ' ') << "VisitEnter2'" << element.Value() << "'\n";
		if (mSkipDefs
			&& strcmp(element.Name(), "defs") == 0
			&& element.Parent() == mCurrentDocument->RootElement()) {
			// already handled in l VisitEnter(const XMLDocument &doc)
			std::cerr << "skipping\n";
			return false;
		}
		const char *attr[NSVG_XML_MAX_ATTRIBS];
		int numAttr = 0;

		const XMLAttribute *attribute = firstAttribute;
		while (attribute && numAttr < NSVG_XML_MAX_ATTRIBS - 3) {
			attr[numAttr++] = attribute->Name();
			attr[numAttr++] = attribute->Value();
			attribute = attribute->Next();
		}

		attr[numAttr++] = 0;
		attr[numAttr++] = 0;	

		mStartElement(mUserData, element.Name(), attr);

		if (strcmp(element.Name(), "use") == 0) {
			const char *href = element.Attribute("href");
			if (!href)
				href = element.Attribute("xlink:href");

			if (href) {
				const char *s = href;
				while (isspace(*s)) {
					s++;
				}
				if (*s == '#') {
					const XMLElement *referee = lookupById(s + 1);
					if (referee) {
						std::cerr << "Using '" << (s+1) << "'\n";
						referee->Accept(this);
					}
					else
						std::cerr << "unable to find reference '" << (s+1) << "'\n";
				}
			}
			else
				std::cerr << "use missing href '" << element.Value() << "'\n";
		}

		return true;
	}

	virtual bool VisitExit(const XMLElement& element) {
		std::cerr << std::string(mDepth--, ' ') << "VisitExit2'" << element.Value() << "'\n";
		mEndElement(mUserData, element.Name());
		return true;
	}
};

int parseSVG(char* input,
             void (*startelCb)(void* ud, const char* el, const char** attr),
             void (*endelCb)(void* ud, const char* el),
             void (*contentCb)(void* ud, const char* s),
             void* ud)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(input);
	NanoSVGVisitor visitor(startelCb, endelCb, contentCb, ud);
	return doc.Accept(&visitor) ? 1 : 0;
}

} // bridge

// Parses SVG file from a file, returns SVG image as paths.
NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi)
{
	FILE* fp = NULL;
	size_t size;
	char* data = NULL;
	NSVGimage* image = NULL;

	fp = fopen(filename, "rb");
	if (!fp) goto error;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	data = (char*)malloc(size+1);
	if (data == NULL) goto error;
	if (fread(data, 1, size, fp) != size) goto error;
	data[size] = '\0';	// Must be null terminated.
	fclose(fp);
	image = nsvgParseEx(data, units, dpi, bridge::parseSVG);
	free(data);

	return image;

 error:
	if (fp) fclose(fp);
	if (data) free(data);
	if (image) nsvgDelete(image);
	return NULL;

	return image;
}
