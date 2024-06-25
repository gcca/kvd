#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "logical_plan.hpp"
#include "ops.hpp"
#include "text_plan.hpp"

using namespace kvd;

namespace {

class OpLine {
  friend std::ostream &operator<<(std::ostream &os, const OpLine &op) {
    op.Explain(os);
    return os;
  }

public:
  explicit OpLine(const std::string_view &name,
                  std::vector<std::string_view> &&args)
      : name_{name},
        args_{std::forward<std::vector<std::string_view>>(std::move(args))} {}

  void Explain(std::ostream &os) const noexcept {
    os << name_;
    if (not args_.empty()) {
      os << ":";
      std::copy(args_.cbegin(), std::prev(args_.cend()),
                std::ostream_iterator<std::string_view>(os, ","));
      os << args_.back();
    }
  }

  [[nodiscard]] std::unique_ptr<Op> ToOp() const {
    return OpsForeach::Apply(name_, args_);
  }

private:
  std::string_view name_;
  std::vector<std::string_view> args_;
};

class TextPlanRefBase : public TextPlan {
protected:
  explicit TextPlanRefBase(const std::string &text) : text_{text} {}

  [[nodiscard]] std::unique_ptr<LogicalPlan> ToLogicalPlan() const final {
    std::vector<std::unique_ptr<Op>> ops;
    ops.reserve(oplines_.size());
    std::transform(oplines_.cbegin(), oplines_.cend(), std::back_inserter(ops),
                   [](const OpLine &o) { return o.ToOp(); });
    return LogicalPlan::Make(std::move(ops));
  }

  void Explain(std::ostream &os) const final {
    std::copy(oplines_.begin(), std::prev(oplines_.end()),
              std::ostream_iterator<OpLine>(os, "\n"));
    os << oplines_.back();
  }

  [[nodiscard]] inline std::vector<OpLine> &oplines() { return oplines_; }

  [[nodiscard]] inline const std::string &text() const { return text_; }

private:
  std::vector<OpLine> oplines_;
  const std::string &text_;
};

class ZTextPlan : public TextPlanRefBase {
public:
  explicit ZTextPlan(const std::string &text) : TextPlanRefBase{text} {
    std::string::const_iterator line_it = text.cbegin();
    std::string::const_iterator const line_end = text.cend();

    std::size_t ops_size = *line_it++;
    oplines().reserve(ops_size);
    while ((line_it != line_end) and (ops_size-- != 0U)) {
      const std::ptrdiff_t opname_size = *line_it++;
      std::string::const_iterator opname_end = line_it + opname_size;

      const std::string_view opname{line_it, opname_end};
      line_it = opname_end;

      std::size_t opargs_size = *line_it++;
      std::vector<std::string_view> opargs;
      opargs.reserve(opargs_size);
      while ((line_it != line_end) and (opargs_size-- != 0U)) {
        std::size_t arg_presize = *line_it++;

        std::ptrdiff_t arg_size = 1;
        while ((line_it != line_end) and (arg_presize-- != 0U)) {
          arg_size *= *line_it++;
        }

        std::string::const_iterator arg_end = line_it + arg_size;
        opargs.emplace_back(line_it, arg_end);
        line_it = arg_end;
      }

      oplines().emplace_back(opname, std::move(opargs));
    }
  }
};

class YTextPlan : public TextPlanRefBase {
public:
  explicit YTextPlan(const std::string &text) : TextPlanRefBase{text} {
    std::string_view const vtext{text};
    oplines().reserve(MAX_ACTIONS_LENGTH);

    std::size_t end = text.find('\n'), carry = 0;

    while (end != std::string::npos) {
      std::string_view const line = vtext.substr(carry, end - carry);
      oplines().emplace_back(From(line));

      carry = end + 1;
      end = text.find('\n', carry);
    }

    std::string_view const line = vtext.substr(carry, vtext.size() - carry);
    oplines().emplace_back(From(line));
  }

private:
  static OpLine From(const std::string_view &line) {
    std::size_t end = line.find(' ');

    if (end == std::string::npos) { return OpLine{std::move(line), {}}; }

    const std::string_view op{line.data(), end};

    std::vector<std::string_view> args;
    args.reserve(4);

    std::size_t start = end + 1;
    end = line.find(' ', start);

    while (end != std::string_view::npos) {
      args.emplace_back(line.data() + start, end - start);
      start = end + 1;
      end = line.find(' ', start);
    }

    args.emplace_back(line.data() + start, line.size() - start);

    return OpLine{std::move(op), std::move(args)};
  }

  static constexpr std::size_t MAX_ACTIONS_LENGTH = 256;
};

} // namespace

std::unique_ptr<TextPlan> TextPlan::MakeZ(const std::string &text) {
  return std::make_unique<ZTextPlan>(text);
}

std::unique_ptr<TextPlan> TextPlan::MakeY(const std::string &text) {
  return std::make_unique<YTextPlan>(text);
}
