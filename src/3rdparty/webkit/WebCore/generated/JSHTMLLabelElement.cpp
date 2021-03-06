/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#include "JSHTMLLabelElement.h"

#include <wtf/GetPtr.h>

#include "HTMLFormElement.h"
#include "HTMLLabelElement.h"
#include "JSHTMLFormElement.h"
#include "KURL.h"

#include <runtime/JSNumberCell.h>
#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSHTMLLabelElement)

/* Hash table */

static const HashTableValue JSHTMLLabelElementTableValues[5] =
{
    { "form", DontDelete|ReadOnly, (intptr_t)jsHTMLLabelElementForm, (intptr_t)0 },
    { "accessKey", DontDelete, (intptr_t)jsHTMLLabelElementAccessKey, (intptr_t)setJSHTMLLabelElementAccessKey },
    { "htmlFor", DontDelete, (intptr_t)jsHTMLLabelElementHtmlFor, (intptr_t)setJSHTMLLabelElementHtmlFor },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsHTMLLabelElementConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLLabelElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 31, JSHTMLLabelElementTableValues, 0 };
#else
    { 9, 7, JSHTMLLabelElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSHTMLLabelElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLLabelElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLLabelElementConstructorTableValues, 0 };
#else
    { 1, 0, JSHTMLLabelElementConstructorTableValues, 0 };
#endif

class JSHTMLLabelElementConstructor : public DOMObject {
public:
    JSHTMLLabelElementConstructor(ExecState* exec)
        : DOMObject(JSHTMLLabelElementConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSHTMLLabelElementPrototype::self(exec, exec->lexicalGlobalObject()), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSHTMLLabelElementConstructor::s_info = { "HTMLLabelElementConstructor", 0, &JSHTMLLabelElementConstructorTable, 0 };

bool JSHTMLLabelElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLLabelElementConstructor, DOMObject>(exec, &JSHTMLLabelElementConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLLabelElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLLabelElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLLabelElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSHTMLLabelElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSHTMLLabelElementPrototype::s_info = { "HTMLLabelElementPrototype", 0, &JSHTMLLabelElementPrototypeTable, 0 };

JSObject* JSHTMLLabelElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLLabelElement>(exec, globalObject);
}

const ClassInfo JSHTMLLabelElement::s_info = { "HTMLLabelElement", &JSHTMLElement::s_info, &JSHTMLLabelElementTable, 0 };

JSHTMLLabelElement::JSHTMLLabelElement(PassRefPtr<Structure> structure, PassRefPtr<HTMLLabelElement> impl)
    : JSHTMLElement(structure, impl)
{
}

JSObject* JSHTMLLabelElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSHTMLLabelElementPrototype(JSHTMLLabelElementPrototype::createStructure(JSHTMLElementPrototype::self(exec, globalObject)));
}

bool JSHTMLLabelElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLLabelElement, Base>(exec, &JSHTMLLabelElementTable, this, propertyName, slot);
}

JSValuePtr jsHTMLLabelElementForm(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLLabelElement* imp = static_cast<HTMLLabelElement*>(static_cast<JSHTMLLabelElement*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->form()));
}

JSValuePtr jsHTMLLabelElementAccessKey(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLLabelElement* imp = static_cast<HTMLLabelElement*>(static_cast<JSHTMLLabelElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->accessKey());
}

JSValuePtr jsHTMLLabelElementHtmlFor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLLabelElement* imp = static_cast<HTMLLabelElement*>(static_cast<JSHTMLLabelElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->htmlFor());
}

JSValuePtr jsHTMLLabelElementConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSHTMLLabelElement*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSHTMLLabelElement::put(ExecState* exec, const Identifier& propertyName, JSValuePtr value, PutPropertySlot& slot)
{
    lookupPut<JSHTMLLabelElement, Base>(exec, propertyName, value, &JSHTMLLabelElementTable, this, slot);
}

void setJSHTMLLabelElementAccessKey(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLLabelElement* imp = static_cast<HTMLLabelElement*>(static_cast<JSHTMLLabelElement*>(thisObject)->impl());
    imp->setAccessKey(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLLabelElementHtmlFor(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLLabelElement* imp = static_cast<HTMLLabelElement*>(static_cast<JSHTMLLabelElement*>(thisObject)->impl());
    imp->setHtmlFor(valueToStringWithNullCheck(exec, value));
}

JSValuePtr JSHTMLLabelElement::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSHTMLLabelElementConstructor>(exec);
}


}
