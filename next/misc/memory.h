#ifndef MEMORY_H
#define MEMORY_H

#include <mutex>
#include <vector>

/**
 * @class locked
 * \brief A thread-safe wrapper class. Contains a unique_lock that gets unlocked when it goes out of scope
 *
 */
template <typename T>
class locked : public std::unique_lock<std::recursive_mutex> {
public:
    /**
     * @brief Constructs an empty invalid locked object
     */
    locked()
        : std::unique_lock<std::recursive_mutex>()
        , data_(nullptr)
    {}
    /**
     * @brief Constructs a valid locked object
     * @param mutex - Needs to be a valid mutex, the application will block if it's already locked in a different thread
     * @param data - Pointer to data to protect
     */
    locked(mutex_type &mutex, T *data)
        : std::unique_lock<std::recursive_mutex>(mutex)
        , data_(data)
    {}
    T* data() { return data_; }
    T *operator->() { return data_; }
    T &operator*() { return *data_; }
    /**
     * @brief operator bool - Returns true if the mutex is locked and the pointer is not null
     */
    operator bool() const {
        return owns_lock() && data_;
    }
private:
    T *data_;
};

/**
 * @class ProtectedModel
 * \brief Contains convenience methods and a set of data to be protected with a single mutex.
 *
 * This class is intended to be used especially with BaseModel-based objects (TimeEntry, Project, etc.)
 * It also provides facilities to lock other objects using the internal mutex.
 */
template <typename T>
class ProtectedModel {
public:
    /**
     * @brief operator () - Convenience method to access the internal locked container directly
     * @return A @ref locked object containing a map of <typename T> types
     */
    locked<std::vector<T*>> operator()() {
        return { mutex_, &container_ };
    }
    /**
     * @brief operator () - Convenience method to access the internal locked container directly
     * @return A @ref locked object containing a map of <typename T> types
     */
    locked<const std::vector<T*>> operator()() const {
        return { mutex_, &container_ };
    }
    /**
     * @brief clear - Clear the @ref container_
     * @param deleteItems - Set to true if the pointers contained in the @ref container_ should be deleted, too
     */
    void clear(bool deleteItems = true) {
        auto lockedContainer = (*this)();
        if (deleteItems) {
            for (auto i : *lockedContainer)
                delete i;
        }
        lockedContainer->clear();
    }
    /**
     * @brief create - Allocate a new instance of <typename T>
     * @return - a @ref locked new instance of T
     * OVERHAUL TODO: this could be more efficient
     */
    locked<T> create() {
        T *val = new T();
        (*this)()->push_back(val);
        return { mutex_, val };
    }
    /**
     * @brief size - Get how many items are contained inside
     * @return - number of items inside the container
     */
    size_t size() {
        auto lockedContainer = (*this)();
        return lockedContainer->size();
    }
    /**
     * @brief make_locked - Makes pointer to any type locked by the internal mutex
     * @param val - can be of any type (but a pointer)
     *
     * @warning beware of using pointers to non-static local variables with this method
     */
    template<typename U> locked<U> make_locked(U *val) {
        return { mutex_, val };
    }
    /**
     * @brief make_locked - Makes pointer to any type locked by the internal mutex
     * @param val - can be of any type (but a pointer)
     *
     * @warning beware of using pointers to non-static local variables with this method
     */
    template<typename U> locked<const U> make_locked(const U *val) const {
        return { mutex_, val };
    }
    /**
     * @brief findByID - Finds a contained BaseModel instance by looking for its ID
     * @param id - ID to look for
     * @return - valid @ref locked object containing a pointer to the instance if found, invalid @ref locked object if not found
     */
    locked<T> findByID(uint64_t id) {
        auto lockedContainer = (*this)();
        for (auto i : *lockedContainer) {
            if (i->ID() == id)
                return { mutex_, i };
        }
        return {};
    }
    /**
     * @brief findByID - Finds a contained BaseModel instance by looking for its ID
     * @param id - ID to look for
     * @return - valid @ref locked object containing a pointer to the instance if found, invalid @ref locked object if not found
     */
    locked<const T> findByID(uint64_t id) const {
        auto lockedContainer = (*this)();
        for (auto i : *lockedContainer) {
            if (i->ID() == id)
                return { mutex_, i };
        }
        return {};
    }
    /**
     * @brief findByID - Finds a contained BaseModel instance by looking for its GUID
     * @param guid - GUID to look for
     * @return - valid @ref locked object containing a pointer to the instance if found, invalid @ref locked object if not found
     */
    locked<T> findByGUID(const std::string &guid) {
        auto lockedContainer = (*this)();
        for (auto i : *lockedContainer) {
            if (i->GUID() == guid)
                return { mutex_, i };
        }
        return {};
    }
    /**
     * @brief findByID - Finds a contained BaseModel instance by looking for its GUID
     * @param guid - GUID to look for
     * @return - valid @ref locked object containing a pointer to the instance if found, invalid @ref locked object if not found
     */
    locked<const T> findByGUID(const std::string &guid) const {
        auto lockedContainer = (*this)();
        for (auto i : *lockedContainer) {
            if (i->GUID() == guid)
                return { mutex_, i };
        }
        return {};
    }
private:
    std::vector<T*> container_;
    mutable std::recursive_mutex mutex_;
};


#endif // MEMORY_H
