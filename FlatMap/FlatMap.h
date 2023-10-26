#ifndef FlatMap_h
#define FlatMap_h
template <class Key, class Value>
class FlatMap
{
public:
    FlatMap() : capacity(DefaultSize), sizeArray(0) //изначальная емкость(capaciry) = 1; изначальное количество элементов(sizeArray) =0
    {
        data = new std::pair<Key, Value>[capacity]; //инициализация динамического массива data, который хранит пары ключ значениt, выделяется в куче с начальной емкостью, определенной значением capacity.
    }

    ~FlatMap()
    {
        delete[] data;
    }

    FlatMap(const FlatMap& b) : capacity(b.capacity), sizeArray(b.sizeArray)//конструктор копирования
    {
        data = new std::pair<Key, Value>[capacity];

        for (size_t i = 0; i < sizeArray; ++i)
        {
            data[i] = b.data[i];
        }
    }
    FlatMap(FlatMap&& b)
    {
        data = b.data;
        capacity = b.capacity;
        sizeArray = b.sizeArray;
        b.data = nullptr;
        b.capacity = 0;
        b.sizeArray = 0;
        
    }
    FlatMap& operator=(const FlatMap& b)
    {// Если не совпадает capacity, то нужно вызвать resize()
        if (this == &b)
            return *this;
        if (sizeArray == b.sizeArray)
        {
            for (size_t i = 0; i < sizeArray; ++i)
                data[i] = b.data[i];
            capacity = b.capacity;
            DefaultSize = b.DefaultSize;
            return *this;
        }
        else if (sizeArray > b.sizeArray)
        {
            for (size_t i = 0; i < b.sizeArray; ++i)
                data[i] = b.data[i];
            sizeArray = b.sizeArray;
            capacity = b.capacity;
            DefaultSize = b.DefaultSize;
            return *this;
        }
        else // (sizeArray < b.sizeArray)
        {
            if (capacity < b.capacity)
                resize();
            sizeArray = b.sizeArray;
            for (size_t i = 0; i < sizeArray; ++i)
                data[i] = b.data[i];
            capacity = b.capacity;
            DefaultSize = b.DefaultSize;
            return *this;
        } //а надо ли делать DefaultSize = b.DefaultSize;?
    }
    FlatMap&& operator=(FlatMap&& b)
    {
        if (this == &b)
            return *this;
        delete[] data;
        data = b.data;
        capacity = b.capacity;
        sizeArray = b.sizeArray;
        // Обнуляем b для предотвращения удаления данных при разрушении b
        b.data = nullptr;
        b.capacity = 0;
        b.sizeArray = 0;
        
    }
    
    
    void swap(FlatMap& b)
    {
        std::swap(data, b.data);
        std::swap(capacity, b.capacity);
        std::swap(sizeArray, b.sizeArray);
    }
    void clear()
    {
        if (sizeArray == 0) return;
        //удалить все элементы
        delete[] data;
        sizeArray = 0;
    }
    // Удаляет элемент по заданному ключу.
    bool erase(const Key& key)
    {
        bool ok;
        size_t index = BinarySearch(key, ok);
        if (!ok)
            return false; // Ключ не найден, удаление не выполнено
        if (data[index].first == key)
        {
            for (size_t i = index; i < sizeArray - 1; ++i)
                data[i] = data[i + 1];
            sizeArray--;
            return true; // Успешно удалено
        }
    }
    // Вставка в контейнер. Возвращаемое значение - успешность вставки.
    bool insert(const Key& key, const Value& value)
    {
        if (sizeArray >= capacity)
        {
            // Если массив заполнен, увеличиваем его размер
            resize();
        }
        else
        {
            bool ok;
            size_t index = BinarySearch(key, ok);
            if (ok)
            {
                data[index] = std::make_pair(key, value);
                return true;
            }
            sizeArray++;
            for (size_t i = index + 1; i < sizeArray - 1; ++i)
                data[i] = data[i - 1];
            data[index] = std::make_pair(key, value);
            return true; // Успешно вставлено
        }
    }

    // Проверка наличия значения по заданному ключу
    bool contains(const Key& key) const
    {
        bool ok;
        size_t index = BinarySearch(key, ok);
        if (ok && data[index].first == key)
        {
            return true;
        }
        else
            return false;
    }
    // Возвращает значение по ключу. Бросает исключение при неудаче.
    Value at(const Key& key) const
    {
        bool ok;
        size_t index = BinarySearch(key, ok);
        if (ok && data[index].first == key)
        {
            return data[index].second;
        }
        throw std::out_of_range("Key not found");
    }
    
    
    // Возвращает значение по ключу. Небезопасный метод.
      // В случае отсутствия ключа в контейнере, следует вставить в контейнер
      // значение, созданное конструктором по умолчанию и вернуть ссылку на него.
    Value& operator[](const Key& key)
    {
        bool ok;
        size_t index = BinarySearch(key, ok);

        if (ok && data[index].first == key)
        {
            // Ключ найден, возвращаем соответствующее значение
            return data[index].second;
        }
        else
        {
            // Ключ отсутствует, вставляем значение по умолчанию
            insert(key, Value());
            return data[sizeArray - 1].second; // Возвращаем ссылку на новое значение
        }
    }
    
    size_t size() const
    {
        return sizeArray;
    }
    bool empty() const //че
    {
        return sizeArray == 0;
    }

private:
    std::pair<Key, Value>* data;
    size_t capacity = 0ull;
    size_t sizeArray = 0ull;
    static constexpr size_t DefaultSize = 1ull;


    // Увеличение емкости массива при заполнении
    void resize()
    {
        capacity *= 2;
        std::pair<Key, Value>* newData = new std::pair<Key, Value>[capacity];
        for (size_t i = 0; i < sizeArray; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    //бинарный поиск, который возвращает либо место найденого и true, либо место, куда нужно вставить и false.
    size_t BinarySearch(const Key k, bool& ok) const
    {
        if (sizeArray == 0) { ok = true; return 0; }
        if (k == data[0].first) { ok = true; return 0; }
        if (k == data[sizeArray - 1].first) { ok = true;  return sizeArray - 1; }

        size_t r = sizeArray - 1;
        size_t l = 0;
        size_t mid = 0;
        while (l <= r)
        {
            mid = (l + r) / 2;
            if (k == data[mid].first) { ok = true;  return mid; }
            else if (k > data[mid].first) l = mid + 1;
            else r = mid - 1;
        }
        ok = false;
        return l;
    }

    friend bool operator==(const FlatMap& a, const FlatMap& b);
    friend bool operator!=(const FlatMap& a, const FlatMap& b);
};
//дружественные операторы сравнения
template<class Key, class Value>
bool operator==(const FlatMap<Key, Value>& a, const FlatMap<Key, Value>& b) {
    if (a.sizeArray == b.sizeArray) {
        for (int i = 0; i < a.sizeArray; i++) {
            if(!(a.data[i].first == b.data[i].first)) return false;
            if(!(a.data[i].second == b.data[i].second)) return false;
        }
        return true;
    }
    else
        return false;
}
template <class Key, class Value>
bool operator!=(const FlatMap<Key, Value>& a, const FlatMap<Key, Value>& b) {
    if (a.sizeArray != b.sizeArray) return true;
    for (int i = 0; i < a.sizeArray; i++) {
        if(!(a.data[i].first == b.data[i].first)) return true;
        if(!(a.data[i].second == b.data[i].second)) return true;
    }
    return false;
}
#endif /* FlatMap_h */

