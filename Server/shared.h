#ifndef SHARED_H
#define SHARED_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/allocators/cached_adaptive_pool.hpp>

#include <boost/interprocess/sync/named_recursive_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_set>

#include <string>
#include <type_traits>

/*!
 * \brief Shared Process schema written by Kevin Brightwell for 
 *        SE3313A at Western Univeristy Canada.
 * This is a very simple wrapper on top of boost::interprocess that creates
 * RAII style wrappers for some basic threading primitives for multi-process
 * programming. 
 */

namespace shared {

namespace bip = boost::interprocess;

//Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
//This allocator will allow placing containers in the segment
template <typename T>
using shmem_allocator = bip::cached_adaptive_pool<T, bip::managed_shared_memory::segment_manager> ;

//Alias a deque that uses the previous STL-like allocator so that allocates
//its values from the segment
template <typename T>
using deque = bip::deque<T, shmem_allocator<T> >;
template <typename T>
using deque_ptr = std::shared_ptr<deque<T> >;

// Also alias a mutex, condtion, and lock
using mutex = bip::interprocess_mutex;
using mutex_ptr = std::shared_ptr<mutex>;

// Conditions that are shareable across shared memory boundary
using condition = bip::interprocess_condition;
using condition_ptr = std::shared_ptr<condition>;

// Lock for locking mutexes
using scoped_lock = bip::scoped_lock<mutex>;

/**
 * \brief A simple semaphore implementation that works across shared memory.
 * 
 * While you can direcly use the constructor, it is recommended to use the methods in the 
 * factory class.
 */
class semaphore final {

public:
    void yield()
    {
        scoped_lock lock(*d_mutex);
        ++*d_count;
        d_condition->notify_one();
    }

    void wait()
    {
        scoped_lock lock(*d_mutex);
        while(!*d_count)
            d_condition->wait(lock);
        --*d_count;
    }
    
    semaphore(const bip::create_only_t&, mutex_ptr mutex, condition_ptr condition, std::shared_ptr<unsigned long> count)
        : d_mutex(mutex), d_condition(condition), d_count(count)
    {
        scoped_lock lock(*d_mutex);
        *d_count = 0;
    }
    
    semaphore(const bip::open_only_t&, mutex_ptr mutex, condition_ptr condition, std::shared_ptr<unsigned long> count)
        : d_mutex(mutex), d_condition(condition), d_count(count)
    { }
    
private:
    mutex_ptr d_mutex;
    condition_ptr d_condition;
    std::shared_ptr<unsigned long> d_count;
    
};

using semaphore_ptr = std::shared_ptr<semaphore>;

/*!
 * \brief Creates RAII style instances of threading primitives
 */
class factory final {

private:
    
    class impl {
    public:
        
        template <class Tag>
        impl(const Tag&, const char* name, const char* mutex_name, const size_t allocatedBytes);
        
        inline
        ~impl() { }
        
        template <typename T>
        deque_ptr<T> createDeque(const char* deque_name);
        
        template <typename T>
        deque_ptr<T> getDeque(const char* deque_name);
        
        /**
         * Create a new shared mutex that can be used within processes. This mutex is removed when 
         * the current `shared` instance is destroyed.
         */
        mutex_ptr createMutex(const char* mutex_name);
        
        mutex_ptr getMutex(const char* mutex_name);
        
        condition_ptr createCondition(const char* condition_name);
        
        condition_ptr getCondition(const char* condition_name);
        
        semaphore_ptr createSemaphore(const char* semaphore_name);
        
        semaphore_ptr getSemaphore(const char* semaphore_name);
        
        template <typename T>
        std::shared_ptr<T> getInst(const char* name);
        
        template <typename T>
        std::shared_ptr<T> createInst(const char* name);
        
        inline
        const std::string& memoryName() const { return d_memoryName; }
        
        inline
        const std::string& dequeMutexName() const { return d_mut_dequesName; }
        
    private:
        inline
        void getSemaphoreVarNames(const char* semaphore_name, 
                                  std::string* const mutex_name, 
                                  std::string* condition_name,
                                  std::string* count_name) {
            assert(("Name(s) were null.", !!mutex_name && !!condition_name && !!count_name)); // just in case
            std::ostringstream m_ss; m_ss << semaphore_name << "_mutex"; 
            std::ostringstream c_ss; c_ss << semaphore_name << "_condition";
            std::ostringstream v_ss; v_ss << semaphore_name << "_count";
            *mutex_name = m_ss.str();
            *condition_name = c_ss.str();
            *count_name = v_ss.str();
        }
        
        //!< The global memory name
        const std::string d_memoryName;
        
        //!< boost::interprocess shared memory segment used to create instances in shared memory
        bip::managed_shared_memory d_memSegment;
        
        const std::string d_mut_dequesName;
        bip::named_recursive_mutex d_mut_instances;
        
        const bool d_createable;
    };
    
public:
    
    
    
    /**
     * Initializes a shared instance which allows one to create containers and other data types.
     * 
     * \param memoryName Name of the new shared memory pool
     * \param allocatedBytes The number of bytes allocated, defaults to 65536
     */
    template <typename Tag>
    factory(const Tag&, const char* memoryName, const size_t allocatedBytes = 65536);
    
    /**
     * FIXME
     */
    ~factory();
    
    //!< Returns the name of the memory segment in use
    inline
    const std::string& memoryName() const { return d_impl->memoryName(); }
    
    //!< Returns the name of the mutex used for locking deques
    inline
    const std::string& dequeMutexName() const { return d_impl->dequeMutexName(); }
    
    /**
     * Creates a new deque with name `deque_name`. The created deque will be removed when the returned
     * pointer falls out of scope.       
     */
    template <typename T>
    inline
    deque_ptr<T> createDeque(const char* deque_name) { return d_impl->createDeque<T>(deque_name); }
        
    
    template <typename T>
    inline
    deque_ptr<T> getDeque(const char* deque_name) { return d_impl->getDeque<T>(deque_name); }
    
    /**
     * Create a new shared mutex that can be used within processes. This mutex is removed when 
     * the current `shared` instance is destroyed.
     */
    inline
    mutex_ptr createMutex(const char* mutex_name) { return d_impl->createMutex(mutex_name); }
    
    inline
    mutex_ptr getMutex(const char* mutex_name) { return d_impl->getMutex(mutex_name); }
    
    inline
    semaphore_ptr createSemaphore(const char* semaphore_name) { return d_impl->createSemaphore(semaphore_name); }
     
    inline
    semaphore_ptr getSemaphore(const char* semaphore_name) { return d_impl->getSemaphore(semaphore_name); }
    
    template <typename T>
    inline
    std::shared_ptr<T> getInst(const char* name) { return d_impl->getInst<T>(name); }
        
    template <typename T>
    inline
    std::shared_ptr<T> createInst(const char* name) { return d_impl->createInst<T>(name); }
    
    /**
     * FIXME
     */
    inline
    condition_ptr createCondition(const char* condition_name) { return d_impl->createCondition(condition_name); }
    
    inline
    condition_ptr getCondition(const char* condition_name) { return d_impl->getCondition(condition_name); }
    
private:
    //!< Pointer to implementation class
    std::unique_ptr<impl> d_impl;
    
    const bool d_cleanup;
};
    
// create_only_t:

template <class Tag>
factory::factory(const Tag& tag, const char* memoryName, const size_t allocatedBytes) 
    : d_cleanup(std::is_same<bip::create_only_t, Tag>::value)
{
    static_assert(std::is_same<bip::create_only_t, Tag>::value 
                  || std::is_same<bip::open_only_t, Tag>::value, "Unknown shared tag.");
    
    std::ostringstream ss; ss << memoryName << "_deque_mux";
    const std::string mux_name(ss.str());
        
    if (d_cleanup) 
    {
        bip::shared_memory_object::remove(memoryName);
        bip::named_mutex::remove(mux_name.c_str());
    }
    
    d_impl = std::unique_ptr<impl>(new impl(tag, memoryName, mux_name.c_str(), allocatedBytes));
}

factory::~factory() 
{
    if (d_cleanup) 
    {
        std::cout << "Destroying all shared memory components under '" << d_impl->memoryName() << "'" << std::endl;
        bip::shared_memory_object::remove(d_impl->memoryName().c_str());
        
        bip::named_recursive_mutex::remove(d_impl->dequeMutexName().c_str());
    }
}

template <>
factory::impl::impl<bip::create_only_t>(const bip::create_only_t&, const char* name, const char* mutex_name, const size_t allocatedBytes) 
    : d_memoryName(name)
    , d_mut_dequesName(mutex_name)
    , d_memSegment(bip::create_only, name, allocatedBytes)
    , d_mut_instances(bip::create_only, mutex_name)
    , d_createable(true) 
{ }

template <>
factory::impl::impl<bip::open_only_t>(const bip::open_only_t&, const char* name, const char* mutex_name, const size_t) 
    : d_memoryName(name)
    , d_mut_dequesName(mutex_name)
    , d_memSegment(bip::open_only, name)
    , d_mut_instances(bip::open_only, mutex_name)
    , d_createable(false) 
{ }

template <typename T>
deque_ptr<T> factory::impl::createDeque(const char* deque_name) 
{
    assert(("Can not create deque in read_only memory", this->d_createable));
    
    auto deleter = [this, deque_name](deque<T> *deq) {
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
        
        this->d_memSegment.destroy<deque<T> >(deque_name);
    };
    
    bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
    d_memSegment.destroy<deque<T> >(deque_name);
    
    auto out_deque = d_memSegment.find<deque<T> >(deque_name).first;
    if (!!out_deque)
    {   // check again, it might have been initiated by another thread/process
        throw std::runtime_error("Tried to reinitialize a deque, " + std::string(deque_name) + ", that exists within " + d_memoryName + ". " 
                                        + "This is likely a race condition.");
    }
    
    //Initialize shared memory STL-compatible allocator
    const shmem_allocator<T> alloc_inst(d_memSegment.get_segment_manager());

    //Construct a deque named "deque_name" in shared memory with argument alloc_inst
    out_deque = d_memSegment.construct<deque<T> >(deque_name)(alloc_inst);
    
    return deque_ptr<T>(out_deque, deleter);
}

mutex_ptr factory::impl::createMutex(const char* mutex_name) 
{
    assert(("Can not create mutex in read_only memory", this->d_createable));
    
    auto deleter = [this, mutex_name](mutex* deq) {
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
        
        this->d_memSegment.destroy<mutex>(mutex_name);
    };
    
    bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
    // destroy it if it exists (just in case)
    d_memSegment.destroy<mutex>(mutex_name);
    
    //Construct a deque named "deque_name" in shared memory with argument alloc_inst
    auto outMutex = d_memSegment.construct<mutex>(mutex_name)();
    
    return mutex_ptr(outMutex, deleter);
}

condition_ptr factory::impl::createCondition(const char* condition_name) 
{ 
    assert(("Can not create condition in read_only memory", this->d_createable));
    
    auto deleter = [this, condition_name](condition* deq) {
        bip::scoped_lock<bip::named_recursive_mutex> lock(this->d_mut_instances);
        
        this->d_memSegment.destroy<condition>(condition_name);
    };
    
    bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
    d_memSegment.destroy<condition>(condition_name);
    
    auto outCondition = d_memSegment.construct<condition>(condition_name)();
    
    return condition_ptr(outCondition, deleter);
}

semaphore_ptr factory::impl::createSemaphore(const char* semaphore_name) 
{ 
    assert(("Can not create semaphore in read_only memory", this->d_createable));
    
    bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
    std::string mutex_name, condition_name, count_name;
    getSemaphoreVarNames(semaphore_name, &mutex_name, &condition_name, &count_name);
    
    auto mutex = this->createMutex(mutex_name.c_str());
    auto condition = this->createCondition(condition_name.c_str());
    auto count = this->createInst<unsigned long>(count_name.c_str());
    
    return semaphore_ptr(new semaphore(bip::create_only, mutex, condition, count));
}

template <typename T>
std::shared_ptr<T> factory::impl::createInst(const char* instance_name) 
{ 
    assert(("Can not create instance in read_only memory", this->d_createable));
    
    auto deleter = [this, instance_name](T*) {
        bip::scoped_lock<bip::named_recursive_mutex> lock(this->d_mut_instances);
        
        this->d_memSegment.destroy<T>(instance_name);
    };
    
    bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
    d_memSegment.destroy<T>(instance_name);
    
    auto outCondition = d_memSegment.construct<T>(instance_name)();
    
    return std::shared_ptr<T>(outCondition, deleter);
}

// open_read_only_t

template <typename T>
deque_ptr< T > factory::impl::getDeque(const char* deque_name) 
{
    auto outDeque = d_memSegment.find<deque<T> >(deque_name).first;
    
    if (!outDeque) 
    {   // We do a double checked lock before determining if it's an error. Just want to be sure that the mutex isn't in the midst
        // of being locked before we decide that it was not loaded
        
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
        outDeque = d_memSegment.find<deque<T> >(deque_name).first;
        
        if (!outDeque) 
        {
            // The deque was never intialized. 
            throw std::runtime_error("Could not get deque, " + std::string(deque_name) + ", that exists within " + d_memoryName + ". " 
                                            + "This is likely a race condition.");
        } 
        
        // if we fall past here, the end return works as expected
    }
    
    // the deque already exists, just return that instance, don't allocate a new one
    return deque_ptr<T>(outDeque, [](deque<T>*) { /* no-op */ });
}

condition_ptr factory::impl::getCondition(const char* condition_name) 
{
    auto outCondition = d_memSegment.find<condition>(condition_name).first;
    if (!outCondition) 
    {   // We do a double checked lock before determining if it's an error. Just want to be sure that the mutex isn't in the midst
        // of being locked before we decide that it was not loaded
        
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
        outCondition = d_memSegment.find<condition>(condition_name).first;
        
        if (!outCondition) 
        {
            // The deque was never intialized. 
            throw std::runtime_error("Could not get condition, " + std::string(condition_name) + ", that exists within " + d_memoryName + ". " 
                                            + "This is likely a race condition.");
        } 
        
        // if we fall past here, the end return works as expected
    }
    
    return condition_ptr(outCondition, [](condition*) { /* no-op */ });
}


mutex_ptr factory::impl::getMutex(const char* mutex_name) 
{
    auto outMutex = d_memSegment.find<mutex>(mutex_name).first;
    if (!outMutex) 
    {   // We do a double checked lock before determining if it's an error. Just want to be sure that the mutex isn't in the midst
        // of being locked before we decide that it was not loaded
        
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
        outMutex = d_memSegment.find<mutex>(mutex_name).first;
        
        if (!outMutex) 
        {
            // The deque was never intialized. 
            throw std::runtime_error("Could not get mutex, " + std::string(mutex_name) + ", that exists within " + d_memoryName + ". " 
                                            + "This is likely a race condition.");
        } 
        
        // if we fall past here, the end return works as expected
    }
    
    return mutex_ptr(outMutex, [](mutex*) { /* no-op */ });
}

semaphore_ptr factory::impl::getSemaphore(const char* semaphore_name) 
{ 
    std::string mutex_name, condition_name, count_name;
    getSemaphoreVarNames(semaphore_name, &mutex_name, &condition_name, &count_name);
    
    auto outMutex = d_memSegment.find<mutex>(mutex_name.c_str()).first;
    auto outCondition = d_memSegment.find<condition>(condition_name.c_str()).first;
    auto outCount = d_memSegment.find<unsigned long>(count_name.c_str()).first;
    
    if (!outMutex && !outCondition && !outCount) 
    {   // they weren't initialized, do a double check lock to make sure.
        outMutex = d_memSegment.find<mutex>(mutex_name.c_str()).first;
        outCondition = d_memSegment.find<condition>(condition_name.c_str()).first;
        outCount = d_memSegment.find<unsigned long>(count_name.c_str()).first;
        
        if (!outMutex || !outCondition || !outCount) 
        {
            // The deque was never intialized. 
            throw std::runtime_error("One of either mutex, '" + mutex_name + "', condition, '" + condition_name 
                                    + "', or count, '" + count_name + "' could not be initialized within " + d_memoryName + "."
                                    + "This is likely a race condition.");
        } 
  
        // fall through
    } 
    else if (!!outMutex ^ !!outCondition)
    {
        throw std::runtime_error("One of either mutex, '" + mutex_name + "', or condition, '" + condition_name 
                                    + "', already exists within " + d_memoryName + ".");
    }
    
    return semaphore_ptr(new semaphore(bip::open_only,
                                       mutex_ptr(outMutex, [](mutex*){ /* no-op */ }), 
                                       condition_ptr(outCondition, [](condition*){ /* no-op */ }), 
                                       std::shared_ptr<unsigned long>(outCount, [](unsigned long*) { /*no-op */ })));
}

template <typename T>
std::shared_ptr< T > factory::impl::getInst(const char* instance_name) 
{
    auto instance = d_memSegment.find<T>(instance_name).first;
    if (!instance) 
    {   // We do a double checked lock before determining if it's an error. Just want to be sure that the mutex isn't in the midst
        // of being locked before we decide that it was not loaded
        
        bip::scoped_lock<bip::named_recursive_mutex> lock(d_mut_instances);
    
        instance = d_memSegment.find<T>(instance_name).first;
        
        if (!instance) 
        {
            // The deque was never intialized. 
            throw std::runtime_error("Could not get instance, " + std::string(instance_name) + ", of type '" + typeid(T).name() + "' " 
                                            + "that exists within " + d_memoryName + ". This is likely a race condition.");
        } 
        
        // if we fall past here, the end return works as expected
    }
    
    return std::shared_ptr<T>(instance, [](T*) { /* no-op */ });
}

} // namespace shared

#endif