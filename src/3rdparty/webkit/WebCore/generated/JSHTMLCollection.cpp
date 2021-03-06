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

#include "JSHTMLCollection.h"

#include <wtf/GetPtr.h>

#include <runtime/PropertyNameArray.h>
#include "AtomicString.h"
#include "HTMLCollection.h"
#include "JSNode.h"
#include "JSNodeList.h"
#include "NameNodeList.h"
#include "Node.h"
#include "NodeList.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSHTMLCollection)

/* Hash table */

static const HashTableValue JSHTMLCollectionTableValues[3] =
{
    { "length", DontDelete|ReadOnly, (intptr_t)jsHTMLCollectionLength, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsHTMLCollectionConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLCollectionTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 15, JSHTMLCollectionTableValues, 0 };
#else
    { 5, 3, JSHTMLCollectionTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSHTMLCollectionConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLCollectionConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLCollectionConstructorTableValues, 0 };
#else
    { 1, 0, JSHTMLCollectionConstructorTableValues, 0 };
#endif

class JSHTMLCollectionConstructor : public DOMObject {
public:
    JSHTMLCollectionConstructor(ExecState* exec)
        : DOMObject(JSHTMLCollectionConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSHTMLCollectionPrototype::self(exec, exec->lexicalGlobalObject()), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSHTMLCollectionConstructor::s_info = { "HTMLCollectionConstructor", 0, &JSHTMLCollectionConstructorTable, 0 };

bool JSHTMLCollectionConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLCollectionConstructor, DOMObject>(exec, &JSHTMLCollectionConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLCollectionPrototypeTableValues[4] =
{
    { "item", DontDelete|Function, (intptr_t)jsHTMLCollectionPrototypeFunctionItem, (intptr_t)1 },
    { "namedItem", DontDelete|Function, (intptr_t)jsHTMLCollectionPrototypeFunctionNamedItem, (intptr_t)1 },
    { "tags", DontDelete|Function, (intptr_t)jsHTMLCollectionPrototypeFunctionTags, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLCollectionPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSHTMLCollectionPrototypeTableValues, 0 };
#else
    { 8, 7, JSHTMLCollectionPrototypeTableValues, 0 };
#endif

const ClassInfo JSHTMLCollectionPrototype::s_info = { "HTMLCollectionPrototype", 0, &JSHTMLCollectionPrototypeTable, 0 };

JSObject* JSHTMLCollectionPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLCollection>(exec, globalObject);
}

bool JSHTMLCollectionPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSHTMLCollectionPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSHTMLCollection::s_info = { "HTMLCollection", 0, &JSHTMLCollectionTable, 0 };

JSHTMLCollection::JSHTMLCollection(PassRefPtr<Structure> structure, PassRefPtr<HTMLCollection> impl)
    : DOMObject(structure)
    , m_impl(impl)
{
}

JSHTMLCollection::~JSHTMLCollection()
{
    forgetDOMObject(*Heap::heap(this)->globalData(), m_impl.get());

}

JSObject* JSHTMLCollection::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSHTMLCollectionPrototype(JSHTMLCollectionPrototype::createStructure(globalObject->objectPrototype()));
}

bool JSHTMLCollection::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSValuePtr proto = prototype();
    if (proto->isObject() && static_cast<JSObject*>(asObject(proto))->hasProperty(exec, propertyName))
        return false;

    const HashEntry* entry = JSHTMLCollectionTable.entry(exec, propertyName);
    if (entry) {
        slot.setCustom(this, entry->propertyGetter());
        return true;
    }
    bool ok;
    unsigned index = propertyName.toUInt32(&ok, false);
    if (ok && index < static_cast<HTMLCollection*>(impl())->length()) {
        slot.setCustomIndex(this, index, indexGetter);
        return true;
    }
    if (canGetItemsForName(exec, static_cast<HTMLCollection*>(impl()), propertyName)) {
        slot.setCustom(this, nameGetter);
        return true;
    }
    return getStaticValueSlot<JSHTMLCollection, Base>(exec, &JSHTMLCollectionTable, this, propertyName, slot);
}

bool JSHTMLCollection::getOwnPropertySlot(ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    if (propertyName < static_cast<HTMLCollection*>(impl())->length()) {
        slot.setCustomIndex(this, propertyName, indexGetter);
        return true;
    }
    return getOwnPropertySlot(exec, Identifier::from(exec, propertyName), slot);
}

JSValuePtr jsHTMLCollectionLength(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLCollection* imp = static_cast<HTMLCollection*>(static_cast<JSHTMLCollection*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->length());
}

JSValuePtr jsHTMLCollectionConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSHTMLCollection*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSHTMLCollection::getPropertyNames(ExecState* exec, PropertyNameArray& propertyNames)
{
    for (unsigned i = 0; i < static_cast<HTMLCollection*>(impl())->length(); ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getPropertyNames(exec, propertyNames);
}

JSValuePtr JSHTMLCollection::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSHTMLCollectionConstructor>(exec);
}

JSValuePtr jsHTMLCollectionPrototypeFunctionItem(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSHTMLCollection::s_info))
        return throwError(exec, TypeError);
    JSHTMLCollection* castedThisObj = static_cast<JSHTMLCollection*>(asObject(thisValue));
    return castedThisObj->item(exec, args);
}

JSValuePtr jsHTMLCollectionPrototypeFunctionNamedItem(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSHTMLCollection::s_info))
        return throwError(exec, TypeError);
    JSHTMLCollection* castedThisObj = static_cast<JSHTMLCollection*>(asObject(thisValue));
    return castedThisObj->namedItem(exec, args);
}

JSValuePtr jsHTMLCollectionPrototypeFunctionTags(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSHTMLCollection::s_info))
        return throwError(exec, TypeError);
    JSHTMLCollection* castedThisObj = static_cast<JSHTMLCollection*>(asObject(thisValue));
    HTMLCollection* imp = static_cast<HTMLCollection*>(castedThisObj->impl());
    const UString& name = args.at(exec, 0)->toString(exec);


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->tags(name)));
    return result;
}


JSValuePtr JSHTMLCollection::indexGetter(ExecState* exec, const Identifier& propertyName, const PropertySlot& slot)
{
    JSHTMLCollection* thisObj = static_cast<JSHTMLCollection*>(asObject(slot.slotBase()));
    return toJS(exec, static_cast<HTMLCollection*>(thisObj->impl())->item(slot.index()));
}
HTMLCollection* toHTMLCollection(JSC::JSValuePtr value)
{
    return value->isObject(&JSHTMLCollection::s_info) ? static_cast<JSHTMLCollection*>(asObject(value))->impl() : 0;
}

}
