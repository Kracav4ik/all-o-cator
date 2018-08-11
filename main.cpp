#include <iostream>
#include <map>
#include <vector>
#include <string>

//template<typename T>
//std::string prettyName() {
//    return __PRETTY_FUNCTION__;
//}
//
//template<typename T>
//std::string prettyPtr(T*) {
//    return prettyName<T>();
//}

namespace ns {
//    const bool WRITE = true;
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
                offset++;
                return res;
            }

            T* allocateIn(std::size_t n) {
//                std::cerr << "sig " << prettyPtr(this) << std::endl;
//                if constexpr (WRITE) {
//                    std::cout << "allocate: [n = " << n << "]" << std::endl;
//                }
                auto p1 = std::malloc(n * sizeof(T));
                if (!p1) {
                    throw std::bad_alloc();
                }
                block = reinterpret_cast<T*>(p1);
                return block;
            }

            void deallocate(T* p1, std::size_t n) {}

            void deallocateOut(T* p1, std::size_t n) {
//                if constexpr (WRITE) {
//                    std::cout << "deallocate: [n  = " << n << "] " << std::endl;
//                }
                std::free(p1);
            }

            template <typename U, typename ...Args>
            void construct(U* p1, Args&& ...args) {
//                if constexpr (WRITE) {
//                    std::cout << "construct" << std::endl;
//                }
                new(p1) U(std::forward<Args>(args)...);
            }

            void destroy(T* p1) {
//                if constexpr (WRITE) {
//                    std::cout << "destroy" << std::endl;
//                }
                p1->~T();
            }
        };
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

    std::vector<int> stlVector;
    stlVector.reserve(10);

    for (int i = 0; i < 10; ++i) {
        stlVector.emplace_back(i);
    }

    std::vector<int, ns::Preallocate<10>::SomeAllocator<int>> myVector;
    myVector.reserve(10);

    for (int i = 0; i < 10; ++i) {
        myVector.emplace_back(i);
    }

    for (auto v : myVector) {
        std::cout << v << ' ';
    }

    return 0;
}

