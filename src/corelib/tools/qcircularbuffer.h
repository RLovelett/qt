/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCIRCULARBUFFER_H
#define QCIRCULARBUFFER_H

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

template <typename T>
class CircularBufferData : public QSharedData
{
public:
    CircularBufferData()
        : m_data(0),
          m_capacity(0),
          m_size(0),
          m_first(-1),
          m_last(-1)
    {}

    ~CircularBufferData()
    {
        // Cleanup the data first
        if (QTypeInfo<T>::isComplex)
        {
            // The type is complex so we manually call the destructor for each item
            // since we used the placement new operator to instantiate them
            T* b = m_data;
            T* i = b + m_capacity;
            while (i-- != b)
                 i->~T();
        }

        // Release the raw memory
        deallocate(m_data);
    }

    T* allocate(int capacity);
    void deallocate(T* data);

    T* m_data;      /**< Array of the actual data */
    int m_capacity; /**< Size of the m_data array */
    int m_size;     /**< Number of elements of m_data actually used */
    int m_first;    /**< Index in m_data of the first element of the circular buffer */
    int m_last;     /**< Index in m_data of the last element of the circular buffer */
};

template <typename T>
T* CircularBufferData<T>::allocate(int capacity)
{
    void* rawMemory = operator new[](capacity * sizeof(T));
    T* data = static_cast<T*>(rawMemory);
    return data;
}

template <typename T>
void CircularBufferData<T>::deallocate(T* data)
{
    operator delete[](data);
}


template <typename T>
class QCircularBuffer
{
public:
    typedef QPair<T*,int> array_range;
    typedef QPair<const T*,int> const_array_range;
    typedef array_range ArrayRange;
    typedef const_array_range ConstArrayRange;

    QCircularBuffer()
        : d(new CircularBufferData<T>())
    {}

    explicit QCircularBuffer(int capacity);
    explicit QCircularBuffer(int capacity, const T &value);
    explicit QCircularBuffer(int capacity, int size, const T &value);

    QCircularBuffer(const QCircularBuffer<T>& other)
        : d(other.d)
    {}

    QCircularBuffer<T>& operator = (const QCircularBuffer<T>& other)
    {
        d = other.d;
        return *this;
    }

    ~QCircularBuffer() {}

    class iterator
    {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        iterator() : m_buffer(0), m_index(-1) {}
        iterator(QCircularBuffer<T>* buffer, int index)
            : m_buffer(buffer), m_index(index)
        {}
        iterator(const iterator& other)
            : m_buffer(other.m_buffer), m_index(other.m_index)
        {}

        T &operator * () const { return (*m_buffer)[ m_index ]; }
        T* operator -> () const
        {
            return m_buffer->d->m_data + ((m_buffer->d->m_first + m_index) % m_buffer->d->m_capacity);
        }
        T &operator [] (int j) const { return (*m_buffer)[ m_index + j ]; }

        bool operator == (const iterator& other) const
        {
            return (m_buffer == other.m_buffer && m_index == other.m_index);
        }

        bool operator != (const iterator& other) const
        {
            return (m_buffer != other.m_buffer || m_index != other.m_index);
        }

        bool operator < (const iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::iterator::operator<", "iterators use different buffers");
            return m_index < other.m_index;
        }

        bool operator <= (const iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::iterator::operator<=", "iterators use different buffers");
            return m_index <= other.m_index;
        }

        bool operator > (const iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::iterator::operator>", "iterators use different buffers");
            return m_index > other.m_index;
        }

        bool operator>=(const iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::iterator::operator>=", "iterators use different buffers");
            return m_index >= other.m_index;
        }

        iterator& operator ++ () { ++m_index; return *this; }
        iterator operator ++ (int)
        {
            iterator ans = *this;
            ++m_index;
            return ans;
        }

        iterator& operator -- () { --m_index; return *this; }
        iterator operator -- (int)
        {
            iterator ans = *this;
            --m_index;
            return ans;
        }

        iterator& operator += (int j) { m_index += j; return *this; }
        iterator& operator -= (int j) { m_index -= j; return *this; }
        iterator operator + (int j) const { return iterator(m_buffer, m_index + j); }
        iterator operator - (int j) const { return iterator(m_buffer, m_index - j); }
        int operator - (iterator other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::iterator::operator-", "iterators use different buffers");
            return m_index - other.m_index;
        }

    private:
        QCircularBuffer<T>* m_buffer;
        int m_index;
        friend class QCircularBuffer;
    };
    friend class iterator;

    class const_iterator
    {
    public:
        typedef std::random_access_iterator_tag  iterator_category;
        typedef ptrdiff_t difference_type;
        typedef T value_type;
        typedef const T* pointer;
        typedef const T &reference;

        const_iterator() : m_buffer(0), m_index(-1) {}
        const_iterator(const QCircularBuffer<T>* buffer, int index)
            : m_buffer(buffer), m_index(index)
        {}
        const_iterator(const const_iterator& other)
            : m_buffer(other.m_buffer), m_index(other.m_index)
        {}
        explicit const_iterator(const iterator& other)
            : m_buffer(other.m_buffer), m_index(other.m_index)
        {}

        const T &operator * () const { return m_buffer->at(m_index); }
        const T* operator -> () const
        {
            return m_buffer->d->m_data + ((m_buffer->d->m_first + m_index) % m_buffer->d->m_capacity);
        }
        const T &operator [] (int j) const { return m_buffer->at(m_index + j); }

        bool operator == (const const_iterator& other) const
        {
            return (m_buffer == other.m_buffer && m_index == other.m_index);
        }

        bool operator != (const const_iterator& other) const
        {
            return (m_buffer != other.m_buffer || m_index != other.m_index);
        }

        bool operator < (const const_iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::const_iterator::operator<", "iterators use different buffers");
            return m_index < other.m_index;
        }

        bool operator <= (const const_iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::const_iterator::operator<=", "iterators use different buffers");
            return m_index <= other.m_index;
        }

        bool operator > (const const_iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::const_iterator::operator>", "iterators use different buffers");
            return m_index > other.m_index;
        }

        bool operator >= (const const_iterator& other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::const_iterator::operator>=", "iterators use different buffers");
            return m_index >= other.m_index;
        }

        const_iterator& operator ++ () { ++m_index; return *this; }
        const_iterator operator ++ (int)
        {
            const_iterator ans = *this;
            ++m_index;
            return ans;
        }

        const_iterator& operator -- () { --m_index; return *this; }
        const_iterator operator -- (int)
        {
            const_iterator ans = *this;
            --m_index;
            return ans;
        }

        const_iterator& operator += (int j) { m_index += j; return *this; }
        const_iterator& operator -= (int j) { m_index -= j; return *this; }
        const_iterator operator + (int j) const { return const_iterator(m_buffer, m_index + j); }
        const_iterator operator - (int j) const { return const_iterator(m_buffer, m_index - j); }
        int operator - (const_iterator other) const
        {
            Q_ASSERT_X(m_buffer == other.m_buffer, "QCircularBuffer<T>::const_iterator::operator-", "iterators use different buffers");
            return m_index - other.m_index;
        }

    private:
        const QCircularBuffer<T>* m_buffer;
        int m_index;
        friend class QCircularBuffer;
    };
    friend class const_iterator;

    iterator begin() { return iterator(this, 0); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator constBegin() const { return const_iterator(this, 0); }
    iterator end() { return iterator(this, d->m_size); }
    const_iterator end() const { return const_iterator(this, d->m_size); }
    const_iterator constEnd() const { return const_iterator(this, d->m_size); }
    iterator insert(iterator before, int count, const T &value)
    {
        insert(before.m_index, count, value);
        return iterator(this, before.m_index);
    }
    iterator insert(iterator before, const T &value) { return insert(before, 1, value); }
    iterator erase(iterator begin, iterator end)
    {
        int count = end - begin;
        remove(begin.m_index, count);
        return iterator(this, end.m_index - count);
    }
    iterator erase(iterator pos) { return erase(pos, pos + 1); }

    // STL compatibility
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef ptrdiff_t difference_type;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef int size_type;

    void push_back(const T &t) { append(t); }
    void push_front(const T &t) { prepend(t); }
    void pop_back() { Q_ASSERT(!isEmpty()); erase(end() - 1); }
    void pop_front() { Q_ASSERT(!isEmpty()); erase(begin()); }
    bool empty() const { return isEmpty(); }
    reference front() { return first(); }
    const_reference front() const { return first(); }
    reference back() { return last(); }
    const_reference back() const { return last(); }

    int refCount() const { return int(d->ref); }

    void append(const T &value);

    const T &at(int i) const
    {
        Q_ASSERT_X(i >= 0 && i < d->m_size, "QCircularBuffer<T>::at", "index out of range");
        int index = (d->m_first + i) % d->m_capacity;
        return d->m_data[ index ];
    }

    const T &operator [] (int i) const
    {
        Q_ASSERT_X(i >= 0 && i < d->m_size, "QCircularBuffer<T>::operator[]", "index out of range");
        int index = (d->m_first + i) % d->m_capacity;
        return d->m_data[ index ];
    }

    T &operator [] (int i)
    {
        Q_ASSERT_X(i >= 0 && i < d->m_size, "QCircularBuffer<T>::operator[]", "index out of range");
        int index = (d->m_first + i) % d->m_capacity;
        return d->m_data[ index ];
    }

    int capacity() const { return d->m_capacity; }

    void clear() { *this = QCircularBuffer<T>(d->m_capacity); }

    bool contains(const T &value) const;
    int count(const T &value) const;
    int count() const { return size(); }

    array_range data()
    {
        if (d->m_size == 0)
            return array_range(0, 0);
        if (!isLinearised())
            linearise();
        return array_range(d->m_data + d->m_first, d->m_last - d->m_first + 1);
    }
    const_array_range data() const { return constData(); }
    const_array_range constData() const
    {
        if (!isLinearised() || d->m_size == 0)
            return const_array_range(0, 0);
        return const_array_range(d->m_data + d->m_first, d->m_last - d->m_first + 1);
    }

    array_range dataOne()
    {
        if (d->m_size == 0)
            return array_range(0, 0);
        if (isLinearised())
            return array_range(d->m_data + d->m_first, d->m_last - d->m_first + 1);
        else
            return array_range(d->m_data + d->m_first, d->m_capacity - d->m_first);
    }
    const_array_range dataOne() const { return constDataOne(); }
    const_array_range constDataOne() const
    {
        if (d->m_size == 0)
            return const_array_range(0, 0);
        if (isLinearised())
            return const_array_range(d->m_data + d->m_first, d->m_last - d->m_first + 1);
        else
            return const_array_range(d->m_data + d->m_first, d->m_capacity - d->m_first);
    }

    array_range dataTwo()
    {
        if (d->m_size == 0 || isLinearised())
            return array_range(0, 0);
        return array_range(d->m_data, d->m_last + 1);
    }
    const_array_range dataTwo() const { return constDataTwo(); }
    const_array_range constDataTwo() const
    {
        if (d->m_size == 0 || isLinearised())
            return const_array_range(0, 0);
        return const_array_range(d->m_data, d->m_last + 1);
    }

    bool endsWith(const T &value) const { return !isEmpty() && last() == value; }
    QCircularBuffer<T>& fill(const T &value, int size = -1);
    T &first() { Q_ASSERT(!isEmpty()); return d->m_data[ d->m_first ]; }
    const T &first() const { Q_ASSERT(!isEmpty()); return d->m_data[ d->m_first ]; }
    int freeSize() const { return sizeAvailable(); }

    static QCircularBuffer<T> fromList(const QList<T>& list);
    static QCircularBuffer<T> fromVector(const QVector<T>& vector);

    int indexOf(const T &value, int from = 0) const;
    void insert(int i, const T &value) { insert(i, 1, value); };
    void insert(int i, int count, const T &value);
    bool isEmpty() const { return d->m_size == 0; }
    bool isFull() const { return d->m_size == d->m_capacity; }
    bool isLinearised() const { return (d->m_last >= d->m_first); }
    T &last() { Q_ASSERT(!isEmpty()); return d->m_data[ d->m_last ]; }
    const T &last() const { Q_ASSERT(!isEmpty()); return d->m_data[ d->m_last ]; }
    int lastIndexOf(const T &value, int from = -1) const;
    void linearise();
    void prepend(const T &value);
    void remove(int i) { remove(i, 1); }
    void remove(int i, int count);

    void replace(int i, const T &value)
    {
        Q_ASSERT_X(i >= 0 && i < d->m_size, "QCircularBuffer<T>::replace", "index out of range");
        const T copy(value);
        (*this)[ i ] = copy;
    }

    void reserve(int capacity) { setCapacity(capacity); }
    void resize(int size);
    void setCapacity(int capacity);
    int size() const { return d->m_size; }
    int sizeAvailable() const { return d->m_capacity - d->m_size; }
    void squeeze() { setCapacity(size()); }
    bool startsWith(const T &value) const { return !isEmpty() && first() == value; }

    QList<T> toList() const;
    QVector<T> toVector() const;

    T value(int i) const
    {
        if (i < 0 || i >= d->m_size)
            return T();
        return at(i);
    }

    T value(int i, const T &defaultValue) const
    {
        if (i < 0 || i >= d->m_size)
            return defaultValue;
        return at(i);
    }

    bool operator == (const QCircularBuffer<T> other) const;
    bool operator != (const QCircularBuffer<T> other) const { return !(*this == other); }

    QCircularBuffer<T>& operator += (const T &other) { append(other); return *this; }
    QCircularBuffer<T>& operator += (const QCircularBuffer<T>& other);
    QCircularBuffer<T>& operator += (const QVector<T>& other);
    QCircularBuffer<T>& operator += (const QList<T>& other);

    QCircularBuffer<T>& operator << (const T &other) { append(other); return *this; }
    QCircularBuffer<T>& operator << (const QCircularBuffer<T>& other) { *this += other; return *this; }
    QCircularBuffer<T>& operator << (const QVector<T>& other) { *this += other; return *this; }
    QCircularBuffer<T>& operator << (const QList<T>& other) { *this += other; return *this; }

private:
    QSharedDataPointer< CircularBufferData<T> > d;
};

template <typename T>
QCircularBuffer<T> operator + (const QCircularBuffer<T>& lhs, const QCircularBuffer<T>& rhs);


template <typename T>
QCircularBuffer<T>::QCircularBuffer(int capacity)
    : d(new CircularBufferData<T>())
{
    // Allocate some raw memory
    d->m_data = d->allocate(capacity);
    d->m_capacity = capacity;

    // Initialise the objects. If the type T is a complex type then we do this
    // using the placement new operator. If the type T is not complex we simply
    // use memset to initialise the memory to zero.
    if (QTypeInfo<T>::isComplex)
    {
        T* b = d->m_data;
        T* i = b + d->m_capacity;
        while (i != b)
            new (--i) T;
    }
    else
    {
        qMemSet(d->m_data, 0, capacity * sizeof(T));
    }
}

template <typename T>
QCircularBuffer<T>::QCircularBuffer(int capacity, const T &value)
    : d(new CircularBufferData<T>())
{
    // Allocate some raw memory
    d->m_data = d->allocate(capacity);
    d->m_capacity = capacity;

    // Initialise the objects. In this case we always use the placement new operator
    T* b = d->m_data;
    T* i = b + d->m_capacity;
    while (i != b)
        new (--i) T(value);
    d->m_first = 0;
    d->m_last = d->m_capacity - 1;
    d->m_size = d->m_capacity;
}

template <typename T>
QCircularBuffer<T>::QCircularBuffer(int capacity, int size, const T &value)
    : d(new CircularBufferData<T>())
{
    Q_ASSERT_X(capacity >= size, "QCircularBuffer<T>::QCircularBuffer(int capacity, int size, const T &value)", "size is greater than capacity");

    // Allocate some raw memory
    d->m_data = d->allocate(capacity);
    d->m_capacity = capacity;

    // Initialise the objects that need to be set to the specified value.
    // In this case we always use the placement new operator
    T* b = d->m_data;
    T* i = b + size;
    while (i != b)
        new (--i) T(value);

    // Initialise the remaining objects using the best method available
    if (QTypeInfo<T>::isComplex)
    {
        T* b = d->m_data + size;
        T* i = b + capacity - size;
        while (i != b)
            new (--i) T;
    }
    else
    {
        qMemSet(d->m_data + size, 0, (capacity - size) * sizeof(T));
    }

    d->m_first = 0;
    d->m_last = size - 1;
    d->m_size = size;
}

template <typename T>
void QCircularBuffer<T>::append(const T &value)
{
    // If we have no capacity we do nothing
    if (!d->m_capacity)
        return;

    if (d->m_size == d->m_capacity)
    {
        // Buffer is full. Overwrite earliest item and rotate
        d->m_data[ d->m_first ] = value;
        d->m_first = (++d->m_first % d->m_capacity);
        d->m_last = (++d->m_last % d->m_capacity);
    }
    else if (d->m_size != 0)
    {
        // Buffer is partially full. Append data to end of array using appropriate method
        int index = (d->m_first + d->m_size) % d->m_capacity;
        if (QTypeInfo<T>::isComplex)
            new (d->m_data + index) T(value);
        else
            d->m_data[ index ] = value;
        ++d->m_size;
        ++d->m_last;
    }
    else
    {
        // Buffer is empty. Append data to end of array using appropriate method
        d->m_size = 1;
        d->m_first = d->m_last = 0;
        if (QTypeInfo<T>::isComplex)
            new (d->m_data + d->m_first) T(value);
        else
            d->m_data[ d->m_first ] = value;
    }
}

template <typename T>
bool QCircularBuffer<T>::contains(const T &value) const
{
    if (isLinearised())
    {
        T* b = d->m_data + d->m_first;
        T* i = b + d->m_size;
        while (i != b)
            if (*--i == value)
                return true;
        return false;
    }
    else
    {
        // Check the array from m_first to the end
        T* b = d->m_data + d->m_first;
        T* i = d->m_data + d->m_capacity;
        while (i != b)
            if (*--i == value)
                return true;

        // Check array from 0 to m_end
        b = d->m_data;
        i = d->m_data + d->m_last + 1;
        while (i != b)
            if (*--i == value)
                return true;

        return false;
    }
}

template <typename T>
int QCircularBuffer<T>::count(const T &value) const
{
    int c = 0;
    if (isLinearised())
    {
        T* b = d->m_data + d->m_first;
        T* i = b + d->m_size;
        while (i != b)
            if (*--i == value)
                ++c;
    }
    else
    {
        // Check the array from m_first to the end
        T* b = d->m_data + d->m_first;
        T* i = d->m_data + d->m_capacity;
        while (i != b)
            if (*--i == value)
                ++c;

        // Check array from 0 to m_end
        b = d->m_data;
        i = d->m_data + d->m_last + 1;
        while (i != b)
            if (*--i == value)
                ++c;
    }
    return c;
}

template <typename T>
QCircularBuffer<T>& QCircularBuffer<T>::fill(const T &value, int size)
{
    Q_ASSERT_X(d->m_capacity >= size, "QCircularBuffer<T>::fill", "size is greater than capacity");
    const T copy(value);
    int oldSize = d->m_size;
    d->m_size = (size < 0 ? d->m_size : size);
    d->m_first = (size == 0 ? -1 : 0);
    d->m_last = d->m_size - 1;

    // Copy item into array size times
    if (d->m_size)
    {
        T *b = d->m_data;
        T *i = d->m_data + d->m_size;
        while (i != b)
            *--i = copy;
    }

    if (d->m_size < oldSize)
    {
        // Cleanup old items beyond end of new array
        T *b = d->m_data + d->m_size;
        T *i = d->m_data + oldSize;
        while (i-- != b)
        {
            i->~T();
            new (i) T();
        }
    }

    return *this;
}

template <typename T>
QCircularBuffer<T> QCircularBuffer<T>::fromList(const QList<T>& list)
{
    QCircularBuffer<T> circ(list.size());
    for (int i = 0; i < list.size(); ++i)
        circ.append(list.at(i));
    return circ;
}

template <typename T>
QCircularBuffer<T> QCircularBuffer<T>::fromVector(const QVector<T>& vector)
{
    QCircularBuffer<T> circ(vector.size());
    for (int i = 0; i < vector.size(); ++i)
        circ.append(vector.at(i));
    return circ;
}

template <typename T>
int QCircularBuffer<T>::indexOf(const T &value, int from) const
{
    Q_ASSERT_X(from < d->m_size, "QCircularBuffer<T>::indexOf", "from is greater than last valid index");
    if (from < 0)
        from = qMax(from + d->m_size, 0);
    else if (from >= d->m_size)
        from = d->m_size - 1;
    for (int i = from; i < d->m_size; ++i)
        if (at(i) == value)
            return i;
    return -1;
}

template <typename T>
void QCircularBuffer<T>::insert(int i, int count, const T &value)
{
    Q_ASSERT_X(i >= 0 && i <= d->m_size, "QCircularBuffer<T>::insert", "index out of range");

    int freeCapacity = d->m_capacity - d->m_size;

    // Calculate number of elements that will actually be inserted. This
    // depends upon where the insertion has been requested and any spare
    // capacity left in the buffer. This is because elements at higher
    // indices will be pushed to the right and will potentially wrap around
    // to overwrite earlier elements.
    int numToInsert = qMin(count, i + freeCapacity);

    // Calculate the number of elements at the beginning of the buffer that
    // will be overwritten
    int numToOverwrite = qMin(i, qMax(0, count - freeCapacity));

    // Decide which way to shift to minimise the amount of copying required.
    if (i < d->m_size / 2)
    {
        // Inserting in lower half of buffer so we shift earlier items down

        // Shift data at the bottom end down. This may only be a subset if some
        // of the early data is to be overwritten.
        if (QTypeInfo<T>::isStatic)
        {
            int start = d->m_first + numToOverwrite;
            int end = d->m_first + i - 1;
            for (int j = start; j <= end; ++j)
            {
                int srcIndex = j % d->m_capacity;
                int dstIndex = (j - numToInsert + d->m_capacity) % d->m_capacity;
                T* src = d->m_data + srcIndex;
                T* dst = d->m_data + dstIndex;

                new (dst) T(*src);
            }
        }
        else
        {
            // We have a movable type so a simple memcopy (or maybe two or
            // three) will suffice to shift the data at the bottom end
            int numToMove = i - numToOverwrite;
            if (numToMove > 0)
            {
                int srcBegin = (d->m_first + numToOverwrite) % d->m_capacity;
                int srcEnd = (d->m_first + i - 1) % d->m_capacity;
                int dstBegin = (srcBegin - numToInsert + d->m_capacity) % d->m_capacity;
                int dstEnd = (srcEnd - numToInsert + d->m_capacity) % d->m_capacity;

                // Do we have any wrap-around problems to deal with?
                bool srcRegionWraps = (srcEnd < srcBegin);
                bool dstRegionWraps = (dstEnd < dstBegin);
                if (!srcRegionWraps && dstRegionWraps)
                {
                    // Destination region wraps so do the move in two steps
                    int wrapCount = abs(srcBegin - numToInsert);
                    memmove(d->m_data + d->m_capacity - wrapCount, d->m_data + srcBegin, wrapCount * sizeof(T));
                    memmove(d->m_data, d->m_data + srcBegin + wrapCount, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && !dstRegionWraps)
                {
                    // Source region wraps so do the move in two steps
                    int wrapCount = d->m_capacity - srcBegin;
                    memmove(d->m_data + dstBegin, d->m_data + d->m_capacity - wrapCount, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin + numToInsert, d->m_data, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && dstRegionWraps)
                {
                    // Source and destination regions wrap so we have to do this in three steps
                    int srcWrapCount = d->m_capacity - srcBegin;
                    memmove(d->m_data + dstBegin, d->m_data + d->m_capacity - srcWrapCount, srcWrapCount * sizeof(T));
                    memmove(d->m_data + d->m_capacity - numToInsert, d->m_data, numToInsert * sizeof(T));
                    memmove(d->m_data, d->m_data + numToInsert, (numToMove - srcWrapCount - numToInsert) * sizeof(T));
                }
                else
                {
                    // No wrap around - do a single memmove
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, numToMove * sizeof(T));
                }
            }
        }

        // Insert the new items
        int end = d->m_first + i;
        int start = end - numToInsert;
        for (int j = start; j < end; ++j)
        {
            T* p = d->m_data + ((j + d->m_capacity) % d->m_capacity);
            new (p) T(value);
        }

        // Adjust the first, last and size indices as needed.
        // NB. The last index never changes in this regime.
        d->m_size += qMin(count, freeCapacity);
        d->m_first = (d->m_first - (numToInsert - numToOverwrite) + d->m_capacity) % d->m_capacity;
    }
    else
    {
        // Inserting in upper half of buffer so we shift later items up

        // Shift data at the top end up which may or may not overwrite some
        // of the earliest data
        if (QTypeInfo<T>::isStatic)
        {
            int start = d->m_first + d->m_size - 1;
            int end = d->m_first + i;
            for (int j = start; j >= end; j--)
            {
                int srcIndex = j % d->m_capacity;
                int dstIndex = (j + numToInsert) % d->m_capacity;
                T* src = d->m_data + srcIndex;
                T* dst = d->m_data + dstIndex;

                new (dst) T(*src);
            }
        }
        else
        {
            // We have a movable type so a simple memcopy (or maybe two or
            // three) will suffice to shift the data at the top end
            int numToMove = d->m_size - i;
            if (numToMove > 0)
            {
                int srcBegin = (d->m_first + i) % d->m_capacity;
                int srcEnd = d->m_last;
                int dstBegin = (srcBegin + numToInsert) % d->m_capacity;
                int dstEnd = (srcEnd + numToInsert) % d->m_capacity;

                // Do we have any wrap-around problems to deal with?
                bool srcRegionWraps = (srcEnd < srcBegin);
                bool dstRegionWraps = (dstEnd < dstBegin);
                if (!srcRegionWraps && dstRegionWraps)
                {
                    // Destination region wraps so do the move in two steps
                    int wrapCount = srcEnd + numToInsert - d->m_capacity + 1;
                    memmove(d->m_data, d->m_data + srcEnd - wrapCount + 1, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && !dstRegionWraps)
                {
                    // Source region wraps so do the move in two steps
                    int wrapCount = d->m_last + 1;
                    memmove(d->m_data + numToInsert, d->m_data, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && dstRegionWraps)
                {
                    // Source and destination regions wrap so we have to do this in three steps
                    int srcWrapCount = d->m_last + 1;
                    memmove(d->m_data + numToInsert, d->m_data, srcWrapCount * sizeof(T));
                    memmove(d->m_data, d->m_data + d->m_capacity - numToInsert, numToInsert * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - srcWrapCount - numToInsert) * sizeof(T));
                }
                else
                {
                    // No wrap around - do a single memmove
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, numToMove * sizeof(T));
                }
            }
        }

        // Insert the new items
        for (int j = d->m_first + i; j < d->m_first + i + numToInsert; ++j)
        {
            T* p = d->m_data + (j % d->m_capacity);
            new (p) T(value);
        }

        // Adjust the first, last and size indices as needed
        d->m_size += qMin(count, freeCapacity);
        d->m_first = (d->m_first + numToOverwrite) % d->m_capacity;
        d->m_last = (d->m_last + numToInsert) % d->m_capacity;
    }
}

template <typename T>
int QCircularBuffer<T>::lastIndexOf(const T &value, int from) const
{
    if (from < 0)
        from = qMax(from + d->m_size, 0);
    else if (from >= d->m_size)
        from = d->m_size - 1;
    for (int i = from; i >= 0; --i)
        if (at(i) == value)
            return i;
    return -1;
}

template <typename T>
void QCircularBuffer<T>::linearise()
{
    if (isLinearised())
        return;

    // Allocate some new raw memory
    T* newData = d->allocate(d->m_capacity);

    // Copy across the elements from the original array...
    // Copy items from m_first to the end of the array
    T* b = d->m_data + d->m_first;
    T* i = d->m_data + d->m_capacity;
    T* j = newData + d->m_capacity - d->m_first;
    while (i != b)
        new (--j) T(*--i);

    // Copy items from the start of the array to m_last
    b = d->m_data;
    i = d->m_data + d->m_last + 1;
    j = newData + d->m_size;
    while (i != b)
        new (--j) T(*--i);

    // Destroy the original items
    if (QTypeInfo<T>::isComplex)
    {
        // The type is complex so we manually call the destructor for each item
        // since we used the placement new operator to instantiate them
        T* b = d->m_data;
        T* i = b + d->m_capacity;
        while (i-- != b)
             i->~T();
    }

    // Release the raw memory
    d->deallocate(d->m_data);

    // Assign the new memory to be our buffer data and fix indices
    d->m_data = newData;
    d->m_first = 0;
    d->m_last = d->m_size - 1;
}

template <typename T>
void QCircularBuffer<T>::prepend(const T &value)
{
    // If we have no capacity we do nothing
    if (!d->m_capacity)
        return;

    if (d->m_size == d->m_capacity)
    {
        // Buffer is full. Overwrite last item and rotate
        d->m_data[ d->m_last ] = value;
        d->m_first = (--d->m_first + d->m_capacity) % d->m_capacity;
        d->m_last = (--d->m_last + d->m_capacity) % d->m_capacity;
    }
    else if (d->m_size != 0)
    {
        // Buffer is partially full. Prepend data to start of array using appropriate method
        d->m_first = (--d->m_first + d->m_capacity) % d->m_capacity;
        ++d->m_size;
        if (QTypeInfo<T>::isComplex)
            new (d->m_data + d->m_first) T(value);
        else
            d->m_data[ d->m_first ] = value;
    }
    else
    {
        // Buffer is empty. Prepend data to start of array using appropriate method
        d->m_size = 1;
        d->m_first = d->m_last = d->m_capacity - 1;
        if (QTypeInfo<T>::isComplex)
            new (d->m_data + d->m_first) T(value);
        else
            d->m_data[ d->m_first ] = value;
    }
}

template <typename T>
void QCircularBuffer<T>::remove(int i, int count)
{
    Q_ASSERT_X(i >= 0 && count > 0 && i + count <= d->m_size, "QCircularBuffer<T>::remove", "index out of range");

    // Calculate the number of items that need to be moved downward
    int numToMoveDown = d->m_size - count - i;
    int numToMoveUp = i;

    if (numToMoveDown < numToMoveUp)
    {
        // Move higher items down
        int numToMove = numToMoveDown;

        if (QTypeInfo<T>::isComplex)
        {
            // Copy items down from higher positions
            int start = d->m_first + i;
            int end = start + numToMove;
            for (int j = start; j < end ; ++j)
            {
                T* src = d->m_data + ((j + count) % d->m_capacity);
                T* dst = d->m_data + (j % d->m_capacity);
                new (dst) T(*src);
            }

            // Clean up items at end of buffer
            for (int j = d->m_last; j > d->m_last - count; --j)
            {
                T* p = d->m_data + ((j + d->m_capacity) % d->m_capacity);
                p->~T();
                new (p) T();
            }
        }
        else
        {
            if (isLinearised())
            {
                // With a linearised buffer we can do a simple move and removal of items
                memmove(d->m_data + d->m_last - numToMove - count + 1, d->m_data + d->m_last - numToMove + 1, numToMove * sizeof(T));
                qMemSet(d->m_data + d->m_last - count + 1, 0, count * sizeof(T));
            }
            else
            {
                // With a non-linearised buffer we need to be careful of wrapping issues
                int srcBegin = (d->m_last - numToMove + 1 + d->m_capacity) % d->m_capacity;
                int srcEnd = d->m_last;
                int dstBegin = (d->m_first + i) % d->m_capacity;
                int dstEnd = (dstBegin + numToMove - 1) % d->m_capacity;

                bool srcRegionWraps = (srcEnd < srcBegin);
                bool dstRegionWraps = (dstEnd < dstBegin);
                if (srcRegionWraps && !dstRegionWraps)
                {
                    // Source region wraps so do the move in two steps
                    int wrapCount = d->m_capacity - srcBegin;
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin + wrapCount, d->m_data, (numToMove - wrapCount) * sizeof(T));
                }
                else if (!srcRegionWraps && dstRegionWraps)
                {
                    // Destination region wraps so do the move in two steps
                    int wrapCount = count - srcBegin;
                    memmove(d->m_data + d->m_capacity - wrapCount, d->m_data + srcBegin, wrapCount * sizeof(T));
                    memmove(d->m_data, d->m_data + srcBegin + wrapCount, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && dstRegionWraps)
                {
                    // Source and destination regions wrap so we have to do this in three steps
                    int srcWrapCount = d->m_capacity - srcBegin;
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, srcWrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin + srcWrapCount, d->m_data, count * sizeof(T));
                    memmove(d->m_data, d->m_data + count, (numToMove - srcWrapCount - count) * sizeof(T));
                }
                else
                {
                    // No wrap around, so we can do this in one hit
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, numToMove * sizeof(T));
                }

                // We potentially have a disjoint region that needs zeroing
                int zeroStart = (d->m_last - count + d->m_capacity + 1) % d->m_capacity;
                int zeroEnd = d->m_last;
                if (zeroEnd < zeroStart)
                {
                    // Region to be zeroed wraps. Do it in two steps.
                    qMemSet(d->m_data, 0, (d->m_last + 1) * sizeof(T));
                    qMemSet(d->m_data + zeroStart, 0, (count - d->m_last - 1) * sizeof(T));
                }
                else
                {
                    // Region to be zeroed is contiguous
                    qMemSet(d->m_data + zeroStart, 0, count * sizeof(T));
                }
            }
        }

        // Adjust the indices
        d->m_size -= count;
        d->m_last = (d->m_last - count + d->m_capacity) % d->m_capacity;
    }
    else
    {
        // Move lower items up
        int numToMove = numToMoveUp;

        if (QTypeInfo<T>::isComplex)
        {
            // Copy items up from lower positions
            int start = d->m_first + i - 1;
            int end = start - numToMove;
            for (int j = start; j > end ; --j)
            {
                T* src = d->m_data + ((j + d->m_capacity) % d->m_capacity);
                T* dst = d->m_data + ((j + d->m_capacity + count) % d->m_capacity);
                new (dst) T(*src);
            }

            // Clean up items at start of buffer
            for (int j = d->m_first; j < d->m_first + count; ++j)
            {
                T* p = d->m_data + (j % d->m_capacity);
                p->~T();
                new (p) T();
            }
        }
        else
        {
            if (isLinearised())
            {
                // With a linearised buffer we can do a simple move and removal of items
                memmove(d->m_data + d->m_first + count, d->m_data + d->m_first, numToMove * sizeof(T));
                qMemSet(d->m_data + d->m_first, 0, count * sizeof(T));
            }
            else
            {
                // With a non-linearised buffer we need to be careful of wrapping issues
                int srcBegin = d->m_first;
                int srcEnd = (srcBegin + numToMove - 1) % d->m_capacity;
                int dstBegin = (srcBegin + count) % d->m_capacity;
                int dstEnd = (dstBegin + numToMove - 1) % d->m_capacity;

                bool srcRegionWraps = (srcEnd < srcBegin);
                bool dstRegionWraps = (dstEnd < dstBegin);
                if (srcRegionWraps && !dstRegionWraps)
                {
                    // Source region wraps so do the move in two steps
                    int wrapCount = srcEnd + 1;
                    memmove(d->m_data + dstEnd - wrapCount + 1, d->m_data, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - wrapCount) * sizeof(T));
                }
                else if (!srcRegionWraps && dstRegionWraps)
                {
                    // Destination region wraps so do the move in two steps
                    int wrapCount = dstEnd + 1;
                    memmove(d->m_data, d->m_data + srcEnd - wrapCount + 1, wrapCount * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - wrapCount) * sizeof(T));
                }
                else if (srcRegionWraps && dstRegionWraps)
                {
                    // Source and destination regions wrap so we have to do this in three steps
                    int srcWrapCount = srcEnd + 1;
                    memmove(d->m_data + dstEnd - srcWrapCount + 1, d->m_data, srcWrapCount * sizeof(T));
                    memmove(d->m_data, d->m_data + d->m_capacity - count, count * sizeof(T));
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, (numToMove - srcWrapCount - count) * sizeof(T));
                }
                else
                {
                    // No wrap around, so we can do this in one hit
                    memmove(d->m_data + dstBegin, d->m_data + srcBegin, numToMove * sizeof(T));
                }

                // We potentially have a disjoint region that needs zeroing
                int zeroStart = d->m_first;
                int zeroEnd = (zeroStart + count - 1) % d->m_capacity;
                if (zeroEnd < zeroStart)
                {
                    // Region to be zeroed wraps. Do it in two steps.
                    qMemSet(d->m_data + zeroStart, 0, (d->m_capacity - d->m_first) * sizeof(T));
                    qMemSet(d->m_data, 0, (count - d->m_capacity + d->m_first) * sizeof(T));
                }
                else
                {
                    // Region to be zeroed is contiguous
                    qMemSet(d->m_data + zeroStart, 0, count * sizeof(T));
                }
            }
        }

        // Adjust the indices
        d->m_size -= count;
        d->m_first = (d->m_first + count) % d->m_capacity;
    }
}

template <typename T>
void QCircularBuffer<T>::setCapacity(int capacity)
{
    if (capacity == d->m_capacity)
        return;

    // Allocate some new raw memory
    T* newData = d->allocate(capacity);

    // How many items can we copy across?
    int newSize = qMin(d->m_size, capacity);

    if (QTypeInfo<T>::isComplex)
    {
        // Copy across the elements from the original array
        for (int i = 0; i < newSize; ++i)
        {
            T* src = d->m_data + ((d->m_first + i) % d->m_capacity);
            T* dst = newData + i;
            new (dst) T(*src);
        }

        // Initialise any memory outside of the valid buffer (ie the unused items)
        for (int i = newSize; i < capacity; ++i)
            new (newData + i) T();

        // Destroy the original items.
        // The type is complex so we manually call the destructor for each item
        // since we used the placement new operator to instantiate them
        T* b = d->m_data;
        T* i = b + d->m_capacity;
        while (i-- != b)
             i->~T();
    }
    else
    {
        // Copy across the elements from the original array. The source region
        // potentially wraps so we may have to do this in one or two steps
        if (isLinearised())
        {
            memmove(newData, d->m_data + d->m_first, newSize * sizeof(T));
        }
        else
        {
            int step1Size = qMin(newSize, d->m_capacity - d->m_first);
            memmove(newData, d->m_data + d->m_first, step1Size * sizeof(T));
            int step2Size = qMax(0, qMin(newSize - d->m_capacity + d->m_first, d->m_last + 1));
            memmove(newData + step1Size, d->m_data, step2Size * sizeof(T));
        }

        // Initialise any memory outside of the valid buffer (ie the unused items)
        qMemSet(newData + newSize, 0, (capacity - newSize) * sizeof(T));
    }

    // Release the raw memory for the old array
    d->deallocate(d->m_data);

    // Assign the new memory to be our buffer data and fix indices
    d->m_data = newData;
    d->m_capacity = capacity;
    d->m_first = 0;
    d->m_size = newSize;
    d->m_last = d->m_size - 1;
}

template <typename T>
void QCircularBuffer<T>::resize(int size)
{
    Q_ASSERT_X(size >= 0 && size <= d->m_capacity, "QCircularBuffer<T>::resize", "size out of range");

    if (size < d->m_size)
        remove(size, d->m_size - size);
    else if (size > d->m_size)
    {
        T t;
        insert(d->m_size, size - d->m_size, t);
    }
}

template <typename T>
bool QCircularBuffer<T>::operator == (const QCircularBuffer<T> other) const
{
    if (capacity() != other.capacity() || size() != other.size())
        return false;
    if (d == other.d)
        return true;
    int count = size();
    for (int i = 0; i < count; ++i)
        if (at(i) != other.at(i))
            return false;
    return true;
}

template <typename T>
QCircularBuffer<T>& QCircularBuffer<T>::operator += (const QCircularBuffer<T>& other)
{
    // How many items do we need to copy? No point in ever copying across a number
    // greater than capacity
    int numToCopy = qMin(other.size(), d->m_capacity);
    int offset = other.size() - numToCopy;
    for (int i = 0; i < numToCopy; ++i)
        append(other.at(offset + i));
    return *this;
}

template <typename T>
QCircularBuffer<T>& QCircularBuffer<T>::operator += (const QVector<T>& other)
{
    // How many items do we need to copy? No point in ever copying across a number
    // greater than capacity
    int numToCopy = qMin(other.size(), d->m_capacity);
    int offset = other.size() - numToCopy;
    for (int i = 0; i < numToCopy; ++i)
        append(other.at(offset + i));
    return *this;
}

template <typename T>
QCircularBuffer<T>& QCircularBuffer<T>::operator += (const QList<T>& other)
{
    // How many items do we need to copy? No point in ever copying across a number
    // greater than capacity
    int numToCopy = qMin(other.size(), d->m_capacity);
    int offset = other.size() - numToCopy;
    for (int i = 0; i < numToCopy; ++i)
        append(other.at(offset + i));
    return *this;
}

template <typename T>
QList<T> QCircularBuffer<T>::toList() const
{
    QList<T> list;
    for (int i = 0; i < size(); ++i)
        list.append(at(i));
    return list;
}

template <typename T>
QVector<T> QCircularBuffer<T>::toVector() const
{
    QVector<T> vector;
    for (int i = 0; i < size(); ++i)
        vector.append(at(i));
    return vector;
}

template <typename T>
QCircularBuffer<T> operator + (const QCircularBuffer<T>& lhs, const QCircularBuffer<T>& rhs)
{
    QCircularBuffer<int> circ(lhs.size() + rhs.size());
    for (int i = 0; i < lhs.size(); ++i)
        circ.append(lhs.at(i));
    for (int i = 0; i < rhs.size(); ++i)
        circ.append(rhs.at(i));
    return circ;
}

Q_DECLARE_SEQUENTIAL_ITERATOR(CircularBuffer)
Q_DECLARE_MUTABLE_SEQUENTIAL_ITERATOR(CircularBuffer)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCIRCULARBUFFER_H
