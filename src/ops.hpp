#pragma once

#include "op_chore.hpp"
#include "result.hpp"

namespace kvd {

/** GET key */
class GetOp : public UnaryOp<"GET"_OP> {
public:
  using UnaryOp::UnaryOp;
  [[nodiscard]] std::unique_ptr<Result> Execute(Bucket &b) const final;
};

/** PUT key value */
class PutOp : public BinaryOp<"PUT"_OP> {
public:
  using BinaryOp::BinaryOp;
  [[nodiscard]] std::unique_ptr<Result> Execute(Bucket &b) const final;
};

/** COUNT key */
class CountOp : public UnaryOp<"COUNT"_OP> {
public:
  using UnaryOp::UnaryOp;
  [[nodiscard]] std::unique_ptr<Result> Execute(Bucket &b) const final;
};

/** INC key */
class IncOp : public UnaryOp<"INC"_OP> {
public:
  using UnaryOp::UnaryOp;
  [[nodiscard]] std::unique_ptr<Result> Execute(Bucket &b) const final;
};

/** VOID */
class VoidOp : public NullaryOp<"VOID"_OP> {
public:
  using NullaryOp::NullaryOp;
  [[nodiscard]] std::unique_ptr<Result> Execute(Bucket &b) const final;
};

using OpsForeach = OpsIt<OpList< //
    GetOp,                       //
    PutOp,                       //
    CountOp,                     //
    IncOp,                       //
    VoidOp>>;

} // namespace kvd
