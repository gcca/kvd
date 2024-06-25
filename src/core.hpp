#pragma once

#define KVD_PROTO(X)                                                           \
public:                                                                        \
  virtual ~X() = default;                                                      \
  X(const X &) = delete;                                                       \
  X(const X &&) = delete;                                                      \
  X &operator=(const X &) = delete;                                            \
  X &operator=(const X &&) = delete;                                           \
                                                                               \
protected:                                                                     \
  X() = default
