#include <set>
#include <algorithm>

using namespace std;

template <typename T>
set<T> set_difference(const set<T> &a, const set<T> &b){
  set<T> res;
  std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
  return res;
}

template <typename T>
set<T> set_union(const set<T> &a, const set<T> &b){
  set<T> res;
  std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
  return res;
}

template <typename T>
set<T> set_intersection(const set<T> &a, const set<T> &b){
  set<T> res;
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
  return res;
}