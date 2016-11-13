#pragma once

#include <set>
#include <algorithm>

namespace util{
  template <typename T>
  T set_difference(const T &a, const T &b){
    T res;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
  }

  template <typename T>
  T set_union(const T &a, const T &b){
    T res;
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
  }

  template <typename T>
  T set_intersection(const T &a, const T &b){
    T res;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
  }
};