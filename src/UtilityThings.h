//
//  UtilityThings.h
//  Soli20200921-Study01
//
//  Created by Ryo Hajika on 2020/09/23.
//

#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <utility>
#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <fstream>

// https://qiita.com/luftfararen/items/e5bc5b72017d71c73226
struct StopWatch {
    StopWatch(){
        start();
    }
    void start(){
        pre_ = std::chrono::high_resolution_clock::now();
    }
    double lap(){ // in MS
        auto tmp = std::chrono::high_resolution_clock::now();
        auto dur = tmp - pre_;
        pre_ = tmp;
        return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / 1000000.0;
    }
    
    std::chrono::high_resolution_clock::time_point pre_;
};

class CsvLogger {
public:
    CsvLogger(){
        
    }
    ~CsvLogger(){
        if(os.is_open()) os.close();
        if(row_content.size()) row_content.clear();
    }
    void start(std::vector<std::string> & header, std::string file_name){
        row_header = &header;
        if(row_content.size()) row_content.clear();
        if(os.is_open()) os.close();
        
        os.open(file_name);
        for(int i=0; i<header.size(); i++){
            os << header[i];
            if(i < header.size() - 1){
                os << ",";
            }else{
                os << "\n";
            }
        }
    }
    template <class T>
    void update(uint64_t first_col_time, std::string target, T content){
        if(ts_atpoint == first_col_time){
            row_content[target] = content;
        }else{
            this->push_row();
            ts_atpoint = first_col_time;
            row_content[target] = content;
        }
        if(target == "soli_frame"){
            soli_frame = content;
        }
        else if(target == "natnet_framecount"){
            natnet_frame = content;
        }
        return;
    }
    
    void stop(){
        this->push_row();
        os.close();
    }
    
private:
    void push_row(){
        os << ts_atpoint << ",";
        os << natnet_frame << ",";
        os << soli_frame << ",";
        ts_atpoint = 0;
        natnet_frame = 0;
        soli_frame = 0;
        
        for(unsigned i=3; i<row_header->size(); i++){
            os << row_content[row_header->at(i)];
            if(i < row_header->size() - 1){
                os << ",";
            }else{
                os << "\n";
            }
            row_content[row_header->at(i)] = -999.;
        }
        return;
    }
    std::ofstream os;
    std::vector<std::string> * row_header;
    std::uint64_t ts_atpoint;
    std::uint64_t soli_frame;
    std::uint64_t natnet_frame;
    std::map<std::string, double> row_content;
};

//namespace detail {
//    template <class T>
//    struct is_reference_wrapper : std::false_type {};
//    template <class U>
//    struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
//
//    template<class T>
//    struct invoke_impl {
//        template<class F, class... Args>
//        static auto call(F&& f, Args&&... args)
//        -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
//    };
//
//    template<class B, class MT>
//    struct invoke_impl<MT B::*> {
//        template<class T, class Td = typename std::decay<T>::type,
//        class = typename std::enable_if<std::is_base_of<B, Td>::value>::type
//        >
//        static auto get(T&& t) -> T&&;
//
//        template<class T, class Td = typename std::decay<T>::type,
//        class = typename std::enable_if<is_reference_wrapper<Td>::value>::type
//        >
//        static auto get(T&& t) -> decltype(t.get());
//
//        template<class T, class Td = typename std::decay<T>::type,
//        class = typename std::enable_if<!std::is_base_of<B, Td>::value>::type,
//        class = typename std::enable_if<!is_reference_wrapper<Td>::value>::type
//        >
//        static auto get(T&& t) -> decltype(*std::forward<T>(t));
//
//        template<class T, class... Args, class MT1,
//        class = typename std::enable_if<std::is_function<MT1>::value>::type
//        >
//        static auto call(MT1 B::*pmf, T&& t, Args&&... args)
//        -> decltype((invoke_impl::get(std::forward<T>(t)).*pmf)(std::forward<Args>(args)...));
//
//        template<class T>
//        static auto call(MT B::*pmd, T&& t)
//        -> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
//    };
//
//    template<class F, class... Args, class Fd = typename std::decay<F>::type>
//    auto INVOKE(F&& f, Args&&... args)
//    -> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));
//
//} // namespace detail
//
//template <class callable, class... arguments>
//void SimpleTimerCallbacker(unsigned after_ms, bool async, callable&& f, arguments&&... args){
//    //using a = std::result_of<callable(arguments...)>::type;
//    using a = decltype(std::result_of<std::declval<callable>(f(args)...)>);
//    std::function<typename a> task(std::bind(std::forward<callable>(f),
//                                                                                        std::forward<arguments>(args)...));
//    if (async){
//        std::thread([after_ms, task]() {
//            std::this_thread::sleep_for(std::chrono::milliseconds(after_ms));
//            task();
//        }).detach();
//    }else{
//        std::this_thread::sleep_for(std::chrono::milliseconds(after_ms));
//        task();
//    }
//}
