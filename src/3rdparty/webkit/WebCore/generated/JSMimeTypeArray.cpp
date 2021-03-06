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

#include "JSMimeTypeArray.h"

#include <wtf/GetPtr.h>

#include <runtime/PropertyNameArray.h>
#include "AtomicString.h"
#include "JSMimeType.h"
#include "MimeType.h"
#include "MimeTypeArray.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSMimeTypeArray)

/* Hash table */

static const HashTableValue JSMimeTypeArrayTableValues[3] =
{
    { "length", DontDelete|ReadOnly, (intptr_t)jsMimeTypeArrayLength, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsMimeTypeArrayConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSMimeTypeArrayTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 15, JSMimeTypeArrayTableValues, 0 };
#else
    { 5, 3, JSMimeTypeArrayTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSMimeTypeArrayConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSMimeTypeArrayConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSMimeTypeArrayConstructorTableValues, 0 };
#else
    { 1, 0, JSMimeTypeArrayConstructorTableValues, 0 };
#endif

class JSMimeTypeArrayConstructor : public DOMObject {
public:
    JSMimeTypeArrayConstructor(ExecState* exec)
        : DOMObject(JSMimeTypeArrayConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSMimeTypeArrayPrototype::self(exec, exec->lexicalGlobalObject()), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSMimeTypeArrayConstructor::s_info = { "MimeTypeArrayConstructor", 0, &JSMimeTypeArrayConstructorTable, 0 };

bool JSMimeTypeArrayConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSMimeTypeArrayConstructor, DOMObject>(exec, &JSMimeTypeArrayConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSMimeTypeArrayPrototypeTableValues[3] =
{
    { "item", DontDelete|Function, (intptr_t)jsMimeTypeArrayPrototypeFunctionItem, (intptr_t)1 },
    { "namedItem", DontDelete|Function, (intptr_t)jsMimeTypeArrayPrototypeFunctionNamedItem, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static const HashTable JSMimeTypeArrayPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSMimeTypeArrayPrototypeTableValues, 0 };
#else
    { 5, 3, JSMimeTypeArrayPrototypeTableValues, 0 };
#endif

const ClassInfo JSMimeTypeArrayPrototype::s_info = { "MimeTypeArrayPrototype", 0, &JSMimeTypeArrayPrototypeTable, 0 };

JSObject* JSMimeTypeArrayPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSMimeTypeArray>(exec, globalObject);
}

bool JSMimeTypeArrayPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSMimeTypeArrayPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSMimeTypeArray::s_info = { "MimeTypeArray", 0, &JSMimeTypeArrayTable, 0 };

JSMimeTypeArray::JSMimeTypeArray(PassRefPtr<Structure> structure, PassRefPtr<MimeTypeArray> impl)
    : DOMObject(structure)
    , m_impl(impl)
{
}

JSMimeTypeArray::~JSMimeTypeArray()
{
    forgetDOMObject(*Heap::heap(this)->globalData(), m_impl.get());

}

JSObject* JSMimeTypeArray::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSMimeTypeArrayPrototype(JSMimeTypeArrayPrototype::createStructure(globalObject->objectPrototype()));
}

bool JSMimeTypeArray::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = JSMimeTypeArrayTable.entry(exec, propertyName);
    if (entry) {
        slot.setCustom(this, entry->propertyGetter());
        return true;
    }
    bool ok;
    unsigned index = propertyName.toUInt32(&ok, false);
    if (ok && index < static_cast<MimeTypeArray*>(impl())->length()) {
        slot.setCustomIndex(this, index, indexGetter);
        return true;
    }
    if (canGetItemsForName(exec, static_cast<MimeTypeArray*>(impl()), propertyName)) {
        slot.setCustom(this, nameGetter);
        return true;
    }
    return getStaticValueSlot<JSMimeTypeArray, Base>(exec, &JSMimeTypeArrayTable, this, propertyName, slot);
}

bool JSMimeTypeArray::getOwnPropertySlot(ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    if (propertyName < static_cast<MimeTypeArray*>(impl())->length()) {
        slot.setCustomIndex(this, propertyName, indexGetter);
        return true;
    }
    return getOwnPropertySlot(exec, Identifier::from(exec, propertyName), slot);
}

JSValuePtr jsMimeTypeArrayLength(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    MimeTypeArray* imp = static_cast<MimeTypeArray*>(static_cast<JSMimeTypeArray*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->length());
}

JSValuePtr jsMimeTypeArrayConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSMimeTypeArray*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSMimeTypeArray::getPropertyNames(ExecState* exec, PropertyNameArray& propertyNames)
{
    for (unsigned i = 0; i < static_cast<MimeTypeArray*>(impl())->length(); ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getPropertyNames(exec, propertyNames);
}

JSValuePtr JSMimeTypeArray::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSMimeTypeArrayConstructor>(exec);
}

JSValuePtr jsMimeTypeArrayPrototypeFunctionItem(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSMimeTypeArray::s_info))
        return throwError(exec, TypeError);
    JSMimeTypeArray* castedThisObj = static_cast<JSMimeTypeArray*>(asObject(thisValue));
    MimeTypeArray* imp = static_cast<MimeTypeArray*>(castedThisObj->impl());
    unsigned index = args.at(exec, 0)->toInt32(exec);


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->item(index)));
    return result;
}

JSValuePtr jsMimeTypeArrayPrototypeFunctionNamedItem(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSMimeTypeArray::s_info))
        return throwError(exec, TypeError);
    JSMimeTypeArray* castedThisObj = static_cast<JSMimeTypeArray*>(asObject(thisValue));
    MimeTypeArray* imp = static_cast<MimeTypeArray*>(castedThisObj->impl());
    const UString& name = args.at(exec, 0)->toString(exec);


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->namedItem(name)));
    return result;
}


JSValuePtr JSMimeTypeArray::indexGetter(ExecState* exec, const Identifier& propertyName, const PropertySlot& slot)
{
    JSMimeTypeArray* thisObj = static_cast<JSMimeTypeArray*>(asObject(slot.slotBase()));
    return toJS(exec, static_cast<MimeTypeArray*>(thisObj->impl())->item(slot.index()));
}
JSC::JSValuePtr toJS(JSC::ExecState* exec, MimeTypeArray* object)
{
    return getDOMObjectWrapper<JSMimeTypeArray>(exec, object);
}
MimeTypeArray* toMimeTypeArray(JSC::JSValuePtr value)
{
    return value->isObject(&JSMimeTypeArray::s_info) ? static_cast<JSMimeTypeArray*>(asObject(value))->impl() : 0;
}

}
