#include <iostream>
#include <map>
#include <vector>
#include <string>

namespace ns {
    int fac(int n) {
        int res = 1;
        for (int i = 2; i <= n; ++i) {
            res *= i;
        }
        return res;
    }
    template <size_t N>
    struct Preallocate {
        template <typename T>
        class SomeAllocator {
        private:
            T* block = nullptr;
            size_t offset = 0;

            T* allocateIn(std::size_t n) {
                auto p1 = std::malloc(n * sizeof(T));
                if (!p1) {
                    throw std::bad_alloc();
                }
                block = reinterpret_cast<T*>(p1);
                return block;
            }

            void deallocateOut(T* p1, std::size_t n) {
                std::free(p1);
            }

        public:
            using value_type = T;

            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;

            SomeAllocator() {
                block = allocateIn(N);
            }

            ~SomeAllocator() {
                deallocateOut(block, N);
            }

            template<typename U>
            explicit SomeAllocator(const SomeAllocator<U>&) {}


            T* allocate(std::size_t n) {
                T* res = block + offset;
                offset += n;
                if (offset > 10) {
                    throw std::bad_alloc();
                }
                return res;
            }

            void deallocate(T* p1, std::size_t n) {}

            template <typename U, typename ...Args>
            void construct(U* p1, Args&& ...args) {
                new(p1) U(std::forward<Args>(args)...);
            }

            void destroy(T* p1) {
                p1->~T();
            }
        };
    };

    template<typename T, typename Alloc = std::allocator<T>>
    class MyVector {
    private:
        Alloc allocator;
        int i = 0;
        T* ptr = nullptr;

    public:
        MyVector() {
            ptr = allocator.allocate(10);
        }

        T* begin() {
            return ptr;
        }

        T* end() {
            return (ptr + i);
        }

        virtual ~MyVector() {
            allocator.deallocate(ptr, 10);
        }

        void push_back(T t) {
            *(ptr + i) = t;
            i++;
        }
    };

}

int main() {
    std::map<const int, int> stlMap;

    for (int i = 0; i < 10; ++i) {
        stlMap.emplace(i, ns::fac(i));
    }

    std::map<int, int, std::less<>, ns::Preallocate<10>::SomeAllocator<std::pair<const int, int>>> myMap;

    for (int i = 0; i < 10; ++i) {
        myMap.emplace(i, ns::fac(i));
    }

    for (auto item : myMap) {
        std::cout << item.first << ' ' << item.second << std::endl;
    }

    ns::MyVector<int> myVectorStlAllocator;

    for (int i = 0; i < 10; ++i) {
        myVectorStlAllocator.push_back(i);
    }

    for (auto v : myVectorStlAllocator) {
        std::cout << v << ' ';
    }
    std::cout << std::endl;

    ns::MyVector<int, ns::Preallocate<10>::SomeAllocator<int>> myVector;

    for (int i = 0; i < 10; ++i) {
        myVector.push_back(i);
    }

    for (auto v : myVector) {
        std::cout << v << ' ';
    }

    return 0;
}

